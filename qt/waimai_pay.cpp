#include "waimai_pay.h"
#include "ui_waimai_pay.h"
#include <QSqlRecord>
#include "lds_messagebox.h"
#include "other_pay_view.h"
#include "bank_card.h"
#include "lds_query_hddpos.h"
#include <QDateTime>
#include "n_func.h"
#include "take_cashier.h"
#include <QSqlError>
#include "backheader.h"
#include "lds_messagebox.h"
#include "public_printby_ch_billno.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_scr_dish_pay_widget.h"

waimai_pay::waimai_pay(const QString &xiaofei,const QString &zhekou,const QString &zengsong,const QString &moling,const QString &shuishou,const QString &billno,const QString &waimaihao,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::waimai_pay)
{
    ui->setupUi(this);

    _xiaofei = xiaofei;
    _zhekou = zhekou;
    _zengsong = zengsong;
    _moling = moling;
    _shuishou = shuishou;
    _billno = billno;
    _waimaihao = waimaihao.trimmed();

    kafujine = 0;
    shifoujiazhang = _waimaihao;
    btn_t_f = false;
    init_control();
    init_data();
}

waimai_pay::~waimai_pay()
{
    delete ui;
}

void waimai_pay::init_control()
{
    ui->lineEdit_consum->setEnabled(false);
    ui->lineEdit_discount->setEnabled(false);
    ui->lineEdit_present->setEnabled(false);
    ui->lineEdit_realreceive->setEnabled(false);
    ui->lineEdit_blotout->setEnabled(false);
    ui->lineEdit_int_rate->setEnabled(false);
    ui->lineEdit_change->setEnabled(false);
    ui->lineEdit_9->setFocus();

    table_model = new QStandardItemModel();
    QStringList list;
    list <<QObject::tr("行号")<<QObject::tr("付款方式")<<QObject::tr("实付现金")<<QObject::tr("付款金额")<<QObject::tr("凭证号");
    table_model->setHorizontalHeaderLabels(list);
    ui->tableView->setModel(table_model);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
}

void waimai_pay::init_data()
{
    ui->lineEdit_consum->setValue(_xiaofei.toDouble());
    ui->lineEdit_discount->setValue(_zhekou.toDouble()); ui->lineEdit_discount->enableShowNegativeValue();
    ui->lineEdit_present->setValue(_zengsong.toDouble()); ui->lineEdit_present->enableShowNegativeValue();
    ui->lineEdit_blotout->setValue(_moling.toDouble()); ui->lineEdit_blotout->enableShowNegativeValue();
    ui->lineEdit_int_rate->setValue(_shuishou.toDouble());

    double yingshou = 0;
    yingshou = _xiaofei.toDouble() - _zhekou.toDouble() - _zengsong.toDouble() - _moling.toDouble() + _shuishou.toDouble();
    ui->lineEdit_realreceive->setValue(yingshou);

    ui->lineEdit_9->setValue(yingshou);

    ui->lineEdit_change->setValue(0);
    ui->label_12->setValue(0);
    ui->label_14->setValue(yingshou);
}

void waimai_pay::on_pushButton_21_clicked()//取消
{
    btn_t_f = false;
    this->close();
}

