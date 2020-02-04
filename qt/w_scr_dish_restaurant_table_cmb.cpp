#include "w_scr_dish_restaurant_table_cmb.h"
#include "ui_w_scr_dish_restaurant_table_cmb.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "lds_query_hddpos.h"
#include "ftableview_standmodel_sql_table.h"
#include "restauranttableview.h"
#include "public_sql.h"
#include "lds_messagebox.h"
#include "n_func.h"
#include <QPainter>
#include <QSqlError>
#include "w_bt_dish.h"
#include "frontviewdialog_virtual.h"
#include "fexpandmain_2.h"
#include "public_logindialog.h"

w_scr_dish_restaurant_table_cmb::w_scr_dish_restaurant_table_cmb(const QString &ch_tableno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_restaurant_table_cmb)
{
    ui->setupUi(this);
    ui->label_cur_table->setText(QString("%1(%2)")
                                 .arg(public_sql::getTableName(ch_tableno))
                                 .arg(ch_tableno));
    //
    lds_query_hddpos  query;
    query.execSelect(QString("select "
                             + restraurantDelegate::sql_ch_billnos + "as ch_billnos "
                             " from cey_bt_table y"
                             " where y.ch_tableno = '%1' ")
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
            + restraurantDelegate::sql_ch_billnos + "as ch_billnos, "
            " ifnull((select int_person from cey_u_table where cey_u_table.ch_billno = y.ch_billno), y.int_person) as int_person,"
            " (select num_cost from cey_u_table where cey_u_table.ch_billno = y.ch_billno) as num_cost "
            " from cey_bt_table y"
            " where "
            " ch_tableno <> '"+ch_tableno+"'"
            ///未开台餐桌
            + " and "+restraurantDelegate::sql_ch_billnos +" <> ',,,,,,,,,'  ";
    ;
    modeltable->sqlother = modeltable->sql_cur + " and y.ch_typeno like '%1%' ";
    modeltable->sql_cur = modeltable->sql_cur.arg("");
    ui->tableView->setModel(modeltable);
    ui->tableView->setItemDelegate(delegatetable);
    ui->tableView->updateFixed_fillData_addAnimation(6, 4, 90, 90);
    ui->tableView->enablePop(restaurantTableView::pop_mutil| restaurantTableView::pop_select_KAITAI);
    ui->tableView->enableMutiSelected();

    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));

    connect(ui->comboBox_tabletype,SIGNAL(currentIndexChanged(int)),this,SLOT(totabletype(int)));
    connect(_9Box, SIGNAL(signal_div_click(QModelIndex)),this, SLOT(toSetgrayFrom(QModelIndex)));
    connect(ui->tableView, SIGNAL(released(QModelIndex)), this,SLOT(toupdatedestination(QModelIndex)));
    totabletype(0);

    ui->lineEdit_destination->setText("");

    //3
    QPixmap p(50, 50);
    QPainter painter;
    painter.begin(&p);
    painter.fillRect(QRect(0, 0, 50, 50), restraurantDelegate::gettableState(tableState_KAITAI).colorname1);
    QPolygonF polygon(QVector<QPointF>() << QPointF(50, 0) << QPointF(50, 50) << QPointF(0, 50));
    QPainterPath path;
    path.addPolygon(polygon);
    painter.fillPath(path, restraurantDelegate::color_light(restraurantDelegate::gettableState( tableState_YUDA).colorname1, 2.0));
    painter.end();

    ui->pushButton_cur_table->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>()<<tableState_KAITAI <<  tableState_YUDA));
    ui->pushButton_destination->setIcon(restraurantDelegate::getPixmapByStateList(QList<tableState>()<<tableState_KAITAI <<  tableState_YUDA));

    //4
    ui->label_desc->setText(QObject::tr("未开台餐桌的将不会显示"));
    ui->label_destination_sum->setText("");
    ui->tableView->enableMutiSelected();

    ui->checkBox_tranfer_print->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_cmb/kitchen_turn_check", false).toBool());
    ui->checkBox_tranfer_out->setChecked(lds::sysconf->value("w_scr_dish_restaurant_table_cmb/out_kitchen_turn_check", false).toBool());
    connect(ui->checkBox_tranfer_out,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenout(bool)));
    connect(ui->checkBox_tranfer_print,SIGNAL(toggled(bool)), this, SLOT(tosavekitchenprint(bool)));


}

