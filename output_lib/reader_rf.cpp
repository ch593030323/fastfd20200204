#include "reader_rf.h"
#include <QFile>

static const QByteArray REQUEST_MODE1 = QByteArray().append(0x01);
static const char REQUEST_MODE[]= {0x01};//idel模式0x00,所有卡0x01
static const uchar LOADKEY_MODE= 0x00;//0,0套密码,1,1套密码,2,2套密码,3,3不允许
static const uchar AUTHENTICATION_MODE= 0x00;//0,0套密码,1,1套密码, 2,2套密码,3,MCM-ROM密码验证
static const uchar BEEPTIME= 0x05;//蜂鸣时长
static const uchar replenish[]={0x1a, 0x2f, 0x45, 0x64, 0x8e, 0x9a, 0xa1, 0xb5, 0xf6, 0x7e, 0xf9, 0xbf, 0x91, 0x34, 0x67, 0x80};
//有效传输密码和用户密码为6字节， 但通信按8 字节发送， 最后两个字节为任意值。
static const uchar TKEY[] ={0x7d, 0x3e, 0x9f, 0x4f, 0x95, 0x95, 0x60, 0xf7};//传输密码
//static const uchar NKEY[] ={0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6};//下载到RAM中的密码
static const uchar NKEY[] ={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static  uchar SECNR=0x01;
static const  uchar ADDRESS[]={uchar(SECNR*4),
                               uchar(SECNR*4 + 1),
                               uchar(SECNR*4 + 2),
                               uchar(SECNR*4 + 3)};//块地址
Reader_RF::Reader_RF(QObject *parent) :
    QObject(parent)
{
    open();
}

Reader_RF::~Reader_RF()
{
    close();
}

bool Reader_RF::open()
{
#ifdef Q_OS_UNIX
    otherError = "device is not open";
    int r;
    dev_handle = NULL;
    ctx=NULL;
    sqr = 0;//序列号
    byte2.resize(64);
    isopen = false;
    //初始化
    if ((r = libusb_init(&ctx)) < 0) {
        goto failure;
    }
    //启动调试
    libusb_set_debug(ctx, 3);    //确保应用程序不关闭文件描述符 stderr输出。
    //按vid,pid打开
    if((dev_handle = libusb_open_device_with_vid_pid(ctx,0x10C4,0x82CD)) == NULL) {//打开失败则 退出 dev_handle == NULL
        goto failure;
    }
    //声明前判断
    r = libusb_kernel_driver_active(dev_handle, 0);           //判断 该接口是否已经被内核声明
    //("libusb_kernel_driver_active = %d\n", r);//0 is the interface to check
    if (r == 1) {                                                                 //若声明一个已经被内核声明过的接口，则脱离该接口的内核驱动程序
        r = libusb_detach_kernel_driver(dev_handle, 0);     //同一次 只会进入一次，及 只会脱离一次
        //("libusb_detach_kernel_driver = %d\n", r);
    }
    //声明
    if((r=libusb_claim_interface(dev_handle,0)) < 0) {
        goto failure;
    }
    //一般错误
    read_card_error=
            QStringList()
            <<""<<QObject::tr("在操作区域内无卡")<<""<<""
           <<QObject::tr("验证不成功")<<""<<QObject::tr("通信错误")<<""<<QObject::tr("读系列号错误")<<""<<QObject::tr("卡片没有通过验证");
    //默认密码
    _DefPwd = QByteArray::fromHex(QByteArray("ffffffffffff"));

    isopen = true;
    otherError="";
    //蜂鸣
    beep();
    goto success;

failure:
    if(ctx != NULL) {
        libusb_exit(ctx);
        ctx = NULL;
    }
success:
#endif
    return isopen;
}

void Reader_RF::close()
{
#ifdef Q_OS_UNIX

    if(dev_handle != NULL) {
        libusb_release_interface(dev_handle,0);
        libusb_close(dev_handle); //close the device we opened
        libusb_exit(ctx); //needs to be called to end the
        dev_handle = NULL;
        ctx = NULL;
    }
    isopen=false;
#endif
}

bool Reader_RF::writecard(const QString &part0, const QString &part1, const QString &part2)
{
    //设备未打开
    if(!isopen)return false;
    if(part0.length() > 16
            ||part1.length() > 16
            ||part2.length() > 16) {
        otherError = QObject::tr("写卡长度必须不能大于16");
        return false;
    }
#ifdef Q_OS_UNIX
    int r;
    //0x45 将卡片置于暂停状态
    QByteArray tmp45 = getBccList((char)0x45, (char) 0x00, QByteArray());
    if((r = getRespone(dev_handle,tmp45)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    //0x41 向天线操作范围内的卡片发出请求和检测信号
    QByteArray tmp41 = getBccList((char)0x41,(char)1,QByteArray(REQUEST_MODE,1));
    if((r = getRespone(dev_handle,tmp41)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }

    //0x42 防冲突操作并返回卡片系列号
    QByteArray tmp42 = getBccList((char)0x42,(char)0x05,QByteArray().append((char)0x00));
    if((r = getRespone(dev_handle, tmp42)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    QByteArray tm;//[]=new byte[6];
    tm.append(byte2[5]);//[0]=byte2[5];
    tm.append(byte2[6]);//tm[1]=byte2[6];
    tm.append(byte2[7]);//tm[2]=byte2[7];
    tm.append(byte2[8]);//tm[3]=byte2[8];

    //0x43 选择卡片并返回卡片容量值
    QByteArray tmp43=getBccList((char)0x43,(char)0x04, tm);
    if((r = getRespone(dev_handle, tmp43)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }

    //0x5f 匹配旧密码
    char m1[] =  {
        AUTHENTICATION_MODE, ADDRESS[0]/*, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff*/
    };//旧密码
    QByteArray tmp5f = getBccList((char)0x5f, (char)0x08, QByteArray(m1, sizeof(m1)) + _DefPwd);
    if((r = getRespone(dev_handle, tmp5f)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    //    //0x4c 改变存放于ASIC 接口内部RAM 中的授权密码
    //    char m[] = {
    //        LOADKEY_MODE,SECNR,TKEY[0],TKEY[1],TKEY[2],
    //        TKEY[3],TKEY[4],TKEY[5],TKEY[6],TKEY[7]
    //    };
    //    QByteArray tmp4c=getBccList((char)0x4c,(char)0x12, QByteArray(m, sizeof(m))+_DefPwd);
    //    if((r = getRespone(dev_handle, tmp4c)) < 0 ) {
    //        otherError = libusb_error_name(r);
    //        return false;
    //    }

    //    //0x44 相互确认
    //    QByteArray tmp44=getBccList((char)0x44,(char)0x02,QByteArray().append(AUTHENTICATION_MODE).append(SECNR));//new byte[]{authentication_mode,secnr});
    //    if((r = getRespone(dev_handle, tmp44)) < 0 ) {
    //        otherError = libusb_error_name(r);
    //        return false;
    //    }
    if(indexof_simpleError() != 0 ) {
        otherError = /*simpleError()+*/QObject::tr("密码错误");
        return false;
    }

    {
        //0x47 将数据写入卡中的某一块
        QByteArray dd=getBccWriteList((char)0x47, (char)0x11,ADDRESS[0], QByteArray().append(part0));
        if((r = getRespone(dev_handle,dd)) < 0 ) {
            otherError = libusb_error_name(r);
            return false;
        }
    }
    {
        //0x47 将数据写入卡中的某一块
        QByteArray dd=getBccWriteList((char)0x47, (char)0x11,ADDRESS[1], QByteArray().append(part1));
        if((r = getRespone(dev_handle,dd)) < 0 ) {
            otherError = libusb_error_name(r);
            return false;
        }
    }
    {
        //0x47 将数据写入卡中的某一块
        QByteArray dd=getBccWriteList((char)0x47, (char)0x11,ADDRESS[2], QByteArray().append(part2));
        if((r = getRespone(dev_handle,dd)) < 0 ) {
            otherError = libusb_error_name(r);
            return false;
        }
    }
    //蜂鸣
    beep();
#endif
    return true;
}

bool Reader_RF::writeSqr(int block_id, const QString &part)
{
    //设备未打开
    if(!isopen)return false;
    if(part.length() > 16) {
        otherError = QObject::tr("写卡长度必须不能大于16");
        return false;
    }
    if(block_id>=0&&block_id<3) {

    } else {
        return false;
    }
#ifdef Q_OS_UNIX

    int r;
    //0x45 将卡片置于暂停状态
    QByteArray tmp45 = getBccList((char)0x45, (char) 0x00, QByteArray());
    if((r = getRespone(dev_handle,tmp45)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    //0x41 向天线操作范围内的卡片发出请求和检测信号
    QByteArray tmp41 = getBccList((char)0x41,(char)1,QByteArray(REQUEST_MODE,1));
    if((r = getRespone(dev_handle,tmp41)) < 0 ) {
        otherError = libusb_error_name(r);

        return false;
    }

    //0x42 防冲突操作并返回卡片系列号
    QByteArray tmp42 = getBccList((char)0x42,(char)0x05,QByteArray().append((char)0x00));
    if((r = getRespone(dev_handle, tmp42)) < 0 ) {
        otherError = libusb_error_name(r);

        return false;
    }
    QByteArray tm;//[]=new byte[6];
    tm.append(byte2[5]);//[0]=byte2[5];
    tm.append(byte2[6]);//tm[1]=byte2[6];
    tm.append(byte2[7]);//tm[2]=byte2[7];
    tm.append(byte2[8]);//tm[3]=byte2[8];

    //0x43 选择卡片并返回卡片容量值
    QByteArray tmp43=getBccList((char)0x43,(char)0x04, tm);
    if((r = getRespone(dev_handle, tmp43)) < 0 ) {
        otherError = libusb_error_name(r);

        return false;
    }
    //0x5f 匹配旧密码
    char m1[] =  {
        AUTHENTICATION_MODE, ADDRESS[0]/*, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff*/
    };//旧密码
    QByteArray tmp5f = getBccList((char)0x5f, (char)0x08, QByteArray(m1, sizeof(m1)) + _DefPwd);
    if((r = getRespone(dev_handle, tmp5f)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    //    //0x4c 改变存放于ASIC 接口内部RAM 中的授权密码
    //    char m[] = {
    //        LOADKEY_MODE,SECNR,TKEY[0],TKEY[1],TKEY[2],
    //        TKEY[3],TKEY[4],TKEY[5],TKEY[6],TKEY[7]
    //    };
    //    QByteArray tmp4c=getBccList((char)0x4c,(char)0x12, QByteArray(m, sizeof(m))+_DefPwd);
    //    if((r = getRespone(dev_handle, tmp4c)) < 0 ) {
    //        otherError = libusb_error_name(r);

    //        return false;
    //    }

    //    //0x44 相互确认
    //    QByteArray tmp44=getBccList((char)0x44,(char)0x02,QByteArray().append(AUTHENTICATION_MODE).append(SECNR));//new byte[]{authentication_mode,secnr});
    //    if((r = getRespone(dev_handle, tmp44)) < 0 ) {
    //        otherError = libusb_error_name(r);

    //        return false;
    //    }
    if(indexof_simpleError() != 0 ) {
        otherError = /*simpleError()+*/QObject::tr("密码错误");
        return false;
    }

    {
        //0x47 将数据写入卡中的某一块
        QByteArray dd=getBccWriteList((char)0x47, (char)0x11,ADDRESS[block_id], QByteArray().append(part));
        if((r = getRespone(dev_handle,dd)) < 0 ) {
            otherError = libusb_error_name(r);
            return false;
        }
    }
    //蜂鸣
    beep();
#endif
    return true;
}

bool Reader_RF::readcard(QString &part0, QString &part1, QString &part2)
{
#ifdef Q_OS_UNIX
    int r;
    //设备未打开
    if(!isopen)return false;

    //0x45 将卡片置于暂停状态
    QByteArray tmp45=getBccList((char)0x45, (char) 0x00, QByteArray());
    if((r = getRespone(dev_handle, tmp45)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }

    //0x41 向天线操作范围内的卡片发出请求和检测信号
    QByteArray tmp41=getBccList((char)0x41,(char)1,QByteArray(REQUEST_MODE,1));
    if((r = getRespone(dev_handle,tmp41)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    if(indexof_simpleError() != 0 )  return false;
    //0x42 防冲突操作并返回卡片系列号
    QByteArray tmp42=getBccList((char)0x42,(char)0x05,QByteArray().append((char)0x00));
    if((r = getRespone(dev_handle, tmp42)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    QByteArray tm;//[]=new byte[6];
    tm.append(byte2[5]);//[0]=byte2[5];
    tm.append(byte2[6]);//tm[1]=byte2[6];
    tm.append(byte2[7]);//tm[2]=byte2[7];
    tm.append(byte2[8]);//tm[3]=byte2[8];

    //0x43 选择卡片并返回卡片容量值
    QByteArray tmp43=getBccList((char)0x43,(char)0x04, tm);
    if((r = getRespone(dev_handle, tmp43)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    //0x5f 匹配旧密码
    char m1[] =  {
        AUTHENTICATION_MODE, ADDRESS[0]/*, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff*/
    };//旧密码
    QByteArray tmp5f = getBccList((char)0x5f, (char)0x08, QByteArray(m1, sizeof(m1)) + _DefPwd);
    if((r = getRespone(dev_handle, tmp5f)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }/*
    //0x4c 改变存放于ASIC 接口内部RAM 中的授权密码
    char m[] = {
        LOADKEY_MODE,SECNR,TKEY[0],TKEY[1],TKEY[2],
        TKEY[3],TKEY[4],TKEY[5],TKEY[6],TKEY[7]
    };

    QByteArray tmp4c =getBccList((char)0x4c,(char)0x12,QByteArray(m, sizeof(m))+_DefPwd);//new byte[]{loadkey_mode,secnr,tkey[0],tkey[1],tkey[2],tkey[3],tkey[4],tkey[5],tkey[6],tkey[7],nkey[0],nkey[1],nkey[2],nkey[3],nkey[4],nkey[5]});
    if((r = getRespone(dev_handle, tmp4c)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }

    //0x44 相互确认
    QByteArray tmp44=getBccList((char)0x44,(char)0x02,QByteArray().append(AUTHENTICATION_MODE).append(SECNR));//new byte[]{authentication_mode,secnr});
    if((r = getRespone(dev_handle, tmp44)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }*/
    if(indexof_simpleError() != 0 ) {
        otherError = /*simpleError()+*/QObject::tr("密码错误");
        return false;
    }

    {
        //0x46 读出卡中某一块的16 个字节数据
        QByteArray tmp46=getBccList((char)0x46,(char)0x01,QByteArray().append(ADDRESS[0]));
        if((r = getRespone(dev_handle, tmp46)) < 0 ) {
            //() << "getRespone(dev_handle,tmp46) = " << libusb_error_name(r);
            otherError = libusb_error_name(r);

            return false;
        }
        QByteArray s= byte2.toHex();//ConversionNumber.Bytes2HexString(byte2);
        s=s.mid(10,32);//s.substring(10, 42);
        part0 = QByteArray::fromHex(s);
    }
    {
        //0x46 读出卡中某一块的16 个字节数据
        QByteArray tmp46=getBccList((char)0x46,(char)0x01,QByteArray().append(ADDRESS[1]));
        if((r = getRespone(dev_handle, tmp46)) < 0 ) {
            //() << "getRespone(dev_handle,tmp46) = " << libusb_error_name(r);
            otherError = libusb_error_name(r);
            return false;
        }
        QByteArray s= byte2.toHex();//ConversionNumber.Bytes2HexString(byte2);
        s=s.mid(10,32);//s.substring(10, 42);
        part1 = QByteArray::fromHex(s);
    }
    {
        //0x46 读出卡中某一块的16 个字节数据
        QByteArray tmp46=getBccList((char)0x46,(char)0x01,QByteArray().append(ADDRESS[2]));
        if((r = getRespone(dev_handle, tmp46)) < 0 ) {
            //() << "getRespone(dev_handle,tmp46) = " << libusb_error_name(r);
            otherError = libusb_error_name(r);

            return false;
        }
        QByteArray s= byte2.toHex();//ConversionNumber.Bytes2HexString(byte2);
        s=s.mid(10,32);//s.substring(10, 42);
        part2 = QByteArray::fromHex(s);
    }
    //蜂鸣
    beep();
#endif
    return true;
}

bool Reader_RF::setDefPwd(const QString &defPwd)
{
    if(defPwd.length() == 12) {
    } else {
        otherError = QObject::tr("设置密码错误，长度必须为12位");
        return false;
    }//ffffffffffff
    //ffff

    _DefPwd = QByteArray::fromHex(QByteArray().append(defPwd));
    return true;
}

QByteArray Reader_RF::defPwd()
{
    return _DefPwd.toHex();
}

bool Reader_RF::pwdChange(const QString &oldPwd, const QString &newPwd)
{
    QByteArray _oldpwd = QByteArray().append(oldPwd);
    QByteArray _newpwd = QByteArray().append(newPwd);
    //设备未打开
    if(!isopen)return false;
    if(_oldpwd.length() != 12 || _newpwd.length() != 12) {
        otherError = QObject::tr("设置密码错误，长度必须为12位");
        return false;
    }
#ifdef Q_OS_UNIX
    int r;
    //0x4e 将RF系统关闭一段时间
    const char m[] = {0x03, 0x00};
    QByteArray tmp4e=getBccList((char)0x4e, (char) 0x02, QByteArray(m, sizeof(m)));
    if((r = getRespone(dev_handle, tmp4e)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }

    //0x41 向天线操作范围内的卡片发出请求和检测信号
    QByteArray tmp41=getBccList((char)0x41,(char)1,QByteArray(REQUEST_MODE,1));
    if((r = getRespone(dev_handle,tmp41)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }


    //0x42 防冲突操作并返回卡片系列号
    QByteArray tmp42=getBccList((char)0x42,(char)0x05,QByteArray().append((char)0x00));
    if((r = getRespone(dev_handle, tmp42)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    QByteArray tm;//[]=new byte[6];
    tm.append(byte2[5]);//[0]=byte2[5];
    tm.append(byte2[6]);//tm[1]=byte2[6];
    tm.append(byte2[7]);//tm[2]=byte2[7];
    tm.append(byte2[8]);//tm[3]=byte2[8];
    //() << tm.toHex();
    //    for(int inddex=0;inddex<byte2.count();inddex++) {
    //        qDebug("%0x", byte2[inddex]);
    //    }
    //0x43 选择卡片并返回卡片容量值
    QByteArray tmp43=getBccList((char)0x43,(char)0x04, tm);
    if((r = getRespone(dev_handle, tmp43)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }

    //0x5f 匹配旧密码
    char m1[] =  {
        AUTHENTICATION_MODE, ADDRESS[0]/*, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff*/
    };//旧密码
    QByteArray tmp5f = getBccList((char)0x5f, (char)0x08, QByteArray(m1, sizeof(m1)) + QByteArray::fromHex(_oldpwd));
    if((r = getRespone(dev_handle, tmp5f)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    if(indexof_simpleError() != 0)  return false;

    //0x46
    QByteArray tmp46=getBccList(0x46, 0x01, QByteArray().append(ADDRESS[3]));
    if((r = getRespone(dev_handle, tmp46)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }

    //    QByteArray m2;
    QByteArray tmp47;
    tmp47.append(byte2[11]);
    tmp47.append(byte2[12]);
    tmp47.append(byte2[13]);
    tmp47.append(byte2[14]);
    tmp47.append(byte2[15]);
    tmp47.append(byte2[16]);
    tmp47.append(byte2[17]);
    tmp47.append(byte2[18]);
    tmp47.append(byte2[19]);
    tmp47.append(byte2[20]);
    //() << tmp47.toHex();
    //    char m2[]={
    //        0xff,0x07,0x80,0x69,
    //        0xff,0xff,  0xff,   0xff,0xff,0xff};

    //0x47
    QByteArray tm47_3=getBccWriteList(0x47, 0x11,ADDRESS[3], QByteArray::fromHex(_newpwd)+tmp47);
    if((r = getRespone(dev_handle,tm47_3)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    if(indexof_simpleError() != 0)  return false;

    //0x45 将卡片置于暂停状态
    QByteArray tmp45=getBccList((char)0x45, (char) 0x00, QByteArray());

    if((r = getRespone(dev_handle, tmp45)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
#endif
    return true;
}

bool Reader_RF::cardisready()
{
#ifdef Q_OS_UNIX
    //设备未打开
    if(!isopen)return false;
    int r;
    QByteArray s;

    //0x45 将卡片置于暂停状态
    QByteArray tmp45=getBccList((char)0x45, (char) 0x00, QByteArray());
    if((r = getRespone(dev_handle, tmp45)) < 0 ) {
        return false;
    }

    //0x41 向天线操作范围内的卡片发出请求和检测信号
    QByteArray tmp41=getBccList((char)0x41,(char)1,QByteArray(REQUEST_MODE,1));
    if((r = getRespone(dev_handle,tmp41)) < 0 ) {
        return false;
    }
    if(indexof_simpleError() == 0 )  return true;
#endif
    return false;
}

bool Reader_RF::pwdiscorrect()
{
#ifdef Q_OS_UNIX
    //设备未打开
    if(!isopen)return false;

    int r;
    //0x45 将卡片置于暂停状态
    QByteArray tmp45 = getBccList((char)0x45, (char) 0x00, QByteArray());
    if((r = getRespone(dev_handle,tmp45)) < 0 ) {
        return false;
    }

    //0x41 向天线操作范围内的卡片发出请求和检测信号
    QByteArray tmp41 = getBccList((char)0x41,(char)1,QByteArray(REQUEST_MODE,1));
    if((r = getRespone(dev_handle,tmp41)) < 0 ) {
        return false;
    }
    if(indexof_simpleError() != 0 )  return false;

    //0x42 防冲突操作并返回卡片系列号
    QByteArray tmp42 = getBccList((char)0x42,(char)0x05,QByteArray().append((char)0x00));
    if((r = getRespone(dev_handle, tmp42)) < 0 ) {
        return false;
    }
    QByteArray tm;//[]=new byte[6];
    tm.append(byte2[5]);//[0]=byte2[5];
    tm.append(byte2[6]);//tm[1]=byte2[6];
    tm.append(byte2[7]);//tm[2]=byte2[7];
    tm.append(byte2[8]);//tm[3]=byte2[8];

    //0x43 选择卡片并返回卡片容量值
    QByteArray tmp43=getBccList((char)0x43,(char)0x04, tm);
    if((r = getRespone(dev_handle, tmp43)) < 0 ) {
        return false;
    }
    //0x5f 匹配旧密码
    char m1[] =  {
        AUTHENTICATION_MODE, ADDRESS[0]/*, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff*/
    };//旧密码
    QByteArray tmp5f = getBccList((char)0x5f, (char)0x08, QByteArray(m1, sizeof(m1)) + _DefPwd);
    if((r = getRespone(dev_handle, tmp5f)) < 0 ) {
        otherError = libusb_error_name(r);
        return false;
    }
    //    //0x4c 改变存放于ASIC 接口内部RAM 中的授权密码
    //    char m[] = {
    //        LOADKEY_MODE,SECNR,TKEY[0],TKEY[1],TKEY[2],
    //        TKEY[3],TKEY[4],TKEY[5],TKEY[6],TKEY[7]
    //    };

    //    QByteArray tmp4c=getBccList((char)0x4c,(char)0x12, QByteArray(m, sizeof(m)) + _DefPwd);
    //    if((r = getRespone(dev_handle, tmp4c)) < 0 ) {
    //        return false;
    //    }

    //() << tmp4c.length();

    //    //0x44 相互确认
    //    QByteArray tmp44=getBccList((char)0x44,(char)0x02,QByteArray().append(AUTHENTICATION_MODE).append(SECNR));//new byte[]{authentication_mode,secnr});
    //    if((r = getRespone(dev_handle, tmp44)) < 0 ) {
    //        return false;
    //    }
    //() << "相互确认";
    if(indexof_simpleError() == 0 )  return true;
    //() << "相互确认";
#endif
    return false;
}
//getRespone_2
// 0 21 255
//getRespone_3
// 1 21 8              1 46 8
//26                  26
//getRespone
// 1 22                1 47
//getRespone_2
// 1 22 255            1 47 255
//getRespone_3
// 1 22 8              0 47 8
//相互确认
bool Reader_RF::beep()
{
#ifdef Q_OS_UNIX

    //设备未打开
    if(!isopen)return false;
    QByteArray array = getBccList((char)0x36, (char)0x02, QByteArray().append((char)BEEPTIME).append((char)0x00));
    int r=libusb_control_transfer(dev_handle,
                                  LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                  LIBUSB_REQUEST_SET_CONFIGURATION, 0x306, 0x0, (unsigned char *)array.data(), 0xff, 1000);
    if(r < 0) {
        otherError = libusb_error_name(r);
        return false;
    }
#endif
    return true;
}

void Reader_RF::setsection(char section)
{
    SECNR=section;
}

QString Reader_RF::lastError()
{    
    QString err;
#ifdef Q_OS_UNIX
    int s = QByteArray().append(byte2[3]).toHex().toInt();
    if(s >= 0 && s < read_card_error.size())  err =  read_card_error.value(s);
#endif

    return otherError.isEmpty() ? err : otherError;
}
#ifdef Q_OS_UNIX

int Reader_RF::getRespone(libusb_device_handle *connection, const QByteArray &tmp)
{
    int r;
    r = libusb_control_transfer(connection,
                                LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                LIBUSB_REQUEST_SET_CONFIGURATION,
                                0x306,
                                0x00, (unsigned char *)tmp.data(), 0xff, 1000);
    if(r < 0) {
        otherError = libusb_error_name(r);
    }

    //读卡器读数据时,前几次可能读到的是空的数据,这里循环30次,保证由于没有及时读到数据而导致的读卡失败
    int loopcount = 0;
    while( (r = libusb_control_transfer(connection,
                                        LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                        LIBUSB_REQUEST_CLEAR_FEATURE,
                                        0x306,
                                        0x00,
                                        (unsigned char *)byte2.data(),
                                        0xff,
                                        1000)) < 0 ) {
        //        qDebug() << libusb_error_name(r) << loopcount;

        loopcount++;
        if(loopcount >= 3) break;
    }
    return r;
}

QByteArray Reader_RF::getBccList(char fuction, char length, const QByteArray &original)
{
    QByteArray req;

    char x = (char)0x00;
    req.append((char)0x06);//0
    req.append((char)0x02);
    req.append((char)sqr);//2
    req.append((char)fuction);
    req.append((char)length);//4
    if(!original.isEmpty()) {
        for(int i=0;i < original.length() && i < length; i++) {
            req.append((char)original[i]);//4 + i
        }
        for(int j=original.length(); j < length; j++) {
            req.append((char)0x00);
        }
    }

    for(int k=2;k<length+5;k++) {
        x=(char) (x^req[k]);
    }

    x=(char) ((x+replenish[x&0x0f]) & 0xff);
    req.append((char)x);
    req.append((char)0x10);
    req.append((char)0x03);
    sqr++;
    sqr = sqr % 256;

    return req;
}

QByteArray Reader_RF::getBccWriteList(char fuction, char length, char area, const QByteArray &original)
{
    QByteArray req;
#ifdef Q_OS_UNIX

    char x = (char)0x00;
    req.append((char)0x06);//0
    req.append((char)0x02);
    req.append((char)sqr);//2
    req.append((char)fuction);
    req.append((char)length);//4
    req.append((char)area);//4

    for(int i=0;i < original.length() && i < length; i++) {
        req.append((char)original[i]);//4 + i
    }
    for(int j=original.length(); j < length - 1; j++) {
        req.append((char)0x00);
    }
    for(int k=2;k<length+5;k++) {
        x=(char) (x^req[k]);
    }
    x=(char) ((x+replenish[x&0xf])&0xff);
    req.append(char(x));
    req.append(char(0x10));
    req.append(char(0x03));
    sqr++;
    sqr = sqr % 256;
#endif

    return req;
}
//    Number of possible configurations:  1
//    Device Class:  0
//    VendorID:  "0x483"
//    ProductID:  "0x811e"
//    Interfaces:  1  |||
//    Number of alternate settings:  1  |
//    Interface Number:  0  |
//    Number of endpoints:  2  |
//    Descriptor Type:  5  |
//    EP Address:  129  |
//    Descriptor Type:  5  |
//    EP Address:  2  |

//find device info : 18 1 "0x200" 9 0 1 "0x8087" 64 "0x24" 0 0 0 0 1
//find device info : 18 1 "0x200" 9 0 1 "0x8087" 64 "0x24" 0 0 0 0 1
//find device info : 18 1 "0x200" 9 0 0 "0x1d6b" 64 "0x2" 787 3 2 1 1
//find device info : 18 1 "0x200" 9 0 0 "0x1d6b" 64 "0x2" 787 3 2 1 1
//find device info : 18 1 "0x210" 0 0 0 "0x3f0" 64 "0x3940" 4213 1 2 3 1
//find device info : 18 1 "0x110" 0 0 0 "0x2188" 8 "0xae1" 256 0 1 0 1


//find device info : 18 1 "0x200" 9 0 1 "0x8087" 64 "0x24" 0 0 0 0 1
//find device info : 18 1 "0x200" 9 0 1 "0x8087" 64 "0x24" 0 0 0 0 1
//find device info : 18 1 "0x200" 9 0 0 "0x1d6b" 64 "0x2" 787 3 2 1 1
//find device info : 18 1 "0x200" 9 0 0 "0x1d6b" 64 "0x2" 787 3 2 1 1
//find device info : 18 1 "0x200" 0 0 0 "0x483" 64 "0x811e" 256 0 0 0 1
//find device info : 18 1 "0x200" 0 0 0 "0x2a63" 64 "0x1" 256 1 2 3 1             //小票打印机
//find device info : 18 1 "0x210" 0 0 0 "0x3f0" 64 "0x3940" 4213 1 2 3 1       //postek打印机
//find device info : 18 1 "0x110" 0 0 0 "0x2188" 8 "0xae1" 256 0 1 0 1

//find device info : 18 1 "0x200" 9 0 1 "0x8087" 64 "0x24" 0 0 0 0 1
//find device info : 18 1 "0x200" 9 0 1 "0x8087" 64 "0x24" 0 0 0 0 1
//find device info : 18 1 "0x200" 9 0 0 "0x1d6b" 64 "0x2" 787 3 2 1 1
//find device info : 18 1 "0x200" 9 0 0 "0x1d6b" 64 "0x2" 787 3 2 1 1
//find device info : 18 1 "0x200" 0 0 0 "0x483" 64 "0x811e" 256 0 0 0 1
//find device info : 18 1 "0x200" 0 0 0 "0x483" 64 "0x811e" 256 0 0 0 1
//find device info : 18 1 "0x210" 0 0 0 "0x3f0" 64 "0x3940" 4213 1 2 3 1
//find device info : 18 1 "0x110" 0 0 0 "0x2188" 8 "0xae1" 256 0 1 0 1


void Reader_RF::print_devs(libusb_device **devs)
{
    ssize_t cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt >=0) {
        for(ssize_t i = 0; i < cnt; i++) {
            libusb_device_descriptor desc;
            int r = libusb_get_device_descriptor(devs[i], &desc);
            if (r < 0) {
                //() <<"failed to get device descriptor";
            } else {
                //                qDebug() << "find device info :"

                //                         << desc.bLength
                //                         << desc.bDescriptorType
                //                         << QString().sprintf("0x%x", desc.bcdUSB)
                //                         << desc.bDeviceClass
                //                         << desc.bDeviceSubClass
                //                         << desc.bDeviceProtocol
                //                         << QString().sprintf("0x%x", desc.idVendor)
                //                         << desc.bMaxPacketSize0
                //                         << QString().sprintf("0x%x", desc.idProduct)
                //                         << desc.bcdDevice
                //                         << desc.iManufacturer
                //                         << desc.iProduct
                //                         << desc.iSerialNumber
                //                         << desc.bNumConfigurations

                //                            ;
            }
            printdev(devs[i]);
        }
    }
}
void Reader_RF::printdev(libusb_device *dev) {
    libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        //() <<"failed to get device descriptor"<<endl;
        return;
    }
    //() <<"Number of possible configurations: "<<(int)desc.bNumConfigurations<<"  ";
    //() <<"Device Class: "<<(int)desc.bDeviceClass<<"  ";
    //() <<"VendorID: "<<QString().sprintf("0x%x", desc.idVendor);
    //() <<"ProductID: "<<QString().sprintf("0x%x", desc.idProduct);
    libusb_config_descriptor *config;
    libusb_get_config_descriptor(dev, 0, &config);
    //() <<"Interfaces: "<<(int)config->bNumInterfaces<<" ||| ";
    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    //    const libusb_endpoint_descriptor *epdesc;
    for(int i=0; i<(int)config->bNumInterfaces; i++) {
        inter = &config->interface[i];
        //() <<"Number of alternate settings: "<<inter->num_altsetting<<" | ";
        for(int j=0; j<inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];
            //() <<"Interface Number: "<<(int)interdesc->bInterfaceNumber<<" | ";
            //() <<"Number of endpoints: "<<(int)interdesc->bNumEndpoints<<" | ";
            for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
                //                epdesc = &interdesc->endpoint[k];
                //() <<"Descriptor Type: "<<(int)epdesc->bDescriptorType<<" | ";
                //() <<"EP Address: "<<(int)epdesc->bEndpointAddress<<" | ";
            }
        }
    }
    //() <<endl<<endl<<endl;
    libusb_free_config_descriptor(config);
}

QString Reader_RF::simpleError() const
{
    int s = QByteArray().append(byte2[3]).toHex().toInt();
    if(s >= 0 && s < read_card_error.size())    return read_card_error.value(s);
    return "unknow error";
}

int Reader_RF::indexof_simpleError() const
{
    int s = byte2[3];
    if(s >= 0 && s < read_card_error.size()) return s;
    return -1;
}

#endif

