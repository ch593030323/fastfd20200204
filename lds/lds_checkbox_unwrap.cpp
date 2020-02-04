#include "lds_checkbox_unwrap.h"
#include<QPainterPath>

lds_checkbox_unwrap::lds_checkbox_unwrap(QWidget *parent)
    : QCheckBox(parent)
{
    connect(this, SIGNAL(toggled(bool)),this,SLOT(toToggled(bool)));
    connect(this, SIGNAL(clicked(bool)),this,SLOT(toClicked(bool)));
}

lds_checkbox_unwrap::~lds_checkbox_unwrap()
{
}

void lds_checkbox_unwrap::setChecked1(QString c)
{
    setChecked(c == "Y");
}

QString lds_checkbox_unwrap::isChecked1()
{
    return isChecked()? "Y":"N";
}

void lds_checkbox_unwrap::toToggled(bool f)
{
    emit toggled(f?"Y":"N");
}

void lds_checkbox_unwrap::toClicked(bool f)
{
    emit clicked(f?"Y":"N");
}
