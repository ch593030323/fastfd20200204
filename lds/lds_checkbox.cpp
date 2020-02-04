#include "lds_checkbox.h"
#include <QApplication>

QString lds_CheckBox::png_h30_path_appcheckon()
{
    if(qApp->layoutDirection() == Qt::RightToLeft)
        return ":/image/apple_arab_check_on_h30.png";
    return ":/image/apple_check_on_h30.png";
}

QString lds_CheckBox::png_h30_path_appcheckoff()
{
    if(qApp->layoutDirection() == Qt::RightToLeft)
        return ":/image/apple_arab_check_off_h30.png";
    return ":/image/apple_check_off_h30.png";
}

QString lds_CheckBox::png_path_appcheckon()
{
    if(qApp->layoutDirection() == Qt::RightToLeft)
        return ":/image/apple_arab_check_on.png";
    return ":/image/apple_check_on.png";
}

QString lds_CheckBox::png_path_appcheckoff()
{
    if(qApp->layoutDirection() == Qt::RightToLeft)
        return ":/image/apple_arab_check_off.png";
    return ":/image/apple_check_off.png";
}

lds_CheckBox::lds_CheckBox(QWidget *parent) :
    lds_checkbox_wrap(parent)
{
    connect(this, SIGNAL(toggled(bool)),this,SLOT(toToggled(bool)));
    connect(this, SIGNAL(clicked(bool)),this,SLOT(toClicked(bool)));
}

void lds_CheckBox::setChecked1(QString YN)
{
    setChecked(YN == "Y");
}

QString lds_CheckBox::isChecked1()
{
    return isChecked()? "Y":"N";
}

void lds_CheckBox::setCheckYN(QString YN)
{
    setChecked(YN == "Y");
}

QString lds_CheckBox::isCheckYN()
{
    return isChecked()? "Y":"N";
}

void lds_CheckBox::setUnchecked(bool uncheck)
{
    setChecked(!uncheck);
}

void lds_CheckBox::toToggled(bool f)
{
    emit toggled(f?"Y":"N");
}

void lds_CheckBox::toClicked(bool f)
{
    emit clicked(f?"Y":"N");
}
