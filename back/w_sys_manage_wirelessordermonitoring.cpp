#include "w_sys_manage_wirelessordermonitoring.h"
#include "ui_w_sys_manage_wirelessordermonitoring_dialog.h"
#include <QStandardItemModel>
#include "qextserialenumerator.h"
#include "lds_tableview_delegate_com.h"
#include "n_func.h"
#include "lds_messagebox.h"
#include <QHostAddress>
#include "lds.h"
#include "lds_query_hddpos.h"
#include <QFile>
#include <QSqlError>
#include "n_func.h"
#include "public_sql.h"
#include <QProcess>
#include "public_logindialog.h"
#include <QDir>
#include "w_bt_dish.h"
#include "fexpandmain_2.h"
#include "public_printby_ch_billno.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_scr_dish_pay_widget.h"
#include "fexpandmain_2.h"



QString w_sys_manage_wirelessordermonitoring::checkdata(const QString &data)
{
    //!核实数据----begin
    QString tmpd;
    foreach(const QChar &c, data) {//去掉null
        if(!c.isNull()) {
            tmpd += c;
        }
    }
    //!核实数据----end
    //        return tmpd;
    return data;
}

void w_sys_manage_wirelessordermonitoring::initdata(QWidget *parent, const QString &dir)
{
    QFile file;
    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    QString info;
    int piecetotal;
    //!酒店名称
    {
        piecetotal = 0;
        file.setFileName(dir+_TXT_jdmc_txt);
        file.open(QFile::WriteOnly);
        file.write("");
        file.close();
        file.open(QFile::WriteOnly|QFile::Append);
        query.execSelect("select vch_company from cey_sys_company ");
        query.next();

        QString piecedata;
        piecedata += lds::strcutBygbnum(query.recordValue("vch_company").toString(), 20, true, ' ', Qt::AlignLeft);
        piecedata = checkdata(piecedata);

        file.write(printer_0::trans_UTF82GBK(piecedata + "\r\n"));
        piecetotal++;

        file.close();
        info+= "vch_company:"+QString::number(piecetotal)+"\n";
    }

    //!基站
    {
    }
    //!点菜机
    {
    }
    //!菜品表
    {
        QString vch_unitno;
        QString vch_unitno2;
        file.setFileName(dir+_TXT_cpb_txt);
        file.open(QFile::WriteOnly);
        file.write("");
        file.close();
        file.open(QFile::WriteOnly|QFile::Append);
        query.execSelect("select ch_dishno, ch_dish_typeno, vch_dishname, ch_unitno, vch_pur_unitno, vch_spell, num_price  from cey_bt_dish where length(ch_dishno) = 5 and ifnull(ch_stopflag, '') <> 'Y' order by ch_dishno ASC ");
        piecetotal = 0;
        while(query.next()) {
            //1单位
            query2.execSelect(QString("select vch_unitname from cey_bt_unit where ch_unitno = '%1' ")
                              .arg(query.recordValue("ch_unitno").toString()));
            query2.next();
            vch_unitno = query2.recordValue("vch_unitname").toString();

            //重量单位
            query2.execSelect(QString("select vch_unitname from cey_bt_unit where ch_unitno = '%1' ")
                              .arg(query.recordValue("vch_pur_unitno").toString()));
            query2.next();
            vch_unitno2 = query2.recordValue("vch_unitname").toString();
            //做法
            QString cook;
            query2.execSelect(QString("select ch_no from cey_bt_cp_dish where ch_dishno = '%1' ")
                              .arg(query.recordValue("ch_dishno").toString()));
            while(query2.next()) {
                cook += query2.recordValue("ch_no").toString();
            }


            QString piecedata;
            piecedata += lds::strcutBygbnum(query.recordValue("ch_dishno").toString(), 5, true, '0', Qt::AlignRight);
            piecedata += lds::strcutBygbnum(query.recordValue("ch_dish_typeno").toString(), 2, true, '0', Qt::AlignRight);
            piecedata += lds::strcutBygbnum(query.recordValue("vch_dishname").toString(), 20, true, ' ', Qt::AlignLeft);
            piecedata += lds::strcutBygbnum(QString().sprintf("%0.2f", query.recordValue("num_price").toFloat()), 9, true, ' ', Qt::AlignRight);
            piecedata += lds::strcutBygbnum(vch_unitno, 4, true, ' ', Qt::AlignLeft);

            piecedata += lds::strcutBygbnum(vch_unitno2, 4, true, ' ', Qt::AlignLeft);
            piecedata += lds::strcutBygbnum("0", 1, true, ' ', Qt::AlignLeft);
            piecedata += lds::strcutBygbnum(cook, 45, true, ' ', Qt::AlignLeft);
            piecedata += lds::strcutBygbnum(query.recordValue("vch_spell").toString(), 10, true, ' ', Qt::AlignLeft);

            piecedata = checkdata(piecedata);
            file.write(printer_0::trans_UTF82GBK(piecedata + "\r\n"));
            piecetotal++;
        }
        file.close();
        info+= "cey_bt_dish:"+QString::number(piecetotal)+"\n";
    }

    //!菜品类别表
    {
        file.setFileName(dir+_TXT_cplbb_txt);
        file.open(QFile::WriteOnly);
        file.write("");
        file.close();
        file.open(QFile::WriteOnly|QFile::Append);
        query.execSelect("select ch_dish_typeno,  vch_dish_typename from cey_bt_dish_type  order by ch_dish_typeno ASC ");
        piecetotal = 0;
        while(query.next()) {
            QString piecedata;
            piecedata += lds::strcutBygbnum(query.recordValue("ch_dish_typeno").toString(), 2, true, '0', Qt::AlignRight);
            piecedata += lds::strcutBygbnum(query.recordValue("vch_dish_typename").toString(), 10, true, ' ', Qt::AlignLeft);

            piecedata = checkdata(piecedata);
            file.write(printer_0::trans_UTF82GBK(piecedata + "\r\n"));
            piecetotal++;
        }
        file.close();
        info+= "cey_bt_dish_type:"+QString::number(piecetotal)+"\n";
    }

    //!菜品套餐
    {
    }

    //!荐菜品   //!推荐菜品内容表

    //!包房名称

    {
        file.setFileName(dir+_TXT_bfmcb_txt);
        file.open(QFile::WriteOnly);
        file.write("");
        file.close();
        file.open(QFile::WriteOnly|QFile::Append);
        query.execSelect("select ch_tableno, vch_tablename from cey_bt_table order by ch_tableno ASC ");
        piecetotal = 0;
        while(query.next()) {
            QString piecedata;
            piecedata += lds::strcutBygbnum(query.recordValue("ch_tableno").toString(), 4, true, '0', Qt::AlignRight);
            piecedata += lds::strcutBygbnum(query.recordValue("vch_tablename").toString(), 10, true, ' ', Qt::AlignLeft);

            piecedata = checkdata(piecedata);
            file.write(printer_0::trans_UTF82GBK(piecedata + "\r\n"));
            piecetotal++;
        }
        file.close();
        info+= "cey_bt_table:"+QString::number(piecetotal)+"\n";
    }

    //!退货理由表
    {
        file.setFileName(dir+_TXT_tclyb_txt);
        file.open(QFile::WriteOnly);
        file.write("");
        file.close();
        file.open(QFile::WriteOnly|QFile::Append);
        query.execSelect("select int_flowID, vch_reason from cey_bt_reason order by int_flowID ASC ");
        piecetotal = 0;
        while(query.next()) {
            QString piecedata;
            piecedata += lds::strcutBygbnum(query.recordValue("int_flowID").toString(), 2, true, '0', Qt::AlignRight);
            piecedata += lds::strcutBygbnum(query.recordValue("vch_reason").toString(), 20, true, ' ', Qt::AlignLeft);

            piecedata = checkdata(piecedata);
            file.write(printer_0::trans_UTF82GBK(piecedata + "\r\n"));
            piecetotal++;
        }
        file.close();
        info+= "cey_bt_reason:"+QString::number(piecetotal)+"\n";
    }

    //!客户要求表
    {
        file.setFileName(dir+_TXT_khyqb_txt);
        file.open(QFile::WriteOnly);
        file.write("");
        file.close();
        piecetotal = 0;
        file.open(QFile::WriteOnly|QFile::Append);
        query.execSelect("select ch_no, vch_cp_memo from cey_bt_cp_memo order by ch_no ASC ");
        while(query.next()) {
            QString piecedata;
            piecedata += lds::strcutBygbnum(query.recordValue("ch_no").toString(), 3, true, '0', Qt::AlignRight);
            piecedata += lds::strcutBygbnum(query.recordValue("vch_cp_memo").toString(), 20, true, ' ', Qt::AlignLeft);

            piecedata = checkdata(piecedata);
            file.write(printer_0::trans_UTF82GBK(piecedata + "\r\n"));
            piecetotal++;
        }
        file.close();
        info+= "cey_bt_cp_memo:"+QString::number(piecetotal)+"\n";
    }

    //!短信息编号
    if(0 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("初始化基础资料成功")+"\n"+info)) {

    }
}

