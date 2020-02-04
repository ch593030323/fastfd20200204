#include "lds_field_name_label.h"
#include "lds.h"

lds_field_name_label::lds_field_name_label(QWidget *parent) :
    QLabel(parent)
{
}

void lds_field_name_label::setText(const QString &text)
{
    QLabel::setText(lds::reportModelMapValue(text));
}
