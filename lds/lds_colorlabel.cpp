#include "lds_colorlabel.h"

lds_colorlabel::lds_colorlabel(QWidget *parent) :
    QLabel(parent)
{
}

QString lds_colorlabel::color()
{
    return _c;
}

void lds_colorlabel::setcolor(const QString &c)
{
    this->setStyleSheet("background:"+kcmap.value(c));
    _c = c;
}
