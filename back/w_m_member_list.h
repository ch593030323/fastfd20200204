#ifndef W_M_MEMBER_LIST_H
#define W_M_MEMBER_LIST_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include "lds_model_sqltablemodel.h"
#include <QStandardItemModel>
#include "uo_rw_member.h"
#include "uo_rw_member.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include <QSqlRelationalDelegate>
#include "uo_rw_member.h"
#include "w_sys_manage_cloudsync.h"

//主界面
class Ui_w_m_member_Dialog;
class QTableView;
class lds_tableView;
class lds_treeWidget;
class QDataWidgetMapper;
class Ui_w_m_member_sendcard_Dialog;
class Ui_w_bt_dish_unitset_Dialog;
class lds_model_sqltablemodel;
class lds_model_sqltablemodel;
class Ui_w_m_member_list_type_new_Dialog;
class Ui_w_m_member_list_recharge_Dialog;
class Ui_w_m_member_pwdchange_Dialog;
class Ui_w_m_member_news_Dialog;
class lds_messagebox_loading_show;

class w_m_member_list : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_list(QWidget *parent = 0);
    lds_tableView *getTableview();
    lds_treeWidget *getTreew();//获取tree

    lds_model_sqltablemodel *getTablemodel();


public:
    static lds_const_bool is_yun_member;//对这儿的会员是否是云端，这里需要开启和关闭功能
//    static bool is_yun_member();

public slots:
    void torefresh();
    void treeselect();
    void setToolbarShown(bool f);

    //外部调用例子    w_m_member_list::statictosendcard(this, tablemodel_2, ui->tableView, false);
    static void statictosendcard(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, bool isenabled=true, const QString &title="");
    //外部调用例子    w_m_member_list::statictorecharge(this, 0, 0,"");
    static void statictorecharge(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, const QString &parm);
    //外部调用例子  statictomembertype(this);
    static void statictomembertype(w_m_member_list *parent);
    //外部调用读卡例子
    static bool static_to_memeber_readcard(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool isSearchMember = false, bool isManualYun = false);
    static bool static_to_memeber_readcard_d(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool isSearchMember, vch_membernoData::chooseFlag choose/*0选择、1本地会员、2云会员*/);
    static bool static_to_memeber_readcard_pay(QWidget *parent, QString &cardid, uo_rw_member &uo_member, vch_membernoData::chooseFlag choose/*0选择、1本地会员、2云会员*/);
    static void yun_member_open();
    static void yun_member_close();
private:
    static bool static_to_memeber_readcard_private(QWidget *parent, QString &cardid, uo_rw_member &uo_member, bool isSearchMember);

private slots:
    void tonew();
    void todel();
    void tonews();
    void tomembertype();
    void tosendcard();
    void torecharge();
    void tosetpwd();
    void toexport();
    void toimport();
    void toexit();
public:
    Ui_w_m_member_Dialog *ui;
private:
    lds_model_sqltablemodel *tablemodel;
    QString tablemodel_arg3;
    /////////
protected:
    bool eventFilter(QObject *o, QEvent *e);
};

class w_m_member_list_yun_auto_close{
public:
    w_m_member_list_yun_auto_close();
    void yunOpen();
    ~w_m_member_list_yun_auto_close();
};

//发卡
class w_m_member_list_sendcard : public QDialog{
    Q_OBJECT
public:
    w_m_member_list_sendcard(QWidget *parent ,
                                      lds_model_sqltablemodel *m, lds_tableView *tableview, bool isenable=true);


protected slots:
    void tonew();
    void todel();
    void tosendcard();
    void toup();
    void todown();
    virtual bool tosave();
    void toexit();
    void toChangeType();

