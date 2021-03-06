#include "w_rpt_report_frame_servicelowcost.h"
#include "w_rpt_report_frame_servicelowcost.h"
#include "ui_w_rpt_report_frame_servicelowcost.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>

w_rpt_report_frame_servicelowcost::w_rpt_report_frame_servicelowcost(QWidget *parent) :
    w_rpt_report_frame(QStringList(), parent)
{
    ui_help = new Ui::w_rpt_report_frame_servicelowcost;
    ui_help->setupUi(this);
    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
}

w_rpt_report_frame_servicelowcost::~w_rpt_report_frame_servicelowcost()
{
    delete ui_help;
}

void w_rpt_report_frame_servicelowcost::toselect()
{
    int index = ui->stackedWidget->currentIndex();
    QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(index));
    //
    static int onepagestep = -1;
    if(onepagestep == -1) {
        QTableWidget t;
        t.setColumnCount(1);
        onepagestep = ( tableview->height() - tableview->horizontalScrollBar()->height()- t.horizontalHeader()->height() ) / tableview->verticalHeader()->defaultSectionSize();
    }
    //
    ui->pushButton_step->init(index);
    switch(index) {
    case 0:
    {
        //
        //
        //
    }
        break;
    }
    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
    

}
