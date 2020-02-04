#include "electroniccountprice_resolve.h"
#include "printer_0.h"
#include "qt_retail_scale_scpos_keyboard_thread.h"

static QByteArray lastBetween(const QByteArray &val, const QByteArray &begin, const QByteArray &end)
{
    int be = val.lastIndexOf(begin);
    int en = val.lastIndexOf(end);
    return val.mid(be, en-be+end.count());
}

static int StrHexToDecimal(QString strHex)
{
    bool ok;
    return strHex.toInt(&ok,16);
}

ElectronicCountPrice_resolve::ElectronicCountPrice_resolve(QObject *parent) :
    QObject(parent),
    timeout(100),
    timeoutRunning(false)
{
}

bool ElectronicCountPrice_resolve::scaleOpen(const QString &type_name, const QString &path, int baudRate, QString *errstring)
{
    this->type_name = type_name;
    open(type_name, &printer, path, baudRate);

    if(false == printer.isOpen()) {
        if(errstring)  {
            *errstring = printer.lastError();
        }
        return false;
    }
    qDebug() << __FILE__ << __LINE__;

    if(false == timeoutRunning) {
        qDebug() << __FILE__ << __LINE__;
        timeoutRunning = true;
        QTimer::singleShot(timeout, this, SLOT(toResolveData()));
    }
    qDebug() << __FILE__ << __LINE__;
    return true;
}

void ElectronicCountPrice_resolve::setTimeOut(int sec)
{
    timeout = sec;
}

void ElectronicCountPrice_resolve::toResolveData()
{
    ElectronicCountPrice_resolve::resolveData qupi;
    ElectronicCountPrice_resolve::resolveData lingwei;
    ElectronicCountPrice_resolve::resolveData scale_weight;
    ElectronicCountPrice_resolve::resolveData scale_tare;

    if(printer.bytesAvailable()) {
        resolve(type_name, printer.readAll(), qupi, lingwei, scale_weight, scale_tare);

        if(scale_weight.isValid) {
            emit weighComming(QString::number(scale_weight.doubleValue));
            emit weighComming(scale_weight.doubleValue);
        }

        if(scale_tare.isValid) {
            emit tareComing(QString::number(scale_tare.doubleValue));
            emit tareComing(scale_tare.doubleValue);
        }

        if(qupi.isValid)
            emit qupiComming(qupi.boolValud);

        if(lingwei.isValid)
            emit lingweiComming(lingwei.boolValud);
    }

    printer.writedirect(ins_quzhong(type_name));
    QTimer::singleShot(timeout, this, SLOT(toResolveData()));
}

void ElectronicCountPrice_resolve::qupi()
{
    printer.writedirect(ins_qupi(type_name));
}

void ElectronicCountPrice_resolve::zhiling()
{
    printer.writedirect(ins_zhiling(type_name));
}

QString ElectronicCountPrice_resolve::jieshi(uchar a)
{
    QString ret;
    switch(a) {
    case 0xee:
        ret="0";
        break;
    case 0xfe:
        ret="0.";
        break;
    case 0x0a:
        ret="1";
        break;
    case 0x1a:
        ret="1.";
        break;
    case 0x67:
        ret="2";
        break;
    case 0x77:
        ret="2.";
        break;
    case 0x4f:
        ret="3";
        break;
    case 0x5f:
        ret="3.";
        break;
    case 0x8b:
        ret="4";
        break;
    case 0x9b:
        ret="4.";
        break;
    case 0xcd:
        ret="5";
        break;
    case 0xdd:
        ret="5.";
        break;
    case 0xed:
        ret="6";
        break;
    case 0xfd:
        ret="6.";
        break;
    case 0x0e:
        ret="7";
        break;
    case 0x1e:
        ret="7.";
        break;
    case 0xef:
        ret="8";
        break;
    case 0xff:
        ret="8.";
        break;
    case 0xcf:
        ret="9";
        break;
    case 0xdf:
        ret="9.";
        break;
    case 0xaf:
        ret="A";
        break;
    case 0xe9:
        ret="B";
        break;
    case 0xe4:
        ret="C";
        break;
    case 0x6b:
        ret="D";
        break;
    case 0xe5:
        ret="E";
        break;
    case 0xa5:
        ret="F";
        break;
    case 0x68:
        ret="U";
        break;
    case 0x69:
        ret="O";
        break;
    case 0x01:
        ret="-";
        break;
    case 0x11:
        ret="-.";
        break;
    case 0x29:
        ret="N";
        break;
    case 0xe0:
        ret="L";
        break;
    case 0x41:
        ret="=";
        break;
    case 0x00:
        ret=" ";
        break;
    case 0x10:
        ret=" .";
        break;
    default:
        ret="X";
        break;
    }
    return ret;
}