//无线点菜监控
QString w_sys_manage_wirelessordermonitoring::dir;
w_sys_manage_wirelessordermonitoring::w_sys_manage_wirelessordermonitoring(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_wirelessordermonitoring_dialog)
{
    ui->setupUi(this);
    //1
    dir = qApp->applicationDirPath();
    standmodel = new QStandardItemModel(this);
    standmodel_2 = new QStandardItemModel(this);
    standmodel->setHorizontalHeaderLabels(QStringList()<<QObject::tr("基站号")<<QObject::tr("端口号"));
    standmodel_2->setHorizontalHeaderLabels(QStringList()<<QObject::tr("基站号")<<QObject::tr("点菜机号")<<QObject::tr("操作员"));
    ui->tableView->setModel(standmodel);
    ui->tableView_2->setModel(standmodel_2);
    //获取当前机器串口号
    QStandardItemModel *model = new QStandardItemModel(this);
    QList<QStandardItem *> items;
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    foreach(QextPortInfo port, ports) {
        items.clear();items << new QStandardItem(port.portName) << new QStandardItem(port.portName);model->appendRow(items);
    }

    lds_tableview_delegate_com *comdelegat = new lds_tableview_delegate_com(model, 1, ui->tableView);
    ui->tableView->setItemDelegate(comdelegat);

    lds_tableview_delegate_com *delegate_2 = new lds_tableview_delegate_com(standmodel,0, ui->tableView_2);
    ui->tableView_2->setItemDelegate(delegate_2);
    delegate_2->setonlyk(true);

    QDir d(dir+"/TXT");
    if(!d.exists()) {
        d.mkdir(d.path());
    }
    //初始化
    initjizhan();
    initdiancaiji();

    //2
    connect(ui->pushButton_del,SIGNAL(clicked()),this,SLOT(todel()));
    connect(ui->pushButton_del2,SIGNAL(clicked()),this,SLOT(todel2()));
    connect(ui->pushButton_new,SIGNAL(clicked()),this,SLOT(tonew()));
    connect(ui->pushButton_new2,SIGNAL(clicked()),this,SLOT(tonew2()));
    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_init,SIGNAL(clicked()),this,SLOT(toinit()));
    connect(ui->pushButton_save,SIGNAL(clicked()),this,SLOT(tosave()));
    connect(ui->pushButton_startjz,SIGNAL(clicked()),this,SLOT(tostartjz()));
    connect(ui->pushButton_close,SIGNAL(clicked()),this,SLOT(towxclose()));
    //3
    ui->checkBox->setChecked(lds::sysconf->value("w_sys_manage_wirelessordermonitoring/start", false).toBool());
    connect(ui->checkBox,SIGNAL(toggled(bool)),this,SLOT(tosavestart(bool)));

    //    ui->pushButton_startjz->hide();
    //    ui->pushButton_close->hide();
}

void w_sys_manage_wirelessordermonitoring::initjizhan()
{
    QFile file;
    file.setFileName(dir+_TXT_jz_txt);
    if(!file.open(QFile::ReadOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未找到基站文件,请先[新增基站]"));
        return;
    }

    while(!file.atEnd()) {
        QString readline = file.readLine();
        QStringList line = readline.trimmed().split(" ");
        standmodel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(line.value(0)) << new QStandardItem(line.value(1)));
    }
    file.close();
}

void w_sys_manage_wirelessordermonitoring::initdiancaiji()
{
    QFile file;
    file.setFileName(dir+_TXT_dcj_txt);
    if(!file.open(QFile::ReadOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未找到点菜机文件,请先[新增点菜机]"));
        return;
    }

    while(!file.atEnd()) {
        QString readline = file.readLine();
        QStringList line = readline.trimmed().split(" ");
        standmodel_2->appendRow(QList<QStandardItem*>()
                                << new QStandardItem(line.value(1)) << new QStandardItem(line.value(0)));
    }
    file.close();
}

bool w_sys_manage_wirelessordermonitoring::savejizhan()
{
    QString value;
    for(int row = 0, rowcount = standmodel->rowCount();row<rowcount;row++) {
        value = standmodel->item(row, 0)->text();
        if(value.length() != 1) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row)+QObject::tr("基站号只能为1位"));
            return false;
        }
        if(value.toInt() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row)+QObject::tr("基站号必须为数字且不能为0"));
            return false;
        }
    }
    QFile file;
    file.setFileName(dir+_TXT_jz_txt);
    if(!file.open(QFile::WriteOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }

    for(int row = 0, rowcount = standmodel->rowCount(); row < rowcount; row++) {
        QByteArray line;
        line += standmodel->data(standmodel->index(row, 0)).toString() + " ";
        line += standmodel->data(standmodel->index(row, 1)).toString() + "\r\n";
        file.write(line);

    }
    file.close();
    return true;
}

bool w_sys_manage_wirelessordermonitoring::savediancaiji()
{
    QString value;
    for(int row = 0, rowcount = standmodel_2->rowCount();row<rowcount;row++) {
        value = standmodel_2->item(row, 1)->text();
        if(value.length() != 3) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row+1)+QObject::tr("点菜机号只能为3位"));
            return false;
        }
        if(value.toInt() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QString("[%1]").arg(row+1)+QObject::tr("点菜机号必须为数字且不能为0"));
            return false;
        }
    }

    QFile file;
    file.setFileName(dir+_TXT_dcj_txt);
    if(!file.open(QFile::WriteOnly)) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, file.errorString());
        return false;
    }

    for(int row = 0, rowcount = standmodel_2->rowCount(); row < rowcount; row++) {
        QByteArray line;
        line += standmodel_2->data(standmodel_2->index(row, 1)).toString() + " ";
        line += standmodel_2->data(standmodel_2->index(row, 0)).toString() + "\r\n";
        file.write(line);

    }
    file.close();
    return true;
}

bool w_sys_manage_wirelessordermonitoring::startjz(QWidget *parent)
{

#ifdef Q_OS_WIN
    QProcess::startDetached(qApp->applicationDirPath()+"/wx.exe",
                            QStringList() << QString().sprintf("%d", public_sql::save_login->winId()));
#endif

#ifdef Q_OS_UNIX
    /// 无线点菜机
    QStringList wxpidlist;
    public_sql::cmd_ps_pid(wxpidlist, "pos/w", "pos/wx");
    if(wxpidlist.count() != 0) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, "WX IS RUNNING");
        return true;
    }

    if(false == QProcess::startDetached(qApp->applicationDirPath()+"/wx",
                                        QStringList(),
                                        qApp->applicationDirPath())) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, "START ERROR");
        return true;
    }
#endif
    return true;
}

void w_sys_manage_wirelessordermonitoring::showjz()
{
    QUdpSocket udpsocket;
    udpsocket.writeDatagram(QByteArray::fromHex("FD"), QHostAddress::LocalHost, 8830);
}

void w_sys_manage_wirelessordermonitoring::closejz()
{
#ifdef Q_OS_UNIX
    QUdpSocket udpsocket;
    udpsocket.writeDatagram(QByteArray::fromHex("FF"), QHostAddress::LocalHost, 8830);
#endif

#ifdef Q_OS_WIN
    if(w_sys_manage_wirelessordermonitoring_wx::wxhandle != 0) {
        ::PostMessageW(HWND(w_sys_manage_wirelessordermonitoring_wx::wxhandle), 0x10, 0, 0);
    }
#endif
}

void w_sys_manage_wirelessordermonitoring::todel()
{
    standmodel->removeRow(ui->tableView->currentIndex().row());
}

void w_sys_manage_wirelessordermonitoring::todel2()
{
    standmodel_2->removeRow(ui->tableView_2->currentIndex().row());
}

void w_sys_manage_wirelessordermonitoring::tonew()
{
    standmodel->insertRow(standmodel->rowCount(), QList<QStandardItem*>()
                          << new QStandardItem
                          << new QStandardItem);
}

void w_sys_manage_wirelessordermonitoring::tonew2()
{
    standmodel_2->insertRow(standmodel_2->rowCount(), QList<QStandardItem*>()
                            << new QStandardItem
                            << new QStandardItem
                            << new QStandardItem
                            );
}

void w_sys_manage_wirelessordermonitoring::toexit()
{
    //    towxclose();
    this->reject();
}

void w_sys_manage_wirelessordermonitoring::toinit()
{

}


void w_sys_manage_wirelessordermonitoring::tosave()
{
    if(savejizhan()) {
        if(savediancaiji()) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
        }
    }
}

void w_sys_manage_wirelessordermonitoring::toshowjz()
{
    showjz();
}

void w_sys_manage_wirelessordermonitoring::tostartjz()
{
    QFile file;
    file.setFileName(dir+_TXT_jz_txt);
    if(!file.exists()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未找到基站文件,请先[新增基站]"));
        return ;
    }
    file.setFileName(dir+_TXT_dcj_txt);
    if(!file.exists()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("未找到点菜机文件,请先[新增点菜机]"));
        return ;
    }

    startjz(this);
}

void w_sys_manage_wirelessordermonitoring::tosavestart(bool f)
{
    lds::sysconf->setValue("w_sys_manage_wirelessordermonitoring/start", f);
}

void w_sys_manage_wirelessordermonitoring::towxclose()
{
    closejz();
}

void w_sys_manage_wirelessordermonitoring_box::writeLog(const QString &/*byte*/)
{
    //    QByteArray str = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit();
    //    QFile file("userdata/w_sys_manage_wirelessordermonitoring_wx.log");
    //    file.open(QFile::Append | QFile::WriteOnly);
    //    file.write(str + ":"+ byte.toLocal8Bit() + "\n");
    //    file.close();
}

w_sys_manage_wirelessordermonitoring_box::w_sys_manage_wirelessordermonitoring_box(QObject *parent) :
    QObject(parent)
{
}

