#ifndef W_M_MEMBER_POINT_H
#define W_M_MEMBER_POINT_H

#include <QDialog>
#include "uo_rw_member.h"
class Ui_w_m_member_point_recharge_Dialog;
class Ui_w_m_member_Dialog;
class lds_model_sqltablemodel;

class w_m_member_point : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_point(QWidget *parent = 0);

public slots:
    void torefresh();//树状图刷新，tableview刷新

private slots:
    void treeselect();//tableview刷新

    void toliping_detail();
    void toliping_info();
    void toadd_point();
    void tosub_point();
    void tochange_liping();
    void toexit();

public:
    Ui_w_m_member_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    QString tablemodel_arg2;
};

//会员积分详情
class Ui_w_bt_dish_unitset_Dialog;
class w_m_member_point_detail : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_point_detail(w_m_member_point *parent = 0);

private slots:
    void setPrintCount();
    void toexit();
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    w_m_member_point *_parent;
};

//礼品资料
class w_m_member_point_liping_info : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_point_liping_info(w_m_member_point *parent = 0);
private slots:
    void tonew();
    void todel();
    void torefresh();
    bool tosave();
    void toexit();
public:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
};

//礼品资料_充积分
class w_m_member_point_recharge : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_point_recharge(const QString &type, w_m_member_point *parent = 0);
    QString _ini_error;//初始化错误
    QString _type;//积分类型
    Ui_w_m_member_point_recharge_Dialog *ui;
protected:
protected slots:
    virtual void took();//将会ｎｅｗ的中took替换
    virtual void toexit();
    void toreadm();

    void toUpdateGiftNumPoint(const QString &num_point);
protected:
    w_m_member_point *_parent;
    QString ch_member_belong_state;
};
#endif // W_M_MEMBER_POINT_H
