#include "saas_guide_desc.h"
#include "ui_saas_guide_desc.h"

saas_guide_desc::saas_guide_desc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saas_guide_desc)
{
    ui->setupUi(this);
}

saas_guide_desc::~saas_guide_desc()
{
    delete ui;
}
