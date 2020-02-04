#include "w_scr_dish_restaurant_table_transfer.h"
#include "ui_w_scr_dish_restaurant_table_transfer.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "lds_query_hddpos.h"
#include "ftableview_standmodel_sql_table.h"
#include "restauranttableview.h"
#include "public_sql.h"
#include "lds_messagebox.h"
#include "n_func.h"
#include <QPainter>
#include "w_bt_dish.h"
#include "public_logindialog.h"

w_scr_dish_restaurant_table_transfer::w_scr_dish_restaurant_table_transfer(const QString &ch_tableno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_restaurant_table_transfer)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("餐桌转台"));

    ch_tableno_from = ch_tableno;
    ui->label_cur_table->setText(QString("%1(%2)")
                                 .arg(public_sql::getTableName(ch_tableno))
                                 .arg(ch_tableno));
    //
    lds_query_hddpos  query;
    query.execSelect(QString("select  " + restraurantDelegate::sql_ch_billnos + " as ch_billnos "
                             " from cey_bt_table y"
                             " where y.ch_tableno = '%1' " )
                     .arg(ch_tableno));

    query.next();
    QString ch_billnos = query.recordValue(0).toString();

    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    //九宫格
    _9Box = new w_scr_dish_restaurant_div_pop_only_table;
    _9Box->update_by_ch_billnos(ch_billno_map);
    ui->horizontalLayout_div->addWidget(_9Box);
    if(restraurantDelegate::ch_billno_is_div(ch_billno_map)) {
        _9Box->enableSinglSelectedWhenMutil();
    } else {
        _9Box->setcurindex_by_sqr(0);
        _9Box->trycombine();
    }

    //
    query.execSelect("select ch_typeno,  vch_typename  FROM cey_bt_table_type union select '%' as ch_typeno, '"+QObject::tr("所有类型")+"' as vch_typename ORDER BY ch_typeno ASC");
    while(query.next()) {
        ui->comboBox_tabletype->addItem(query.recordValue("vch_typename").toString(), query.recordValue("ch_typeno").toString());
    }
    //
    restraurantDelegate *delegatetable = new restraurantDelegate(ui->tableView);
    modeltable = new ftableview_standmodel_sql_table;
    modeltable->sql_cur =
            " select y.vch_tablename, y.ch_tableno, "
            + restraurantDelegate::sql_ch_billnos
            + "as ch_billnos, "
            " ifnull((select int_person from cey_u_table where cey_u_table.ch_billno = y.ch_billno), y.int_person) as int_person,"
            " (select num_cost from cey_u_table where cey_u_table.ch_billno = y.ch_billno) as num_cost "
            " from cey_bt_table y "
            " where y.ch_tableno <> " + QString(" '%1'  and  ").arg(ch_tableno)
            ///未满单
            + restraurantDelegate::sql_ch_billnos +" like '%,,%' and "
            ///not已开台未分单
            + " not("+restraurantDelegate::sql_ch_billnos +"like '%,,,,,,,,,' and ifnull(ch_billno, '') <> '') "
            ;

    modeltable->sqlother = modeltable->sql_cur + " and y.ch_typeno like '%1%' ";
    ui->tableView->setModel(modeltable);
    ui->tableView->setItemDelegate(delegatetable);
    ui->tableView->updateFixed_fillData_addAnimation(6, 4, 90, 90);
    ui->tableView->enablePop(restaurantTableView::pop_single| restaurantTableView::pop_select_KONGXIAN);
    ui->tableView->enableMutiSelected();
    ui->tableView->enableSelectOneWhenMultiSelected();


    totabletype(0);
    ui->lineEdit_destination->setText("");
    //3
    ui->pushButton_cur_table->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>() << tableState_KAITAI <<  tableState_YUDA));
    ui->pushButton_destination->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>() <<  tableState_KONGXIAN));

    //4
    ui->checkBox_tranfer_print->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_transfer/kitchen_turn_check", false).toBool());
    ui->checkBox_tranfer_out->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_transfer/out_kitchen_turn_check", false).toBool());
    ui->label_desc->setText(QObject::tr("已开台未分单、已开台分单满单，的将不会显示"));
    connect(ui->checkBox_tranfer_out,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenout(bool)));
    connect(ui->checkBox_tranfer_print,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenprint(bool)));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->comboBox_tabletype,SIGNAL(currentIndexChanged(int)),this,SLOT(totabletype(int)));
    connect(ui->tableView, SIGNAL(released(QModelIndex)), this,SLOT(toupdatedestination(QModelIndex)));
}

w_scr_dish_restaurant_table_transfer::~w_scr_dish_restaurant_table_transfer()
{
    delete ui;
}

void w_scr_dish_restaurant_table_transfer::totabletype(int index)
{
    modeltable->toinitBySqlother(ui->comboBox_tabletype->itemData(index).toString());
}

void w_scr_dish_restaurant_table_transfer::tosavekitchenout(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_transfer/out_kitchen_turn_check", f);
}

void w_scr_dish_restaurant_table_transfer::tosavekitchenprint(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_transfer/kitchen_turn_check",f);
}

