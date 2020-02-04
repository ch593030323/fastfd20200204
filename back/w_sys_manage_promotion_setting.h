#ifndef W_SYS_MANAGE_PROMOTION_SETTING_H
#define W_SYS_MANAGE_PROMOTION_SETTING_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include <QDataWidgetMapper>
#include "lds_model_mapwidget_vertical.h"

class Ui_w_sys_managepromotion_setting;

//namespace Ui {
//class w_sys_manage_promotion_setting;
//}

class w_sys_manage_promotion_setting : public lds_model_mapwidget_vertical
{
    Q_OBJECT

public:
    explicit w_sys_manage_promotion_setting(QWidget *parent = 0);
    ~w_sys_manage_promotion_setting();
protected:
    //不存在则插入空值
    virtual bool insert_sql_objectname(const QString &objectname);
private slots:
    void to_ok();

    void torefresh();
    void toexit();
    void treeselect();
    void tobath();
private:
    bool saveData();
    bool dataDirty();
    int checkDirty();
private:
    Ui_w_sys_managepromotion_setting *ui;
    lds_model_sqltablemodel *tablemodel_dish;
};

class Ui_w_sys_manage_promotion_setting_bath;
class w_sys_manage_promotion_setting_bath : public QDialog{
    Q_OBJECT

public:
    explicit w_sys_manage_promotion_setting_bath(QWidget *parent = 0);
    ~w_sys_manage_promotion_setting_bath();
    Ui_w_sys_manage_promotion_setting_bath *ui;

private slots:
    void took();
    void tocancel();
};

#endif // W_SYS_MANAGE_PROMOTION_SETTING_H
