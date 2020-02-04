#include "w_scr_dish_restaurant_preorder.h"
#include "ui_w_scr_dish_restaurant_preorder.h"
#include <QStandardItemModel>
#include <QScrollBar>
#include <QSqlRecord>
#include "lds_messagebox.h"
#include "ui_w_scr_dish_restaurant_preorder_add_dialog.h"
#include "lds_query_hddpos.h"
#include <QSqlError>
#include "n_func.h"
#include "public_sql.h"
#include "ui_w_scr_dish_restaurant_preorder_add_dialog.h"
#include "backheader.h"
#include "ftableview_standmodel_sql.h"
#include "lds.h"
#include "w_scr_dish_restaurant_div_pop.h"

w_scr_dish_restaurant_preorder::w_scr_dish_restaurant_preorder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_restaurant_preorder)
{
    ui->setupUi(this);
    //1
    querymodel = new DateQueryModel(this);
    selectsql = QString(
                " SELECT vch_booker, vch_tables, vch_tel, int_person, dt_come,  dt_warn, dt_operdate, vch_operID, vch_tel_second, vch_memo, ch_state, ch_bookno  from "
                "  cybr_book_master where dt_operdate >= '%1' and (ifnull(ch_state, '') <> 'Y' %2 ) ");
    ui->tableView->setModel(querymodel);
    if(querymodel->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }
    //2
    //3
    ui->checkBox_ch_state->setChecked(lds::sysconf->value("w_scr_dish_restaurant_preorder/ch_state", false).toBool());
    //4

    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tochange()));
    connect(ui->pushButton_previous,SIGNAL(clicked()),this,SLOT(toprevious()));
    connect(ui->pushButton_next,SIGNAL(clicked()),this,SLOT(tonext()));
    connect(ui->pushButton_exit,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_order,SIGNAL(clicked()),this,SLOT(toadd()));
    connect(ui->pushButton_change,SIGNAL(clicked()),this,SLOT(tochange()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_setting,SIGNAL(clicked()),this,SLOT(tosetting()));
    connect(ui->pushButton_printbill,SIGNAL(clicked()),this,SLOT(toprintbill()));
    connect(ui->pushButton_orderopentable,SIGNAL(clicked()),this,SLOT(toorderopentable()));
    connect(ui->checkBox_ch_state,SIGNAL(toggled(bool)),this,SLOT(tosaveChstate(bool)));

    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd");
    ui->dateTimeEdit->setDate(n_func::f_get_sysdatetime().date());
    connect(ui->pushButton_search, SIGNAL(clicked()),this,SLOT(tosearch()));
    ui->pushButton_next->hide();
    ui->pushButton_previous->hide();
    ui->pushButton_search->animateClick();


}

w_scr_dish_restaurant_preorder::~w_scr_dish_restaurant_preorder()
{
    delete ui;
}

void w_scr_dish_restaurant_preorder::toprevious()
{
    ui->tableView->verticalScrollBar()->setValue(ui->tableView->verticalScrollBar()->value()-ui->tableView->verticalScrollBar()->pageStep());
}

void w_scr_dish_restaurant_preorder::tonext()
{
    ui->tableView->verticalScrollBar()->setValue(ui->tableView->verticalScrollBar()->value()+ui->tableView->verticalScrollBar()->pageStep());
}

void w_scr_dish_restaurant_preorder::toexit()
{
    this->reject();
}

void w_scr_dish_restaurant_preorder::toadd()
{
    w_scr_dish_restaurant_preorder_add_Dialog dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        torefresh();
    }
}

void w_scr_dish_restaurant_preorder::tochange()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        QString ch_bookno = querymodel->record(row).value("ch_bookno").toString();
        //
        QString ch_state = querymodel->record(row).value("ch_state").toString();
        if(ch_state == "Y") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该餐桌已经预订,")+QObject::tr("操作取消"));
            return;
        }
        //
        w_scr_dish_restaurant_preorder_add_Dialog dialog(this);
        dialog.ui->lineEdit_ch_bookno->setText(ch_bookno);
        dialog.torefresh();
        dialog.setWindowTitle(_TEXT_SLOT(this));
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            torefresh();
        }
    }
}

