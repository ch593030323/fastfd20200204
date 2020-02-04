#ifndef ERWEIMA_H
#define ERWEIMA_H

#include <QWidget>
#include "qrencode.h"
#include<QPaintEvent>
#include<qpainter.h>
#include<qdebug.h>
class erweima : public QWidget
{
    Q_OBJECT
public:
    explicit erweima(QWidget *parent = 0);
    void setstr(const char *pcStr);
signals:

public slots:
private:
    void paintEvent(QPaintEvent *event);
    //QRcode *CreatQr(const char *pcStr);

    const char *str1 ;
};

#endif // ERWEIMA_H
