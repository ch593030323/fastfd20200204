#include "w_sys_manage_outer_settings_img_show.h"
#include "ui_w_sys_manage_outer_settings_img_show.h"
#include <QtDebug>
w_sys_manage_outer_settings_img_show::w_sys_manage_outer_settings_img_show(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_outer_settings_img_show)
{
    ui->setupUi(this);
    this->setWindowTitle("Image");
    ui->scrollArea->setWidget(label = new QLabel(this));
    ui->scrollArea->setStyleSheet("QScrollArea {background-color:transparent;}");
    ui->scrollArea->viewport()->setStyleSheet("background-color:skyblue;");

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
}

w_sys_manage_outer_settings_img_show::~w_sys_manage_outer_settings_img_show()
{
    delete ui;
}

void w_sys_manage_outer_settings_img_show::showPixmap(const QPixmap &pixmap)
{
    label->setPixmap(pixmap);
}

void w_sys_manage_outer_settings_img_show::took()
{
    this->accept();
}
