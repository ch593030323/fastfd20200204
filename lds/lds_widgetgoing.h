#ifndef LDS_WIDGETGOING_H
#define LDS_WIDGETGOING_H

#include <QWidget>

class lds_widgetgoing : public QWidget
{
    Q_OBJECT
public:
    explicit lds_widgetgoing(QWidget *parent = 0);
public slots:
    void setstep(int cur, int total);
private:
    virtual void paintEvent(QPaintEvent *);

    int stepcur;
    int steptotal;
};

#endif // LDS_WIDGETGOING_H
