#include "business_documents_query.h"
#include "lds_tableview.h"
#include "lds_tableview_delegate_check.h"
#include "lds_messagebox.h"
#include "lds_messagebox.h"
#include "lds_query_hddpos.h"
#include "n_func.h"
#include <QSqlRecord>
#include "lds_messagebox.h"
#include <QDesktopWidget>
#include <QApplication>
#include "lds_tableview_delegate_check.h"
#include <QSqlError>
#include "backheader.h"
#include "printer_0.h"
#include "uo_rw_member.h"
#include "frontviewdialog_virtual.h"
#include "public_logindialog.h"
#include "public_printby_ch_billno.h"
#include "w_rpt_report.h"
#include "fexpandmain_2.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "public_sql.h"
#include "w_bt_dish.h"
#include "w_scr_dish_pay_widget.h"
#include "lds_pushbuttonlock.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_rpt_report_dataexport.h"
#include "w_rpt_report_printformat.h"
#include "w_scr_dish_pay_widget.h"
#include "w_scr_dish_main.h"
#include "w_scr_dish_main_table_800x600.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "w_scr_dish_main_table_pay.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "fexpandmain_delegate_vch_printmemo.h"
#include "ui_business_documents_query.h"

//static void showInsTableview(const blockIns &ins)
//{

//    QTableView *tableView = new QTableView;
//    //    tableView->setAttribute(Qt::WA_DeleteOnClose);

//    QStandardItemModel *model = new QStandardItemModel(tableView);
//    tableView->horizontalHeader()->setDefaultSectionSize(20);
//    tableView->verticalHeader()->setDefaultSectionSize(20);
//    tableView->horizontalHeader()->hide();
//    tableView->verticalHeader()->hide();
//    //    tableView->setShowGrid(false);
//    tableView->setModel(model);
//    QList<QStandardItem*>  items;
//    QSet<QStandardItem *> span_item_set;
//    foreach(const pieceIns & p, ins) {

//        QList<QByteArray> plist = p.first.split(',');
//        foreach(QByteArray p, plist) {
//            QByteArray pins = p.mid(1);

//            //
//            if(p == "-" && !items.isEmpty()) {//分隔线
//                model->appendRow(items);
//                items.clear();
//                for(int k = 0; k < 48; k++) {
//                    items << new QStandardItem("-");
//                }
//                model->appendRow(items);
//                items.clear();
//            } else if(p.startsWith("i")) {//初始化
//            } else if(p.startsWith("a")) {//对齐方式
//                if(pins == "10") {

//                }
//                if(pins == "00") {

//                }
//                if(pins == "01") {

//                }
//            } else if(p.startsWith("s")) {//倍高倍宽
//                if(pins == "00") {

//                }
//                if(pins == "01") {

//                }
//                if(pins == "10") {

//                }
//                if(pins == "11") {

//                }

//            }  else if(p.startsWith("b")) {//加粗
//            } else {
//            }
//        }



//        foreach(const QChar &c, QString(p.second)) {
//            if(items.count() >= 48 || QString(c) == "\n") {
//                model->appendRow(items);
//                items.clear();

//                if(QString(c) == "\n")
//                    continue;
//            }

//            if(QByteArray().append(c).length() == 1) {
//                items << new QStandardItem(QString(c));
//            } else {
//                items << new QStandardItem(QString(c));
//                span_item_set.insert(items.last());
//                items << new QStandardItem();
//            }
//        }
//    }

//    foreach(QStandardItem *item, span_item_set) {
//        tableView->setSpan(item->row(), item->column(), 1, 2);
//    }

//    tableView->show();
//}



business_documents_query::business_documents_query(QWidget *parent, bool just_quitbill) :
    QDialog(parent),
    ui(new Ui_business_documents_query)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    model_top = new report_querymodel(this);
    model_left = new report_querymodel(this);
    model_right = new report_querymodel(this);

    ui->tableView->setTransferHheader();
    ui->tableView->setModel(model_top);
    ui->tableView_2->setTransferHheader();
    ui->tableView_2->setModel(model_left);
    ui->tableView_3->setTransferHheader();
    ui->tableView_3->setModel(model_right);

    ui->lineEdit->setPlaceholderText(QObject::tr("付款号或单号"));
    if(false == just_quitbill)
        lds::hideWidget(ui->lineEdit);


    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));

    connect(ui->tableView->tableView, SIGNAL(selectChanged(QModelIndex)),this,SLOT(totableview(QModelIndex)));
    connect(ui->pushButton_export, SIGNAL(clicked()), this, SLOT(toexport()));
    connect(ui->pushButton_print, SIGNAL(clicked()), this, SLOT(toprint()));
    connect(ui->pushButton_refund, SIGNAL(clicked()), this, SLOT(torefund()));
    connect(ui->pushButton_search, SIGNAL(clicked()), this, SLOT(torefresh()));
    connect(ui->pushButton_uncheck, SIGNAL(clicked()), this, SLOT(touncheck()));
    connect(ui->pushButton_exit, SIGNAL(clicked()), this, SLOT(toexit()));
    connect(ui->pushButton_print2, SIGNAL(clicked()), this, SLOT(toprint2()));
    connect(ui->checkBox_cancel, SIGNAL(clicked(bool)), this, SLOT(torefresh()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(torefresh()));

    ui->pushButton_print->hide();
    if(just_quitbill) {
        ui->pushButton_export->hide();
        ui->pushButton_print->hide();
        ui->pushButton_uncheck->hide();
        ui->pushButton_print2->hide();
        ui->checkBox_cancel->hide();
        setWindowTitle(QObject::tr("退单"));
        QTimer::singleShot(0, this, SLOT(torefresh()));
    } else {
        setWindowTitle(QObject::tr("营业数据按单查询"));
        QTimer::singleShot(0, this, SLOT(right_check_and_init()));
    }
}