void w_scr_dish_restaurant_preorder::tocancel()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        //
        QString ch_state = querymodel->record(row).value("ch_state").toString();
        if(ch_state == "Y") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该餐桌已经预订,")+QObject::tr("操作取消"));
            return;
        }
        //
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否取消预订"), QObject::tr("是"), QObject::tr("否"))) {
            return;
        }
        lds_query_hddpos::tran_saction();
        QString ch_bookno = querymodel->record(row).value("ch_bookno").toString();
        if(todel(ch_bookno)) {
            lds_query_hddpos::com_mit();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            torefresh();
            return;
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    }
}

void w_scr_dish_restaurant_preorder::tosetting()
{
    w_scr_dish_restaurant_preorder_set dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();

}
void w_scr_dish_restaurant_preorder::toprintbill()
{
    int row = ui->tableView->currentIndex().row();
    if(row >= 0) {
        QString ch_bookno = querymodel->record(row).value("ch_bookno").toString();
        if(!ch_bookno.isEmpty()) {
            lds_query_hddpos  query;
            query.execSelect(QString("select  vch_tables, dt_come, vch_booker, vch_tel, int_person, dt_operdate, vch_operID, vch_tel_second, vch_memo, dt_warn from cybr_book_master where ch_bookno = '%1' ")
                             .arg(ch_bookno));

            query.next();
            blockIns printins;
            printins.append(backheader::printer.drawText("a00,s11", QObject::tr("预订详情")+"\n","a10,s00"));
            printins.append(backheader::printer.drawText("", QObject::tr("订单号") + ":" + ch_bookno +"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("预到桌号") + ":" + query.recordValue("vch_tables").toString()+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("预订人") + ":" + query.recordValue("vch_booker").toString()+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("联系电话") + ":" + query.recordValue("vch_tel").toString()+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("备用电话") + ":" + query.recordValue("vch_tel_second").toString()+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("人数") + ":" + query.recordValue("int_person").toString()+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("预到时间") + ":" + query.recordValue("dt_come").toDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("操作员") + ":" + query.recordValue("vch_operid").toString()+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("操作时间") + ":" + query.recordValue("dt_operdate").toDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n"));
            printins.append(backheader::printer.drawText("", QObject::tr("备注") + ":" + query.recordValue("vch_memo").toString()+"\n"));
            printins.append(backheader::printer.drawTextEnd("1B6405 1D5601", ""));//走纸5x， 切纸

            backheader::reslove_write(printins);
        }
    }
}