void ElectronicCountPrice_resolve::resolve(const QString &type_name, const QByteArray &scale_data, ElectronicCountPrice_resolve::resolveData &qupi, ElectronicCountPrice_resolve::resolveData &lingwei, ElectronicCountPrice_resolve::resolveData &scale_weight, ElectronicCountPrice_resolve::resolveData &scale_tare)
{
    qDebug() << "scale_data" << scale_data;
    qupi.isValid = false;
    lingwei.isValid = false;
    scale_weight.isValid = false;
    scale_tare.isValid = false;

    switch(creat_pricecount_scaletypes().indexOf(type_name)) {
    case 0://凯丰
        scale_weight.doubleValue = scale_data.toDouble(&scale_weight.isValid) / 1000.0;
        break;
    case 1://凯士
        //        没有负数，回读无效
    {
        int index = scale_data.indexOf("\x0a\x0d");
        if(scale_data.mid(index).size() >= 2/*0a0d*/ + 5/*净重*/) {// + 2 + 5/*单价*/ + 2 + 6/*总价*/) {
            scale_weight.doubleValue = scale_data.mid(index + 2, 5).trimmed().toDouble(&scale_weight.isValid) / 1000.0;
        }
    }
        break;
    case 2://KF-PC秤
    {
        // Sleep(100);//延时100毫秒保证接收到的是一条完整的数据,而不是脱节的
        QByteArray temp=lastBetween(scale_data, QByteArray().append(0x04), QByteArray().append(0x28));
        if(temp.size() >= 24) {
            //        }
            //        QList<QByteArray> newdatalist =  scale_data.split(0x0d);

            //        if (temp.size()>=24) {
            //            if (temp[0]==0x04 && temp[1]==0x02 && temp[2]==0x08 && temp[3]==0x20 && temp[20]==0x06&& temp[21]==0x82&& temp[22]==0x48&& temp[23]==0x28 )

            {
                QString s;
                QString zhongliang;
                QString danjia;
                QString heji;
                for(int i = 3; i < 20; i++) {
                    s.sprintf("%02x", (unsigned char)temp.at(i));
                    switch(i) {
                    case 4:
                        zhongliang=jieshi(StrHexToDecimal(s));//strHex.toInt(&ok,16);
                        if (zhongliang.indexOf(".")>0)
                        {
                            zhongliang=zhongliang.left(1);

                            qupi.isValid = true;
                            qupi.boolValud = true;
                        }
                        else
                        {
                            qupi.isValid = true;
                            qupi.boolValud = false;
                        }
                        break;
                    case 5:

                        zhongliang+=jieshi(StrHexToDecimal(s));
                        break;
                    case 6:

                        zhongliang+=jieshi(StrHexToDecimal(s));
                        break;
                    case 7:

                        zhongliang+=jieshi(StrHexToDecimal(s));
                        break;
                    case 8:
                        if (jieshi(StrHexToDecimal(s)).indexOf(".")>0)
                        {
                            lingwei.isValid = true;
                            lingwei.boolValud = true;
                            zhongliang+=jieshi(StrHexToDecimal(s)).left(1);
                        }
                        else
                        {
                            lingwei.isValid = true;
                            lingwei.boolValud = false;
                            zhongliang+=jieshi(StrHexToDecimal(s));
                        }
                        scale_weight.doubleValue = zhongliang.toDouble(&scale_weight.isValid);
                        break;

                    default:
                        break;
                    }
                }
            }
        }
    }
        break;
    case 3://顶尖PBX
    {
        int index0, index1;//head1 索引， head2索引
        char head1,head2,flag1,sign,checksum,tail1,tail2,flag2;
        QByteArray weight,unit;
        head1=0x01;
        head2=0x02;
        tail1=0x03;
        tail2=0x04;
        if((index0=scale_data.indexOf("\x01\x02"))) {//QByteArray()+head1+head2))>-1) {//0x0102固定为0x0102 开头
            if((index1=scale_data.indexOf("\x03\x04"))) {//QByteArray()+tail1+tail2, index0))>-1) {//0x0304固定为0x0304 结束
                QByteArray inter =scale_data.mid(index0, index1-index0 + 3);
                if(inter.count()==16) {
                    flag1=inter[2];
                    sign=inter[3];
                    weight=QByteArray()+inter[4]+inter[5]+inter[6]+inter[7]+inter[8]+inter[9];
                    unit=QByteArray()+inter[10] + inter[11];
                    checksum=inter[12];
                    flag2=inter[15];
                    if(flag1=='F') {
                        //重量溢出，或没开机
                        //qDebug() << "重量溢出，或没开机";
                    } else if(flag1=='U') {
                        //重量不稳定
                        //qDebug() << "重量不稳定";
                    } else if(flag1=='S') {
                        //重量稳定
                        //qDebug() << "重量稳定";
                    }
                    if(flag2&0x10) {//0001 1100
                        //【1重量为零，重量非零】
                        //qDebug() << "重量为零";
                    } else {
                        //qDebug() << "重量非零";
                    }
                    if(flag2&0x08) {
                        //【1去皮模式，非去皮模式】
                        //qDebug() << "去皮模式";
                    }else {
                        //qDebug() << "非去皮模式";
                    }
                    if(flag2&0x04) {
                        //【1重量溢出或没开机，0重量正常或已经开机】
                        //qDebug() << "重量溢出或没开机";
                    }else {
                        //qDebug() << "重量正常或已经开机";
                    }
                    if(printer_0::_2xor(inter.left(12))!=checksum) {
                        //奇偶校验失败
                        //qDebug() << "奇偶校验失败";
                    }
                    scale_weight.doubleValue = weight.toDouble(&scale_weight.isValid);
                    if(sign == '-')
                        scale_weight.doubleValue = -1 * scale_weight.doubleValue;
                }
            }
        }
    }
        break;
    case 4://大华ASC-A
    {
        int index0, index1;
        if((index0=scale_data.indexOf("\x02\x20\x20"))>-1) {// 开头
            if((index1=scale_data.indexOf("\x20\x4b\x47", index0))>-1) {// 结束
                QByteArray inter =scale_data.mid(index0, index1-index0);
                inter=inter.mid(3);
                if(inter.count()==5) {
                    scale_weight.doubleValue = inter.toDouble(&scale_weight.isValid);
                }
            }
        }
    }
        break;
    case 5://顶尖PS1X
    {
        //0.105
        QList<QByteArray> newdatalist =  scale_data.split('\n');
        QByteArray newdata;
        for(int end = newdatalist.count() - 1; end >=0; end--) {
            newdata = newdatalist[end].trimmed();
            if(newdata.toHex().startsWith("02")) {
                if(newdata.length() == 7) {
                    scale_weight.doubleValue = scale_data.mid(1, 5).toInt(&scale_weight.isValid) / 1000.0;
                    if(scale_weight.isValid) {
                        break;
                    }
                }
            }
        }
    }
        break;
    case 6://风华电子称
    {
        //0 1 2     3 4 5
        if(scale_data.length()==6) {
            QByteArray num=scale_data[5] + scale_data.mid(0, 5);
            scale_weight.doubleValue = num.toInt(&scale_weight.isValid) / 1000.0;
        }
    }
        break;
    case 7://友中衡计价秤
    {
        //c4 0xc0 + 0x04
        QList<QByteArray> newdatalist =  scale_data.split(0x0d);
        QByteArray newdata;
        int end = newdatalist.count() - 1;
        for(; end >=0; end--) {
            newdata = newdatalist[end];
            if(newdata.count() >= 10) {
                scale_weight.doubleValue = newdata.right(10).left(8).replace(' ',"").toDouble(&scale_weight.isValid);
                if(scale_weight.isValid) {
                    break;
                }
            }
        }
    }
        break;
    case 8://友中衡加强版
    {
        //c4 0xc0 + 0x04
        QList<QByteArray> newdatalist =  scale_data.split(0x0d);
        QByteArray newdata;
        int end = newdatalist.count() - 1;
        for(; end >=0; end--) {
            newdata = newdatalist[end];
            if(newdata.count() >= 10) {
                scale_weight.doubleValue = newdata.right(10).left(8).replace(' ',"").toDouble(&scale_weight.isValid);
                QByteArray state = QByteArray::fromHex(newdata.right(12).left(2));
                if(scale_weight.isValid) {
                    if(state.count() == 1) {
                        lingwei.isValid = true;
                        lingwei.boolValud = (state[0]&0x01) > 0;

                        qupi.isValid = true;
                        qupi.boolValud = (state[0]&0x02) > 0;
                    }
                    break;
                }
            }
        }
    }
        break;
    case 9://建宇-DAHUA
    {
        int index = scale_data.indexOf("\x0a\x0d");
        if(scale_data.mid(index).size() >= 2/*0a0d*/ + 5/*净重*/) {// + 2 + 5/*单价*/ + 2 + 6/*总价*/) {
            scale_weight.doubleValue = scale_data.mid(index + 2, 5).trimmed().toDouble(&scale_weight.isValid) / 1000.0;
        }
    }
        break;
    case 10://建宇-YAOHUA
    {
        int index0 = scale_data.indexOf("=") + 1;
        if(scale_data.mid(index0).length() >= 8) {
            QByteArray d = scale_data.mid(index0, 8);
            QByteArray d0;
            foreach(const char &c, d) {
                d0.prepend(c);
            }
            scale_weight.doubleValue = d0.toDouble(&scale_weight.isValid);

            //            info += "yaohua" + line.mid(index0, 8);
            //            info += "\n";
            //812.0000  = 0.218
            //没有负数
            //            qDebug() << line.mid(index0, 8);
            //0.00200- == -200.0kg
        }
    }
        break;
    case 11://建宇-TSCALE
    {
        //cRLF 0d0a
        int index0 = scale_data.indexOf("ST,") + 3;
        int index1 = scale_data.indexOf("\x0d\x0a", index0);
        //        ui->lineEdit_jijia_readback->setText(QString::number(index0) + ":" + QString::number(index1));
        if(index1 - index0 == 3/*NT,/GS,*/ + 1/*+-*/+7/*weight*/+3/*,kg*/) {
            scale_weight.doubleValue = scale_data.mid(index0 + 3, 8).replace(" ", "").toDouble(&scale_weight.isValid);
            //            info += "tscale" + scale_data.mid(index0,2) + scale_data.mid(index0 + 3, 8);
            //            info += "\n";
            //读取到的数据
            //__1.345
            //-_1.345
        }
    }
        break;
    case 12://建宇-G-310
    {
        //cRLF 0d0a
        int index0 = scale_data.indexOf("\x0d") + 1;
        if(scale_data.mid(index0).length() >= 7) {
            scale_weight.doubleValue = scale_data.mid(index0, 7).trimmed().toDouble(&scale_weight.isValid);

            //            info += "G-310" + scale_data.mid(index0, 7);
            //            info += "\n";
            //            qDebug() << scale_data.mid(index0, 7);
            //读不到负数:UF
            //__0.125
            //www.www
        }
    }
        break;
    case 13://建宇-STE-1
    {
        //cRLF 0d0a
        int index0 = scale_data.indexOf("N") + 1;
        int index1 = scale_data.indexOf("\x0d\x0a", index0);
        //        ui->lineEdit_jijia_readback->setText(QString::number(index0) + ":" + QString::number(index1));
        if(index1 - index0 == 7/*净重*/+1/*T*/+7/*皮重*/) {
            scale_weight.doubleValue = scale_data.mid(index0, 7).trimmed().toDouble(&scale_weight.isValid);

            //            info += "STE-1" + scale_data.mid(index0, 16);
            //            info += "\n";
            //没有负数
            //__1.256
            //            qDebug() << scale_data.mid(index0+1, 7);
            //www.www
            //            qDebug() << scale_data.mid(index0+1+7+1, 7);
            //www.www
        }
    }
        break;
    case 14://acs-jc 191
    {
        int k0 = scale_data.indexOf('<');
        int k1 = scale_data.indexOf('>', k0);
        if(k0 >= 0 && k1 >= 0) {
            QString w = scale_data.mid(k0 + 1, k1 - k0 - 1/*<*/ - 1/*C*/);
            int index = 0;
#ifdef QT_DEBUG
            //            index = 2;
#endif
            QString weight = w.mid(index, 7);index += 7;//毛重
            index += 1;//空格
            QString tare = w.mid(index, 7);index += 7;//皮重
            index += 1;//空格
            QString net = w.mid(index, 7);index += 7;//净重
            index += 1;//空格
            QString flag_weight = w.mid(index, 1);index += 1;//毛重是否为负
            index += 1;//空格
            QString flag_net = w.mid(index, 1);index += 1;//净重是否为负
            index += 1;//空格
            QString flag_overload = w.mid(index, 1);index += 1;//是否超载
            index += 1;//空格
            QString flag_zero = w.mid(index, 1);index += 1;//是否零位
            index += 1;//空格
            QString flag_tare = w.mid(index, 1);index += 1;//是否去皮
            index += 1;//空格
            QString flag_stable = w.mid(index, 1);index += 1;//是否稳定

            scale_weight.doubleValue = net.toDouble(&scale_weight.isValid) / 1000.0;
            if(flag_net == "1")
                scale_weight.doubleValue = -1 * scale_weight.doubleValue;//负数

            //皮重
            scale_tare.doubleValue = tare.toDouble(&scale_tare.isValid) / 1000.0;
            qDebug() << "scale_tare" << tare << scale_tare.doubleValue;

            //零位
            lingwei.isValid = true;
            lingwei.boolValud = (flag_zero == "1");

            //去皮
            qupi.isValid = true;
            qupi.boolValud = (flag_tare == "1");


            if(flag_overload == "1") {//超载
                scale_weight.isValid = false;
            }
        }
    }
        break;
    case 15://顶尖NS1X
    {
        int index0, index1;//head1 索引， head2索引
        char flag1,sign,checksum;
        QByteArray weight,unit;
        if( (index0 = scale_data.indexOf("\x01\x02")) > -1) {//0x0102固定为0x0102 开头
            if( (index1 = scale_data.indexOf("\x03\x04", index0)) > -1) {//0x0304固定为0x0304 结束
                QByteArray inter =scale_data.mid(index0, index1-index0);
                if(inter.count() == 13) {
                    flag1=inter[2];
                    sign=inter[3];
                    weight=QByteArray()+inter[4]+inter[5]+ "." +inter[7]+inter[8]+inter[9];//inter[5] 是逗号
                    unit=QByteArray()+inter[10] + inter[11];
                    checksum=inter[12];

                    scale_weight.doubleValue = weight.toDouble(&scale_weight.isValid);
                    if(sign == '-')
                        scale_weight.doubleValue = -1 * scale_weight.doubleValue;
                }
            }
        }
    }
        break;

    case 16://友声电子秤-C
    {
        int index0 = scale_data.indexOf("WT") + 2;
        int index1 = scale_data.indexOf("\x0d\x0a", index0);
        if(index1 - index0 == 14) {
            QByteArray d = scale_data.mid(index0, 16);
            QByteArray flag = d.left(2);
            QByteArray content = d.mid(2, 10);
            QByteArray kg = d.mid(12, 2);

            content.replace(" ", "");
            scale_weight.doubleValue = content.toDouble(&scale_weight.isValid);
        }
    }
        break;

    case 17://商超键盘电子秤
    {
        QString key;
        int begin = scale_data.indexOf("\x0d\x0a") - 18;
        if(begin >= 0) {
            qt_retail_scale_scpos_keyboard_event::reslove(scale_data.mid(begin, 20), qupi, lingwei, scale_weight, key);
        }
    }
        break;
    }//switch
}

