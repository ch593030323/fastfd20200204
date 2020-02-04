#include "lds_pushbutton_oblique.h"
#include <QPainter>
#include <QtDebug>
#include <QStylePainter>
#include <QStyleOptionButton>

lds_pushbutton_oblique::lds_pushbutton_oblique(QWidget *parent) :
    lds_pushbutton(parent)
{
    enablePaintMenuPopup(true);
}
