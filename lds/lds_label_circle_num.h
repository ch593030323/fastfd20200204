#ifndef LDS_LABEL_CIRCLE_NUM_H
#define LDS_LABEL_CIRCLE_NUM_H

#include "label_click.h"
#include <QBasicTimer>

class lds_label_circle_num : public label_click
{
    Q_OBJECT
public:
    explicit lds_label_circle_num(QWidget *parent = 0);

    void setup(QWidget *buddy, const QSize& size, const QColor &color, int xoff, int yoff);

    void setColor(const QColor &color);
    void stop();
    void start();
protected:
    void timerEvent(QTimerEvent *event);
    void paintEvent(QPaintEvent *);
private:
    QBasicTimer timer;
    int step;
    QColor color;
};

#endif // LDS_LABEL_CIRCLE_NUM_H
