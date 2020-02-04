#include "w_scr_dish_main_table_pay.h"
#include "ui_w_scr_dish_main_table_pay.h"
#include "public_sql.h"
#include "lds_roundeddialog_rect_frame.h"

w_scr_dish_main_table_pay::w_scr_dish_main_table_pay(fexpandmain_model_sqltablemodel_data *tablemodel, QWidget *parent, const QList<int> &rowListAsc, w_scr_dish_pay_widget::operatModes flags)
    : w_scr_dish_main(tablemodel->cur_ch_tableno(), tablemodel->cur_ch_billno(), windowTypeZhongCan, parent),
      ui(new Ui::w_scr_dish_main_table_pay)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    //widget
    ui->frame_deskinfo->setGeometry(0, 0, lds::PAY_LEFT_SPACE, 140);
    ui->tableView->setGeometry(0, ui->frame_deskinfo->geometry().bottom(), lds::PAY_LEFT_SPACE,  lds::MAIN_WINDOW_SIZE.height() - ui->frame_deskinfo->height());
    ui->frame_main->setGeometry(ui->frame_deskinfo->geometry().right(), 0, lds::MAIN_WINDOW_SIZE.width() - ui->frame_deskinfo->width(), lds::MAIN_WINDOW_SIZE.height());

    //label_logo
    ui->label_logo->clear();
    ui->label_text->clear();
    ui->label_num->clear();
    ui->label_total->setText("");
    ui->tableView->setFocusPolicy(Qt::NoFocus);
    ui->label_total->setFocusPolicy(Qt::NoFocus);

    initLogo(ui->label_logo, ui->label_text);
    //
    this->tablemodel = tablemodel;
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableView->setShowGrid(false);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->verticalHeader()->setDefaultSectionSize(61);
    ui->tableView->setModel(tablemodel);
    ui->tableView->justShowColumn(0);
    //
    ui->label_ch_billno->setText(ch_billno);
    ui->label_ch_servant->setText(tablemodel->get_cey_u_table_value(ch_billno, "vch_waiter").toString());
    ui->label_ch_tableno->setText(ch_tableno);

    ui->label_num->setText(tablemodel->dish_num_str(tablemodel->getKeepdata().dish_num()));
    ui->label_total->setText(tablemodel->dish_total_str(tablemodel->getKeepdata().dish_total()));


    //
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->tableView->clearSelection();
    foreach(int row, rowListAsc) {
        ui->tableView->selectRow(row);
    }

    //frame
    lds_roundeddialog_rect_frame *frame = new lds_roundeddialog_rect_frame(this);
    frame->setGeometry(0, 0, ui->tableView->width(), ui->tableView->geometry().bottom());
    frame->bgColor = &bgColor;

    //widget_pay
    ui->frame_main->initGeometry(flags);
    ui->frame_main->initdataByM(tablemodel);
    //
    ui->tableView->updateTableHeightAll();

    //connec
    connect(ui->frame_main, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->frame_main, SIGNAL(rejected()),this,SLOT(reject()));

}

w_scr_dish_main_table_pay::~w_scr_dish_main_table_pay()
{
    delete ui;
}