void w_scr_dish_restaurant_preorder::toorderopentable()
{
    int row = ui->tableView->currentIndex().row();
    QDateTime dt = n_func::f_get_sysdatetime();
    QString ch_bookno = querymodel->record(row).value("ch_bookno").toString();
    lds_query_hddpos  query;
    QString msg;
    QString ch_billno;
    QString ch_tableno;
    QString int_person;
    QString vch_operid;
    if(row >= 0) {
        vch_operid = querymodel->record(row).value("vch_operid").toString();
        ch_bookno = querymodel->record(row).value("ch_bookno").toString();
        //
        QString ch_state = querymodel->record(row).value("ch_state").toString();
        if(ch_state == "Y") {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该餐桌已经预订,")+QObject::tr("操作取消"));
            return;
        }
        //
        query.execSelect(QString("select a.ch_tableno from cey_bt_table a, cybr_book_table b where b.ch_bookno = '%1' and a.ch_tableno = b.ch_tableno and ifnull(a.ch_billno, '') <> '' ")
                         .arg(ch_bookno));
        if(query.next()) {
            msg = query.recordValue("ch_tableno").toString()+",";
            while(query.next()) {
                msg += query.recordValue("ch_tableno").toString()+",";
            }
        }
        if(!msg.isEmpty()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, msg+QObject::tr("有餐桌已经开台"));
            return;
        }

        //开台
        lds_query_hddpos  querygoing;
        lds_query_hddpos::tran_saction();
        querygoing.execSelect(QString("select a.ch_tableno, a.int_person from cey_bt_table a, cybr_book_table b where b.ch_bookno = '%1' and a.ch_tableno = b.ch_tableno")
                              .arg(ch_bookno));
        while(querygoing.next()) {
            ch_tableno = querygoing.recordValue("ch_tableno").toString();
            int_person = querygoing.recordValue("int_person").toString();

            if(lds_scr_table::get_max_ch_billno_add1_Insert_cey_u_master_Create_serial_no("B", ch_billno)) {
                if(query.execUpdate("cey_bt_table",qrtVariantPairList()
                                    << qrtVariantPair("ch_billno", ch_billno)
                                    << qrtVariantPair("dt_datetime", dt)
                                    << qrtVariantPair("int_person", int_person)
                                    << qrtVariantPair("vch_memo", ""),
                                    qrtEqual("ch_tableno", ch_tableno))) {
                    if(query.execInsert("cey_u_table",qrtVariantPairList()

                                        << qrtVariantPair("ch_billno", ch_billno)
                                        << qrtVariantPair("ch_tableno", ch_tableno)
                                        << qrtVariantPair("int_person", int_person)
                                        << qrtVariantPair("ch_state", 1)
                                        << qrtVariantPair("vch_operID", vch_operid)

                                        << qrtVariantPair("dt_operdate", dt)
                                        << qrtVariantPair("ch_order_state", 0)
                                        << qrtVariantPair("vch_waiter", vch_operid)
                                        << qrtVariantPair("vch_memo", ""))) {
                        if(lds_scr_table::cey_u_taxsale_master_insert_table(query, ch_billno)) {
                            continue;
                        }
                    }
                }
            }
            goto rollback;
        }//while querygoing
        //        if(todel(ch_bookno))
        if(query.execUpdate("cybr_book_master", "ch_state", "Y",qrtEqual("ch_bookno", ch_bookno))) {
            lds_query_hddpos::com_mit();
            torefresh();
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            return;
        }
rollback:
        lds_query_hddpos::roll_back();
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
    }
    return;
}

void w_scr_dish_restaurant_preorder::tosearch()
{
    torefresh();

    querymodel->setHeaderData(0, Qt::Horizontal, QObject::tr("预订人"));
    querymodel->setHeaderData(1, Qt::Horizontal, QObject::tr("餐桌名称"));
    querymodel->setHeaderData(2, Qt::Horizontal, QObject::tr("联系电话"));
    querymodel->setHeaderData(3,Qt::Horizontal, QObject::tr("人数"));
    querymodel->setHeaderData(4, Qt::Horizontal, QObject::tr("预到时间"));
    querymodel->setHeaderData(5, Qt::Horizontal, QObject::tr("预警时间"));
    querymodel->setHeaderData(6, Qt::Horizontal, QObject::tr("操作日期"));
    querymodel->setHeaderData(7, Qt::Horizontal, QObject::tr("操作员"));
    querymodel->setHeaderData(8, Qt::Horizontal, QObject::tr("备用电话"));
    querymodel->setHeaderData(9, Qt::Horizontal, QObject::tr("备注"));
    querymodel->setHeaderData(10, Qt::Horizontal, QObject::tr("是否预订"));
    querymodel->setHeaderData(11, Qt::Horizontal, QObject::tr("单号"));
}

void w_scr_dish_restaurant_preorder::tosaveChstate(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_preorder/ch_state", f);
}


void w_scr_dish_restaurant_preorder::torefresh()
{
    querymodel->setQuery(selectsql
                         .arg(ui->dateTimeEdit->date().toString("yyyy-MM-dd"))
                         .arg(ui->checkBox_ch_state->isChecked()?" or ch_state = 'Y' ":""));
    ui->tableView->restore();
    if(querymodel->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }
}

