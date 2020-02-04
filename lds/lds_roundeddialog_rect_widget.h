#ifndef LDS_ROUNDEDDIALOG_RECT_WIDGET_H
#define LDS_ROUNDEDDIALOG_RECT_WIDGET_H

#include <QWidget>
#include "lds_bool.h"

class lds_roundeddialog_rect_widget : public QWidget
{
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
    explicit lds_roundeddialog_rect_widget(QWidget *parent = 0);

    void reject();
    void accept();
    int exec();

    void enable_click_empty_reject();
protected:
    void paintEvent(QPaintEvent *e);
    void  mouseReleaseEvent(QMouseEvent *e);
private:
    lds_bool_default_false click_empty_reject;

};

#endif // LDS_ROUNDEDDIALOG_RECT_WIDGET_H
