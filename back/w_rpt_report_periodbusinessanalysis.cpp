#include "w_rpt_report_periodbusinessanalysis.h"
//时段营业情况分析
#include "ui_w_rpt_report_periodbusinessanalysis_dialog.h"
#include "lds_query_hddpos.h"

#include "n_func.h"
#include "lds_tableview.h"
#include <QSqlError>
#include "backheader.h"
#include "lds_messagebox.h"
#include "lds_chartdialog.h"
#include "lds_chart_nightcharts.h"
#include "lds.h"

w_rpt_report_periodbusinessanalysis::w_rpt_report_periodbusinessanalysis(QWidget *parent) :
    QDialog(parent),ui(new Ui_w_rpt_report_periodbusinessanalysis_Dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame->setFixedWidth(w_rpt_report::frame_left_fixed_width);
    //! init
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
//    ui->dateTimeEdit->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(0,0,0)));
    ui->dateTimeEdit_2->setDateTime(QDateTime(n_func::f_get_sysdatetime().date(), QTime(23,59,59)));

    tab1sqlinit();
    ui->comboBox_charttype->addItems(QStringList()  << QObject::tr("开台桌位数") << QObject::tr("就餐人数")<< QObject::tr("消费项目") << QObject::tr("消费金额") << QObject::tr("收银次数") << QObject::tr("收银金额"));
    //! tableview
    standmodel=new lds_model_sqlstandardmodel(this);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);ui->tableView->setModel(standmodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //! toolbar
    ui->widget->setup(QStringList()<<QObject::tr("查询") << QObject::tr("图表分析")<<QObject::tr("退出"), Qt::AlignTop);
    connect(ui->widget->index_widget(QObject::tr("查询")),SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("图表分析")),SIGNAL(clicked()),this,SLOT(tochart()));
    connect(ui->widget->index_widget(QObject::tr("退出")),SIGNAL(clicked()),this,SLOT(toexit()));
    //    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(toselect()));
    ui->comboBox->addItem(QObject::tr("时段营业情况分析"));
}

void w_rpt_report_periodbusinessanalysis::tab1sqlinit()
{
}

void w_rpt_report_periodbusinessanalysis::toselect()
{
    //ue_query (none) returns (none)
    QString ls_begin;
    QString ls_end;
    QString ls_start;
    QString ls_stop;
    QString ls_begin_print;
    QString ls_end_print;
    QDateTime ldt_start;
    QDateTime ldt_stop;
    QDateTime ldt_stop1;
    qlonglong ll_period;
//    qlonglong ll_minute;
    qlonglong ll_count;
//    qlonglong ll_i;
    qlonglong ll_pay;
//    qlonglong ll_row;
    qlonglong ll_free;
    qlonglong ll_table;
    qlonglong ll_person;
    double ldec_itemnum;
    double ldec_pay;
    double ldec_itemamount;

    double ll_table_total=0.0;
    double ll_person_total=0.0;
    double lldec_itemnum_total=0.0;
    double ldec_itemamount_total=0.0;
    double ll_pay_free_total=0.0;
    double ldec_pay_total=0.0;

    lds_query_hddpos  query;
    QList<QStandardItem *> items;
    standmodel->removeRows(0, standmodel->rowCount());
    if(ui->dateTimeEdit->dateTime() > ui->dateTimeEdit_2->dateTime()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("开始时间不能大于结束时间"));
        return;
    }
    //获取开始时段 和 结束时段的 中间间隔分钟数