void w_sys_manage_wirelessordermonitoring_box::KJ(int baseNo)
{
    QFile file;
    QStringList infolist;
    QString readline;
    QString info;
    QString dcjno_3;
    QString lxcode_4;//类型码

    file.setFileName(qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString();
        goto send ;
    }

    readline = file.readLine().trimmed();
    file.close();

    lxcode_4 = __FUNCTION__;
    infolist = readline.split(" ", QString::SkipEmptyParts);
    dcjno_3 = infolist.value(1);
    info = QObject::tr("正常开机");
    //ret
send:
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,true, info);
}

void w_sys_manage_wirelessordermonitoring_box::DL(int baseNo)
{
    QFile file;
    QString readline;
    QStringList infolist;
    QString ls_error;
    QString info;
    QString dcjno_3;
    QString jqsbh_10;
    QString operid_4;
    QString opername_8;
    bool resultf = false;
    QString lxcode_4;//类型码

    file.setFileName(qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString();
        goto send ;
    }

    // "-------------------请求登陆";

    readline = file.readLine().trimmed();
    lxcode_4 = "DL";
    dcjno_3 = readline.right(3);
    readline = file.readLine().trimmed();
    file.close();
    infolist = readline.split(" ", QString::SkipEmptyParts);
    jqsbh_10 = infolist.value(0);
    operid_4 = infolist.value(1);
    operpwd_8 = infolist.value(2);
    /*处理....*/
    //"-------------------处理登陆";
    ls_error = n_func::f_confirm_user_pwd(operid_4, operpwd_8);

    info = "0";
    opername_8 = ls_error;
    if(ls_error.isEmpty()) {
        opername_8 = n_func::f_get_opername(operid_4);
        info = "1";
        resultf = true;

        dcj_opid_map.insert(dcjno_3, operid_4);
        goto send;//!成功
    }

send:
    //"-------------------登陆返回" << info;
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info +" "+ lds::strcutBygbnum(QString().append(opername_8), 8,  false, ' ', Qt::AlignLeft);

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::DL10()
{
    //    QString readline;
    //    QStringList infolist;
    //    QString ls_error;
    //    QString dcjno_3;
    //    QString jqsbh_10;
    //    QString operid_4;
    //    QString opername_8;
    //    bool resultf = false;
    //    QString lxcode_4;//类型码

    //    file.setFileName(qApp->applicationDirPath()+QString("/TXT/DL.txt"));
    //    if(!file.open(QFile::ReadOnly)) {
    //        info = file.errorString();
    //        goto send ;
    //    }

    QFile file(qApp->applicationDirPath() +"/TXT/DL.txt");
    file.open(QFile::WriteOnly);
    file.write("1");
    file.close();
    after(10,true, "");
    return;


    //    // "-------------------请求登陆";

    //    readline = file.readLine().trimmed();
    //    lxcode_4 = "DL";
    //    dcjno_3 = readline.right(3);
    //    readline = file.readLine().trimmed();
    //    file.close();
    //    infolist = readline.split(" ", QString::SkipEmptyParts);
    //    jqsbh_10 = infolist.value(0);
    //    operid_4 = infolist.value(1);
    //    operpwd_8 = infolist.value(2);
    //    /*处理....*/
    //    //"-------------------处理登陆";
    //    ls_error = n_func::f_confirm_user_pwd(operid_4, operpwd_8);

    //    info = "0";
    //    opername_8 = ls_error;
    //    if(ls_error.isEmpty()) {
    //        opername_8 = n_func::f_get_opername(operid_4);
    //        info = "1";
    //        resultf = true;

    //        dcj_opid_map.insert(dcjno_3, operid_4);
    //        goto send;//!成功
    //    }

    //send:
    //    //"-------------------登陆返回" << info;
    //    //ret
    //    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    //    senddata += info +" "+ lds::strcutBygbnum(QString().append(opername_8), 8,  false, ' ', Qt::AlignLeft);

    //    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    //    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::SJZB11()
{
    after(11,true, "");
}

void w_sys_manage_wirelessordermonitoring_box::KT(int baseNo, const QString err)
{
    Q_UNUSED(err)
    //2
    QFile file;
    QString readline;
    QString ch_billno;
    QString info;
    QStringList infolist;
    QDateTime dt;
    lds_query_hddpos  query;
    bool resultf = false;
    QString dcjno_3;
    QString tableservant_5;//服务员号
    QString tablepersonnum_2;//客人数量
    QString tableno_4;//台号
    QString lxcode_4;//类型码

    dt = n_func::f_get_sysdatetime();

    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }
    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    readline = file.readLine().trimmed();
    file.close();
    infolist = readline.split(" ", QString::SkipEmptyParts);
    tableno_4 = infolist.value(0);
    tablepersonnum_2 = infolist.value(1);
    tableservant_5 = infolist.value(2);

    //没有桌台
    query.execSelect(QString("select count(0) from cey_bt_table where ch_tableno = '%1' ")
                     .arg(tableno_4));
    query.next();
    info = tableno_4+QObject::tr("该桌台不存在");
    if(query.recordValue(0).toLongLong() == 0) {
        goto send;
    }
    //若不为空则必须判断服务员
    if(!tableservant_5.isEmpty()) {
        query.execSelect(QString("SELECT count(0) from cey_sys_operator  where vch_operID = '%1' ")
                         .arg(tableservant_5));
        query.next();
        info = QObject::tr("服务员不存在")+QString("[%1]").arg(tableservant_5);
        if(query.recordValue(0).toLongLong() == 0) {
            goto send;
        }
    }
    ///是否分单
    query.execSelect(QString("SELECT ch_billno from cey_bt_table where ch_tableno = '%1' ").arg(tableno_4));
    query.next();
    ch_billno = query.recordValue(0).toString();
    //是否分单
    if(!ch_billno.isEmpty()) {
        info = tableno_4 + QObject::tr("餐桌已开台");
        goto send;
    }
    ///获取单号
    lds_query_hddpos::tran_saction();
    info = tableno_4+QObject::tr("数据库错误")+QString::number(__LINE__);
    if(lds_scr_table::get_max_ch_billno_add1_Insert_cey_u_master_Create_serial_no("B", ch_billno)) {
        if(query.execUpdate("cey_bt_table",  qrtVariantPairList()
                            << qrtVariantPair("ch_billno", ch_billno)
                            << qrtVariantPair("dt_datetime", dt)
                            << qrtVariantPair("int_person", tablepersonnum_2)
                            << qrtVariantPair("vch_memo", ""),
                            QString(" ch_tableno= '%1' ").arg(tableno_4))) {
            //num_cost不计入
            if(query.execInsert("cey_u_table", qrtVariantPairList()
                                << qrtVariantPair("ch_billno", ch_billno)
                                << qrtVariantPair("ch_tableno", tableno_4)
                                << qrtVariantPair("int_person", tablepersonnum_2)
                                << qrtVariantPair("ch_state", 1)
                                << qrtVariantPair("vch_operID", lds::gs_operid) //dcj_opid_map.value(dcjno_3))

                                << qrtVariantPair("dt_operdate", dt)
                                << qrtVariantPair("ch_order_state", 0)
                                << qrtVariantPair("vch_waiter", dcj_opid_map.value(dcjno_3))
                                << qrtVariantPair("vch_memo", ""))) {
                if(lds_scr_table::cey_u_taxsale_master_insert_table(query, ch_billno)) {
                    lds_query_hddpos::com_mit();
                    info = QObject::tr("开台成功")+QString("[%1]").arg(tableno_4);
                    resultf = true;
                    goto send;
                }
            }
        }
    }
    

    lds_query_hddpos::roll_back();
send:
    //"-------------------处理返回" << info;
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::CT(int baseNo)
{
    //2
    QFile file;
    QString readline;
    lds_query_hddpos  query;
    QString ch_billno;
    QString info;
    QDateTime dt;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString tableserial_3;//账单流水号
    QString lxcode_4;//类型码
    dt = n_func::f_get_sysdatetime();

    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }
    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    readline = file.readLine().trimmed();
    file.close();
    tableno_4 = readline.left(4);
    tableserial_3 = readline.right(3);

    ///
    if(fexpandmain_2::cancelTable_transaction(tableno_4, 0, info)) {
        info = QObject::tr("撤台成功")+QString("[%1]").arg(tableno_4);
        resultf = true;
    }

    //ret
send:
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::DC(int baseNo, const QString err)
{
    /*
台号或帐单号(7位)、菜品号(5位)、数量(4位)、做法(12位)、套餐号(2位)、等叫标记(1位)

    、流水号(8位)、只数(4位)、客人数(4位)
*/
    /*DC   001
0105                                 00:02:26
0105    01002    1                 0 00:02:26      2 */
    //2
    //emit signalwxcurstepinfo(QObject::tr("请求点菜"));

    //新版
    ///20031121
    //    1000                                 03:04:39
    //    1000    10000    1            03:04:39
    //          03:04:39
    //    1000    10001    1            03:04:39
    //    旧版
    ///    1000                                 00:04:23
    //    1000    10000    1                 0 00:04:23
    //    1000    10000    1                 0 00:04:23        "


    //            opername_8 = "张小丫";
    QFile file;
    QString readline;
    lds_query_hddpos  query;
    QStringList infolist;
    QString info;
    QString ch_dishno;
    QString num_num;
    QString vch_cook;
    QString vch_suitno;
    QString vch_dishname;
    QString ch_waitflag;
    QString num_zishu;
    QString num_person;
    QString ch_billno;
    qlonglong int_id = 0;
    QString num_price;
    QDateTime dt;
    bool checkdata;
    QString vch_wirelessno;
    bool resultf = false;
    w_bt_dish::kitchenState  s0 = w_bt_dish::KITCHEN_VOID;
    QString senddata;
    QString dcjno_3;
    QString tableserial_3;//账单流水号
    QString tableno_4;//
    QString lxcode_4;//类型码
    QString vch_memo;
    fexpandmain_model_sqltablemodel_data tablemodel;
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));

    //#ifdef QT_DEBUG
    //    infolist << "   1000 11028    1                   " + QDateTime::currentDateTime().toString("hh:mm:ss");
    //    infolist << "   1000 11029    1                   " + QDateTime::currentDateTime().toString("hh:mm:ss");
    //    infolist << "   1000 11032    1                   " + QDateTime::currentDateTime().toString("hh:mm:ss");
    //    goto tootder;
    //#endif
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString();
        goto send;
    }

    dt = n_func::f_get_sysdatetime();

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);

    readline = file.readLine();//去掉第一行、10001                                                                                 00：:0：:00：流水号这个
    //开台备注
    vch_memo = readline.mid(7 + 1 +10 +1, 12).trimmed();
    while(!file.atEnd()) {
        //7151411212111844
        infolist += file.readLine().trimmed();
    }
    file.close();

    if(!err.isEmpty()) {
        info = err;
        goto send;
    }
    //2


    checkdata = false;
    lds_query_hddpos::tran_saction();
    foreach(const QString &p, infolist) {
        if(p.indexOf(":") != 39) {
            info = QObject::tr("点菜失败，流水号格式异常");
            goto rollback;
        }
        int index = 0;
        tableno_4 = p.mid(index, 7).trimmed();
        index+=7;
        index+=1;
        ch_dishno = p.mid(index, 5).trimmed();// if(ch_dishno.trimmed().isEmpty()) continue;//对空数据跳过, 一般第一行ch_dishno 为 空
        index+=5;
        index+=1;
        num_num = p.mid(index, 4).trimmed();
        index+=4;
        index+=1;
        vch_cook = p.mid(index, 12).trimmed();
        index+=12;
        index+=1;
        vch_suitno = p.mid(index, 2).trimmed();
        index+=2;
        index+=1;
        ch_waitflag = p.mid(index, 1);
        index+=1;
        index+=1;
        tableserial_3 = p.mid(index, 8).trimmed();
        index+=8;
        num_zishu = p.mid(index, 4).trimmed();
        index+=4;
        num_person = p.mid(index, 4).trimmed();
        if("-1:-1:-1" == tableserial_3) {
            info = QObject::tr("点菜失败，发现单号异常，请重试");
            goto rollback;
        }
        vch_wirelessno = dcjno_3+dt.toString("yyMMdd")+tableserial_3;//10
        //若是第一次成功，接下来都会跳过
        if(checkdata) goto interloopp0;

        ///餐桌不存在
        query.execSelect(QString("select ch_billno from cey_bt_table where ch_tableno = '%1' ")
                         .arg(tableno_4));
        if(!query.next()) {
            info = tableno_4 + QObject::tr("餐桌不存在");
            goto rollback;
        }
        ch_billno = query.recordValue("ch_billno").toString();

        ///餐桌未开台
        if(ch_billno.isEmpty()) {
            info = tableno_4 + QObject::tr("餐桌未开台");
            goto rollback;
        }
        ///流水号是否存在
        query.execSelect(QString("select count(0) from cey_u_orderdish where ch_billno = '%2' and vch_wx_serial = '%1' ")
                         .arg(vch_wirelessno)
                         .arg(ch_billno));
        query.next();
        if(query.recordValue(0).toLongLong() > 0 ) {
            info = QObject::tr("点菜失败，流水号已存在") + ":"+vch_wirelessno;
            goto rollback;
        }
        ///餐桌已经锁定
        info = QObject::tr("餐桌已经锁定")+QString("[%1]").arg(tableno_4);
        if(lds_scr_table::cey_bt_table_has_locked(tableno_4, 0)) {
            goto rollback;
        }
        //lock
        lds_scr_table::cey_bt_table_lock(tableno_4, 0);

        //判断厨打类型
        s0  = w_bt_dish::KITCHEN_MIN_DEF;//"N";//正常
        if(ch_waitflag == "1") {
            s0 = w_bt_dish::KITCHEN_GUAQI;//vch_waitno = "T";//挂起
        }  else if(ch_waitflag == "2") {
            s0 = w_bt_dish::KITCHEN_JIAOQI;//vch_waitno = "V";//叫起
        }  else {
            query.execSelect(QString("select count(0) from cey_u_orderdish where ch_billno = '%1' ").arg(ch_billno));
            query.next();
            if(query.recordValue(0).toLongLong() > 0) {
                s0 = w_bt_dish::KITCHEN_JIACAI;
            }
        }
        tablemodel.setTable("cey_u_orderdish");
        tablemodel.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
        tablemodel.select(ch_billno);
        query.execSelect(QString("select max(int_id) from cey_u_orderdish where ch_billno  = '%1' ")
                         .arg(ch_billno));
        query.next();
        int_id = query.recordValue(0).toInt();

        checkdata = true;
interloopp0:
        int_id ++;

        query.execSelect(QString("select num_price, vch_dishname, int_rate from cey_bt_dish where ch_dishno = '%1' ")
                         .arg(ch_dishno));
        if(false == query.next()) {
            info = QObject::tr("菜品没有找到")+QString("[%1]").arg(ch_dishno);
            goto rollback;
        }
        //guqing
        info.clear();
        if(!fexpandmain_2::guqing_check(info, ch_dishno, num_num.toFloat())) {
            goto rollback;
        }
        if(!fexpandmain_2::guqing_update(ch_dishno, num_num.toFloat())) {
            info = QObject::tr("数据库错误")+QString::number(__LINE__);
            goto rollback;
        }
        //tablemodel
        num_price = query.recordValue("num_price").toString() ;
        vch_dishname = query.recordValue("vch_dishname").toString() ;

        vch_print_memoData vch_print_memo;
        vch_print_memo.from100110(query.db, vch_cook);

        QString vch_plan_k3o3_state;
        QString vch_plan_k3o3_id;

        int row = tablemodel.rowCount();
        tablemodel.insertRow(row);
        w_bt_dish::get_vch_plan_k3o3(vch_plan_k3o3_state, vch_plan_k3o3_id, ch_dishno, s0);
        tablemodel.model_data_set(row, "ch_billno", ch_billno);
        tablemodel.model_data_set(row, "ch_tableno", tableno_4);
        tablemodel.model_data_set(row, "int_id", int_id);
        tablemodel.model_data_set(row, "ch_dishno", ch_dishno);
        tablemodel.model_data_set(row, "num_price", num_price);

        tablemodel.model_data_set(row, "num_price_org", num_price);
        tablemodel.model_data_set(row, "num_price_add", vch_print_memo.total());
        tablemodel.model_data_set(row, "num_num", num_num);
        tablemodel.model_data_set(row, "num_back", 0);
        tablemodel.model_data_set(row, "int_discount", 100);

        tablemodel.model_data_set(row, "ch_suitflag", "N");
        tablemodel.model_data_set(row, "ch_waidai_flag", "N");
        tablemodel.model_data_set(row, "ch_addon_flag", "N");
        tablemodel.model_data_set(row, "vch_operID", lds::gs_operid); //dcj_opid_map.value(dcjno_3));
        tablemodel.model_data_set(row, "dt_operdate", dt);
        tablemodel.model_data_set(row, "vch_print_memo", vch_print_memo.sqlValue());
        tablemodel.model_data_set(row, "vch_plan_k3o3_id", vch_plan_k3o3_id);

        tablemodel.model_data_set(row, "vch_plan_k3o3_state", vch_plan_k3o3_state);

        tablemodel.model_data_set(row, "ch_presentflag", "N");
        tablemodel.model_data_set(row, "ch_specialflag", "N");
        tablemodel.model_data_set(row, "vch_wx_serial", vch_wirelessno);
        tablemodel.model_data_set(row, "int_rate", tablemodel.get_ch_dishno_rate(ch_billno, ch_dishno));

        continue;
    }


    if(vch_memo.length() != 0)
        query.execUpdate("cey_u_table", "vch_memo", vch_memo, qrtEqual("ch_billno", ch_billno));
    //success:
    //更新金额
    if(tablemodel.try_commit_data(info)) {
        info = QObject::tr("数据库错误")+QString::number(__LINE__);
        if(query.execUpdate("cey_u_table", "num_cost", public_sql::getTotalBy_ch_billno(ch_billno), QString("ch_billno = '%1' ").arg(ch_billno))) {
            lds_scr_table::cey_bt_table_unlock(tableno_4, 0);
            lds_query_hddpos::com_mit();
            //自动删除已发送帐单：点菜机收到的回复信息中前四个字为汉字“点菜成功”，则自动删除。
            info = QObject::tr("点菜成功");
            resultf = true;
            goto send;
        }
    }
    info = tablemodel.msg_mid_by_N(info, 0, 3);
