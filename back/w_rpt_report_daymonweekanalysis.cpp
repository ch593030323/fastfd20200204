#include "w_rpt_report_daymonweekanalysis.h"
//日月周报表分析
#include "ui_w_rpt_report_daymonweekanalysis_dialog.h"
#include "lds_query_hddpos.h"
#include <QStandardItemModel>
#include "backheader.h"
#include "n_func.h"
#include "lds_tableview.h"

w_rpt_report_daymonweekanalysis::w_rpt_report_daymonweekanalysis(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_rpt_report_daymonweekanalysis_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    lds_query_hddpos  query;
    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //! init
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));
    //商品大类
    ui->comboBox_series->addItems(w_rpt_report::querymodelsetfilters("select * from cey_bt_dish_type"));
    //商品小类
    connect(ui->comboBox_series,SIGNAL(currentIndexChanged(QString)),this,SLOT(tocomtype(QString)));
    //选择商品
    QStandardItemModel *comdish=new QStandardItemModel;
    query.execSelect("select ch_dishno, vch_dishname from cey_bt_dish");
    while(query.next()) {
        QList<QStandardItem *> items;
        items << new QStandardItem(query.recordValue("ch_dishno").toString());
        items << new QStandardItem(query.recordValue("vch_dishname").toString());
        comdish->appendRow(items);
    }
    comdish->setHorizontalHeaderLabels(QStringList() << QObject::tr("编号") << QObject::tr("名称"));

    //! sqlinit
    tab1sqlinit();
    tab2sqlinit();

    //时段
    ui->comboBox_period->addItems(tab1info.keys());

    querymodel=new datetimeModel;
    standmodel=new datetimestandModel;
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(standmodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("查询")<<QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    //tabwidget header
    ui->comboBox->addItem(QObject::tr("商品销售统计表"));

    ui->comboBox_end->hide();
    ui->comboBox_start->hide();
}

w_rpt_report_daymonweekanalysis::~w_rpt_report_daymonweekanalysis()
{
}