    virtual void torefresh();
protected:
    bool sendCard();
    bool createMember();
protected:
    Ui_w_m_member_sendcard_Dialog *ui;
    lds_model_sqltablemodel *_tablemodel;
    lds_tableView *_tableview;
    QDataWidgetMapper *mapper;
    lds_model_sqltablemodel *tablemodel;
    lds_model_sqltablemodel *tablemodel_3;
    QString is_cardtype;
    uo_rw_member *cb5;//pb其实时发卡按钮
    QWidget *_parent;
};

//新增
class w_m_member_list_new : public w_m_member_list_sendcard{
    Q_OBJECT

public:
    explicit w_m_member_list_new(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, bool isnew=true);
    void updateVch_tel_handtel(const QString &vch_tel);
protected slots:
    virtual bool tosave();
    virtual void torefresh();
    void tosavesend();
private:
    void clearData();
};
//会员类型
class w_m_member_list_type : public QDialog{
    Q_OBJECT
public:
    explicit w_m_member_list_type(QWidget *parent = 0);
    lds_model_sqltablemodel *getTablemodel();
    lds_tableView *getTableView();
public slots:
    void torefresh();
private slots:
    void tonew();
    void todel();
    bool tosave();
    void toexit();
    void tochange();


public:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;

};

//会员类型x修改
class w_m_member_list_type_change: public QDialog{
    Q_OBJECT
public:
    explicit w_m_member_list_type_change(w_m_member_list_type *parent);
protected slots:
    virtual bool tosave();
    virtual void toexit();
    virtual void mapperupdate();
    void toup();
    void todown();

    void is_recharge(int index);
    void is_point(int index);
    void is_fav(int index);

    void toSaaSCheck(int index);

protected:
    Ui_w_m_member_list_type_new_Dialog *ui;
    w_m_member_list_type *_parent;
    QDataWidgetMapper *mapper;

};

//会员类型新增
class w_m_member_list_type_new : public w_m_member_list_type_change{
    Q_OBJECT
public:
    explicit w_m_member_list_type_new(w_m_member_list_type *parent);
private slots:
    virtual bool tosave();
    virtual void toexit();
    virtual void mapperupdate();
};
//会员卡充值
class w_m_member_list_recharge : public QDialog{
    Q_OBJECT
public:
    explicit w_m_member_list_recharge(QWidget *parent, lds_model_sqltablemodel *tablemodel, lds_tableView *tableview, const QString &parm /*""充值, "C"取款, "D"退卡*/, bool filter_by_handtel = false);
    ~w_m_member_list_recharge();
    //取款C
    void ue_fetch();
    //退卡D
    void ue_blankcard();
    void card_info_clear();

private slots:
    void rechargeprint();
    void toremainprint();
    void toreadcard();
    void torecharge();
    void torechargeOrRead();
    void toexit();

    void TtolineEdit_amount();

    void tochecksave();
    void tochecksave_2();

    void tocheckpaymode(int index);
    void toreplacenum();

    void toUpdateRechargeScheme();
protected:
    Ui_w_m_member_list_recharge_Dialog *ui;
    QDataWidgetMapper *mapper;
    bool _isready;
    lds_model_sqltablemodel *_tablemodel;
    lds_tableView *_tableview;
    QString _parm;
    uo_rw_member::read_card_type lst_card;
    bool _filter_by_handtel;
private:
    void btnForRechargeScheme();
};
//密码修改
class w_m_member_pwdchange  : public QDialog{
    Q_OBJECT
public:
    explicit w_m_member_pwdchange(const QString &vch_memberno, QWidget *parent);
    void enableForget();
private slots:
    void took();
    void tocancel();
private:
    Ui_w_m_member_pwdchange_Dialog *ui;
    QString is_password;
    QString is_memberno;

};
//批量新增
class w_m_member_news  : public QDialog{
    Q_OBJECT
public:
    explicit w_m_member_news(w_m_member_list *parent);
private slots:
    void took();
    void tocancel();
    void tonewtype();
private:
    Ui_w_m_member_news_Dialog *ui;
    w_m_member_list *_parent;

};
#endif // W_M_MEMBER_LIST_H