business_documents_query::~business_documents_query()
{
    delete ui;
}

void business_documents_query::showLoginerContent()
{
    if(lds::gs_operid == "0000") {
        is_show_login_data= false;
        return;
    }
    lds_query_hddpos  query;
    query.execSelect(QString("select * from cey_bt_dish_type_oper where ch_state = 'Y' and vch_operid = '%1' ;").arg(lds::gs_operid));
    is_show_login_data = query.next();
}

void business_documents_query::right_check_and_init()
{
    if(false == backheader::operate_right_check_code(this, "0201")) {//营业数据按单查询
        this->reject();
        return;
    }
    torefresh();
}

void business_documents_query::toexit()
{
    this->reject();
}

void business_documents_query::torefresh()//检索
{
    lds_widget_auto_enable d(ui->pushButton_search);//强制置灰，析构后还原

    model_top->setQuery(
                (
                    QString()
                    +"SELECT "
                    "ch_payno, "
                    "ch_billno,"
                    "ch_tableno, "
                    "IFNULL((Select vch_tablename from cey_bt_table where ch_tableno = cey_u_checkout_master.ch_tableno),'Q000') as vch_tablename,"
                    "IFNULL((Select int_person from cey_u_table where ch_billno = cey_u_checkout_master.ch_billno), 0) as int_person,"
                    "num_cost,"
                    "num_present,"
                    "num_discount,"
                    "num_rate,"
                    "num_blotout,"
                    "num_service,"
                    "num_lowcost,"
                    "num_cash,"
                    "vch_operID,"
                    "dt_operdate, "
                    "ch_state, "
                    "ch_tableno,"
                    "vch_memberno"
                    " FROM cey_u_checkout_master "
                    "Where dt_operdate >= '%1' and dt_operdate <= '%2' and "
                    +(ui->checkBox_cancel->isChecked() ? "ch_state = 'P'" : "ch_state = 'Y'")
                    +(is_show_login_data ? QString(" and vch_operid = '%1' ").arg(lds::gs_operid) : QString(""))
                    )
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                + " and " + QString("( ch_billno like '%%1%' " ).arg(ui->lineEdit->text())
                + " or  " + QString(" ch_payno like '%%1%' )" ).arg(ui->lineEdit->text())
                +" order by dt_operdate desc");

    ui->tableView->updateSum(model_top->fieldIndex("int_person"));
    ui->tableView->updateSum(model_top->fieldIndex("num_cost"));
    ui->tableView->updateSum(model_top->fieldIndex("num_present"));
    ui->tableView->updateSum(model_top->fieldIndex("num_discount"));
    ui->tableView->updateSum(model_top->fieldIndex("num_rate"));
    ui->tableView->updateSum(model_top->fieldIndex("num_blotout"));
    ui->tableView->updateSum(model_top->fieldIndex("num_service"));
    ui->tableView->updateSum(model_top->fieldIndex("num_lowcost"));
    ui->tableView->updateSum(model_top->fieldIndex("num_cash"));
    ui->tableView->restore(0);
    ui->tableView->resizeColumnsToContents();

    //
    lds_query_hddpos  query;
    query.execSelect(QString("select ch_billno from cey_sys_lan_terminal where ch_billno <> '' and vch_terminal = '%1';")
                     .arg(lds::terminalCode));
    ui->pushButton_uncheck->show_flag_top_right(query.next());
}