void ElectronicCountPrice_resolve::open(const QString &type_name, Printer_POS_SERIAL *printer, const QString &path, int baudRate)
{
    switch(creat_pricecount_scaletypes().indexOf(type_name)) {
    case 0://凯丰
        break;
    case 1://凯士
        break;
    case 2://KF-PC秤
        break;
    case 3://顶尖PBX
        break;
    case 4://大华ASC-A
        break;
    case 5://顶尖PS1X
        printer->psettings.DataBits=DATA_7;
        printer->psettings.Parity=PAR_EVEN;
        printer->psettings.StopBits=STOP_1;
        break;
    case 6://风华电子称
        break;
    case 7://友中衡计价秤
    case 8://友中衡加强版
        break;
        //建宇
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        break;
    case 14://acs-jc 191
        break;
    case 15://顶尖NS1X
        break;
    case 16://友声电子秤-C
        break;
    case 17://商超键盘电子秤
        break;
    }
    printer->tryOpen(path);
    printer->setBaudRate(baudRate);
}

QByteArray ElectronicCountPrice_resolve::ins_zhiling(const QString &type_name)
{
    QByteArray retv;
    switch(creat_pricecount_scaletypes().indexOf(type_name)) {
    case 0: //凯丰
        break;
    case 1://凯士PR PLUS
        break;
    case 2://KF-PC秤
        retv = QByteArray::fromHex("8D");
        break;
    case 3://顶尖PBX
        break;
    case 4://大华ASC-A
        break;
    case 5://顶尖PS1X
        break;
    case 6://风华电子称
        retv = QByteArray::fromHex("59");
        break;
    case 7://友中衡计价秤
    case 8://友中衡加强版
        retv = QByteArray::fromHex("0125FF0D");
        break;
    case 14://acs-jc 191
        retv= QByteArray("<ZER >");
        break;
    case 15://顶尖NS1X
        break;
    case 16://友声电子秤-C
        break;
    case 17://商超键盘电子秤
        retv = QByteArray::fromHex("540d0a");
        break;
    }
    qDebug() << __FILE__ << __LINE__ << retv;
    return retv;
}