rollback:
    lds_query_hddpos::roll_back();

    //    file.open(QFile::ReadOnly);
    //    file.readLine();
    //    file.readLine();
    //    {   QString p = file.readAll();
    //        info += ch_billno + p.split(" ", QString::SkipEmptyParts).join("-").toLocal8Bit();
    //    }
    //    file.close();
send:
    //ret
    senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
    return;
}

void w_sys_manage_wirelessordermonitoring_box::DC()
{
    DC(0, "");
}

void w_sys_manage_wirelessordermonitoring_box::TC(int baseNo)
{
    /*
12． 退货

    请求格式：类型码(4位)、点菜机机号(3位)

    台号或帐单号(7位)、菜品号或流水号(10位)、数量(4位)、单位(4位)、退货理由(10位)

    说明:无退货理由则退货理由写空格
示例：TC~~~001

    0002001~02001~~~~2~~~~盘~0102
*/
    /*    TC   002
    0105    01002      1                       */
    /*基站回读:2004    10005      1         00  00    */
    /*基站回读:2004    10005      5             00    */
    //2
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;
    lds_query_hddpos  query;
    QString dishno;
    QString dishnum;
    float dishnum_back_total;
    QString disnunit;
    QString quitreason;
    int index = 0;
    bool resultf = false;
    QString errstring;
    QString tableno_4;//
    QString dcjno_3;
    QString lxcode_4;//类型码
    QList<num_backData> d_list;
    //emit signalwxcurstepinfo(QObject::tr("请求退单"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    readline = file.readLine().trimmed();
    file.close();
    //    7_10_4_4_10
    index = 0;
    tableno_4  = readline.mid(index, 7);
    index += 7;
    index += 1;
    dishno = readline.mid(index,  10);
    index += 10;
    index += 1;
    dishnum = readline.mid(index,  4);
    index += 4;
    index += 1;
    disnunit = readline.mid(index,  4);
    index += 4;
    index += 1;
    quitreason = readline.mid(index,  10);
    index += 10;
    index += 1;
    tableno_4 = tableno_4.trimmed();
    dishno = dishno.trimmed();
    dishnum = dishnum.trimmed();
    disnunit = disnunit.trimmed();
    quitreason = quitreason.trimmed();
    dishnum_back_total = dishnum.toFloat();

    ///餐桌已经锁定
    if(lds_scr_table::cey_bt_table_has_locked(tableno_4, 0)) {
        info = QObject::tr("餐桌已经锁定")+QString("[%1]").arg(tableno_4);
        goto send;
    }

    ///餐桌不存在
    query.execSelect(QString("select ch_billno from cey_bt_table where ch_tableno = '%1' ")
                     .arg(tableno_4));
    if(!query.next()) {
        info = tableno_4 + QObject::tr("餐桌不存在");
        goto send;
    }
    ch_billno = query.recordValue("ch_billno").toString();

    ///餐桌未开台
    if(ch_billno.isEmpty()) {
        info = tableno_4 + QObject::tr("餐桌未开台");
        goto send;
    }
    ///退货数量超过总量
    query.execSelect(QString("select sum(num_num - num_back) from cey_u_orderdish where ch_billno = '%1' and ch_dishno = '%2' ")
                     .arg(ch_billno)
                     .arg(dishno));
    query.next();
    info = QObject::tr("退货数量超过总量") + QString("%1(%2)").arg(dishnum_back_total).arg(query.recordValue(0).toFloat());
    if(dishnum_back_total > query.recordValue(0).toFloat()) {
        goto send;
    }
    ///
    query.execSelect(QString("select num_price, vch_dishname from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(dishno));
    info = QObject::tr("菜品没有找到")+QString("[%1]").arg(dishno);
    if(!query.next()) {
        goto send;
    }
    ///
    lds_query_hddpos::tran_saction();
    if(quitdish(errstring, ch_billno, dishno, dishnum_back_total, quitreason, lds::gs_operid, d_list)) {//dcj_opid_map.value(dcjno_3))) {
        //更新金额
        info = QObject::tr("数据库错误")+QString::number(__LINE__);
        if(query.execUpdate("cey_u_table", "num_cost", public_sql::getTotalBy_ch_billno(ch_billno), QString(" ch_billno = '%1' ").arg(ch_billno))) {
            info = QObject::tr("数据库错误")+QString::number(__LINE__);
            if(fexpandmain_2::guqing_update(dishno, -dishnum_back_total)) {
                lds_query_hddpos::com_mit();
                info = QObject::tr("退货成功")+QString("[%1]").arg(dishno);
                resultf = true;
                public_printby_ch_billno::toprint_dish_return(ch_billno, public_printby_ch_billno::defpath(), num_backData::int_quit_flow_id_list(d_list));
                goto send;
            }
        }
    }
    //rollback:
    lds_query_hddpos::roll_back();
    info  = QObject::tr("数据库错误");
send:
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::DXFD(int baseNo)
{
    /*请求格式：类型码(4位)、点菜机机号(3位)

    台号或帐单号(7位)

    示例：DYD~001

    0002001

    返回格式: 类型码(4位)、点菜机机号(3位)

    信息提示

    示例：DYDC~001

    0002台001帐单已打印点菜单！*/
    //2
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;
    lds_query_hddpos  query;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码
    //"-------------------请求打印点菜单";


    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    //"-------------------处理打印点菜单";
    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    readline = file.readLine().trimmed();
    tableno_4 =readline.left(4);
    file.close();
    //操作员不在记录
    ///获取单号
    ///餐桌不存在
    query.execSelect(QString("select ch_billno from cey_bt_table where ch_tableno = '%1' ")
                     .arg(tableno_4));
    if(!query.next()) {
        info = tableno_4 + QObject::tr("餐桌不存在");
        goto send;
    }
    ch_billno = query.recordValue("ch_billno").toString();

    ///餐桌未开台
    if(ch_billno.isEmpty()) {
        info = tableno_4 + QObject::tr("餐桌未开台");
        goto send;
    }

    //获取单号
    info = QObject::tr("打印结账单")+QString("[%1]").arg(tableno_4);
    resultf = true;

send:
    //"-------------------打印点菜单返回";
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;
    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);

    if(resultf)
    {
        public_printby_ch_billno printer(ch_billno);
        printer.toprint_preprint(printer.defpath(), QObject::tr("预打单"));
    }
}

void w_sys_manage_wirelessordermonitoring_box::KXHZ(int baseNo)
{
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;

    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码

    //emit signalwxcurstepinfo(QObject::tr("空闲餐台汇总"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    file.close();

    //2
    info = "";
    query.execSelect("select ch_areano, vch_areaname  from cey_bt_table_area;");
    while(query.next()) {
        query2.execSelect(QString("SELECT ch_tableno, vch_tablename, ch_billno from cey_bt_table where ch_areano = '%1'  ")
                          .arg(query.recordValue("ch_areano").toString()));
        int tablenums = 0;
        
        while(query2.next()) {
            ch_billno = query2.recordValue("ch_billno").toString();
            if(ch_billno.isEmpty()) {
                tablenums++;
            }
        }
        info += query.recordValue("vch_areaname").toString()+"("+query.recordValue("ch_areano").toString()+")"
                +"  "+QObject::tr("空闲") + ":" + QString::number(tablenums)+"\n";
        resultf = true;
    }
send:
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::LBKX(int baseNo)
{
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;
    QString ch_typeno;

    lds_query_hddpos  query;
    lds_query_hddpos  query2;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码

    //emit signalwxcurstepinfo(QObject::tr("按类别查空闲"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    ch_typeno = file.readLine().trimmed();
    file.close();

    //2
    info = "";
    info = ch_typeno + QObject::tr("餐桌类别不存在");
    query.execSelect(QString("select vch_typename  from cey_bt_table_type  where  ch_typeno = '%1';").arg(ch_typeno));
    if(query.next()) {
        QString vch_typename = query.recordValue("vch_typename").toString();

        query2.execSelect(QString("select ch_tableno, vch_tablename, ch_billno  from cey_bt_table   where  ch_typeno = '%1'  ")
                          .arg(ch_typeno));
        info = vch_typename+"  "+QObject::tr("空闲") + ":" +"\n";
        while(query2.next()) {
            tableno_4 = query2.recordValue("ch_tableno").toString();
            ch_billno = query2.recordValue("ch_billno").toString();
            if(ch_billno.isEmpty()) {
                info += tableno_4+",";
            }
        }
        resultf = true;
    }

send:
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::THKX(int baseNo)
{
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;

    lds_query_hddpos  query;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码

    //emit signalwxcurstepinfo(QObject::tr("按台号查空闲"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    tableno_4 = file.readLine().trimmed();
    file.close();

    //2
    ///    餐桌不存在
    info = tableno_4 + QObject::tr("餐桌不存在");
    query.execSelect(QString("select ch_tableno, vch_tablename, ch_billno  from cey_bt_table   where  ch_tableno = '%1'  ")
                     .arg(tableno_4));
    if(query.next()) {
        info = query.recordValue("vch_tablename").toString()+QObject::tr("开台状态")+"\n";
        ch_billno = query.recordValue("ch_billno").toString();
        if(ch_billno.isEmpty()) {
            info += QObject::tr("未开台");
        } else {
            info += QObject::tr("已开台");
        }
        resultf = true;
    }

send:
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::ZDCX(int baseNo)
{
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;

    lds_query_hddpos  query;
    bool resultf = false;
    float subamount;
    float subnum;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码
    lds_model_sqltablemodel tablemodel;
    QString ch_dishno;
    QString ch_unitno;

    //emit signalwxcurstepinfo(QObject::tr("账单查询"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    tableno_4 = file.readLine().trimmed();
    file.close();

    //
    ///餐桌不存在
    query.execSelect(QString("select ch_billno from cey_bt_table where ch_tableno = '%1' ")
                     .arg(tableno_4));
    if(!query.next()) {
        info = tableno_4 + QObject::tr("餐桌不存在");
        goto send;
    }
    ch_billno = query.recordValue("ch_billno").toString();

    ///餐桌未开台
    if(ch_billno.isEmpty()) {
        info = tableno_4 + QObject::tr("餐桌未开台");
        goto send;
    }

    //2
    info = "";
    subamount = 0;
    subnum = 0;
    query.execSelect(QString("select  int_id, ch_dishno, num_num-num_back as  num_dish, if(ch_presentflag = 'Y',  0, ( (num_num - num_back) * num_price  )*int_discount*0.01 )+num_price_add as num_total2 from cey_u_orderdish where ch_billno ='%1' ;")
                     .arg(ch_billno));
    while(query.next()) {
        QString p0;
        QString p1;
        ch_dishno = query.recordValue("ch_dishno").toString();
        ch_unitno = tablemodel.tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "ch_unitno").toString();

        p0 = query.recordValue("int_id").toString() + "." + tablemodel.tablename_kvmap_value("cey_bt_dish", ch_dishno, "ch_dishno", "vch_dishname").toString();
        p1 = " "+query.recordValue("num_dish").toString() +
                tablemodel.tablename_kvmap_value("cey_bt_unit", ch_unitno, "ch_unitno", "vch_unitname").toString();

        info += p0.leftJustified(14 - lds::size_g_of_u(p1), ' ', true) + p1 + "\n";

        subamount += query.recordValue("num_total2").toFloat();
        subnum += query.recordValue("num_dish").toFloat();
    }
    subamount = w_scr_dish_pay_widget::valuerountbysys(subamount);//四舍五入
    info =  QObject::tr("数量") + ":" + QString::number(subnum) +" "+ QObject::tr("小计") + ":" + QString::number(subamount) + "\n"
            + info;
    resultf = true;
send:
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::ZZCC(int baseNo)
{
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;

    lds_query_hddpos  query;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码

    //emit signalwxcurstepinfo(QObject::tr("整桌催起"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    tableno_4 = file.readLine().trimmed();
    file.close();

    ///餐桌不存在
    query.execSelect(QString("select ch_billno from cey_bt_table where ch_tableno = '%1' ")
                     .arg(tableno_4));
    if(!query.next()) {
        info = tableno_4 + QObject::tr("餐桌不存在");
        goto send;
    }
    ch_billno = query.recordValue("ch_billno").toString();

    ///餐桌未开台
    if(ch_billno.isEmpty()) {
        info = tableno_4 + QObject::tr("餐桌未开台");
        goto send;
    }

    lds_query_hddpos::tran_saction();
    info = QObject::tr("数据库错误")+QString::number(__LINE__);
    if(!query.execUpdate("cey_u_orderdish",
                         "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)),
                         qrtEqual("ch_billno", ch_billno))) {
        lds_query_hddpos::roll_back();
        goto send;
    }
    
    lds_query_hddpos::com_mit();
    info = QObject::tr("整桌催起")+QString("[%1]").arg(tableno_4);
    resultf = true;

send:
    //ret
    QString senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);

    //    "ZZCC 002" "1001   " 1578    整桌催起
    //   "CLCC 002" "1001    01        " 1578   菜品类别
    //    "CPCC 002" "1001    10010" 1578   菜品
}

void w_sys_manage_wirelessordermonitoring_box::CLCC(int baseNo)
{
    //    "ZZCC 002" "1001   " 1578    整桌催起
    //   "CLCC 002" "1001    01        " 1578   菜品类别
    //    "CPCC 002" "1001    10010" 1578   菜品
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;
    QString ch_dish_typeno;

    lds_query_hddpos  query;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码
    QString senddata;
    int index = 0;

    //emit signalwxcurstepinfo(QObject::tr("类别催起"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    readline = file.readLine();
    index = 0;
    tableno_4 = readline.mid(index, 7).trimmed();
    index+=7;
    index+=1;
    ch_dish_typeno = readline.mid(index, 2).trimmed();
    file.close();

    //2
    //大类不存在
    info = ch_dish_typeno + QObject::tr("大类不存在");
    query.execSelect(QString("select ch_dish_typeno from cey_bt_dish_type where ch_dish_typeno = '%1' ")
                     .arg(ch_dish_typeno));
    if(!query.next()) {
        goto send;
    }
    ///餐桌不存在
    query.execSelect(QString("select ch_billno from cey_bt_table where ch_tableno = '%1' ")
                     .arg(tableno_4));
    if(!query.next()) {
        info = tableno_4 + QObject::tr("餐桌不存在");
        goto send;
    }
    ch_billno = query.recordValue("ch_billno").toString();

    ///餐桌未开台
    if(ch_billno.isEmpty()) {
        info = tableno_4 + QObject::tr("餐桌未开台");
        goto send;
    }
    //获取单号
    lds_query_hddpos::tran_saction();
    info = QObject::tr("数据库错误")+QString::number(__LINE__);
    if(!query.execUpdate("cey_u_orderdish",
                         QStringList() << "int_flowID",
                         QString("select a.int_flowID,  '%1' as `vch_plan_k3o3_state`  from "
                                 "cey_u_orderdish a,  cey_bt_dish b where "
                                 "a.ch_dishno = b.ch_dishno and   b.ch_dish_typeno = '%2'  and a.ch_billno = '%3' ")
                         .arg(QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)))
                         .arg(ch_dish_typeno)
                         .arg(ch_billno))) {
        lds_query_hddpos::roll_back();
        goto send;
    }

    lds_query_hddpos::com_mit();
    info = QObject::tr("类别催起")+QString("[%1]").arg(ch_dish_typeno);
    resultf = true;

send:
    //ret
    senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

void w_sys_manage_wirelessordermonitoring_box::CPCC(int baseNo)
{
    //    "ZZCC 002" "1001   " 1578    整桌催起
    //   "CLCC 002" "1001    01        " 1578   菜品类别
    //    "CPCC 002" "1001    10010" 1578   菜品
    QFile file;
    QString readline;
    QString info;
    QString ch_billno;
    QString ch_dishno;

    lds_query_hddpos  query;
    bool resultf = false;
    QString dcjno_3;
    QString tableno_4;//
    QString lxcode_4;//类型码
    QString senddata;
    int index = 0;

    //emit signalwxcurstepinfo(QObject::tr("菜品催起"));
    file.setFileName((qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo)));
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString() ;
        goto send;
    }

    readline = file.readLine().trimmed();
    lxcode_4 = __FUNCTION__;
    dcjno_3 = readline.right(3);
    readline = file.readLine();
    index = 0;
    tableno_4 = readline.mid(index, 7).trimmed();
    index+=7;
    index+=1;
    ch_dishno = readline.mid(index, 5).trimmed();
    file.close();

    //2
    //菜品不存在
    info = ch_dishno + QObject::tr("菜品不存在");
    query.execSelect(QString("select ch_dishno from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(ch_dishno));
    if(!query.next()) {
        goto send;
    }
    ///餐桌不存在
    query.execSelect(QString("select ch_billno from cey_bt_table where ch_tableno = '%1' ")
                     .arg(tableno_4));
    if(!query.next()) {
        info = tableno_4 + QObject::tr("餐桌不存在");
        goto send;
    }
    ch_billno = query.recordValue("ch_billno").toString();

    ///餐桌未开台
    if(ch_billno.isEmpty()) {
        info = tableno_4 + QObject::tr("餐桌未开台");
        goto send;
    }

    //获取单号
    lds_query_hddpos::tran_saction();
    info = QObject::tr("数据库错误")+QString::number(__LINE__);
    if(!query.execUpdate("cey_u_orderdish",
                         QStringList() << "int_flowID",
                         QString("select a.int_flowID, '%1' as `vch_plan_k3o3_state` from "
                                 "cey_u_orderdish a,  cey_bt_dish b where "
                                 "a.ch_dishno = b.ch_dishno  and a.ch_billno = '%2' ")
                         .arg(QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)))
                         .arg(ch_billno))) {
        lds_query_hddpos::roll_back();
        goto send;
    }
    lds_query_hddpos::com_mit();

    info = QObject::tr("催起")+QString("[%1]").arg(ch_dishno);
    resultf = true;

send:
    //ret
    senddata = lxcode_4.leftJustified(4, ' ', true) + ' ' + dcjno_3.rightJustified(3, '0', true) +"\n";
    senddata += info;

    writeback(printer_0::trans_UTF82GBK(senddata), baseNo);
    after(baseNo,resultf, info);
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_jz_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/jz.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/基站.txt";
#endif
    return "";
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_dcj_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/dcj.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/点菜机.txt";
#endif
    return "";
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_jdmc_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/jdmc.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/酒店名称.txt";
#endif
    return "";
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_cpb_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/cpb.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/菜品表.txt";
#endif
    return "";
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_cplbb_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/cplbb.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/菜品类别表.txt";
#endif
    return "";
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_bfmcb_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/bfmcb.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/包房名称表.txt";
#endif
    return "";
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_tclyb_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/tclyb.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/退货理由表.txt";
#endif
    return "";
}

QString w_sys_manage_wirelessordermonitoring_box::_TXT_khyqb_txt_str()
{
#ifdef Q_OS_UNIX
    return "/TXT/khyqb.txt";
#endif

#ifdef Q_OS_WIN
    return "/TXT/客户要求表.txt";
#endif
    return "";
}

bool w_sys_manage_wirelessordermonitoring_box::writeback(const QByteArray &d, int baseNo)
{
    //    record(d);
    QFile file(qApp->applicationDirPath()+QString("/TXT/R%1.txt").arg(baseNo));
    if(file.open(QFile::WriteOnly)) {
        file.write(d);
        file.close();
        writeLog("TXT写入回读内容" + d);
        return true;
    }
    return false;
}

bool w_sys_manage_wirelessordermonitoring_box::quitdish(QString &errstring, const QString &ch_billno, const QString &ch_dishno, double dishnum_back_total, const QString &vch_reason, const QString &operid, QList<num_backData> &d_list)
{
    lds_query_hddpos  query;
    qlonglong int_flowid;
    QDateTime dt = n_func::f_get_sysdatetime();

    //非子套菜
    query.execSelect(QString("select int_flowid, ch_suitflag from cey_u_orderdish where ch_suitflag <> 'Y' and ch_billno = '%1' and ch_dishno = '%2' ")
                     .arg(ch_billno)
                     .arg(ch_dishno)
                     );
    if(!query.next()) {
        errstring = ch_dishno + QObject::tr("菜品没有找到");
        return false;
    }
    query.previous();

    while(dishnum_back_total  > 0 && query.next()) {
        int_flowid = query.recordValue("int_flowid").toLongLong();
        if(!w_bt_dish_back::static_toquit_1_suit(errstring, int_flowid, dishnum_back_total,vch_reason,operid, dt, d_list)) {
            goto failure;
        }
    }
    return true;
failure:
    return false;
}

w_sys_manage_wirelessordermonitoring_wx_bar::w_sys_manage_wirelessordermonitoring_wx_bar(QObject *parent)
    :w_sys_manage_wirelessordermonitoring_box(parent)
{

}

w_sys_manage_wirelessordermonitoring_wx_bar::~w_sys_manage_wirelessordermonitoring_wx_bar()
{

}

void w_sys_manage_wirelessordermonitoring_wx_bar::tonative(unsigned long lParam)
{
    switch(lParam) {
    case 1://基站1
        if(wxhandle != 0) {
            basestationT1();
        }
        break;
    default:
        if(wxhandle == 0 && lParam > 10) {//获得句柄
            wxhandle = lParam;
        }
        break;
    }
}

void w_sys_manage_wirelessordermonitoring_wx_bar::basestationT1()
{
    int baseNo = 1;
    QFile file(qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo));
    QString readline;
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }
    readline = file.readLine().trimmed();
    file.close();

    //2更具请求内容获取相关参数
    //开机[KJ---001]
    if(readline.startsWith("KJ")) {
        KJ(baseNo);
    }
    //登陆[DL---001]
    else if(readline.startsWith("DL")) {
        DL(baseNo);
    }
    //修改密码
    else if(readline.startsWith("XGMM")) {

    }
    //开台
    else if(readline.startsWith("KT")) {
        KT(baseNo);
    }
    //5换台
    else if(readline.startsWith("HT")) {

    }

    //并台
    else if(readline.startsWith("BT")) {

    }
    //撤台
    else if(readline.startsWith("CT")) {

    }
    //修改台头
    else if(readline.startsWith("XGTT")) {

    }
    //点菜
    else if(readline.startsWith("DC")) {
        DC(baseNo);
    }
    //10客户要求
    else if(readline.startsWith("KHYQ")) {
    }

    ///打印点菜单-------------------------begin
    else if(readline.startsWith("DXFD")) {//DXFD
        DXFD(baseNo);
    }
    //加菜
    else if(readline.startsWith("JC")) {

    }

    ///退货-------------------------begin
    else if(readline.startsWith("TC")) {
        TC(baseNo);
    }
    ///退货-------------------------end

    //勾挑
    else if(readline.startsWith("GT")) {

    }
    //帐单查询
    else if(readline.startsWith("ZDCX")) {
        ZDCX(baseNo);
    }
    //空闲餐台汇总
    else if(readline.startsWith("KXHZ")) {
        KXHZ(baseNo);
    }
    //按类别查空闲
    else if(readline.startsWith("LBKX")) {
        LBKX(baseNo);
    }
    //按台号查空闲
    else if(readline.startsWith("THKX")) {
        THKX(baseNo);
    }
    ///整桌催起-------------------------begin
    else if(readline.startsWith("ZZCC")) {
        ZZCC(baseNo);
    }
    ///整桌催起-------------------------end
    ///
    ///类别催起-------------------------begin
    else if(readline.startsWith("CLCC")) {
        CLCC(baseNo);
    }
    ///类别催起-------------------------end
    ///
    ///菜品催起-------------------------begin
    else if(readline.startsWith("CPCC")) {
        CPCC(baseNo);
    }
    ///菜品催起-------------------------end
}