void w_scr_dish_restaurant_table_transfer::toupdatedestination(const QModelIndex &index)
{
    Q_UNUSED (index)
    QList<ftableview_standmodel_sql_table::checkData> checkdata = modeltable->get_value_select();

    QString text;
    foreach(const ftableview_standmodel_sql_table::checkData & d, checkdata) {
        if(!d.ch_tableno.isEmpty()) text += d.ch_tableno;
        //        if(!d.checklist.isEmpty()) text += "["+ d.checklist + "]";
        text += ",";
    }
    if(!text.isEmpty()) text.chop(1);
    ui->lineEdit_destination->setText(text);
}

void w_scr_dish_restaurant_table_transfer::tocancel()
{
    this->reject();
}

void w_scr_dish_restaurant_table_transfer::took()
{
    QString errstring;
    QString ch_tableno_to;
    lds_query_hddpos  query;
    QModelIndexList indexlist = _9Box->get_cur_check_index_list();

    int div_sqr_to = 0;
    int div_sqr_from = 0;

    QString state;
    if(lds::sysconf->value("w_scr_dish_restaurant_table_transfer/kitchen_turn_check", false).toBool()) {
        state += QString(3, '0'+w_bt_dish::KITCHEN_ZHUANTAI);
    } else {
        state += QString(3, '0'+w_bt_dish::KITCHEN_FINISH);
    }
    if(lds::sysconf->value("w_scr_dish_restaurant_table_transfer/out_kitchen_turn_check", false).toBool()) {
        state += QString(3, '0'+w_bt_dish::KITCHEN_ZHUANTAI);
    } else {
        state += QString(3, '0'+w_bt_dish::KITCHEN_FINISH);
    }
    //

    errstring = ui->pushButton_cur_table->text()+QObject::tr("未设置");
    if(!indexlist.isEmpty()) {
        errstring = ui->pushButton_destination->text()+QObject::tr("未设置");
        QString ch_billno_from = indexlist[0].data(w_scr_dish_restaurant_div_pop::Model_ch_billno).toString();//都是单选
        div_sqr_from = indexlist[0].data(w_scr_dish_restaurant_div_pop::Model_sqr).toInt();//都是单选
        if(lds_scr_table::cey_bt_table_has_locked(ch_tableno_from, div_sqr_from)) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("该餐桌已经被锁定,请确定"));
            return;
        }

        ftableview_standmodel_virtual::usrVarMapIterator mapitr(modeltable->currentSelected());
        if(mapitr.hasNext()) {
            mapitr.next();

            ch_tableno_to = mapitr.value().value(1/*ch_tableno*/).toString();//->page_record_value(1/*ch_tableno*/).value(0).toString();
            QStringList check_to_list = mapitr.value().value(7/*checklist*/).toString().split(",", QString::SkipEmptyParts);
            QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(mapitr.value().value(2/*ch_billnos*/).toString());

            //分单满单， 未分单满单
            if(ch_billno_map.count() == 9
                    || (ch_billno_map.count() == 1 && !ch_billno_map.value(0).isEmpty())) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->pushButton_destination->text() + QObject::tr("操作失败"));
                return;
            }
            if(restraurantDelegate::ch_billno_is_div(ch_billno_map) && check_to_list.isEmpty()) {//已分单、没有分单号，操作错误

            } else  {
                if(!check_to_list.isEmpty()) {
                    div_sqr_to = check_to_list[0].toInt();
                }
                errstring = "";
                QString transferinfo =
                        ui->pushButton_cur_table->text()+":"+ui->label_cur_table->text()        +  ( _9Box->iscombine ? "" : ( "," + QObject::tr("分单号") + indexlist[0].data(Qt::UserRole + 1).toString() ))+"\n" +
                        ui->pushButton_destination->text()+":"+ui->lineEdit_destination->text() +  (check_to_list.isEmpty() ? "" : ( "," + QObject::tr("分单号") + check_to_list[0]))//都是单选
                        ;
                if(0 == lds_messagebox::information(this, MESSAGE_TITLE_VOID,transferinfo,QObject::tr("确认"), QObject::tr("取消"))) {

                    lds_query_hddpos::tran_saction();
                    errstring = QObject::tr("数据库错误");

                    if(query.execUpdate("cey_u_table","ch_tableno", ch_tableno_to, qrtEqual("ch_billno", ch_billno_from))) {
                        if(query.execUpdate("cey_u_orderdish",qrtVariantPairList()
                                            << qrtVariantPair("vch_plan_desc", "\n" + transferinfo)
                                            << qrtVariantPair("ch_tableno", ch_tableno_to),
                                            qrtEqual("ch_billno", ch_billno_from))) {
                            if(query.execUpdate("cey_u_orderdish","vch_plan_k3o3_state", state, qrtEqual("ch_billno", ch_billno_from))) {
                                if(lds_scr_table::cey_bt_table_update(ch_tableno_from, "", div_sqr_from)) {
                                    if(lds_scr_table::cey_bt_table_update(ch_tableno_to, ch_billno_from, div_sqr_to)) {
                                        lds_query_hddpos::com_mit();
                                        lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
                                        if(ui->checkBox_tranfer_out->isChecked() || ui->checkBox_tranfer_print->isChecked()) {
                                            public_sql::save_login->to_everytime_to_print_failed_kitchen_bill();
                                        }
                                        this->accept();
                                        return;
                                    }
                                }
                            }
                        }
                        lds_query_hddpos::roll_back();
                    }
                }
            }
        }
    }
    if(!errstring.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
}