void business_documents_query::totableview(const QModelIndex &index)
{
    //商品消费合计
    model_left->setQuery(QString(
                             "SELECT "
                             "a.ch_billno,"
                             "b.vch_dishname,"
                             "a.num_price,"
                             "a.num_num - a.num_back as num_dish,"
                             "c.vch_unitname,"
                             "a.num_price * (a.num_num - a.num_back)  as num_total,"
                             "a.num_price * (a.num_num - a.num_back) * a.int_discount * 0.01 as num_total2,"
                             "a.vch_print_memo,"
                             "a.num_price_add,"
                             "a.num_price * (a.num_num - a.num_back) * a.int_discount * 0.01+a.num_price_add as num_total3,"
                             "a.int_discount,"
                             "a.vch_operID,"
                             "a.dt_operdate,"
                             "a.ch_suitflag,"
                             "a.ch_specialflag,"
                             "a.ch_presentflag,"
                             "a.vch_back_operID,"
                             "a.dt_back_operdate,"
                             "a.vch_dis_operID,"
                             "a.dt_dis_operdate, "
                             "a.vch_pre_operID,"
                             "a.dt_pre_operdate, "
                             "a.int_flowID "
                             "FROM cey_bt_dish b, cey_u_orderdish a, cey_bt_unit c  "
                             "WHERE (c.ch_unitno=b.ch_unitno) and(a.ch_dishno = b.ch_dishno)and( a.ch_billno = '%1')")
                         .arg(model_top->model_data(index.row(), "ch_billno").toString()));

    ui->tableView_2->resizeColumnsToContents();
    ui->tableView_2->restore(0);
    if(ui->tableView_2->itemDelegateForColumn(model_left->fieldIndex("vch_print_memo")) == 0) {
        ui->tableView_2->setItemDelegateForColumn(model_left->fieldIndex("vch_print_memo"), new fexpandmain_delegate_vch_printmemo(this));
    }
    if(ui->tableView_2->itemDelegateForColumn(model_left->fieldIndex("ch_suitflag")) == 0) {
        ui->tableView_2->setItemDelegateForColumn(model_left->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(
                                                      this, ldsVariantMap("P", QObject::tr("主商品"), "Y", QObject::tr("子商品"))
                                                      ));
    }

    //消费明细表
    model_right->setQuery(QString(
                              "SELECT a.ch_payno,"
                              "b.vch_paymodename,"
                              "a.num_realamount,"
                              "a.num_payamount,"
                              "a.num_change,"
                              "a.vch_voucherno,"
                              "a.vch_memo  "
                              "FROM cey_u_checkout_detail a, cey_bt_paymode b where a.ch_paymodeno = b.ch_paymodeno and a.ch_payno = '%1'")
                          .arg(model_top->model_data(index.row(), "ch_payno").toString()));
    ui->tableView_3->resizeColumnsToContents();
    ui->tableView_3->restore(0);
}

void business_documents_query::toprint2()//打印
{
    int row=ui->tableView->currentIndex().row();
    if(row>=0) {
        QString ch_billno=model_top->record(row).value("ch_billno").toString();//PXXX
        public_printby_ch_billno print_ch_billno(ch_billno);
        print_ch_billno.toprint(lds::sysconf->value("system_setting/printerpath").toString(), 1, public_printby_ch_billno::payReprint);
    }
}

