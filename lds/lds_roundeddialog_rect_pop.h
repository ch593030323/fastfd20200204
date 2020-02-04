#ifndef LDS_ROUNDEDDIALOG_RECT_POP_H
#define LDS_ROUNDEDDIALOG_RECT_POP_H


#include <QDialog>
#include "lds_bool.h"
class lds_roundeddialog_rect_pop : public QDialog
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
    explicit lds_roundeddialog_rect_pop(QDialog *buddy, QWidget *parent, Qt::WindowFlags flags = Qt::Popup | Qt::FramelessWindowHint);
    bool posIsValid();
    void posDisable();
    void updatePos(QWidget *basewidget);

    QDialog *get_buddy();
public slots:
    void toUpdatePos();
    void updatePos(const QPoint &pos);

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void todone(int result);
private:
    QDialog *buddy;
    lds_bool_default_false buddy_pos_is_valid;
    QWidget *basewidget;
};

#endif // LDS_ROUNDEDDIALOG_RECT_POP_H
