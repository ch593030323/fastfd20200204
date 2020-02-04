#ifndef LDS_PUSHBUTTON_H
#define LDS_PUSHBUTTON_H

#include <QPushButton>
#include <QStylePainter>
#include <QStyleOptionButton>
#include "lds_bool.h"
#include "lds_record_do_longpress.h"

class lds_pushbutton : public QPushButton
{
    Q_OBJECT
public:
    static lds_pushbutton *createButton(const QIcon &icon, const QSize &icon_size);
public:
    explicit lds_pushbutton(QWidget *parent = 0);

    void enablePaintMenuPopup(bool enable);

    void setData(const QString & role, const QVariant &value);
    QVariant getData(const QString & role);

    void setData(int role, const QVariant &value);
    QVariant getData(int role);
protected:
    void paintEvent(QPaintEvent *e);

    void paintBackground(QStylePainter *painter, const QStyleOptionButton &option);
private:
    lds_bool_default_false p_menu_popup;

    QMap<QString, QVariant> str_map;
    QMap<int, QVariant> int_map;

    /////////////////////////////////////
public slots:
    void toUpdateText(const QString &text);
public:
    void enableLongPress(bool flag, QWidget *parent = 0);
signals:
    void timeout();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
protected:
    lds_record_do_longpress *do_longpress;
};

#endif // LDS_PUSHBUTTON_H