int business_documents_query::f_member_delete(const QString &as_payno, const QString &as_operid)
{
    //Public function f_member_delete (string as_payno,string as_operid) returns integer
    //string as_payno
    //string as_operid
    QString ls_paymodeno;
    QString ls_memberno;
    QString ls_parm;
    //    int li_flowid;
    int li_exist;
    float ldec_pay;
    float ldec_remain;
    float ldec_point;
    //    float ldec_num_point;
    lds_query_hddpos  cur_mpay;
    lds_query_hddpos  query;
    QString KenShuChooseCase_1;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    int li_rate;
    float ldec_backcash;

    cur_mpay.execSelect(QString("SELECT a.vch_voucherno,  a.num_payamount,  a.ch_paymodeno,  ifnull( (select b.num_point from t_m_point b "
                                " where a.ch_payno =b.ch_payno and a.vch_voucherno =b.vch_memberno ),  0 ) as num_point"
                                " from cey_u_checkout_detail a"
                                " where a.ch_payno = '%1' and a.ch_paymodeno in ( '01',  '02' )")
                        .arg(as_payno));

    while(cur_mpay.next()) {


        ls_memberno = cur_mpay.recordValue("vch_voucherno").toString();
        ldec_pay = cur_mpay.recordValue("num_payamount").toFloat();
        ls_paymodeno = cur_mpay.recordValue("ch_paymodeno").toString();
        ldec_point = cur_mpay.recordValue("num_point").toFloat();


        KenShuChooseCase_1 = ls_paymodeno;
        if("01" == KenShuChooseCase_1) {

            //INTO :li_exist
            query.execSelect(QString("SELECT count(vch_memberno ) "
                                     " from t_m_pay "
                                     " where ch_payno ='%1' and vch_memberno = '%2'  "
                                     )
                             .arg(as_payno)
                             .arg(ls_memberno));
            query.next();
            li_exist = query.recordValue(0).toLongLong();
            if(li_exist == 0) {
                continue;
            }

            if(!query.execInsert("t_m_pay",qrtVariantPairList()
                                 << qrtVariantPair("vch_memberno", ls_memberno)
                                 << qrtVariantPair("ch_payno", as_payno)
                                 << qrtVariantPair("ch_paymodeno", ls_paymodeno)
                                 << qrtVariantPair("num_amount", 0)
                                 << qrtVariantPair("num_pay", 0 - ldec_pay)
                                 << qrtVariantPair("vch_operID", as_operid)
                                 << qrtVariantPair("ch_balanceno", QVariant())
                                 << qrtVariantPair("int_checkflow", 0)
                                 << qrtVariantPair("dt_operdate", qrtFunc("curdate()"))
                                 )) {
                return -1;
            }
            if(ldec_point != 0) {
                if(!query.execInsert("t_m_point",qrtVariantPairList()
                                     << qrtVariantPair("vch_memberno", ls_memberno)
                                     << qrtVariantPair("ch_type", 7)
                                     << qrtVariantPair("ch_payno", as_payno)
                                     << qrtVariantPair("num_point", 0 - ldec_point)
                                     << qrtVariantPair("vch_memo", QObject::tr("反结帐"))
                                     << qrtVariantPair("vch_operID", lds::gs_operid)
                                     << qrtVariantPair("dt_operdate", qrtFunc("curdate()"))
                                     )) {
                    return -1;
                }
            }
            if(!query.execUpdate("t_m_curamount",
                                 "num_amount", qrtFunc("num_amount + " + QString::number(ldec_pay)),
                                 "num_point", qrtFunc("num_point - " + QString::number(ldec_point)),
                                 qrtEqual("vch_memberno", ls_memberno))) {
                return -1;
            }
        } else if("02" == KenShuChooseCase_1) {

            //INTO :li_exist
            query.execSelect(QString("SELECT count( vch_memberno ) "
                                     " from t_m_pay "
                                     " where ch_payno = '%1' and vch_memberno = '%2'  ")
                             .arg(as_payno)
                             .arg(ls_memberno));
            query.next();
            li_exist = query.recordValue(0).toLongLong();
            if(li_exist == 0) {
                continue;
            }

            if(ldec_point != 0) {
                if(!query.execInsert("t_m_point",qrtVariantPairList()
                                     << qrtVariantPair("vch_memberno", ls_memberno)
                                     << qrtVariantPair("ch_type", 7)
                                     << qrtVariantPair("ch_payno", as_payno)
                                     << qrtVariantPair("num_point", ldec_point)
                                     << qrtVariantPair("vch_memo", QObject::tr("反结帐"))
                                     << qrtVariantPair("vch_operID", lds::gs_operid)
                                     << qrtVariantPair("dt_operdate", qrtFunc("curdate()")))) {
                    return -1;
                }
            }

            if(!query.execUpdate("t_m_curamount",
                                 "num_point", qrtFunc("num_point - " + QString::number(ldec_point)),
                                 qrtEqual("vch_memberno", ls_memberno))) {
                return -1;
            }

            if(!query.execInsert("t_m_pay",qrtVariantPairList()
                                 << qrtVariantPair("vch_memberno", ls_memberno)
                                 << qrtVariantPair("ch_payno", as_payno)
                                 << qrtVariantPair("ch_paymodeno", ls_paymodeno)
                                 << qrtVariantPair("num_amount", 0)
                                 << qrtVariantPair("num_pay", 0 - ldec_pay)

                                 << qrtVariantPair("vch_operID", as_operid)
                                 << qrtVariantPair("ch_balanceno", QVariant())
                                 << qrtVariantPair("int_checkflow", 0)
                                 << qrtVariantPair("dt_operdate", qrtFunc("curdate()")))) {

                return -1;
            }


            uo_member.of_init(this, "4");
            lst_card = uo_member.of_read_card(this, ls_memberno);
            ldec_remain = lst_card.amount;
            ldec_remain = ldec_remain + ldec_pay;

            //INTO :li_rate
            query.execSelect(QString("SELECT a.int_backcash_rate "
                                     " from t_m_member_type a,  t_m_member b "
                                     " where a.ch_typeno =b.ch_typeno and b.vch_memberno = '%1'  "
                                     )
                             .arg(ls_memberno));
            query.next();

            li_rate = query.recordValue(0).toLongLong();
            if(li_rate > 0) {
                ldec_backcash = ldec_pay * (li_rate*1.0 / 100);
            } else {
                ldec_backcash = 0;
            }
            ldec_remain = ldec_remain - ldec_backcash;

            if(!query.execUpdate("t_m_curamount", qrtVariantPairList()
                                 << qrtVariantPair("num_amount", ldec_remain)
                                 << qrtVariantPair("vch_operID", lds::gs_operid)
                                 << qrtVariantPair("dt_operdate", qrtFunc("curdate()")),
                                 qrtEqual("vch_memberno", ls_memberno))) {

                return -1;
            }
            if(ldec_backcash != 0) {
                if(!query.execInsert("t_m_deposit",qrtVariantPairList()
                                     << qrtVariantPair("vch_memberno", ls_memberno)
                                     << qrtVariantPair("num_deposit", ldec_backcash*- 1)
                                     << qrtVariantPair("num_realamount", 0)
                                     << qrtVariantPair("ch_deposit_mode", 4)
                                     << qrtVariantPair("ch_pay_mode", 5)
                                     << qrtVariantPair("vch_operID", as_operid)
                                     << qrtVariantPair("vch_memo", QObject::tr("反结帐充值付款返现"))
                                     << qrtVariantPair("dt_operdate", qrtFunc("curdate()")))) {
                    return -1;
                }
            }
        }
    }
    return 0;
}