bool w_scr_dish_restaurant_preorder::todel(const QString &ch_bookno)
{
    lds_query_hddpos  query;
    if(query.execDelete("cybr_book_master", QString("ch_bookno = '%1' ").arg(ch_bookno) )) {
        if(query.execDelete("cybr_book_table", QString("ch_bookno = '%1' ")
                            .arg(ch_bookno)
                            )) {
            return true;
        }
    }
    return false;
}

QVariant w_scr_dish_restaurant_preorder::DateQueryModel::data(const QModelIndex &item, int role) const
{
    QVariant d = QSqlQueryModel::data(item, role);
    if(role==Qt::DisplayRole) {
        if(d.type()==QVariant::DateTime) {
            return d.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }
    }
    return d;
}

//////////////////////////////////////////////////////////////////
/// \brief w_scr_dish_restaurant_preorder_add_Dialog::w_scr_dish_restaurant_preorder_add_Dialog
/// \param parent
///
#include "ui_w_scr_dish_restaurant_preorder_add_dialog.h"
#include "uo_rw_member.h"
#include "w_m_member_list.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "ftableview_standmodel_sql_table.h"
#include "ftableview_delegate.h"

w_scr_dish_restaurant_preorder_add_Dialog::w_scr_dish_restaurant_preorder_add_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_scr_dish_restaurant_preorder_add_Dialog)
{
    ui->setupUi(this);
    //1
    ftableview_standmodel_sql *modeltype = new ftableview_standmodel_sql;
    modeltype->sql_cur = "SELECT  concat(vch_typename,'[',ch_typeno,']') as nametype, ch_typeno  FROM cey_bt_table_type union select '"+QObject::tr("所有类型")+"' as vch_typename, '%' as ch_typeno  ORDER BY ch_typeno ASC";
    ui->tableView_tabletype->setModel(modeltype);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView_tabletype);
    ui->tableView_tabletype->setProperty("outer_stylesheet", "tableView_type");
    d->keepConnect(ui->tableView_tabletype);
    ui->tableView_tabletype->setItemDelegate(d);
    ui->tableView_tabletype->updateFixed_fillData_addAnimation(1, 5, 100, 50);
    //    modeltype->toinit();
    //5
    modeltable = new ftableview_standmodel_sql_table;
    modeltable->sql_cur =
            " select y.vch_tablename, y.ch_tableno, " +
            restraurantDelegate::sql_ch_billnos + " as ch_billnos,"
            " ifnull((select int_person from cey_u_table where cey_u_table.ch_billno = y.ch_billno), y.int_person) as int_person,"
            " (select num_cost from cey_u_table where cey_u_table.ch_billno = y.ch_billno) as num_cost "
            " from cey_bt_table y"
            ;
    modeltable->sqlother = modeltable->sql_cur + " where y.ch_typeno like '%1%' " ;
    //    modeltable->sql_cur = modeltable->sql_cur + " group by y.ch_tableno ";
    ui->tableView_table->setModel(modeltable);
    restraurantDelegate *delegatetable = new restraurantDelegate(ui->tableView_table);
    ui->tableView_table->setItemDelegate(delegatetable);
    ui->tableView_table->updateFixed_fillData_addAnimation(4, 4, 90, 90);
    ui->tableView_table->enableMutiSelected();
    //6
    ui->tableView_tabletype->m->refreshcur();
    ui->tableView_table->m->refreshcur();

    ui->spinBox_warn_diff->setValue(lds::sysconf->value("w_scr_dish_restaurant_preorder_add_Dialog/warn_diff", 30).toInt());

    //2
    ui->lineEdit_int_person->setText("1");
    ui->dateTimeEdit_dt_come->setDateTime(n_func::f_get_sysdatetime().addSecs(2*60*60));
    toupdateWarndiff(ui->dateTimeEdit_dt_come->dateTime());

    ui->comboBox_vch_operid->addItemsBySql(backheader::SELECT_OPERID_NAME_FLAG);
    ui->lineEdit_ch_bookno->setEnabled(false);

    ui->spinBox_int_overtime->setValue(n_func::f_get_sysparm("table_max_over_time", "240").toInt());

    //3
    connect(ui->tableView_table,SIGNAL(signalclick(QModelIndex)),this,SLOT(toupdateTabledesc()));
    connect(ui->tableView_tabletype,SIGNAL(signalclick(QString)),modeltable,SLOT(toinitBySqlother(QString)));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_member,SIGNAL(clicked()),this,SLOT(tomember()));
    connect(ui->pushButton_int_overtime,SIGNAL(clicked()),this,SLOT(tosetovertime()));
    connect(ui->dateTimeEdit_dt_come,SIGNAL(dateTimeChanged(QDateTime)),this,SLOT(toupdateWarndiff(QDateTime)));
    connect(ui->spinBox_warn_diff,SIGNAL(valueChanged(int)),this,SLOT(tosavewarndiff(int)));

