#include "lds_roundeddialog_title.h"
#include <QPainter>
#include "lds.h"
#include <QtDebug>


lds_roundeddialog_title::lds_roundeddialog_title(QWidget *parent)
    : QWidget(parent) {
    titleColor = 0;
    this->paintLeftTop = NULL;
}

lds_roundeddialog_title::lds_roundeddialog_title(QWidget *parent, void (*paintLeftTop)(const QRect &, QPainter *), const QString &title, QColor *titleColor)
    : QWidget(parent) {
    setTitle(title);
    this->titleColor = titleColor;
    this->paintLeftTop = paintLeftTop;
}
void lds_roundeddialog_title::setTitle(const QString &title)
{
    this->title = title;
    update();
}

void lds_roundeddialog_title::setTitleColor(QColor *titleColor)
{
    this->titleColor = titleColor;
    update();
}

void lds_roundeddialog_title::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRect rect0 = this->rect();
    //bg RIGHT
    if(titleColor) {
        lds::paintPolygon(&p, rect0, *titleColor);
    }
    //top left
    if(paintLeftTop != NULL) {
        paintLeftTop(rect0, &p);
    }
    //text
    if(title.count() > 0) {
        QFont f = p.font();
        QFontMetrics fm(f);
        qreal fmrate =  fm.height() *1.0 / fm.width(title);
        f.setPixelSize(qMin(rect0.height(), int(fmrate * rect0.width()))  - 6 );
        p.setFont(f);
    }
    p.setPen(QColor("white"));
    p.drawText(rect0, Qt::AlignCenter, title);
}