void w_rpt_report_daymonweekanalysis::tab1sqlinit()
{
    tab1info.insert(QObject::tr("日"),
                    w_rpt_report::TABTYPE(QStringList(),
                            QString (

                                " SELECT A.ch_dishno,"
                                " CC.vch_dishname,"
                                " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=CC.ch_unitno) as 'vch_unitname',"
                                " ifnull(SUM(ifnull(A.num_num - A.num_back, 0)), 0) as 'num_num',"
                                " CC.num_price1,"
                                "  DATE_FORMAT(D.dt_operdate, '%Y-%m-%d') as 'dt_date' "
                                " FROM cey_u_orderdish A,"
                                " cey_u_checkout_master D,"
                                " (Select distinct a.ch_dishno,"
                                " a.ch_unitno,"
                                " a.vch_dishname,"

                                " a.num_price as 'num_price1' "
                                " from cey_bt_dish a, cey_u_orderdish b"
                                " where ch_stopflag <> 'Y'"
                                " and b.ch_dishno = a.ch_dishno"
                                " and a.ch_dish_typeno like '%1%'"
                                " and a.ch_sub_typeno like '%2%'"
                                " and a.ch_dishno like '%3%'"
                                " union"
                                " Select ch_dishno,"

                                " ch_unitno,"
                                " vch_dishname,"

                                " num_price as 'num_price1' "
                                " from cey_bt_dish"
                                " where ch_stopflag <> 'Y'"
                                " and ch_dish_typeno like '%1%'"
                                " and ch_sub_typeno like '%2%'"
                                " and ch_dishno like '%3%') CC"
                                " where A.ch_dishno = CC.ch_dishno"
                                " AND A.ch_billno = D.ch_billno"
                                " and A.ch_payno = D.ch_payno"
                                " and D.ch_state = 'Y'"
                                " and DATE_FORMAT(D.dt_operdate, '%Y-%m-%d') >=DATE_FORMAT('%4', '%Y-%m-%d')  "
                                " and DATE_FORMAT(D.dt_operdate, '%Y-%m-%d') <=DATE_FORMAT('%5', '%Y-%m-%d')  "
                                " group by A.ch_dishno,"
                                " CC.num_price1,"
                                " CC.vch_dishname,"
                                " CC.ch_unitno"
                                " order by dt_date asc"
                                )
                            ));
    tab1info.insert(QObject::tr("月"),
                    w_rpt_report::TABTYPE(QStringList(),
                            QString (

                                " SELECT A.ch_dishno,"
                                " CC.vch_dishname,"
                                " (select vch_unitname from cey_bt_unit where cey_bt_unit.ch_unitno=CC.ch_unitno) as 'vch_unitname',"
                                " ifnull(SUM(A.num_num - A.num_back), 0)  as 'num_num',"
                                " CC.num_price as 'num_price1',"

                                " DATE_FORMAT(D.dt_operdate, '%Y-%m')  as 'dt_date' "
                                " FROM cey_u_orderdish       A,"
                                " cey_u_checkout_master D,"
                                " cey_bt_dish           CC"

                                " where A.ch_dishno = CC.ch_dishno"
                                " AND A.ch_billno = D.ch_billno"
                                " and A.ch_payno = D.ch_payno"
                                " and D.ch_state = 'Y'"
                                " and CC.ch_dish_typeno like '%1%'"
                                " and CC.ch_sub_typeno like '%2%'"
                                " and CC.ch_dishno like '%3%'"
                                " and DATE_FORMAT(D.dt_operdate, '%Y-%m') >= DATE_FORMAT('%4', '%Y-%m') "
                                " and DATE_FORMAT(D.dt_operdate, '%Y-%m') >= DATE_FORMAT('%5', '%Y-%m') "
                                " group by A.ch_dishno, CC.num_price,"
                                " CC.vch_dishname, CC.ch_unitno"
                                " order by dt_date asc"
                                )
                            ));
 }

