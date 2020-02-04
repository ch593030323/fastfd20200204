#include "w_scr_dish_pay_dialog.h"
#include "ui_w_scr_dish_pay_dialog.h"

w_scr_dish_pay_dialog::w_scr_dish_pay_dialog(w_scr_dish_pay_widget::operatModes m, QWidget *parent, const fexpandmain_model_sqltablemodel_data *tablemodel) :
    lds_roundeddialog_rect(parent),
    ui(new Ui::w_scr_dish_pay_dialog)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->frame_main->setGeometry(lds::PAY_LEFT_SPACE, 0, lds::MAIN_WINDOW_SIZE.width() - lds::PAY_LEFT_SPACE, lds::MAIN_WINDOW_SIZE.height());
    ui->frame_main->initGeometry(m);
    ui->frame_main->initdataByM(tablemodel);

    connect(ui->frame_main, SIGNAL(accepted()), this,SLOT(accept()));
    connect(ui->frame_main, SIGNAL(rejected()), this,SLOT(reject()));
}

w_scr_dish_pay_dialog::~w_scr_dish_pay_dialog()
{
    delete ui;
}

w_scr_dish_pay_widget *w_scr_dish_pay_dialog::pay_widget()
{
    return ui->frame_main;
}