void waimai_pay::on_pushButton_19_clicked()//代金券
{
    float label_14 = 0;
    label_14 = ui->label_14->value();
    if(ui->label_14->value() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已足额付款无需继续付款"));
        return;
    }

    QString ch_paymodeno = "04";
    QString vch_paymodename = QObject::tr("代金券");
    bank_card dialog(ch_paymodeno, ui->label_14->value(), ui->lineEdit_consum->value(), this);
    dialog.setWindowTitle(vch_paymodename);
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        kafujine = dialog.getRealPay();
        fukuanpingzheng = dialog.getVoucher();
        if(ui->label_14->value() <= dialog.getRealPay())  {
            ui->lineEdit_9->setValue(0);
            float l_12 = 0;
            l_12 = ui->label_14->value() + ui->label_12->value();
            ui->label_12->setValue((l_12));
            ui->label_14->setValue(0);
        } else {
            float yue = 0;
            yue = ui->label_14->value() - dialog.getRealPay();
            ui->lineEdit_9->setValue((yue));
            ui->label_12->setValue((dialog.getRealPay() + ui->label_12->value()));
            ui->label_14->setValue((yue));
        }
    } else
    {
        return;
    }

    int row = table_model->rowCount();
    table_model->setRowCount(row + 1);
    float t_shifuxianjin = 0;
    float t_fukuanjie = 0;
    if(label_14 <= kafujine) {//付清
        t_shifuxianjin = label_14;
        t_fukuanjie = kafujine;
    } else//未付清
    {
        t_shifuxianjin = kafujine;
        t_fukuanjie = kafujine;
    }

    table_model->setItem(row,0,new QStandardItem(QString::number(row+1))); //序号
    table_model->setItem(row,1,new QStandardItem(vch_paymodename));
    table_model->setItem(row,2,new QStandardItem(QString().sprintf("%0.2f",t_shifuxianjin)));
    table_model->setItem(row,3,new QStandardItem(QString().sprintf("%0.2f",t_fukuanjie)));
    table_model->setItem(row,4,new QStandardItem(fukuanpingzheng));
    table_model->setItem(row,5,new QStandardItem(ch_paymodeno));
    table_model->setItem(row,6,new QStandardItem(dialog.getMemo()));
    ui->tableView->selectRow(row);

#ifdef QT_DEBUG
#else
    ui->tableView->setColumnHidden(5,true);
    ui->tableView->setColumnHidden(6,true);
    ui->tableView->setColumnHidden(7,true);
#endif
}

void waimai_pay::on_pushButton_20_clicked()//其他付款
{
    float label_14 = 0;
    label_14 = ui->label_14->value();

    if(ui->label_14->value() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("已足额付款无需继续付款"));
        
        return;
    }
    other_pay_view pay_dialog("2", this);
    pay_dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&pay_dialog).exec();
    if(pay_dialog.btn_t_f == true)  {
        QString vch_paymodename = pay_dialog.payname[pay_dialog.curindex_id];
        QString ch_paymodeno = pay_dialog.payfanshi[pay_dialog.curindex_id];

        bank_card dialog(ch_paymodeno, ui->label_14->value(), ui->lineEdit_consum->value(), this);
        dialog.setWindowTitle(pay_dialog.payname[pay_dialog.curindex_id]);
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            kafujine = dialog.getRealPay();
            fukuanpingzheng = dialog.getVoucher();
            if(ui->label_14->value() <= dialog.getRealPay())   {
                ui->lineEdit_9->setValue(0);
                float l_12 = 0;
                l_12 = ui->label_14->value() + ui->label_12->value();
                ui->label_12->setValue((l_12));
                ui->label_14->setValue(0);
            }   else
            {
                float yue = 0;
                yue = ui->label_14->value() - dialog.getRealPay();
                ui->lineEdit_9->setValue((yue));
                ui->label_12->setValue((dialog.getRealPay() + ui->label_12->value()));
                ui->label_14->setValue((yue));
            }
        }   else
        {
            return;
        }

        int row = table_model->rowCount();
        table_model->setRowCount(row + 1);
        float t_shifuxianjin = 0;
        float t_fukuanjie = 0;
        if(label_14 <= kafujine) {//付清
            t_shifuxianjin = label_14;
            t_fukuanjie = kafujine;
        }   else//未付清
        {
            t_shifuxianjin = kafujine;
            t_fukuanjie = kafujine;
        }

        table_model->setItem(row,0,new QStandardItem(QString::number(row+1))); //序号
        table_model->setItem(row,1,new QStandardItem(vch_paymodename));
        table_model->setItem(row,2,new QStandardItem(QString().sprintf("%0.2f",t_shifuxianjin)));
        table_model->setItem(row,3,new QStandardItem(QString().sprintf("%0.2f",t_fukuanjie)));
        table_model->setItem(row,4,new QStandardItem(fukuanpingzheng));
        table_model->setItem(row,5,new QStandardItem(ch_paymodeno));
        table_model->setItem(row,6,new QStandardItem(dialog.getMemo()));
        ui->tableView->selectRow(row);