#ifdef QT_DEBUG
    ui->lineEdit_vch_booker->setText("x");
    ui->lineEdit_vch_memo->setText("x");
    ui->lineEdit_vch_tel->setText("x");
    ui->lineEdit_vch_tel_second->setText("x");
#endif
    ui->spinBox_warn_diff->setMaximum(ui->spinBox_int_overtime->value());
    ui->label_tabledesc->setText("");
}

w_scr_dish_restaurant_preorder_add_Dialog::~w_scr_dish_restaurant_preorder_add_Dialog()
{
    delete ui;
}

void w_scr_dish_restaurant_preorder_add_Dialog::took()
{
    if(saveData()) {
        this->accept();
    }
}

void w_scr_dish_restaurant_preorder_add_Dialog::tocancel()
{
    this->reject();
}
void w_scr_dish_restaurant_preorder_add_Dialog::tomember()
{
    uo_rw_member uo_member;
    QString cardno;
    n_func::READCARDINFO cardinfo;
    //读卡
    if(!w_m_member_list::static_to_memeber_readcard(this,cardno, uo_member, true, true)) {
        w_m_member_list::yun_member_close();//! 关闭云会员
        goto end;
    }
    //判断卡是否存在
    if(cardno.trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("无可查询的卡"));
        goto end;
    }
    //获取会员信息
    if(cardinfo.vch_memberno.isEmpty()
            || cardinfo.vch_memberno != cardno) {
        if(!n_func::qtcardinfo_readcard(cardinfo, cardno)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID,cardinfo.errstring);
            goto end;
        }
    }
    //判断密码
    if(!cardinfo.vch_password.isEmpty()) {
        lds_dialog_input inputdialog(this);
        inputdialog.hideKey();
        inputdialog.ui->label->setText(QObject::tr("密码"));
        inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
        inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            goto end;
        }
        if(cardinfo.vch_password != inputdialog.ui->lineEdit->text()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            goto end;
        }
    }
    //判断卡状态
    if(cardinfo.vch_memberno.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡未发卡或不存在"));
        goto end;
    }
    if(cardinfo.ch_state == "4") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经作废"));
        goto end;
    }
    if(cardinfo.ch_state == "3") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经挂失"));
        goto end;
    }
    if(cardinfo.ch_state == "2") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("此卡已经止付"));
        goto end;
    }
    if(cardinfo.ch_state.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("非本公司卡或无效卡"));
        goto end;
    }

    ui->lineEdit_vch_booker->setText(cardinfo.vch_member);
    ui->lineEdit_vch_tel->setText(cardinfo.vch_handtel);
    ui->lineEdit_vch_tel_second->setText(cardinfo.vch_handtel);
end:
    return;
}

void w_scr_dish_restaurant_preorder_add_Dialog::tosave()
{

}

void w_scr_dish_restaurant_preorder_add_Dialog::tosetovertime()
{
    w_scr_dish_restaurant_preorder_set dialog(this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        //if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("是否更新最大翻台时间"), QObject::tr("是"), QObject::tr("否")))
        {
            ui->spinBox_int_overtime->setValue(n_func::f_get_sysparm("table_max_over_time", "240").toInt());
        }
    }
}