void w_sys_manage_wirelessordermonitoring_wx_bar::after(int baseNo, bool f, const QString &info)
{
    //"-------------------会送扫码棒1" << wxhandle;
    if(wxhandle != 0) {
        //"-------------------会送扫码棒2" << wxhandle;
#if defined (Q_OS_WIN)
        ::PostMessageW(HWND(wxhandle), 1280, 0, baseNo);
#endif
    }
    stepf = f;
    stepinfo = info;
}

void w_sys_manage_wirelessordermonitoring_wx_bar::KT(int baseNo)
{
    w_sys_manage_wirelessordermonitoring_box::KT(baseNo, stepf?"":stepinfo);
}

void w_sys_manage_wirelessordermonitoring_wx_bar::DC(int baseNo)
{
    w_sys_manage_wirelessordermonitoring_box::DC(baseNo, stepf?"":stepinfo);
}


unsigned long  w_sys_manage_wirelessordermonitoring_wx::wxhandle = 0;
w_sys_manage_wirelessordermonitoring_wx::w_sys_manage_wirelessordermonitoring_wx(QObject *parent) :
    w_sys_manage_wirelessordermonitoring_box(parent)
{
}

void w_sys_manage_wirelessordermonitoring_wx::enableUDP()
{
    udpsocket.bind(lds::SYS_WX_PORT);
    connect(&udpsocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
}

void w_sys_manage_wirelessordermonitoring_wx::enableWindowSLOT()
{
#ifdef Q_OS_WIN
    connect(this, SIGNAL(signalNative(ulong)), this, SLOT(toNative(ulong)));
#endif
}

void w_sys_manage_wirelessordermonitoring_wx::record_wx_request(const QByteArray &d)
{
    QString dir = "userdata/wxrecord";
    if(!QDir(dir).exists()) {
        QDir().mkpath(dir);
    }
    QFile file("userdata/wxrecord/"+QDateTime::currentDateTime().toString("hhmmss")+"wx.txt");
    file.open(QFile::WriteOnly);
    file.write(d);
    file.close();
}

void w_sys_manage_wirelessordermonitoring_wx::processPendingDatagrams()
{
    QHostAddress host;
    quint16 port;
    QByteArray datagram;
    datagram.resize(udpsocket.pendingDatagramSize());
    udpsocket.readDatagram(datagram.data(), datagram.size(), &host, &port);

    basestationTUDP(datagram, host, port);
}

void w_sys_manage_wirelessordermonitoring_wx::basestationTUDP(const QByteArray &request, const QHostAddress &host, quint16 port)
{
    //1 来之基站1的信息请求
    request_tmp = request;
    int baseNo = request.toHex().toInt();
    ////////////////安卓对接接口
    if(request.startsWith("_")) {//电子菜谱的前缀
        basestationTAndroid(request, host, port);
        return ;
    }
    basestationTDCJ(baseNo);
}

void w_sys_manage_wirelessordermonitoring_wx::basestationTAndroid(const QByteArray &request, const QHostAddress &host, quint16 port)
{
    QString readline = request;
    QStringList lines = readline.split(",", QString::SkipEmptyParts);
    lds_query_hddpos  query;
    QString ch_billno;
    QString ch_dishno;
    QString ch_tableno;
    QString errstring;
    QString operid;

    if(readline.startsWith("_DXFD")) {//DXFD
        //_DXFD,B16010700005
        //打印点菜单
        ch_billno = lines.value(1);
        public_printby_ch_billno printer(ch_billno);
        printer.toprint_preprint(printer.defpath(), QObject::tr("预打单"));

        udpsocket.writeDatagram(QByteArray().append(SUCCESS), host, port);
    } else if(readline.startsWith("_TC")) {//_TC
        //退货  1桌号、2编号、3退货数量、4退货原因、5操作员
        ch_tableno = lines.value(1);
        ch_dishno = lines.value(2);
        float dishnum_back_total = lines.value(3).toFloat();
        QString quitreason = lines.value(4);
        operid = lines.value(5);


        ///获取单号
        query.execSelect(QString("SELECT ch_billno from cey_bt_table where ch_tableno = '%1' ")
                         .arg(ch_tableno));
        query.next();
        ch_billno = query.recordValue(0).toString();
        if(ch_billno.isEmpty()) {
            udpsocket.writeDatagram(QByteArray().append(TABLE_NOT_OPEN), host, port);
            return;
        }
        //获取单号

        //退货数量超过总量
        query.execSelect(QString("select sum(num_num - num_back) from cey_u_orderdish where ch_billno = '%1' and ch_dishno = '%2' ")
                         .arg(ch_billno)
                         .arg(ch_dishno));
        query.next();
        if(dishnum_back_total > query.recordValue(0).toFloat()) {
            //退货数量超过总量
            udpsocket.writeDatagram(QByteArray().append(QUITNUM_MORE_THAN_TOTAL), host, port);
            return;
        }
        QList<num_backData> d_list;
        lds_query_hddpos::tran_saction();
        if(quitdish(errstring, ch_billno, ch_dishno, dishnum_back_total, quitreason, operid, d_list)) {
            if(query.execUpdate("cey_u_table", "num_cost", public_sql::getTotalBy_ch_billno(ch_billno), QString(" ch_billno = '%1'").arg(ch_billno))) {
                lds_query_hddpos::com_mit();
                udpsocket.writeDatagram(QByteArray().append(SUCCESS), host, port);
                return;
            }
        }
        lds_query_hddpos::roll_back();
        udpsocket.writeDatagram(QByteArray().append(DB_ERROR), host, port);
        return;
    } else if(readline.startsWith("_ZZCC")) {//整桌催起
        ch_tableno = lines.value(1);

        ///获取单号
        query.execSelect(QString("SELECT ch_billno from cey_bt_table where ch_tableno = '%1' ")
                         .arg(ch_tableno));
        query.next();
        ch_billno = query.recordValue(0).toString();
        if(ch_billno.isEmpty()) {
            udpsocket.writeDatagram(QByteArray().append(TABLE_NOT_OPEN), host, port);
            return;
        }
        //获取单号

        lds_query_hddpos::tran_saction();
        if(query.execUpdate("cey_u_orderdish", "vch_plan_k3o3_state", QString (6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)),
                            QString("ch_billno = '%1' ").arg(ch_billno))) {
            lds_query_hddpos::com_mit();
            udpsocket.writeDatagram(QByteArray().append(SUCCESS), host, port);
            return;
        }
        lds_query_hddpos::roll_back();
        udpsocket.writeDatagram(QByteArray().append(DB_ERROR), host, port);
        return;
    } else if(readline.startsWith("_CLCC")) {//类别催起
        ch_tableno = lines.value(1);
        QString ch_dish_typeno = lines.value(2);

        ///获取单号
        query.execSelect(QString("SELECT ch_billno from cey_bt_table where ch_tableno = '%1' ")
                         .arg(ch_tableno));
        query.next();
        ch_billno = query.recordValue(0).toString();
        if(ch_billno.isEmpty()) {
            udpsocket.writeDatagram(QByteArray().append(TABLE_NOT_OPEN), host, port);
            return;
        }
        //获取单号

        lds_query_hddpos::tran_saction();

        if(query.execUpdate("cey_u_orderdish",
                             QStringList() << "int_flowID",
                             QString("select a.int_flowID, '%1' as `vch_plan_k3o3_state` from "
                                     "cey_u_orderdish a,  cey_bt_dish b where "
                                     "a.ch_dishno = b.ch_dishno and b.ch_dish_typeno = '%2' and a.ch_billno = '%3' ")
                             .arg(QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)))
                             .arg(ch_dish_typeno)
                             .arg(ch_billno))) {
            lds_query_hddpos::com_mit();
            udpsocket.writeDatagram(QByteArray().append(SUCCESS), host, port);
            return;
        }
        lds_query_hddpos::roll_back();
        udpsocket.writeDatagram(QByteArray().append(DB_ERROR), host, port);
        return;
    } else if(readline.startsWith("_CPCC")) {//菜品催起
        ch_tableno = lines.value(1);
        QString ch_dishno = lines.value(2);

        ///获取单号
        query.execSelect(QString("SELECT ch_billno from cey_bt_table where ch_tableno = '%1' ")
                         .arg(ch_tableno));
        query.next();
        ch_billno = query.recordValue(0).toString();
        if(ch_billno.isEmpty()) {
            udpsocket.writeDatagram(QByteArray().append(TABLE_NOT_OPEN), host, port);
            return;
        }
        //获取单号
        lds_query_hddpos::tran_saction();
        if(query.execUpdate("cey_u_orderdish a", "a.vch_plan_k3o3_state", QString (6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_CUICAI)),
                            QString(" a.ch_dishno = '%1' and a.ch_billno = '%2' ")
                            .arg(ch_dishno)
                            .arg(ch_billno))) {
            lds_query_hddpos::com_mit();
            udpsocket.writeDatagram(QByteArray().append(SUCCESS), host, port);
            return;
        }
        lds_query_hddpos::roll_back();
        udpsocket.writeDatagram(QByteArray().append(DB_ERROR), host, port);
        return;
    } else if(readline.startsWith("_DYFK")) {//打印付款单
        //_DYFK,B16010700005
        //打印点菜单
        ch_billno = lines.value(1);
        if(lds::sysconf->value("system_setting/maidanzhangdan").toString() == "1") {
            QString bp_languageKey = lds::sysconf->value("system_setting/maidan_language").toString();
            public_printby_ch_billno print_ch_billno(ch_billno, bp_languageKey);
            print_ch_billno.toprint(print_ch_billno.defpath(), n_func::f_get_sysparm("bp_count", "1").toInt());
        }

        udpsocket.writeDatagram(QByteArray().append(SUCCESS), host, port);
    }
    return;
}

