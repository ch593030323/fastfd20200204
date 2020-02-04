#ifndef PUBLIC_PUSHBUTON_RT_CHECK_H
#define PUBLIC_PUSHBUTON_RT_CHECK_H

#include "lds_toolbar_button.h"
#include "fexpandmain_model_sqltablemodel_data.h"
class public_pushbuton_rt_check : public lds_toolbar_button
{
    Q_OBJECT
public:
    enum checkState{
        none = 0,
        info,
        yunm,
        warn,
    };

public:
    explicit public_pushbuton_rt_check(QWidget *parent = 0);
    void set_rt_checked(const vch_membernoData &m);
    void set_rt_checked(bool f);
    void set_rt_checked_warn(bool f);


    static checkState rt_checked(const vch_membernoData &m);
    static checkState rt_checked(bool f);
    static checkState rt_checked_warn(bool f);

    static QPixmap pixmap_checked(checkState state);
    static QPixmap pixmap_checked(const vch_membernoData &m);
protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *event);
private:
    QString get_m_belong_state();
    QString m_belong_state;
    checkState  rt_state;
};

#endif // PUBLIC_PUSHBUTON_RT_CHECK_H
