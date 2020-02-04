#include "lds_doublespinbox_lineedit.h"
#include "lds.h"

lds_doublespinbox_lineedit::lds_doublespinbox_lineedit(QWidget *parent) :
    QLineEdit(parent)
{
    this->setEnabled(false);
}

double lds_doublespinbox_lineedit::value()
{
    return _value * 100 / 100.0;
}

void lds_doublespinbox_lineedit::setValue(double value)
{
    this->_value = value;

    int f  = 1;
    if(bShowNegativeValue) f = -1;
    setText(lds::septNumber(f * value, 2));
}

void lds_doublespinbox_lineedit::enableShowNegativeValue()
{
    bShowNegativeValue = true;
    setValue(_value);
}

void lds_doublespinbox_lineedit::setText(const QString &text)
{
    QLineEdit::setText(text);
}

QString lds_doublespinbox_lineedit::text() const
{
    return QLineEdit::text();
}
