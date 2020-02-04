#ifndef LDS_GRAPHICSVIEW_H
#define LDS_GRAPHICSVIEW_H

#include <QGraphicsView>
class lds_graphicsview : public QGraphicsView
{
public:
    lds_graphicsview(QWidget *parent = 0);
    ~lds_graphicsview();
    void resizeEvent(QResizeEvent *event);
    QSize sizeHint() const;
    int idealh();
};

#endif // LDS_GRAPHICSVIEW_H