int business_documents_query::f_member_delete_y(const QString &as_payno, const QString &as_operid)
{
    return -1;
    ////////////////////////////////////////////////////////
    //Public function f_member_delete (string as_payno,string as_operid) returns integer
    //string as_payno
    //string as_operid
    QString ls_paymodeno;
    QString ls_memberno;
    QString ls_parm;
    //    int li_flowid;
    //    int li_exist;
    float ldec_pay;
    float ldec_remain;
    float ldec_point;
    //    float ldec_num_point;
    lds_query_hddpos  cur_mpay;
    lds_query_hddpos  query;
    QString KenShuChooseCase_1;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    int li_rate;
    float ldec_backcash;

    cur_mpay.execSelect(QString("SELECT a.vch_voucherno,  a.num_payamount,  a.ch_paymodeno,  ifnull( (select b.num_point from t_m_point b "
                                " where a.ch_payno =b.ch_payno and a.vch_voucherno =b.vch_memberno ),  0 ) as num_point"
                                " from cey_u_checkout_detail a"
                                " where a.ch_payno = '%1' and a.ch_paymodeno in ( '10' )")
                        .arg(as_payno));

    while(cur_mpay.next()) {

        ls_memberno = cur_mpay.recordValue("vch_voucherno").toString();
        ldec_pay = cur_mpay.recordValue("num_payamount").toFloat();
        ls_paymodeno = cur_mpay.recordValue("ch_paymodeno").toString();
        ldec_point = cur_mpay.recordValue("num_point").toFloat();



        if(ldec_point != 0) {
            if(!query.execInsert("t_m_point",qrtVariantPairList()
                                 <<  qrtVariantPair("vch_memberno", ls_memberno)
                                 <<  qrtVariantPair("ch_type", 7)
                                 <<  qrtVariantPair("ch_payno", as_payno)
                                 <<  qrtVariantPair("num_point", 0 - ldec_point)
                                 <<  qrtVariantPair("vch_memo", QObject::tr("反结帐"))
                                 <<  qrtVariantPair("vch_operID", lds::gs_operid)
                                 <<  qrtVariantPair("dt_operdate", qrtFunc("curdate()")))) {

                return -1;
            }
        }

        //        memberPointList; //积分负数
        //        memberCuramount; //充值金额负数
        //        memberPay; //会员付款记录负数

        //        t_m_curamount;
        //        t_m_pay;
        //        t_m_curamount;
        //        t_m_deposit;//会员付款返现时充值


        if(!query.execUpdate("t_m_curamount","num_point", qrtFunc("num_point - " + QString::number(ldec_point)),
                             qrtEqual("vch_memberno", ls_memberno))) {
            return -1;
        }

        if(!query.execInsert("t_m_pay", qrtVariantPairList()
                             <<  qrtVariantPair("vch_memberno", ls_memberno)
                             <<  qrtVariantPair("ch_payno", as_payno)
                             <<  qrtVariantPair("ch_paymodeno", ls_paymodeno)
                             <<  qrtVariantPair("num_amount", 0)
                             <<  qrtVariantPair("num_pay", 0 - ldec_pay)

                             <<  qrtVariantPair("vch_operID", as_operid)
                             <<  qrtVariantPair("ch_balanceno", QVariant())
                             <<  qrtVariantPair("int_checkflow", 0)
                             <<  qrtVariantPair("dt_operdate", qrtFunc("curdate()")))) {

            return -1;
        }


        uo_member.of_init(this, "4");
        lst_card = uo_member.of_read_card(this, ls_memberno);
        ldec_remain = lst_card.amount;
        ldec_remain = ldec_remain + ldec_pay;

        //INTO :li_rate
        query.execSelect(QString("SELECT a.int_backcash_rate "
                                 " from t_m_member_type a,  t_m_member b "
                                 " where a.ch_typeno =b.ch_typeno and b.vch_memberno = '%1'  "
                                 )
                         .arg(ls_memberno));
        query.next();

        li_rate = query.recordValue(0).toLongLong();
        if(li_rate > 0) {
            ldec_backcash = ldec_pay * (li_rate*1.0 / 100);
        } else {
            ldec_backcash = 0;
        }
        ldec_remain = ldec_remain - ldec_backcash;

        if(!query.execUpdate("t_m_curamount",qrtVariantPairList()
                             <<  qrtVariantPair("num_amount", ldec_remain)
                             <<  qrtVariantPair("vch_operID", lds::gs_operid)
                             <<  qrtVariantPair("dt_operdate", qrtFunc("curdate()")),
                             qrtEqual("vch_memberno", ls_memberno))) {
            return -1;
        }
        if(ldec_backcash != 0) {
            if(!query.execInsert("t_m_deposit",qrtVariantPairList()
                                 <<  qrtVariantPair("vch_memberno", ls_memberno)
                                 <<  qrtVariantPair("num_deposit", ldec_backcash*- 1)
                                 <<  qrtVariantPair("num_realamount", 0)
                                 <<  qrtVariantPair("ch_deposit_mode", 4)
                                 <<  qrtVariantPair("ch_pay_mode", 5)

                                 <<  qrtVariantPair("vch_operID", as_operid)
                                 <<  qrtVariantPair("vch_memo", QObject::tr("反结帐充值付款返现"))
                                 <<  qrtVariantPair("dt_operdate", qrtFunc("curdate()")))) {
                return -1;
            }
        }
    }

    return 0;
}