void w_rpt_report_daymonweekanalysis::modeltranslate(const QString &sql, QStandardItemModel *model)
{
    model->removeColumns(0, model->columnCount());
    model->removeRows(0,model->rowCount());
    //ch_dishno] vch_dishname] vch_unitname] num_num] num_price] dt_date]
    QVector<QStandardItemModel *> models;//记录多个models
    lds_query_hddpos  query;
    query.execSelect(sql);

    QList<QStandardItem* >ch_dishno_column_items;
    QList<QStandardItem* >vch_dishname_column_items;
    QList<QStandardItem* >vch_unitname_column_items;
    QList<QStandardItem* >num_price_column_items;

    //分割
    QStringList interheaders;
    QString pre;

    while(query.next()) {
        QList<QStandardItem *> items;
        items<<new QStandardItem(query.recordValue("ch_dishno").toString());
        items<<new QStandardItem(query.recordValue("vch_dishname").toString());
        items<<new QStandardItem(query.recordValue("vch_unitname").toString());
        items<<new QStandardItem(query.recordValue("num_num").toString());
        items<<new QStandardItem(query.recordValue("num_price1").toString());
        items<<new QStandardItem(query.recordValue("dt_date").toString());

        if(updateitems(ch_dishno_column_items, items[0]->text())) {
            vch_dishname_column_items<<new QStandardItem(items[1]->text());
            vch_unitname_column_items<<new QStandardItem(items[2]->text());
            num_price_column_items<<new QStandardItem(items[4]->text());
        }
        if(pre!=items.last()->text()) {
            pre=items.last()->text();
            models.append(new QStandardItemModel);
            interheaders.append(items.last()->text());
        }
        models.last()->appendRow(items);
    }


    //拼接
    QStringList headers;
    model->appendColumn(ch_dishno_column_items);headers.append(QObject::tr("编号"));//编号
    model->appendColumn(vch_dishname_column_items);headers.append(QObject::tr("名称"));//名称
    model->appendColumn(vch_unitname_column_items);headers.append(QObject::tr("单位"));//单位
    model->appendColumn(num_price_column_items);headers.append(QObject::tr("价格"));//价格
    headers.append(interheaders);//中间日期
    QList<QStandardItem* >num_total_items;headers.append(QObject::tr("总销售量"));//总销售量
    QList<QStandardItem* >price_total_items;headers.append(QObject::tr("金额"));//金额

    for(int r=0,r_count=ch_dishno_column_items.count();r<r_count;r++) {
        num_total_items<<new QStandardItem("0");
        price_total_items<<new QStandardItem("0");
    }
    foreach(QStandardItemModel *model_t, models) {//2015-12-25。。。
        QList<QStandardItem *> columnitems;
        int model_t_row;
        int model_t_rowcount=model_t->rowCount();
        QString columntext;
        foreach(QStandardItem *item, ch_dishno_column_items) {
            //model_t的递增和ch_dishno_column_items规律一致
            columntext="";
            for(model_t_row=0;model_t_row<model_t_rowcount;model_t_row++) {
                if(item->text()==model_t->item(model_t_row, 0)->text()) {
                    columntext=model_t->item(model_t_row, 3)->text();
                    break;
                }
            }
            columnitems<<new QStandardItem(columntext);
        }
        model->appendColumn(columnitems);
        for(int r=0,r_count=columnitems.count();r < r_count;r++) {
            num_total_items[r]->setText(
                        QString::number(num_total_items[r]->text().toDouble() + columnitems[r]->text().toDouble()));
        }
    }
    for(int r=0,r_count = num_total_items.count();r<r_count;r++) {
        price_total_items[r]->setText(QString::number(model->item(r,3)->text().toDouble() * num_total_items[r]->text().toDouble()));
    }
    model->appendColumn(num_total_items);
    model->appendColumn(price_total_items);
    model->setHorizontalHeaderLabels(headers);

}

bool w_rpt_report_daymonweekanalysis::updateitems(QList<QStandardItem *> &items, const QString &text)
{
    foreach(QStandardItem *item, items) {
        if(item->text()==text)return false;
    }

    items<< new QStandardItem(text);
    return true;
}

