#ifndef LABEL_CLICK_H
#define LABEL_CLICK_H

#include <QLabel>

class label_click : public QLabel
{
    Q_OBJECT
public:
    label_click(QWidget *parent);

signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // LABEL_CLICK_H
