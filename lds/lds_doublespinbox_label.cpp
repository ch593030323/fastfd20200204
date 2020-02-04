#include "lds_doublespinbox_label.h"
#include "lds.h"

lds_doublespinbox_label::lds_doublespinbox_label(QWidget *parent) :
    QLabel(parent)
{
}

double lds_doublespinbox_label::value()
{
    return lds::groupSeparatorStr2v(this->text());
}

void lds_doublespinbox_label::setValue(double value)
{
    setText(lds::septNumber(value, 2));
}

void lds_doublespinbox_label::setText(const QString &text)
{
    QLabel::setText(text);
}

QString lds_doublespinbox_label::text() const
{
    return QLabel::text();
}
