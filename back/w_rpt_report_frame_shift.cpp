#include "w_rpt_report_frame_shift.h"
#include "ui_w_rpt_report_frame_shift.h"
#include "ui_w_rpt_report_frame.h"
#include "lds_query_hddpos.h"
#include <QTableWidget>
#include <QScrollBar>
#include <QtDebug>
#include "w_scr_dish_switch_detail.h"
#include "n_func.h"


w_rpt_report_frame_shift::w_rpt_report_frame_shift(QWidget *parent) :
    w_rpt_report_frame(QStringList()
                       << QObject::tr("流水")
                       << QObject::tr("非收入")
                          , parent)
{
    ui_help = new Ui::w_rpt_report_frame_shift;
    ui_help->setupUi(ui->frame_other);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));

    connect(ui_help->pushButton_reprint, SIGNAL(clicked()), this, SLOT(toreprint()));
    //
    ui->tableView_sum->hide();
}

w_rpt_report_frame_shift::~w_rpt_report_frame_shift()
{
    delete ui_help;
}

void w_rpt_report_frame_shift::toselect()
{
    lds_query_hddpos  query;
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
    //    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());
    //
    static int onepagestep = -1;
    if(onepagestep == -1) {
        QTableWidget t;
        t.setColumnCount(1);
        onepagestep = ( tableview->height() - tableview->horizontalScrollBar()->height() - t.horizontalHeader()->height() ) / tableview->verticalHeader()->defaultSectionSize();
    }
    ui->pushButton_step->init(index);
    //
    switch(index) {
    case 0://交班记录
    {
        QString sql_where =
                QString(
                    "FROM cey_u_handover_total a,  cey_u_handover_master b "
                    " where a.int_id = b.int_id and b.dt_end >= '%1' and b.dt_end <= '%2' and b.int_id > 0"
                    )
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                ;
        query.execSelect(QString("select count(0) "+sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " SELECT "
                                      "b.int_id as `"+QObject::tr("序号") + "`,"
                                      "b.dt_end as `"+QObject::tr("交班时间") + "`,"
                                      "b.vch_operID as `"+QObject::tr("操作员") + "`,"
                                      "a.vch_item as `"+QObject::tr("项目") + "`,"
                                      "a.num_amount as `"+QObject::tr("金额") + "`,"
                                      "b.dt_begin as `"+QObject::tr("开始时间") + "`"
                                      +    sql_where
                                      )
                                  );
        toselect_page();
    }
        break;
    case 1://交班时非收入支出
    {
        //a.vch_item, a.num_amount
        QString sql_where =
                QString(
                    " FROM cey_u_handover_total a,  cey_u_handover_master b "
                    " where a.int_id = b.int_id and b.dt_end >= '%1' and b.dt_end <= '%2' and b.int_id > 0"
                    " and a.vch_item2 like '%CASH%' "
                    )
                .arg(ui->dateTimeEdit->dateTimeStr())
                .arg(ui->dateTimeEdit_2->dateTimeStr())
                ;
        query.execSelect(QString("select count(0) "+sql_where));
        query.next();
        int rowcount = query.recordValue(0).toInt();
        ui->pushButton_step->init(index, rowcount, onepagestep,
                                  QString(
                                      " SELECT "
                                      "a.vch_item as `" + QObject::tr("项目") + "`,"
                                      "a.num_amount as `" + QObject::tr("金额") + "`"
                                      +    sql_where
                                      )
                                  );
        toselect_page();
    }
        break;
    }
}

void w_rpt_report_frame_shift::toreprint()
{
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());


    int row = tableview->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有数据被选中"));
        return;
    }

    w_scr_dish_switch_detail::print_by_int_id(model->model_data(row, QObject::tr("序号")).toInt());
}

void w_rpt_report_frame_shift::toprint()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());

    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat *dialog
            = new w_rpt_report_printFormat(w_rpt_report_printFormat::show_model_quick, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(),
                                           QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                                           ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                           tableview, ui->pushButton_step->get_limit_sql(index), ui->tableView_sum,
                                           0);
    dialog->pop(b, this);
}

void w_rpt_report_frame_shift::select_page_after()
{
    tableview_setpan(QObject::tr("序号"), QStringList()
                     << QObject::tr("序号")
                     << QObject::tr("交班时间")
                     << QObject::tr("操作员")
                     << QObject::tr("开始时间")
                     );
}
