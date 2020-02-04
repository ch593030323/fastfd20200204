#ifndef PUBLIC_PUSHBUTON_LIKERB_PUR_H
#define PUBLIC_PUSHBUTON_LIKERB_PUR_H

#include <QPushButton>
#include "lds_record_do_longpress.h"

class public_pushbuton_likerb_pur : public QPushButton
{
    Q_OBJECT
public:
    explicit public_pushbuton_likerb_pur(QWidget *parent = 0);

    void setChecked1(bool f);
    void setExclusive(bool isexclusive);//set false: if state is down then release after click, default false
    void enableLongPress(bool flag);
    void setNormalBtn(bool flag);
    public_pushbuton_likerb_pur *curentCheckBtn();
    void setData(int role, const QVariant &value);
    QVariant getData(int role);

    QSize sizeHint() const;

signals:
    void signalpressed(bool execPress);
    void timeout();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *e);
private:
    void scan_all_except_self();
    bool isexclusive;
    bool isnormalbtn;
    lds_record_do_longpress *do_longpress;
    QMap<int, QVariant> data_map;
};

//一个容器
class public_pushbuton_likerb_pur_auto_release {
public:
    public_pushbuton_likerb_pur_auto_release(public_pushbuton_likerb_pur *b);
    ~public_pushbuton_likerb_pur_auto_release();

    public_pushbuton_likerb_pur *rb() {return b;}
    static public_pushbuton_likerb_pur *current_checked_rb(QWidget *parent);
    public_pushbuton_likerb_pur *b;
};

//点击后
class public_pushbuton_likerb_pur_unexclusive : public public_pushbuton_likerb_pur {

    Q_OBJECT
public:
    explicit public_pushbuton_likerb_pur_unexclusive(QWidget *parent = 0);
};

//点击后
class public_pushbuton_likerb_pur_false : public public_pushbuton_likerb_pur {

    Q_OBJECT
public:
    explicit public_pushbuton_likerb_pur_false(QWidget *parent = 0);
};

#endif // PUBLIC_PUSHBUTON_LIKERB_PUR_H
