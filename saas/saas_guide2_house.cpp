#include "saas_guide2_house.h"
#include <QtDebug>
#include <QPainter>
#include "lds.h"

saas_guide2_house::saas_guide2_house(QWidget *parent) :
    QFrame(parent)
{
}
void saas_guide2_house::addWidget(QWidget *w)
{
    if(w == 0) return;
    if(w_list.count() > 0) {
        delete w_list.takeFirst();
    }

    w_list.append(w);
    w->setParent(this);
    w->setGeometry(QRect(0, 0, width(), height()));
    w->show();
}

QWidget *saas_guide2_house::tabkeWidget()
{
    QWidget *w = 0;
    if(w_list.count() > 0)  w = w_list.takeLast();
    if(w_list.count() > 0) w_list.last()->show();

    return w;
}

QWidget *saas_guide2_house::curWidget()
{
    if(w_list.count() > 0) return w_list.last();
    return 0;
}

int saas_guide2_house::countWidget()
{
    return w_list.count();
}

void saas_guide2_house::printWidgets()
{
    for(int k = 0; k < w_list.count(); k++) {
        qDebug() << this << w_list[k];
    }
}

void saas_guide2_house::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    QPainter p(this);
    lds::paintDense7Pattern(&p);
}

void saas_guide2_house::hideAllWidgets()
{
    for(int k = 0; k < w_list.count(); k++) {
        w_list[k]->hide();
    }
}