//    QTime().secsTo(ui->dateTimeEdit->dateTime().time())/60;
//    QTime().secsTo(ui->dateTimeEdit_2->dateTime().time())/60;

    ls_begin = ui->dateTimeEdit->dateTimeStr();
    ls_end = ui->dateTimeEdit_2->dateTimeStr();
    ls_start=ui->dateTimeEdit->dateTime().toString("hh:mm");
    ls_stop=ui->dateTimeEdit_2->dateTime().toString("hh:mm");
    ll_period=ui->spinBox_period->value();

    if(ls_stop < ls_start) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("结束时段不能小于起始时段"));
        return;
    }
    if(ll_period<=0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("时段间隔不能小于或等于零"));
        return;
    }
    ldt_start=QDateTime(QDate(2001,01,01), QTime::fromString(ls_start, "hh:mm"));
    ldt_stop=QDateTime(QDate(2001,01,01), QTime::fromString(ls_stop, "hh:mm"));
    ll_count=w_rpt_report::sqltime_mindiff(ldt_start, ldt_stop);
    ll_count +=1;
    if(ll_count%ll_period>0) {
        ll_count = ll_count / ll_period + 1;
    } else {
        ll_count = ll_count / ll_period;
    }

    for(long ll_i=0;ll_i < ll_count;ll_i++) {
        ldt_stop1=ldt_start.addSecs((ll_period-1)*60);
        ls_start=ldt_start.toString("hh:mm");
        ls_stop=ldt_stop1.toString("hh:mm");

        query.execSelect(QString("select count(a.ch_tableno), IFNULL(sum(a.int_person), 0)from cey_u_table a,  cey_u_checkout_master b  where a.ch_payno =b.ch_payno and b.ch_state ='Y' and  b.dt_operdate >=  '%1' and b.dt_operdate <= '%2' and DATE_FORMAT(b.dt_operdate, '%H:%i') >= '%3' and DATE_FORMAT(b.dt_operdate, '%H:%i') <= '%4'")
                      .arg(ls_begin)
                      .arg(ls_end)
                      .arg(ls_start)
                      .arg(ls_stop)
                      );
        query.next();
        ll_table=query.recordValue(0).toLongLong();
        ll_person=query.recordValue(1).toLongLong();

        query.execSelect(QString("select IFNULL(sum(a.num_num - a.num_back), 0), IFNULL(sum(case when a.ch_presentflag ='Y' then 0 else ((a.num_num - a.num_back) * a.num_price)*0.01 *a.int_discount end), 0)from cey_u_orderdish a,  cey_u_checkout_master b    where(a.ch_payno =b.ch_payno and b.ch_state ='Y')and b.dt_operdate>= '%1' and b.dt_operdate <= '%2' and DATE_FORMAT(b.dt_operdate, '%H:%i')  >= '%3' and DATE_FORMAT(b.dt_operdate, '%H:%i')  <= '%4'")
                .arg(ls_begin)
                .arg(ls_end)
                .arg(ls_start)
                .arg(ls_stop)
                );
        query.next();
        ldec_itemnum=query.recordValue(0).toLongLong();
        ldec_itemamount=query.recordValue(1).toLongLong();

        query.execSelect(QString("select count(a.ch_payno), IFNULL(sum(a.num_realamount), 0)  from cey_u_checkout_master b,  cey_u_checkout_detail a   where a.ch_payno =b.ch_payno and b.ch_state ='Y' and b.dt_operdate >= '%1' and b.dt_operdate <= '%2' and DATE_FORMAT(b.dt_operdate, '%H:%i')  >= '%3' and DATE_FORMAT(b.dt_operdate, '%H:%i')  <= '%4'")
                .arg(ls_begin)
                .arg(ls_end)
                .arg(ls_start)
                .arg(ls_stop)
                );
        query.next();
        ll_pay=query.recordValue(0).toLongLong();
        ldec_pay=query.recordValue(1).toDouble();

        query.execSelect(QString("select count(a.ch_paymodeno), sum(a.num_realamount) from cey_u_checkout_master b,  cey_u_checkout_detail a    where a.ch_payno =b.ch_payno and b.ch_state ='Y' and a.ch_paymodeno ='XX' and b.dt_operdate >= '%1' and b.dt_operdate <= '%2' and DATE_FORMAT(b.dt_operdate, '%H:%i')  >= '%3' and DATE_FORMAT(b.dt_operdate, '%H:%i')  <= '%4'")
                .arg(ls_begin)
                .arg(ls_end)
                .arg(ls_start)
                .arg(ls_stop)
                );
        query.next();
        ll_free=query.recordValue(0).toLongLong();

        items.clear();
        items << lds_model_sqlstandardmodel::newItem(QString("%1-%2").arg(ls_start).arg(ls_stop));//时段
        items << lds_model_sqlstandardmodel::newItem(ll_table);//开台数
        ll_table_total+=items.last()->text().toDouble();
        items << lds_model_sqlstandardmodel::newItem("");//ll_table_total2

        items << lds_model_sqlstandardmodel::newItem(ll_person);//人数
        ll_person_total+=items.last()->text().toDouble();
        items << lds_model_sqlstandardmodel::newItem("");//ll_person_total4

        items << lds_model_sqlstandardmodel::newItem(ldec_itemnum);//消费项数
        lldec_itemnum_total+=items.last()->text().toDouble();
        items << lds_model_sqlstandardmodel::newItem("");//lldec_itemnum_total6

        items << lds_model_sqlstandardmodel::newItem(ldec_itemamount);//消费金额
        ldec_itemamount_total+=items.last()->text().toDouble();
        items << lds_model_sqlstandardmodel::newItem("");//ldec_itemamount_total8

        items << lds_model_sqlstandardmodel::newItem(ll_pay+ll_free);//收银次数
        ll_pay_free_total+=items.last()->text().toDouble();
        items << lds_model_sqlstandardmodel::newItem("");//ll_pay_free_total10

        items << lds_model_sqlstandardmodel::newItem(ldec_pay);//收银金额
        ldec_pay_total+=items.last()->text().toDouble();
        items << lds_model_sqlstandardmodel::newItem("");//ldec_pay_total12

        standmodel->appendRow(items);

        ldt_start=ldt_stop1.addSecs(60);
    }
    for(int r=0,r_count=standmodel->rowCount(); r<r_count;r++) {
        for(int c=0,c_count=standmodel->columnCount(); c<c_count;c++) {
            standmodel->model_data_set(r, 2, (ll_table_total==0? 0.0: (standmodel->model_data(r, 1).toDouble() / ll_table_total)));
            standmodel->model_data_set(r, 4, (ll_person_total==0? 0.0: (standmodel->model_data(r,3).toDouble() / ll_person_total)));
            standmodel->model_data_set(r, 6, (lldec_itemnum_total==0? 0.0: (standmodel->model_data(r,5).toDouble() / lldec_itemnum_total)));
            standmodel->model_data_set(r, 8, (ldec_itemamount_total==0? 0.0: (standmodel->model_data(r, 7).toDouble() / ldec_itemamount_total)));
            standmodel->model_data_set(r, 10, (ll_pay_free_total==0? 0.0: (standmodel->model_data(r, 9).toDouble() / ll_pay_free_total)));
            standmodel->model_data_set(r, 12, (ldec_pay_total==0? 0.0: (standmodel->model_data(r, 11).toDouble() / ldec_pay_total)));
        }
    }
    //更新表头
    standmodel->setHorizontalHeaderLabels(QStringList () << QObject::tr("时段") << QObject::tr("开台桌位数") << QObject::tr("比例") << QObject::tr("就餐人数") << QObject::tr("比例") << QObject::tr("消费项数") << QObject::tr("比例") << QObject::tr("消费金额") << QObject::tr("比例") << QObject::tr("收银次数") << QObject::tr("比例") << QObject::tr("收银金额") << QObject::tr("比例"));


    //更新合计
    ui->tableView->updateSumRange(1, 12);
    ui->tableView->resizeColumnsToContents();
}

void w_rpt_report_periodbusinessanalysis::tochart()
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

void w_rpt_report_periodbusinessanalysis::toexit()
{
    this->reject();
}

void w_rpt_report_periodbusinessanalysis::tocom12(const QString &/*text*/)
{

}
