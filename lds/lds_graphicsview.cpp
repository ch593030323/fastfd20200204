#include "lds_graphicsview.h"
#include <QtDebug>

lds_graphicsview::lds_graphicsview(QWidget *parent):
    QGraphicsView(parent)
{
}

lds_graphicsview::~lds_graphicsview()
{
    
}

void lds_graphicsview::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if(this->scene()) {
        this->fitInView(this->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
    //去掉QGraphicsView的边框和内间距
    //background:transparent;
    //padding: -2px;
}

QSize lds_graphicsview::sizeHint() const
{
    return QGraphicsView::sizeHint();
}

int lds_graphicsview::idealh()
{
    float r = scene()->sceneRect().width()*1.0/scene()->sceneRect().height();
    int idealwidth = this->height()*r;
    QSize size;
    if(idealwidth < this->width()) {
        size = QSize(idealwidth, idealwidth/r);
    } else {
        size = QSize(this->width(), this->width()/r);
    }
    return size.height();
}