void w_sys_manage_wirelessordermonitoring_wx::basestationTDCJ(int baseNo)
{
    QFile file(qApp->applicationDirPath()+QString("/TXT/T%1.txt").arg(baseNo));
    QString readline;
    QString info;

    ///////////////点菜宝
    if(!file.open(QFile::ReadOnly)) {
        info = file.errorString();
        goto send;
    }
    readline = file.readLine().trimmed();
    file.close();

    //2更具请求内容获取相关参数
    //开机[KJ---001]
    if(readline.startsWith("KJ")) {
        KJ(baseNo);
        return;
    }
    //登陆[DL---001]
    if(readline.startsWith("DL")) {
        DL(baseNo);
        return;
    }
    //修改密码
    if(readline.startsWith("XGMM")) {

    }
    //开台
    if(readline.startsWith("KT")) {
        KT(baseNo);
        return;
    }
    //5换台
    if(readline.startsWith("HT")) {

    }

    //并台
    if(readline.startsWith("BT")) {

    }
    //撤台
    if(readline.startsWith("CT")) {
        CT(baseNo);
        return;
    }
    //修改台头
    if(readline.startsWith("XGTT")) {

    }
    //点菜
    if(readline.startsWith("DC")) {
        DC(baseNo);
        return;
    }
    //10客户要求
    if(readline.startsWith("KHYQ")) {
    }

    ///打印点菜单-------------------------begin
    if(readline.startsWith("DXFD")) {//DXFD
        DXFD(baseNo);
        return;
    }
    ///打印点菜单-------------------------end

    //加菜
    if(readline.startsWith("JC")) {

    }

    ///退货-------------------------begin
    if(readline.startsWith("TC")) {
        TC(baseNo);
        return;
    }
    ///退货-------------------------end

    //勾挑
    if(readline.startsWith("GT")) {

    }
    //帐单查询
    if(readline.startsWith("ZDCX")) {
        ZDCX(baseNo);
        return;
    }
    //空闲餐台汇总
    if(readline.startsWith("KXHZ")) {
        KXHZ(baseNo);
        return;
    }
    //按类别查空闲
    if(readline.startsWith("LBKX")) {
        LBKX(baseNo);
        return;
    }
    //按台号查空闲
    if(readline.startsWith("THKX")) {
        THKX(baseNo);
        return;
    }
    ///整桌催起-------------------------begin
    if(readline.startsWith("ZZCC")) {
        ZZCC(baseNo);
        return;
    }
    ///整桌催起-------------------------end
    ///
    ///类别催起-------------------------begin
    if(readline.startsWith("CLCC")) {
        CLCC(baseNo);
        return;
    }
    ///类别催起-------------------------end
    ///
    ///菜品催起-------------------------begin
    if(readline.startsWith("CPCC")) {
        CPCC(baseNo);
        return;
    }
    ///菜品催起-------------------------end
    ///

    info = QObject::tr("没有此功能");
send:
    writeback(printer_0::trans_UTF82GBK(readline+"\n"+info), baseNo);
    after(baseNo,false, info);
}

