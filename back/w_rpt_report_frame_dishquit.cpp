#include "w_rpt_report_frame_dishquit.h"
#include "ui_w_rpt_report_frame_dishquit.h"
#include "ui_w_rpt_report_frame_uncheckout.h"
#include "ui_w_rpt_report_frame.h"
#include "n_func.h"
#include "backheader.h"
#include <QtDebug>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTableWidget>
#include <QScrollBar>
#include "lds.h"

w_rpt_report_frame_dishquit::w_rpt_report_frame_dishquit(QWidget *parent) :
    w_rpt_report_frame(QStringList() << QObject::tr("流水"), parent)
{
    ui_help = new Ui::w_rpt_report_frame_dishquit;
    ui_help->setupUi(ui->frame_other);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->widget->setup(QStringList() << QObject::tr("查询")<< QObject::tr("导出")<< QObject::tr("打印")<< QObject::tr("退出"));
    //
    lds::hideWidget(ui->pushButton_step);

    connect(ui->widget->index_widget(QObject::tr("查询")), SIGNAL(clicked()),this,SLOT(toselect()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->widget->index_widget(QObject::tr("导出")), SIGNAL(clicked()),this,SLOT(toexport()));
    connect(ui->widget->index_widget(QObject::tr("打印")), SIGNAL(clicked()),this,SLOT(toprint()));
    connect(ui_help->checkBox_uncheck, SIGNAL(clicked(bool)), this, SLOT(toselect()));
}

w_rpt_report_frame_dishquit::~w_rpt_report_frame_dishquit()
{
    delete ui_help;
}

void w_rpt_report_frame_dishquit::toselect()
{
    pushbutton_auto_enable dd(ui->widget->index_widget(QObject::tr("查询")));

    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());
    //
    static int onepagestep = -1;
    if(onepagestep == -1) {
        QTableWidget t;
        t.setColumnCount(1);
        onepagestep = ( tableview->height() - tableview->horizontalScrollBar()->height()- t.horizontalHeader()->height() ) / tableview->verticalHeader()->defaultSectionSize();
    }
    //
    QString select_sql =
            " SELECT '"+
            QObject::tr("点单退单") + "'  AS 'ch_type',"
                         "  a.ch_billno,"
                         "  a.ch_dishno as vch_dishname,"
                         "  a.num_back,"
                         "  a.vch_back_operID, "
                         "  a.dt_back_operdate, "
                         "  (SELECT vch_reason FROM cey_u_orderdish_log WHERE	a.int_flowid = int_orderflow AND ch_type = '1' LIMIT 1) AS `vch_reason`"
                         "  FROM"
                         "   cey_u_orderdish a,"
                         "   cey_u_checkout_master b"
                         "  WHERE"
                         "   a.ch_payno = b.ch_payno"
                         "       AND a.num_back > 0 "
                         "       AND b.ch_state = 'Y' "
                         "       AND b.dt_operdate >= '%1' "
                         "       AND b.dt_operdate <= '%2' "
                         "       AND a.ch_billno like '%3%' "
                         "       union ALL "
                         "       SELECT '" +
            QObject::tr("外卖退单") + "' AS `ch_type`, "
                         "                   a.ch_togono, "
                         "                   a.ch_dishno, "
                         "                   a.num_back, "
                         "                   b.vch_operID AS `vch_back_operID`, "
                         "                   b.dt_operdate AS `dt_back_operdate`, "
                         "                   (SELECT vch_reason FROM cey_u_orderdish_log WHERE	a.int_flowid = int_orderflow AND ch_type = '1' LIMIT 1) AS `vch_reason` "
                         "               FROM "
                         "                   cey_u_orderdish a, "
                         "                   cey_u_togo b "
                         "               WHERE "
                         "                   a.ch_togono = b.ch_togono "
                         "               AND b.ch_state = 'P' "
                         "               AND a.num_back > 0 "
                         "               AND b.dt_operdate >= '%1' "
                         "               AND b.dt_operdate <= '%2' "
                         "               AND a.ch_togono like '%3%' ";
    if(ui_help->checkBox_uncheck->isChecked()) {
        select_sql +=
                "               union ALL "
                "               SELECT '"+
                QObject::tr("反结账") + "' AS `ch_type`, "
                            "             a.ch_billno, "
                            "             a.ch_dishno,  "
                            "             (a.num_num - a.num_back) as `num_back`, "
                            "              b.vch_operID2 AS `vch_back_operID`,  "
                            "              b.dt_operdate2 AS `dt_back_operdate`,  "
                            "               (SELECT vch_reason FROM cey_u_orderdish_log WHERE	a.int_flowid = int_orderflow AND ch_type = '1' LIMIT 1) AS `vch_reason` "
                            "           FROM "
                            "               cey_u_orderdish a, "
                            "               cey_u_checkout_master b "
                            "           WHERE "
                            "               a.ch_billno = b.ch_billno "
                            "           AND b.ch_state = 'P' "
                            "           AND (a.num_num - a.num_back) <> 0 "
                            "           AND b.dt_operdate >= '%1' "
                            "           AND b.dt_operdate <= '%2' "
                            "       AND a.ch_billno like '%3%' ";
    }
    ui->pushButton_step->init(index, 0, -1,select_sql
                              .arg(ui->dateTimeEdit->dateTimeStr())
                              .arg(ui->dateTimeEdit_2->dateTimeStr())
                              .arg(ui_help->lineEdit_billno->text())
                              );
    toselect_page();
    if(tableview->itemDelegateForColumn(model->fieldIndex("vch_dishname")) == 0) {
        tableview->setItemDelegateForColumn(model->fieldIndex("vch_dishname"),
                                            new lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish", "ch_dishno", "vch_dishname"));
        tableview->setItemDelegateForColumn(model->fieldIndex("vch_back_operID"),
                                                           new lds_model_sqltablemodel_delegate_com(this, "("+backheader::SELECT_OPERID_NAME+")t", "vch_operId", "vch_operator" ));

        tableview->resizeColumnsToContents2();
    }


    w_rpt_report_frame_btn_pageindex::total_desc_value_Data sum_d;
    sum_d.fieldname = "num_back";
    sum_d.value = 0;
    for(int k = 0; k < model->rowCount(); k ++) {
        sum_d.value += model->model_data(k, "num_back").toDouble();
    }
    QList<w_rpt_report_frame_btn_pageindex::total_desc_value_Data> sum_d_list;
    sum_d_list.append(sum_d);
    sum_filldata(sum_d_list);
}

void w_rpt_report_frame_dishquit::toprint()
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