void w_scr_dish_restaurant_preorder_add_Dialog::toupdateWarndiff(const QDateTime &dt)
{
    Q_UNUSED(dt)
    ui->dateTimeEdit_dt_warn->setDateTime(ui->dateTimeEdit_dt_come->dateTime().addSecs(-ui->spinBox_warn_diff->value()*60));
}

void w_scr_dish_restaurant_preorder_add_Dialog::tosavewarndiff(int value)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_preorder_add_Dialog/warn_diff", value);
    toupdateWarndiff(ui->dateTimeEdit_dt_come->dateTime());
}

void w_scr_dish_restaurant_preorder_add_Dialog::todeltable()
{
    //    int row = ui->tableView->currentIndex().row();
    //    if(row >= 0) {
    //        standmodel_table->removeRow(row);
    //    }
}

void w_scr_dish_restaurant_preorder_add_Dialog::toupdateTabledesc()
{
    ui->label_tabledesc->setText(QObject::tr("当前台数") + ":" + QString::number(ui->tableView_table->get_usr_list(Qt::UserRole + 1).count()));
}

void w_scr_dish_restaurant_preorder_add_Dialog::torefresh()
{
    QString ch_bookno = ui->lineEdit_ch_bookno->text();
    if(ch_bookno.isEmpty()) return;

    lds_query_hddpos  query;
    query.execSelect(QString("select ch_bookno, vch_tables, dt_come, vch_booker, vch_tel, int_person, dt_operdate, vch_operID, vch_tel_second, vch_memo, dt_warn from cybr_book_master where ch_bookno = '%1' ")
                     .arg(ch_bookno));
    while(query.next()) {
        ui->lineEdit_ch_bookno->setText(query.recordValue("ch_bookno").toString());
        //        ui->lineEdit_vch_tables->setText(query.recordValue("vch_tables").toString());
        ui->dateTimeEdit_dt_come->setDateTime(query.recordValue("dt_come").toDateTime());
        ui->lineEdit_vch_booker->setText(query.recordValue("vch_booker").toString());
        ui->lineEdit_vch_tel->setText(query.recordValue("vch_tel").toString());
        ui->lineEdit_int_person->setText(query.recordValue("int_person").toString());
        //        ui->dateTimeEdit_dt_operdate->setDateTime(query.recordValue("dt_operdate").toDateTime());
        ui->comboBox_vch_operid->setCurrentIndex(backheader::set_index_of_interdata(query.recordValue("vch_operID").toString(), ui->comboBox_vch_operid));
        ui->lineEdit_vch_tel_second->setText(query.recordValue("vch_tel_second").toString());
        ui->lineEdit_vch_memo->setText(query.recordValue("vch_memo").toString());
        ui->dateTimeEdit_dt_warn->setDateTime(query.recordValue("dt_warn").toDateTime());
    }

    QStringList ch_tableno_list;
    query.execSelect(QString("select b.ch_tableno, b.vch_tablename FROM hddpos.cybr_book_table a, cey_bt_table b where a.ch_tableno = b.ch_tableno and a.ch_bookno = '%1' ")
                     .arg(ch_bookno));
    while(query.next()) {
        ch_tableno_list << query.recordValue("ch_tableno").toString();
    }
    ui->tableView_table->loadTableList(ch_tableno_list);
    //    3
    toupdateTabledesc();
}