void business_documents_query::toexport()
{
    w_rpt_report_dataExport dialog(ui->tableView->tableView, 0,  this);
    dialog.setWindowTitle(this->windowTitle());
    dialog.defFilepath(this->windowTitle());
    transparentCenterDialog(&dialog).exec();
}

void business_documents_query::toprint()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(),
                                           this->metaObject()->className(),
                                           ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                           ui->tableView,
                                           0
                                           );
    dialog->pop(b, this);
}

bool business_documents_query::prepareUnCheck(const QString &ch_payno, const QString &ch_billno)
{
    lds_query_hddpos  query;
    //cur_dt_str
    QString cur_dt_str = n_func::f_get_sysdatetime_str();
    //num_cost
    query.execSelect(QString(" select num_cost from cey_u_checkout_master where ch_payno = '%1' ")
                     .arg(ch_payno));
    query.next();
    double num_cost = query.recordValue("num_cost").toDouble();
    //作废
    if(!query.execUpdate("cey_u_checkout_master",
                         qrtVariantPairList()
                         << qrtVariantPair("ch_state", "P")
                         << qrtVariantPair("vch_memberno", "")
                         << qrtVariantPair("vch_operID2", lds::gs_operid)
                         << qrtVariantPair("dt_operdate2", cur_dt_str),
                         qrtEqual("ch_payno", ch_payno))) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
        return false;
    }
    if(!query.execUpdate("cey_u_orderdish", "ch_payno", QVariant(), qrtEqual("ch_payno", ch_payno))) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
        return false;
    }
    if(!query.execUpdate("cey_u_table","ch_payno", QVariant(), qrtEqual("ch_payno",  ch_payno))) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
        return false;
    }
    //会员退款
    query.execSelect(QString("SELECT count(1 )"
                             " from cey_u_checkout_detail"
                             " where ch_payno = '%1' and ( ch_paymodeno ='01' or ch_paymodeno ='02' )"
                             ).arg(ch_payno));
    query.next();
    qlonglong ll_exist = query.recordValue(0).toLongLong();
    //INTO :ll_exist1
    query.execSelect(QString("SELECT count(1 )"
                             " from t_m_point"
                             " where ch_payno = '%1' ")
                     .arg(ch_payno));
    query.next();
    qlonglong ll_exist1 = query.recordValue(0).toLongLong();

    if((ll_exist + ll_exist1) > 0) {
        if(ll_exist > 0) {
            if(f_member_delete(ch_payno,lds::gs_operid) == -1) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
                return false;
            }
        }
    }
    //日志记录
    if(!query.execInsert("cey_u_uncheckout_log",qrtVariantPairList()
                         << qrtVariantPair("ch_billno", ch_billno)
                         << qrtVariantPair("ch_payno_un", ch_payno)
                         << qrtVariantPair("num_cost_un", num_cost)
                         << qrtVariantPair("vch_operid", lds::gs_operid)
                         << qrtVariantPair("dt_operdate", cur_dt_str))) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
        return false;
    }

    return true;
}

bool business_documents_query::doWarin()
{
    lds_query_hddpos  query;
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return false;
    }

    QString ch_tableno = model_top->model_data(row, "ch_tableno").toString();
    query.execSelect(QString(" select ch_billno from cey_bt_table where ifnull(ch_billno, '') <> '' and ch_tableno = '%1' ")
                     .arg(ch_tableno));
    if(query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("指定结算单中的桌位正在使用"));
        return false;
    }

    ///中餐下点单，点单退单是，改动的num_back， 退单，改的是直接改成num_num = -(num_num - num_back);
    QString ch_billno = model_top->model_data(row, "ch_billno").toString();
    query.execSelect(QString(" select 1 from cey_u_orderdish where (num_num - num_back) < 0 and ch_billno = '%1' limit 1 ")
                     .arg(ch_billno));
    if(query.next()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("本单已做过退单处理"));
        return false;
    }

    if(model_left->rowCount() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return false;
    }

    return true;
}