void w_rpt_report_daymonweekanalysis::tab2sqlinit()
{
    tab2info.insert(QObject::tr("日"),
                    w_rpt_report::TABTYPE(QStringList(),
                            QString (

                                //Select "business_day" = cast('2015-03-26' as varchar(10)),
                                " Select  ifnull(count(distinct a.ch_billno), 0) as 'bills',"
                                " ifnull((Select sum(bb.int_person) as 'person' "
                                " From cey_u_table bb, cey_u_checkout_master cc"
                                " Where bb.ch_payno = cc.ch_payno"

                                " and cc.ch_state = 'Y'"
                                " and cc.dt_operdate >= '%1'"
                                " and cc.dt_operdate <= '%2'),"
                                " 0),"
                                "  ifnull(count(distinct b.ch_tableno), 0) as 'tables',"
                                "  ifnull(sum(b.num_cost), 0) as 'cost',"
                                "  ifnull(sum(b.num_present), 0) as 'present',"
                                "  ifnull(sum(b.num_discount), 0) as 'discount',"
                                "  ifnull(sum(ifnull(b.num_service, 0)), 0) as  'service',"
                                "  ifnull(sum(ifnull(b.num_lowcost, 0)), 0) as 'low',"
                                " "
                                " ifnull(sum(b.num_cost + ifnull(b.num_service, 0) +"
                                " ifnull(b.num_lowcost, 0) - b.num_present - b.num_discount),"
                                " 0)  as 'pay_amt'"
                                " from cey_u_table a, cey_u_checkout_master b"
                                " where a.ch_billno = b.ch_billno"
                                " and a.ch_tableno = b.ch_tableno"
                                " and a.ch_payno = b.ch_payno"
                                " and a.ch_state = '1'"
                                " and b.ch_state = 'Y'"
                                " and b.dt_operdate >= '%1'"
                                " and b.dt_operdate <= '%2'"
                                )
                            ));
    tab2info.insert(QObject::tr("月"),
                    w_rpt_report::TABTYPE(QStringList(),
                            QString (
                                //Select "business_day" = cast('2015-03-26' as varchar(10)),
                                " Select ifnull(count(distinct a.ch_billno), 0) as 'bills',"
                                "  ifnull((Select sum(bb.int_person) as 'person"
                                " From cey_u_table bb, cey_u_checkout_master cc"
                                " Where bb.ch_payno = cc.ch_payno"

                                " and cc.ch_state = 'Y'"
                                " and cc.dt_operdate >= '%1'"
                                " and cc.dt_operdate <= '%2'),"
                                " 0),"
                                " ifnull(count(distinct b.ch_tableno), 0) as 'tables',"
                                " ifnull(sum(b.num_cost), 0) as 'cost',"
                                " ifnull(sum(b.num_present), 0) as 'present',"
                                " ifnull(sum(b.num_discount), 0) as 'discount',"
                                " ifnull(sum(ifnull(b.num_service, 0)), 0) as 'service',"
                                " ifnull(sum(ifnull(b.num_lowcost, 0)), 0 as 'low'),"
                                " "
                                " ifnull(sum(b.num_cost + ifnull(b.num_service, 0) +"
                                " ifnull(b.num_lowcost, 0) - b.num_present - b.num_discount),"
                                " 0) as 'pay_amt"
                                " from cey_u_table a, cey_u_checkout_master b"
                                " where a.ch_billno = b.ch_billno"
                                " and a.ch_tableno = b.ch_tableno"
                                " and a.ch_payno = b.ch_payno"
                                " and a.ch_state = '1'"
                                " and b.ch_state = 'Y'"
                                " and b.dt_operdate >= '%1'"
                                " and b.dt_operdate <= '%2'"
                                )
                            ));
}