bool w_scr_dish_restaurant_preorder_add_Dialog::saveData()
{
    if(ui->tableView_table->selectionModel()->selectedIndexes().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有桌台被选中"));
        return false;
    }

    if(ui->comboBox_vch_operid->currentText().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("预订人不能为空"));
        return false;
    }
    if(ui->lineEdit_vch_booker->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("预订人不能为空"));
        return false;
    }

    if(ui->lineEdit_vch_tel->text().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("预订人电话不能为空"));
        return false;
    }

    if(ui->lineEdit_int_person->text().toInt() < ui->tableView_table->selectionModel()->selectedIndexes().count())  {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("人数不能小于餐桌数"));
        return false;
    }
    if(ui->dateTimeEdit_dt_warn->dateTime().secsTo(ui->dateTimeEdit_dt_come->dateTime()) < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("预警时间不能大于预到时间"));
        return false;
    }
    if((ui->dateTimeEdit_dt_warn->dateTime().secsTo(ui->dateTimeEdit_dt_come->dateTime()) /60) > ui->spinBox_int_overtime->value()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("预到时间-预警时间不能大于最大翻台时间"));
        return false;
    }
    //桌台可分配空间
    lds_query_hddpos  query;
    QString  ch_bookno = ui->lineEdit_ch_bookno->text();
    QDateTime dt = n_func::f_get_sysdatetime();
    QString vch_tables;
    int table_max_over_time = n_func::f_get_sysparm("table_max_over_time", "240").toInt();
    QString vch_tables_failure;

    QStringList tablenolist = ui->tableView_table->get_usr_list(Qt::UserRole + 1/*ch_tableno*/);
    foreach (const QString &ch_tableno, tablenolist) {
        QDateTime dt_come;
        QDateTime dt_come_left;
        QDateTime dt_come_right;
        //1
        if(ch_bookno.isEmpty()) {
            query.execSelect(QString("select  max(a.dt_come) FROM cybr_book_master a, cybr_book_table b where a.ch_bookno = b.ch_bookno and ifnull(a.ch_state,'') <> 'Y' and b.ch_tableno = '%1' ")
                             .arg(ch_tableno)
                             );
            query.next();
            dt_come_left = query.recordValue(0).toDateTime().addSecs(table_max_over_time * 60);
        } else {
            query.execSelect(QString("select  max(a.dt_come) FROM cybr_book_master a, cybr_book_table b where a.ch_bookno = b.ch_bookno and ifnull(a.ch_state,'') <> 'Y'  and b.ch_tableno = '%1' "
                                     " and dt_come <  (select a.dt_come FROM cybr_book_master a, cybr_book_table b where a.ch_bookno = b.ch_bookno and ifnull(a.ch_state,'') <> 'Y'  and b.ch_tableno = '%1' and a.ch_bookno = '%2' )"
                                     )
                             .arg(ch_tableno)
                             .arg(ch_bookno)
                             );
            query.next();
            dt_come_left = query.recordValue(0).toDateTime().addSecs(table_max_over_time * 60);

            query.execSelect(QString("select  min(a.dt_come) FROM cybr_book_master a, cybr_book_table b where a.ch_bookno = b.ch_bookno and ifnull(a.ch_state,'') <> 'Y'  and b.ch_tableno = '%1' "
                                     " and dt_come >  (select a.dt_come FROM cybr_book_master a, cybr_book_table b where a.ch_bookno = b.ch_bookno and ifnull(a.ch_state,'') <> 'Y'  and b.ch_tableno = '%1' and a.ch_bookno = '%2' )"
                                     )
                             .arg(ch_tableno)
                             .arg(ch_bookno)
                             );
            query.next();
            dt_come_right = query.recordValue(0).toDateTime().addSecs(-table_max_over_time * 60);
        }
        //3
        dt_come = ui->dateTimeEdit_dt_come->dateTime();
        //4
        vch_tables += ch_tableno+",";

        if(!checkDtcomePeriod(dt_come, dt_come_left, dt_come_right)) {
            vch_tables_failure += ch_tableno+",";
        }
    }
    if(!vch_tables_failure.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID,
                                QString("%1:%2,%3,%4:%5%6")
                                .arg(QObject::tr("餐桌"))
                                .arg(vch_tables_failure)
                                .arg(QObject::tr("最大翻台时间内已经被预订"))
                                .arg(QObject::tr("最大翻台时间"))
                                .arg(table_max_over_time)
                                .arg(QObject::tr("分钟"))
                                );
        return false;
    }


    //ch_bookno 这个不管该单是否已经被预订，获取最大单号，以免重复
    if(ch_bookno.isEmpty()) {
        query.execSelect(QString("select max(ch_bookno) from cybr_book_master where ch_bookno like '%1%' ")
                         .arg("B"+dt.toString("yyMMdd")+"-"));
        query.next();
        ch_bookno = query.recordValue(0).toString();
        if(ch_bookno.isEmpty()) {
            ch_bookno = "B"+dt.toString("yyMMdd")+"-"+"000";
        } else {
            ch_bookno = ch_bookno.left(8)+QString().sprintf("%04d", ch_bookno.right(3).toInt()+1);
        }
    }
    //transaction
    lds_query_hddpos::tran_saction();
    if(query.execDelete("cybr_book_master", QString("ch_bookno = '%1' ")
                        .arg(ch_bookno))) {
        if(query.execInsert("cybr_book_master",qrtVariantPairList()

                            << qrtVariantPair("ch_bookno", ch_bookno)<< qrtVariantPair("vch_tables", vch_tables)<< qrtVariantPair("dt_come", ui->dateTimeEdit_dt_come->dateTime())<< qrtVariantPair("vch_booker", ui->lineEdit_vch_booker->text())<< qrtVariantPair("vch_tel", ui->lineEdit_vch_tel->text())
                            << qrtVariantPair("int_person", ui->lineEdit_int_person->text())<< qrtVariantPair("dt_operdate", dt)<< qrtVariantPair("vch_operID", backheader::getinterdata(ui->comboBox_vch_operid->currentText()))<< qrtVariantPair("vch_tel_second", ui->lineEdit_vch_tel_second->text())<< qrtVariantPair("vch_memo", ui->lineEdit_vch_memo->text())
                            << qrtVariantPair("dt_warn", ui->dateTimeEdit_dt_warn->dateTime())<< qrtVariantPair("ch_type", ""))) {
            if(query.execDelete("cybr_book_table", QString("ch_bookno = '%1' ")
                                .arg(ch_bookno))) {
                QStringList tablenolist = ui->tableView_table->get_usr_list(Qt::UserRole + 1);
                foreach (const QString &ch_tableno, tablenolist) {
                    if(query.execInsert("cybr_book_table", qrtVariantPairList()
                                        << qrtVariantPair("ch_bookno", ch_bookno)<< qrtVariantPair("ch_tableno", ch_tableno))) {
                        continue;
                    }
                    goto failure;
                }
                lds_query_hddpos::com_mit();
                ui->lineEdit_ch_bookno->setText(ch_bookno);
                torefresh();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                return true;
            }
        }
    }