bool business_documents_query::insert_cey_u_table(const QString &ch_billno_old, const QString &ch_billno_new, const QString &cur_dt_str, const QString &vch_memo)
{
    lds_query_hddpos  query;
    lds_model_sqltablemodel tablemodel;
    tablemodel.setTable("cey_u_table");
    tablemodel.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    query.execSelect(QString("SELECT * from cey_u_table where ch_billno = '%1' ").arg(ch_billno_old));
    query.next();
    tablemodel.insertRow(0);
    int qcount = query.recordCount();
    for(int q = 0; q < qcount; q++) {
        tablemodel.model_data_set(0, query.record().fieldName(q), query.recordValue(q));
    }
    tablemodel.model_data_set(0, "ch_payno", "");
    tablemodel.model_data_set(0, "ch_billno", ch_billno_new);
    tablemodel.model_data_set(0, "vch_operid", lds::gs_operid);
    tablemodel.model_data_set(0, "dt_operdate", cur_dt_str);
    tablemodel.model_data_set(0, "vch_memo", vch_memo);//云端未上传

    return tablemodel.trySubmitAll();
}

int business_documents_query::getDishSelected(QList<qlonglong> &ret_int_flowID_list)
{
    QDialog dialog(this);
    Ui_w_bt_dish_unitset_Dialog ui;
    ui.setupUi(&dialog);
    lds::hideWidget(ui.label_desc);
    ui.widget->setup(QStringList() << QObject::tr("全选") << QObject::tr("全不选") << QObject::tr("上一页") << QObject::tr("下一页")<< QObject::tr("退单") << QObject::tr("取消"), Qt::AlignRight);
    ui.tableView->setTransferHheader();
    ui.tableView->setModel(model_left);
    ui.tableView->resizeColumnsToContentsDelay();
    ui.tableView->setSelectionMode(QTableView::MultiSelection);
    ui.tableView->setItemDelegateForColumn(model_left->fieldIndex("vch_print_memo"), new fexpandmain_delegate_vch_printmemo(this));
    ui.tableView->setItemDelegateForColumn(model_left->fieldIndex("ch_suitflag"), new lds_model_sqltablemodel_delegate_com(
                                               &dialog, ldsVariantMap("P", QObject::tr("主商品"), "Y", QObject::tr("子商品"))
                                               ));
    connect(ui.widget->index_widget(QObject::tr("退单")), SIGNAL(clicked()),&dialog,SLOT(accept()));
    connect(ui.widget->index_widget(QObject::tr("取消")), SIGNAL(clicked()),&dialog,SLOT(reject()));
    connect(ui.widget->index_widget(QObject::tr("上一页")), SIGNAL(clicked()),ui.tableView,SLOT(toPageUp()));
    connect(ui.widget->index_widget(QObject::tr("下一页")), SIGNAL(clicked()),ui.tableView,SLOT(toPageDown()));
    connect(ui.widget->index_widget(QObject::tr("全选")), SIGNAL(clicked()),ui.tableView,SLOT(toSelectAll()));
    connect(ui.widget->index_widget(QObject::tr("全不选")), SIGNAL(clicked()),ui.tableView,SLOT(toClearAll()));
    dialog.setWindowTitle(QObject::tr("商品选择"));
    //
    int ret = transparentCenterDialog(&dialog).exec();
    //
    foreach(int k, ui.tableView->getSelectedRowListAsc()) {
        ret_int_flowID_list << model_left->model_data(k, "int_flowID").toLongLong();
    }

    return ret;
}

bool business_documents_query::doUnCheck(const QString &ch_tableno, const QString &ch_billno)
{
    bool f = false;
    //lock table
    qDebug() << __LINE__ ;
    lds_scr_table::cey_bt_table_lock(ch_tableno, 0);//快餐会失败
    w_scr_dish_main_table_800x600 dialog(ch_tableno, ch_billno, 0);
    dialog.move(0, 0);
    qDebug() << __LINE__ ;
    if(QDialog::Accepted ==dialog.exec()) {//没有付款或付款失败/取消
        //cancel table
        lds_scr_table::cey_bt_table_update(ch_tableno, "", 0);//快餐支持
        f = true;
    }
    qDebug() << __LINE__ ;
    //unlock
    lds_scr_table::cey_bt_table_unlock(ch_tableno, 0);//快餐会失败
    qDebug() << __LINE__ ;
    return f;
}