#ifdef QT_DEBUG
#else
        ui->tableView->setColumnHidden(5,true);
        ui->tableView->setColumnHidden(6,true);
        ui->tableView->setColumnHidden(7,true);
#endif
    }

}

void waimai_pay::on_pushButton_13_clicked()//买单
{
    if(shifoujiazhang != _waimaihao)  {
        return;
    }
    if(ui->lineEdit_9->value() + ui->label_12->value() < ui->lineEdit_realreceive->value()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("对不起,您的付款金额不够"));
        
        return;
    }

    QString ch_payno;
    QDateTime cur_dt = n_func::f_get_sysdatetime();

    ch_payno = lds_scr_table::get_max_ch_payno_add1(cur_dt);
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    QString zhuohao = "Q000";

    //付款明细
    if(query.execInsert("cey_u_checkout_master",qrtVariantPairList()
                        << qrtVariantPair("ch_payno", ch_payno)
                        << qrtVariantPair("ch_billno", _billno)
                        << qrtVariantPair("ch_tableno", zhuohao)
                        << qrtVariantPair("num_cost", ui->lineEdit_consum->value())
                        << qrtVariantPair("num_discount", ui->lineEdit_discount->value())

                        << qrtVariantPair("num_present", ui->lineEdit_present->value())
                        << qrtVariantPair("num_service", 0)
                        << qrtVariantPair("num_lowcost", 0)
                        << qrtVariantPair("num_blotout", ui->lineEdit_blotout->value())
                        << qrtVariantPair("num_run", 0)

                        << qrtVariantPair("vch_run_operID", "")
                        << qrtVariantPair("ch_state", "Y")
                        << qrtVariantPair("vch_operID", lds::gs_operid)
                        << qrtVariantPair("dt_operdate", cur_dt)
                        << qrtVariantPair("vch_operID2", QVariant())

                        << qrtVariantPair("dt_operdate2", QVariant())
                        << qrtVariantPair("num_rate", ui->lineEdit_int_rate->value())
                        ))
    {

    }
    else
    {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }


    //现金付款
    if(ui->label_14->value() != 0)  {
        float aa = 0;
        aa = ui->lineEdit_9->value() - ui->lineEdit_change->value();
        if(!query.execInsert("cey_u_checkout_detail", qrtVariantPairList()
                             << qrtVariantPair("ch_payno", ch_payno)
                             << qrtVariantPair("ch_paymodeno", "00")
                             << qrtVariantPair("num_payamount", ui->lineEdit_9->value())
                             << qrtVariantPair("num_change", ui->lineEdit_change->value())
                             << qrtVariantPair("num_realamount", QString().sprintf("%0.2f",aa))

                             << qrtVariantPair("num_face", 0)
                             << qrtVariantPair("vch_operID", lds::gs_operid)
                             << qrtVariantPair("dt_operdate", cur_dt)
                             << qrtVariantPair("num_card_remain", 0)
                             << qrtVariantPair("vch_voucherno", QVariant())

                             << qrtVariantPair("num_face_Num", 0)))
        {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            
            return;
        }
    }

    if(ui->lineEdit_blotout->value() != 0) {//存在自动抹零
        if(query.execInsert("cey_u_checkout_detail",qrtVariantPairList()
                            << qrtVariantPair("ch_payno", ch_payno)
                            << qrtVariantPair("ch_paymodeno", "YY")
                            << qrtVariantPair("num_payamount", ui->lineEdit_blotout->value())
                            << qrtVariantPair("num_change", 0)
                            << qrtVariantPair("num_realamount", ui->lineEdit_blotout->value())

                            << qrtVariantPair("num_face", 0)
                            << qrtVariantPair("vch_operID", lds::gs_operid)
                            << qrtVariantPair("dt_operdate", cur_dt)
                            << qrtVariantPair("num_card_remain", 0)
                            << qrtVariantPair("vch_voucherno", QVariant())

                            << qrtVariantPair("num_face_Num", 0)))
        {

        }
        else
        {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());

            return;
        }
    }


    for(int i = 0; i < table_model->rowCount(); i++)
    {
        QString pingzheng = table_model->item(i,4)->text();
        if(pingzheng.trimmed().isEmpty()) {
            pingzheng = "null";
        }

        if(query.execInsert("cey_u_checkout_detail", qrtVariantPairList()
                            << qrtVariantPair("ch_payno", ch_payno)
                            << qrtVariantPair("ch_paymodeno", table_model->item(i,5)->text())
                            << qrtVariantPair("num_payamount", table_model->item(i,2)->text())
                            << qrtVariantPair("num_change", 0)
                            << qrtVariantPair("num_realamount", table_model->item(i,3)->text())

                            << qrtVariantPair("num_face", 0)
                            << qrtVariantPair("vch_operID", lds::gs_operid)
                            << qrtVariantPair("dt_operdate", cur_dt)
                            << qrtVariantPair("num_card_remain", 0)
                            << qrtVariantPair("vch_voucherno", pingzheng)

                            << qrtVariantPair("num_face_Num", 0)))
        {

        }
        else
        {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());

            return;
        }
    }

    //付款成功
    if(query.execUpdate("cey_u_table","ch_payno", ch_payno,qrtEqual("ch_billno", _billno))) {
    }
    else
    {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());

        return;
    }
    if(!query.execUpdate("cey_u_orderdish",qrtVariantPairList()
                         << qrtVariantPair("ch_payno", ch_payno),
                         QString("ch_togono = '%1' ").arg(_waimaihao))) {

        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }

    if(query.execUpdate("cey_u_togo",qrtVariantPairList()
                        << qrtVariantPair("ch_payno", ch_payno) << qrtVariantPair("ch_state", "Y"),
                        qrtEqual("ch_togono", _waimaihao)))
    {

    }
    else
    {
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
        return;
    }

    lds_query_hddpos::com_mit();
    pay_print();
    shifoujiazhang.clear();
    btn_t_f = true;
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("外卖交易成功"));
    this->close();
    return;
}