void w_rpt_report_daymonweekanalysis::modeltranslate_2(const QString &sql, QStandardItemModel *model)
{
    QStringList headers;
    model->removeColumns(0, model->columnCount());
    model->removeRows(0,model->rowCount());
    lds_query_hddpos  query;
    int begin=ui->comboBox_start->currentIndex();
    int end=ui->comboBox_end->currentIndex();
    //! <遍历所有营业日 begin>
    for(;begin<=end;begin++) {
        QList<QStandardItem *> items;
        QString column0=ui->comboBox_start->model()->index(begin,0).data().toString();
        query.execSelect(sql);
        if(query.next()) {
            if(ui->comboBox_period->currentText()==QObject::tr("月")) {
                items<<new QStandardItem(QDateTime::fromString(column0, "yyyy-MM-dd").toString("yyyy-MM"));updateheaders(headers, QObject::tr("月份"));
            } else {
                items<<new QStandardItem(column0);updateheaders(headers, QObject::tr("营业日"));
            }
            items<<new QStandardItem(query.recordValue("bills").toString());updateheaders(headers, QObject::tr("单数"));
            items<<new QStandardItem(query.recordValue("person").toString());updateheaders(headers, QObject::tr("人数"));
            items<<new QStandardItem(query.recordValue("tables").toString());updateheaders(headers, QObject::tr("餐桌使用数"));
            items<<new QStandardItem(query.recordValue("cost").toString());updateheaders(headers, QObject::tr("消费金额"));
            items<<new QStandardItem(query.recordValue("present").toString());updateheaders(headers, QObject::tr("赠送金额"));
            items<<new QStandardItem(query.recordValue("discount").toString());updateheaders(headers, QObject::tr("折扣"));
            items<<new QStandardItem(query.recordValue("service").toString());updateheaders(headers, QObject::tr("服务费"));
            items<<new QStandardItem(query.recordValue("low").toString());updateheaders(headers, QObject::tr("低消金额"));
            items<<new QStandardItem(query.recordValue("pay_amt").toString());updateheaders(headers, QObject::tr("应收金额"));
            items<<new QStandardItem("0");updateheaders(headers, QObject::tr("人均消费"));
            if(query.recordValue("person").toDouble()!=0) {
                items.last()->setText(QString::number(
                                          query.recordValue("pay_amt").toDouble()/query.recordValue("person").toDouble())
                                      );
            }
            items<<new QStandardItem("0");updateheaders(headers, QObject::tr("餐桌平均消费"));
            if(query.recordValue("tables").toDouble()!=0) {
                items.last()->setText(QString::number(
                                          query.recordValue("pay_amt").toDouble()/query.recordValue("tables").toDouble())
                                      );
            }
            if(ui->comboBox_period->currentText()==QObject::tr("月")) {
                bool manualbreak=false;
                //! <将原来的model 的某列增加 begin>
                for(int r=0,r_count=model->rowCount();r<r_count;r++) {
                    if(model->item(r, 0)->text()==items[0]->text()) {
                        for(int c=1,c_count=model->columnCount();c<c_count;c++) {
                            model->item(r,c)->setText(QString::number(model->item(r,c)->text().toDouble() + items[r]->text().toDouble()));
                        }
                        manualbreak=true;
                        break;
                    }
                }
                //! <将原来的model 的某列增加 end>
                if(!manualbreak)
                    model->appendRow(items);
            } else {
                model->appendRow(items);
            }
        }
    }
    //! <遍历所有营业日 end>
    model->setHorizontalHeaderLabels(headers);
}

void w_rpt_report_daymonweekanalysis::toselect()
{
    QString ls_begin;
    QString ls_end;
    ls_begin = ui->dateTimeEdit->dateTimeStr();
    ls_end = ui->dateTimeEdit_2->dateTimeStr();

    QString ch_dish_typeno = backheader::getinterdata(ui->comboBox_series->currentText());
    QString ch_sub_typeno = backheader::getinterdata(ui->comboBox_type->currentText());
    QString ch_dishno=ui->selectgoods->gettext();

    modeltranslate(tab1info[ui->comboBox_period->currentText()].sql
                   .arg(ch_dish_typeno)
                   .arg(ch_sub_typeno)
                   .arg(ch_dishno)
                   .arg(ls_begin)
                   .arg(ls_end),
                   standmodel
                   );
    ui->tableView->updateSum(4);
    ui->tableView->updateSum(5);
    ui->tableView->updateSum(6);
    ui->tableView->updateSum(7);
    ui->tableView->updateSum(8);

    if(standmodel->rowCount() > 0) {
        ui->tableView->resizeColumnsToContents();
    }
}

void w_rpt_report_daymonweekanalysis::toexit()
{
    this->reject();
}

void w_rpt_report_daymonweekanalysis::tocomtype(const QString &text)
{
    QStringList comtypes;
    comtypes<<QObject::tr("全部");
    if(text.isEmpty() || text==QObject::tr("全部")) {

    } else {
        QString filter=text.split("(")[1];
        filter.chop(1);
        lds_query_hddpos  query;
        query.execSelect(QString("select concat(vch_sub_typename,'(',ch_sub_typeno,')') from cey_bt_dish_subtype where ch_dish_typeno='%1'").arg(filter));
        while(query.next()) {
            comtypes<<query.recordValue(0).toString();
        }
    }
    ui->comboBox_type->addItems(comtypes);
}

void w_rpt_report_daymonweekanalysis::updateheaders(QStringList &headers, const QString &headername)
{
    if(!headers.contains(headername))
        headers.append(headername);
}

