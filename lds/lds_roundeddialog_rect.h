#ifndef LDS_ROUNDEDDIALOG_RECT_H
#define LDS_ROUNDEDDIALOG_RECT_H

#include <QDialog>
#include "lds_bool.h"

class lds_roundeddialog_rect0 : public QDialog{
public:

    Q_OBJECT
    Q_PROPERTY(QColor bgColor READ getbgColor WRITE setbgColor DESIGNABLE true)
    Q_PROPERTY(QColor titleColor READ gettitleColor WRITE settitleColor DESIGNABLE true)
public:
    void setbgColor(const QColor &color);
    QColor getbgColor() const;
    QColor bgColor;

    void settitleColor(const QColor &color);
    QColor gettitleColor() const;
    QColor titleColor;
public:
    explicit lds_roundeddialog_rect0(QWidget *parent = 0);

    void enable_click_empty_reject();
    void enable_transparent_pay();
protected:
    void paintEvent(QPaintEvent *e);
    void  mouseReleaseEvent(QMouseEvent *e);
protected:
    lds_bool_default_false click_empty_reject;
    lds_bool_default_false transparent_paint;
};

class lds_roundeddialog_rect : public lds_roundeddialog_rect0
{
public:
    explicit lds_roundeddialog_rect(QWidget *parent = 0);
};


#endif // LDS_ROUNDEDDIALOG_RECT_H
