#ifndef LDS_ROUNDEDDIALOG_RECT_ALIGN_H
#define LDS_ROUNDEDDIALOG_RECT_ALIGN_H

#include <QDialog>
#include "lds_dialog_drog.h"

class QVBoxLayout;
class QLineEdit;
class lds_roundeddialog_title;

#define paintLeftTopNull NULL

class lds_roundeddialog_rect_align : public lds_dialog_drog
{
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

    void setbgPixmap(const QPixmap &pixmap);
    QPixmap getbgPixmap();
    QPixmap bgPixmap;

    static void setSizeNotResize(QDialog *buddy);

public:
    ~lds_roundeddialog_rect_align();

    void setTitle(const QString &title);
    void disableAutoDestory();
    void enable_click_empty_reject();
    void enable_click_dialog_accept();

    static int titleHeight;
    QDialog *get_buddy();
    void buddyResize();
public slots:
    void toUpdatePos();
public Q_SLOTS:
    int exec();
protected:
    void paintEvent(QPaintEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *e);

private slots:
    void todone(int result);
    void toBuddyResize();
private:
    void printerPath(QPainter *painter, const QList<QPoint> &pointlist, const QColor &color);

    QDialog *buddy;
    bool _show_title;

    QWidget *old_parent;
    Qt::WindowFlags old_w_flags;

    lds_roundeddialog_title *label_title;

    bool isdestroy;
    bool click_empty_reject;
    bool click_dialog_accept;

    QSize buddy_min_size;
protected:
    void keyPressEvent(QKeyEvent *e);
    void  mousePressEvent(QMouseEvent *e);

    lds_roundeddialog_rect_align(QDialog *buddy,
                                 void (*paintLeftTop)(const QRect &, QPainter *),
                                 bool show_title,
                                 bool click_empty_reject,
                                 const QSize &buddy_min_size);
};

class lds_roundeddialog_rect_align_200x100 : public lds_roundeddialog_rect_align{

    Q_OBJECT
public:
    lds_roundeddialog_rect_align_200x100(QDialog *buddy);
};

class lds_roundeddialog_rect_align_600x400 : public lds_roundeddialog_rect_align{

    Q_OBJECT
public:
    lds_roundeddialog_rect_align_600x400(QDialog *buddy);
};
#endif // LDS_ROUNDEDDIALOG_RECT_ALIGN_H