w_scr_dish_restaurant_table_cmb::~w_scr_dish_restaurant_table_cmb()
{
    delete ui;
}


void w_scr_dish_restaurant_table_cmb::totabletype(int index)
{
    modeltable->toinitBySqlother(ui->comboBox_tabletype->itemData(index).toString());
}

void w_scr_dish_restaurant_table_cmb::tocurindex_checklist(const QString &checklist)
{
    modeltable->setData(ui->tableView->currentIndex(), checklist, Qt::UserRole+7/*check*/);
}

void w_scr_dish_restaurant_table_cmb::tocancel()
{
    this->reject();
}

void w_scr_dish_restaurant_table_cmb::took()
{
    QString errstring;
    lds_query_hddpos  query;
    lds_query_hddpos  query_going;
    lds_query_hddpos  query_going2;

    QModelIndexList indexlist = _9Box->get_cur_check_index_list();

    QList<cmbData> ch_billno_from_list;
    QString ch_billno_from_list_desc;
    QString vch_plan_desc;


    errstring = ui->pushButton_destination->text()+QObject::tr("未设置");
    if(!indexlist.isEmpty()) {
        QString ch_billno_to = indexlist[0].data(w_scr_dish_restaurant_div_pop::Model_ch_billno).toString();//都是单选
        QString ch_tableno_to = public_sql::getTablenoByChbillno(ch_billno_to);

        ftableview_standmodel_virtual::usrVarMapIterator mapitr(modeltable->currentSelected());
        while(mapitr.hasNext()) {
            mapitr.next();

            QString ch_billnos = mapitr.value().value(2/*ch_billnos*/).toString();
            QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);

            if(restraurantDelegate::ch_billno_is_div(ch_billno_map)) {//分单
                foreach(const QString &sqr, lds::removeStrList(mapitr.value().value(7/*checked*/).toString().split(",", QString::SkipEmptyParts),
                                                               mapitr.value().value(8/*disable*/).toString().split(",", QString::SkipEmptyParts))) {
                    cmbData cmdd;
                    cmdd.ch_billno = ch_billno_map.value(sqr.toInt());
                    cmdd.ch_tableno = mapitr.value().value( + 1/*ch_tableno*/).toString();
                    cmdd.sqr = sqr.toInt();

                    if(ch_billno_to == cmdd.ch_billno) continue;//from == to 时 continue

                    ch_billno_from_list.append(cmdd);

                    ch_billno_from_list_desc += QString("%1(%2)")
                            .arg(mapitr.value().value(0/*vch_tablename*/).toString())
                            .arg(mapitr.value().value( 1/*ch_tableno*/).toString())
                            + ("," + QObject::tr("分单号") + ":" +sqr)
                            + "\n"
                            ;
                }
            } else {
                cmbData cmdd;
                cmdd.ch_billno = ch_billno_map.value(0);
                cmdd.ch_tableno = mapitr.value().value(1/*ch_tableno*/).toString();
                cmdd.sqr = 0;

                ch_billno_from_list.append(cmdd);

                ch_billno_from_list_desc += QString("%1(%2)")
                        .arg(mapitr.value().value(0/*vch_tablename*/).toString())
                        .arg(mapitr.value().value(1/*ch_tableno*/).toString())
                        + "\n"
                        ;
            }
        }

        errstring = ui->pushButton_cur_table->text()+QObject::tr("未设置");
        if(!ch_billno_from_list.isEmpty()) {
            //vch_plan_desc
            vch_plan_desc = "";
            foreach(const cmbData &d, ch_billno_from_list)
                vch_plan_desc += d.ch_billno + ",";
            vch_plan_desc += "-";
            vch_plan_desc += _9Box->get_ch_billno_by_index(_9Box->get_cur_check_index_list().value(0));


            QString transferinfo =
                    ui->pushButton_cur_table->text()+":\n"+ch_billno_from_list_desc + "\n" +//都是单选
                    ui->pushButton_destination->text()+":\n"+ui->label_cur_table->text()        +  ( _9Box->iscombine ? "" : ( "," + QObject::tr("分单号") + indexlist[0].data(w_scr_dish_restaurant_div_pop::Model_sqr).toString() ))
                    ;
            errstring = "";
            if(0 == lds_messagebox::question(this, this->windowTitle(), transferinfo,QObject::tr("确认"), QObject::tr("取消"))) {
                //state
                QString state;
                if(ui->checkBox_tranfer_print->isChecked()) {
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_BINGTAI));
                } else {
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
                }
                if(ui->checkBox_tranfer_out->isChecked()) {
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_BINGTAI));
                } else {
                    state += QString(3, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_FINISH));
                }


                lds_query_hddpos::tran_saction();
                query.execSelect(QString("select ifnull(max(int_id), 0) from cey_u_orderdish where ch_billno = '%1' ")
                                 .arg(ch_billno_to));
                query.next();
                int int_id_to_max = query.recordValue(0).toInt();
                foreach(const cmbData &ch_billno_from, ch_billno_from_list) {
                    query_going.execSelect(QString("select int_flowID from cey_u_orderdish where ch_billno = '%1' ")
                                           .arg(ch_billno_from.ch_billno));
                    while(query_going.next()) {
                        int_id_to_max ++;
                        if(query_going2.execUpdate("cey_u_orderdish",
                                                   qrtVariantPairList()
                                                   << qrtVariantPair("int_id", int_id_to_max)
                                                   << qrtVariantPair("ch_billno", ch_billno_to)
                                                   << qrtVariantPair("ch_tableno", ch_tableno_to)
                                                   << qrtVariantPair("vch_plan_desc", vch_plan_desc + "\n" + transferinfo),
                                                   QString(" int_flowID = '%1'  ").arg(query_going.recordValue("int_flowID").toString()))) {
                            if(query_going2.execUpdate("cey_u_orderdish",
                                                       qrtVariantPairList()
                                                       << qrtVariantPair("vch_plan_k3o3_state", state)
                                                       << qrtVariantPair("ch_billno", ch_billno_to),
                                                       QString(" int_flowID = '%1'  ").arg(query_going.recordValue("int_flowID").toString()))) {
                                continue;
                            }
                        }
                        goto rollback;
                    }

                    //!消台from table
                    if(!fexpandmain_2::cancelTable(ch_billno_from.ch_tableno, ch_billno_from.sqr, errstring)) {
                        goto rollback;
                    }
                }

                //!更新总价
                if(!query.execUpdate("cey_u_table",qrtVariantPairList()
                                     << qrtVariantPair("num_cost", public_sql::getTotalBy_ch_billno(ch_billno_to)),
                                     QString(" ch_billno = '%1' ").arg(ch_billno_to))) {
                    errstring = query.errorString();
                    goto rollback;
                }
                //!成功
                lds_query_hddpos::com_mit();

                lds_messagebox::information(this, this->windowTitle(), QObject::tr("操作成功"));
                if(ui->checkBox_tranfer_out->isChecked() || ui->checkBox_tranfer_print->isChecked()) {
                    public_sql::save_login->to_everytime_to_print_failed_kitchen_bill();
                }
                this->accept();
                return;
            }
        }
    }