void w_sys_manage_wirelessordermonitoring_wx::toNative(unsigned long lParam)
{
    switch(lParam) {
    case 1://基站1
    case 2:
    case 3:
    case 4:
    case 5:

    case 6:
    case 7:
    case 8://基站8
        if(wxhandle != 0) {
            basestationTDCJ(lParam);
        }
        break;
    case 9://空
        break;
    case 10://登录验证密码
        DL10();
        break;
    case 11://下载数据、导出
        SJZB11();//数据下载准备，发送11
        break;
    default:
        if(wxhandle == 0 && lParam > 10) {//获得句柄
            wxhandle = lParam;
        }
        break;
    }
}

void w_sys_manage_wirelessordermonitoring_wx::after(int baseNo, bool f, const QString &info)
{
    Q_UNUSED(f)
    Q_UNUSED(info)

    if(wxhandle == 0) {//非句柄通信
        if(!request_tmp.isEmpty()) {
            writeLog("响应基站" + request_tmp);
            udpsocket.writeDatagram(request_tmp, QHostAddress::LocalHost, 8830);
        }
        request_tmp.clear();
    } else {//句柄通信
#ifdef Q_OS_WIN
        ::PostMessageW(HWND(wxhandle), 1280, 0, baseNo);
#endif
    }
}