void waimai_pay::on_lineEdit_9_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    {
        float xianjin = 0;
        xianjin = ui->lineEdit_9->value();
        float yingshou = 0;
        yingshou = ui->label_14->value();
        if(xianjin > yingshou)   {
            ui->lineEdit_change->setValue(xianjin - yingshou);
        }
        else    {
            ui->lineEdit_change->setValue(0);
        }
    }
}

void waimai_pay::print_lost(const QString &str)
{
    ;
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,str);

}

void waimai_pay::pay_print()
{
    //开钱箱
    if(ui->lineEdit_9->value() != 0  //现金不为0
            && lds::sysconf->value("system_setting/qianxiang", true).toBool()) {
        public_printby_ch_billno::openDrawer();
    }
    //根据单号
    public_printby_ch_billno printer(_billno);
    printer.toprintwaimai_pay(printer.defpath(), n_func::f_get_sysparm("bp_togo_count", "1").toInt());
}

void waimai_pay::on_pushButton_11_clicked()//<<
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        ui->lineEdit_9->setValue(b->text().toDouble());
        ui->lineEdit_9->selectAll();
    }
}

void waimai_pay::on_pushButton_7_clicked()//100
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        ui->lineEdit_9->setValue(b->text().toDouble());
        ui->lineEdit_9->selectAll();
    }
}

void waimai_pay::on_pushButton_4_clicked()//50
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    if(b) {
        ui->lineEdit_9->setValue(b->text().toDouble());
        ui->lineEdit_9->selectAll();
    }
}