QByteArray ElectronicCountPrice_resolve::ins_qupi(const QString &type_name)
{
    QByteArray retv;
    switch(creat_pricecount_scaletypes().indexOf(type_name)) {
    case 0: //凯丰
        break;
    case 1://凯士PR PLUS
        break;
    case 2://KF-PC秤
        retv = QByteArray::fromHex("8C");
        break;
    case 3://顶尖PBX
        break;
    case 4://大华ASC-A
        break;
    case 5://顶尖PS1X
        break;
    case 6://风华电子称
        retv = QByteArray::fromHex("5A");
        break;
    case 7://友中衡计价秤
    case 8://友中衡加强版
        retv = QByteArray::fromHex("0125EF0D");
        break;
    case 14://acs-jc 191
        retv = QByteArray("<TAR >");
        break;
    case 15://顶尖NS1X
        break;
    case 16://友声电子秤-C
        break;
    case 17://商超键盘电子秤
        retv = QByteArray::fromHex("5a0d0a");
        break;
    }

    qDebug() << __FILE__ << __LINE__ << retv;
    return retv;
}

QByteArray ElectronicCountPrice_resolve::ins_quzhong(const QString &type_name)
{
    QByteArray retv;
    switch(creat_pricecount_scaletypes().indexOf(type_name)) {
    case 0://凯丰
        retv = QByteArray("S");
        break;
    case 1://凯士
        break;
    case 2://KF-PC秤
        break;
    case 3://顶尖PBX
        retv = QByteArray("!0R");
        break;
    case 4://大华ASC-A
        retv = QByteArray("P");
        break;
    case 5://顶尖PS1X
        retv = QByteArray("W");
        break;
    case 6://风华电子称
        retv = QByteArray("S");
        break;
    case 7://友中衡计价秤
    case 8://友中衡加强版
        retv = QByteArray::fromHex("0157300D");
        break;
        //建宇
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        break;
    case 14://acs-jc 191
        retv = QByteArray("<WEI >");
        break;
    case 15://顶尖NS1X
        break;
    case 16://友声电子秤-C
        break;
    case 17://商超键盘电子秤
        break;
    }
    return retv;
}

QStringList ElectronicCountPrice_resolve::creat_pricecount_scaletypes()
{
    static QStringList pricecout_scaletypes;
    if(pricecout_scaletypes.isEmpty()) {
        pricecout_scaletypes
                << QObject::tr("凯丰(商超专用)")
                << QObject::tr("凯士PR PLUS")
                << QObject::tr("KF-PC秤")
                << QObject::tr("顶尖PBX")
                << QObject::tr("大华ASC-A")

                << QObject::tr("顶尖PS1X")
                << QObject::tr("风华电子称")
                << QObject::tr("友中衡计价秤")
                << QObject::tr("友中衡加强版")
                << QObject::tr("建宇")+"-"+"DAHUA"

                << QObject::tr("建宇")+"-"+"YAOHUA"
                << QObject::tr("建宇")+"-"+"TSCALE"
                << QObject::tr("建宇")+"-"+"G-310"
                << QObject::tr("建宇")+"-"+"STE-1"
                << "acs-jc 191"

                << QObject::tr("顶尖NS1X")
                << QObject::tr("友声电子秤-C")
                << QObject::tr("商超键盘电子秤")
                   ;
    }
    return pricecout_scaletypes;
}
