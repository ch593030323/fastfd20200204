#include "w_rpt_report_categorysalesanalysis.h"
//品类销售营业分析
#include "ui_w_rpt_report_categorysalesanalysis_dialog.h"
#include "lds_query_hddpos.h"

#include "n_func.h"
#include "lds_tableview.h"
#include <QSqlError>
#include "backheader.h"
#include "lds_chartdialog.h"
#include "lds_chart_nightcharts.h"
#include "lds.h"

w_rpt_report_categorysalesanalysis::w_rpt_report_categorysalesanalysis(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_rpt_report_categorysalesanalysis_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //! init
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    //    ui->dateTimeEdit->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));

    tab1sqlinit();

    standmodel=new lds_model_sqlstandardmodel(this);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(standmodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->comboBox_charttype->addItems(QStringList() << QObject::tr("销售数量") << QObject::tr("销售金额") << QObject::tr("折扣") << QObject::tr("赠送金额") << QObject::tr("实际金额"));
    //! toolbar
    //    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));
    ui->widget->setup(QStringList()<<QObject::tr("查询")<< QObject::tr("图表分析") <<QObject::tr("退出"),Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("图表分析")),SIGNAL(clicked()),this,SLOT(tochart()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    //! after
    //    ui->comboBox_series->addItems(w_rpt_report::querymodelsetfilters("select * from cey_bt_dish_type"));
    //stackedWidget header
    ui->stackedWidget->setCurrentIndex(0);
    ui->comboBox_dishtype->addItems(QStringList() << QObject::tr("小类")<< QObject::tr("大类") );
}

void w_rpt_report_categorysalesanalysis::tab1sqlinit()
{
    QString intersql =
            " sum(a.num_num - a.num_back) as 'num',"

            " sum(((a.num_num - a.num_back) * a.num_price + a.num_price_add)) as 'amount',"

            " sum(((a.num_num - a.num_back) * a.num_price) *"

            " (1 - a.int_discount * 0.01)) as 'discount',"

            " sum(((case a.ch_presentflag  when 'Y' then (a.num_num - a.num_back) else  0 end) * a.num_price) * a.int_discount * 0.01) as 'present'"

            " FROM cey_u_orderdish a,"

            " cey_u_checkout_master b,"

            " cey_bt_dish c,"

            " cey_bt_dish_type d"

            " WHERE a.ch_payno = b.ch_payno"

            " and b.ch_state = 'Y'"

            " and a.ch_dishno = c.ch_dishno"

            " and c.ch_dish_typeno = d.ch_dish_typeno"

            " and b.dt_operdate >= '%1' "

            " and b.dt_operdate <= '%2' ";

    ///////////
    tab1info.insert(QObject::tr("品类销售营业分析"),
                    w_rpt_report::TABTYPE(QStringList()<<QObject::tr("类型")<<QObject::tr("编号")<<QObject::tr("销售数量")<<QObject::tr("比例")<<QObject::tr("销售金额")<<QObject::tr("比例")<<QObject::tr("折扣")<<QObject::tr("比例")<<QObject::tr("赠送金额")<<QObject::tr("比例")<<QObject::tr("实际金额")<<QObject::tr("比例"),
                                          intersql,
                                          QStringList()
                                          <<
                                          " SELECT c.ch_sub_typeno as 'ch_seriesno', "
                                          " (select d.vch_sub_typename from cey_bt_dish_subtype d where d.ch_sub_typeno = c.ch_sub_typeno) as 'ch_seriesname', %1 GROUP BY c.ch_sub_typeno"
                                          <<
                                          " SELECT c.ch_dish_typeno as 'ch_seriesno', "
                                          " (select d.vch_dish_typename from cey_bt_dish_type d where d.ch_dish_typeno = c.ch_dish_typeno) as 'ch_seriesname', %1 GROUP BY c.ch_dish_typeno"
                                          ));
    ui->comboBox->addItem(QObject::tr("品类销售营业分析"));
    ////////////
}

void w_rpt_report_categorysalesanalysis::toselect()
{
    //
    QString ls_begin;
    QString ls_end;
    ls_begin=ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd hh:mm:ss");
    ls_end=ui->dateTimeEdit_2->dateTime().toString("yyyy-MM-dd hh:mm:ss");

    //model select
    standmodel->removeRows(0, standmodel->rowCount());
    lds_query_hddpos  query;
    QString sql = tab1info[QObject::tr("品类销售营业分析")].sql
            .arg(ls_begin)
            .arg(ls_end)
            ;
    query.execSelect(tab1info[QObject::tr("品类销售营业分析")].filters.at(ui->comboBox_dishtype->currentIndex()).arg(sql));
    double num_total=0.0;
    double amount_total=0.0;
    double discount_total=0.0;
    double present_total=0.0;

    double realamount=0.0;
    double realamount_total=0.0;

    while(query.next()) {
        QList<QStandardItem *> items;
        //商品类型
        items << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_seriesname"));
        //编号
        items << lds_model_sqlstandardmodel::newItem(query.recordValue("ch_seriesno"));

        //销售数量
        items << lds_model_sqlstandardmodel::newItem(query.recordValue("num"));num_total+=items.last()->text().toDouble();
        items << new QStandardItem("");//比例3

        //销售金额
        items << lds_model_sqlstandardmodel::newItem(query.recordValue("amount"));amount_total+=items.last()->text().toDouble();
        items << new QStandardItem("");//比例5

        //折扣
        items << lds_model_sqlstandardmodel::newItem(query.recordValue("discount"));discount_total+=items.last()->text().toDouble();
        items << new QStandardItem("");//比例7

        //赠送金额
        items << lds_model_sqlstandardmodel::newItem(query.recordValue("present"));present_total+=items.last()->text().toDouble();
        items << new QStandardItem("");//比例9

        //实际金额
        realamount=query.recordValue("amount").toDouble()-query.recordValue("discount").toDouble()-query.recordValue("present").toDouble();
        items << lds_model_sqlstandardmodel::newItem(realamount);realamount_total+=items.last()->text().toDouble();
        items << new QStandardItem("");//比例11
        standmodel->appendRow(items);
    }

    for(int r=0,r_count=standmodel->rowCount(); r<r_count;r++) {
        for(int c=0,c_count=standmodel->columnCount(); c<c_count;c++) {
            lds_model_sqlstandardmodel::setItemData(standmodel->item(r,3), (num_total==0? 0.0: (standmodel->item(r, 2)->text().toDouble() / num_total)));
            lds_model_sqlstandardmodel::setItemData(standmodel->item(r, 5), (amount_total==0? 0.0: (standmodel->item(r, 4)->text().toDouble() / amount_total)));
            lds_model_sqlstandardmodel::setItemData(standmodel->item(r, 7), (discount_total==0? 0.0: (standmodel->item(r, 6)->text().toDouble() / discount_total)));
            lds_model_sqlstandardmodel::setItemData(standmodel->item(r, 9), (present_total==0? 0.0: (standmodel->item(r, 8)->text().toDouble() / present_total)));
            lds_model_sqlstandardmodel::setItemData(standmodel->item(r, 11), (realamount_total==0? 0.0: (standmodel->item(r, 10)->text().toDouble() / realamount_total)));
        }
    }
    //更新表头
    standmodel->setHorizontalHeaderLabels(tab1info[ui->comboBox->currentText()].headers);
    //更新合计
    ui->tableView->updateSumRange(2, 11);
    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_categorysalesanalysis::tochart()
{
    lds_chart_nightcharts dialog(this);
    int color_h=240;
    int color_s=240;
    int color_v=240;

    dialog.chart->chart_items.clear();
    for(int r=0,
        r_count=standmodel->rowCount(),
        c=ui->comboBox_charttype->currentIndex()*2+2,
        c_count=standmodel->columnCount();

        r<r_count && c<c_count;

        r++) {
        float cur_value=0.0;
        cur_value=standmodel->item(r, c)->text().toDouble();
        dialog.chart->chart_items.append(
                    lds_chart_nightcharts_content::CHART_ITEMS(standmodel->item(r, 0)->text(),
                                                               QColor::fromHsv(
                                                                   color_h*((r+1)*1.0)/r_count,
                                                                   color_s,
                                                                   color_v),
                                                               cur_value
                                                               ));
    }

    dialog.chart->chart_items_after();
    dialog.setWindowTitle(QObject::tr("统计图"));
    dialog.resize(lds::MAIN_WINDOW_SIZE);
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_categorysalesanalysis::toexit()
{
    this->reject();
}