void business_documents_query::touncheck()
{
    data_refresh refd(this);//析构自动刷新

    lds_query_hddpos  query;
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    //中餐台位正在使用、没有商品、已做过退单
    if(false == doWarin()) {
        return;
    }
    if(!backheader::operate_right_check_code(this, "0815")) {//反结账查询
        return;
    }
    //快餐
    query.execSelect(QString("select ch_billno from cey_sys_lan_terminal where ch_billno <>'' and vch_terminal = '%1';")
                     .arg(lds::terminalCode));
    if(query.next()) {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("快餐上次反结账未完成"), QObject::tr("继续"), QObject::tr("跳过"), QObject::tr("取消"))) {
        case 0://继续
            if(false == doUnCheck(lds_scr_table::Q000, query.recordValue("ch_billno").toString())) {
                return;
            }
            if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("继续反结账"), QObject::tr("继续"), QObject::tr("取消"))) {
                return;
            }
            break;
        case 1://跳过
            break;
        case 2://取消
            return;
        }
    }
    QString ch_billno_old = model_top->model_data(row, "ch_billno").toString();
    if(ch_billno_old.startsWith("Q")) {
        query.execSelect(QString("select ch_billno from cey_sys_lan_terminal where ifnull(ch_billno, '') <>'' and vch_terminal = '%1';")
                         .arg(lds::terminalCode));
        if(query.next()) {
            lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("快餐上次反结账未完成"));
            return;
        }
    }
    //~快餐
    if(1 == lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("反结账将会清除以前结算记录"), QObject::tr("继续"), QObject::tr("取消"))) {
        return;
    }
    QString ch_payno_old = model_top->model_data(row, "ch_payno").toString();
    lds_query_hddpos::tran_saction();
    if(false == prepareUnCheck(ch_payno_old, ch_billno_old)) {
        lds_query_hddpos::roll_back();
        return;
    }
    lds_query_hddpos::com_mit();

    QString ch_tableno = model_top->model_data(row, "ch_tableno").toString();
    lds_scr_table::cey_bt_table_update(ch_tableno, ch_billno_old, 0);//快餐支持
    doUnCheck(ch_tableno, ch_billno_old);
}

void business_documents_query::torefund()
{
    data_refresh refd(this);//析构自动刷新
    lds_query_hddpos  query;

    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }
    //中餐台位正在使用、没有商品、已做过退单
    if(false == doWarin()) {
        return;
    }
    //获取商品
    QList<qlonglong> int_flowID_list;
    if(QDialog::Accepted == getDishSelected(int_flowID_list)) {
        //int_flowID_list
        if(int_flowID_list.count() == 0) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
            return;
        }
        lds_query_hddpos::tran_saction();
        //cey_u_master
        QString ch_billno_old =  model_top->model_data(row, "ch_billno").toString();
        QString ch_billno_new;
        if(false == lds_scr_table::get_max_ch_billno_add1_Insert_cey_u_master_Create_serial_no(ch_billno_old.left(1), ch_billno_new)) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
            return;
        }
        //cey_u_table
        QString ch_payno_old = model_top->model_data(row, "ch_payno").toString();
        QString cur_dt_str = n_func::f_get_sysdatetime_str();
        if(false == insert_cey_u_table(ch_billno_old, ch_billno_new, cur_dt_str, ch_payno_old + QObject::tr("提示"))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败")+QString::number(__LINE__));
            return;
        }
        lds_query_hddpos::com_mit();
        //~cey_u_table

        //cey_u_orderdish
        fexpandmain_model_sqltablemodel_data tablemodel;
        tablemodel.setTable("cey_u_orderdish");
        tablemodel.setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
        tablemodel.select(ch_billno_new);
        foreach(int int_flowid, int_flowID_list) {
            query.execSelect(QString("SELECT * from cey_u_orderdish where int_flowid = %1 ")
                             .arg(int_flowid));
            query.next();
            int sqr = tablemodel.rowCount();
            tablemodel.insertRow(sqr);
            int qcount = query.recordCount();
            for(int q = 0; q < qcount; q++) {
                tablemodel.model_data_set(sqr, query.record().fieldName(q), query.recordValue(q));
            }
            tablemodel.model_data_set(sqr, "ch_payno", "");
            tablemodel.model_data_set(sqr, "ch_billno", ch_billno_new);
            tablemodel.model_data_set(sqr, "vch_operid", lds::gs_operid);
            tablemodel.model_data_set(sqr, "dt_operdate", cur_dt_str);
        }
        tablemodel.getKeepdata().print();
        //~cey_u_orderdish
        //pay
        w_scr_dish_pay_widget::operatModes pay_flags = w_scr_dish_pay_widget::OPERAT_QUITBILL;
        if(ch_billno_new.startsWith("Q")) {
            pay_flags |= w_scr_dish_pay_widget::TYPE_FAST;
        } else {
            pay_flags |= w_scr_dish_pay_widget::TYPE_RESTAURANT;
        }
        tablemodel.tablename_kvmap_print("fexpandmain_delegate_row");
        w_scr_dish_main_table_pay pay(&tablemodel, this, QList<int>(), pay_flags);
        if(QDialog::Accepted == pay.exec()) {//付款成功
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        }
        //~pay
    }
}