failure:
    lds_query_hddpos::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("数据库错误"));
    return false;
}

bool w_scr_dish_restaurant_preorder_add_Dialog::checkDtcomePeriod(const QDateTime &dt_come, const QDateTime &dt_come_left, const QDateTime &dt_come_right)
{
    if(dt_come_left.isValid() || dt_come_right.isValid()) {
        if(dt_come_left.isValid() && dt_come_right.isValid()) {
            return (dt_come >= dt_come_left && dt_come <= dt_come_right);
        }  else if(dt_come_left.isValid()) {
            return (dt_come >= dt_come_left);
        }  else {
            return (dt_come <= dt_come_right);
        }
    }
    return true;
}


/////////////////////////////////////
/// \brief w_scr_dish_restaurant_preorder_set::w_scr_dish_restaurant_preorder_set
/// \param parent
///
#include "ui_w_scr_dish_restaurant_preorder_set.h"
w_scr_dish_restaurant_preorder_set::w_scr_dish_restaurant_preorder_set(QWidget *parent)
    : QDialog(parent),
      ui(new Ui_w_scr_dish_restaurant_preorder_set)
{
    ui->setupUi(this);
    ui->spinBox->setValue(n_func::f_get_sysparm("table_max_over_time", "240").toInt());
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
}

w_scr_dish_restaurant_preorder_set::~w_scr_dish_restaurant_preorder_set()
{

}

void w_scr_dish_restaurant_preorder_set::took()
{
    n_func::f_set_sysparm("table_max_over_time", ui->spinBox->value(), "最大翻台时间(软件自动添加)");
    this->accept();
}

void w_scr_dish_restaurant_preorder_set::tocancel()
{
    this->reject();
}