end:
    if(!errstring.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
    }
    return;
rollback:
    lds_query_hddpos::roll_back();
    goto end;
}

void w_scr_dish_restaurant_table_cmb::toSetgrayFrom(const QModelIndex &index_to)
{
    QString sqr = index_to.data(Qt::UserRole + 1/*sqr*/).toString();
    QString ch_billno = index_to.data(Qt::UserRole/*ch_billno*/).toString();
    QString ch_tableno;
    lds_query_hddpos  query;
    query.execSelect(QString("select ch_tableno from cey_u_table where ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    ch_tableno = query.recordValue("ch_tableno").toString();

    QModelIndex index_from = ui->tableView->getIndexByChtableno(ch_tableno);
    ui->tableView->m->setData(index_from, sqr, Qt::UserRole + 8/*disable*/);
}

void w_scr_dish_restaurant_table_cmb::tosavestate(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_cmb/iskitchen", f);
}

void w_scr_dish_restaurant_table_cmb::tosavekitchenout(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_cmb/out_kitchen_turn_check", f);
}

void w_scr_dish_restaurant_table_cmb::tosavekitchenprint(bool f)
{
    lds::sysconf->setValue("w_scr_dish_restaurant_table_cmb/kitchen_turn_check", f);
}

void w_scr_dish_restaurant_table_cmb::toupdatedestination(const QModelIndex &index)
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
