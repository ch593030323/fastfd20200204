#include "label_click.h"
#include <QMouseEvent>

label_click::label_click(QWidget *parent)
    : QLabel(parent)
{

}

void label_click::mousePressEvent(QMouseEvent *e)
{
    QLabel::mousePressEvent(e);
}

void label_click::mouseReleaseEvent(QMouseEvent *e)
{
    if(rect().contains(e->pos())) {
        emit clicked();
    }
    QLabel::mouseReleaseEvent(e);
}

