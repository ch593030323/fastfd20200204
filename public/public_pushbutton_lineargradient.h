#ifndef PUBLIC_PUSHBUTTON_LINEARGRADIENT_H
#define PUBLIC_PUSHBUTTON_LINEARGRADIENT_H

#include <QPushButton>
#include "lds_record_do_longpress.h"

class public_pushbutton_LinearGradient : public QPushButton
{
    Q_OBJECT
public:
    explicit public_pushbutton_LinearGradient(QWidget *parent = 0);
    void setBgColor(const QColor &obsidian_color, const QColor &skyblue_color);
protected:
    void paintEvent(QPaintEvent *e);

    /////////////////////////////////////
public:
    void enableLongPress(bool flag);
signals:
    void timeout();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
protected:
    lds_record_do_longpress *do_longpress;
};

#endif // PUBLIC_PUSHBUTTON_LINEARGRADIENT_H
