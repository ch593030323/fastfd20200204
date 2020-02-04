#ifndef W_BT_DISH_H
#define W_BT_DISH_H

#include <QDialog>
#include <QStandardItemModel>
#include "lds.h"
#include "w_bt_dish_flag.h"

class QComboBox;
class w_bt_dish : public QDialog, public w_bt_dish_flag
{
    Q_OBJECT
public:
    explicit w_bt_dish(QWidget *parent = 0);
private:
    //基础资料
    static QStandardItemModel   MODEL_DISH_DEDUCTTYPE;//t提成方式
    //基础资料
    static QStandardItemModel   MODEL_DISH_SERVICE_MODE;//服务费模式
    static QStandardItemModel   MODEL_DISH_SERVICE_MODE_CUT;//服务费模式
    static QStandardItemModel   MODEL_DISH_SERVICE_MODE_CUT_HELP;//服务费模式
    //基础资料
    static QStandardItemModel   MODEL_DISH_LOW_MODE;//低消方式
    //基础资料
    static QStandardItemModel   MODEL_UNIT_NUM_TYPE;//数量类型
    //基础资料
    static QStandardItemModel   MODEL_UNIT;//单位
    //餐桌资料
    static QStandardItemModel   MODEL_TABLE_SERVICE_FLAG_MODE;//Y:折前服务率; N:折后服务率
    //餐桌人数说明
    static QStandardItemModel   MODEL_TABLE_INT_PERSON;

    static QList< QPair<QString, QString> > kvaddresslist;
    static QMap<QString, QString> kcmap;
    static QMap<QString, QString> kcmapdesc;
public:
    static void clearCache();
    //基础资料
    static QStandardItemModel&   get_MODEL_DISH_DEDUCTTYPE();//t提成方式
    //基础资料
    static QStandardItemModel&   get_MODEL_DISH_SERVICE_MODE();//服务费模式
    static QStandardItemModel&   get_MODEL_DISH_SERVICE_MODE_CUT();//服务费模式
    static QStandardItemModel&   get_MODEL_DISH_SERVICE_MODE_CUT_HELP();//服务费模式
    //基础资料
    static QStandardItemModel&   get_MODEL_DISH_LOW_MODE();//低消方式
    //基础资料
    static QStandardItemModel&   get_MODEL_UNIT_NUM_TYPE();//数量类型
    //基础资料
    static QStandardItemModel&   get_MODEL_UNIT();//单位
    //餐桌资料
    static QStandardItemModel&   get_MODEL_TABLE_SERVICE_FLAG_MODE();//Y:折前服务率; N:折后服务率
    //餐桌人数说明
    static QStandardItemModel&   get_MODEL_TABLE_INT_PERSON();

    static QMap<QString, QString>& get_kcmap();
    static QMap<QString, QString>& get_kcmapdesc();
    static QList< QPair<QString, QString> >& get_kvaddresslist();

    static void get_vch_plan_k3o3(QString &state, QString &id, const QString &ch_dishno, kitchenState s0 = KITCHEN_MIN_DEF);

    static char k2o3_state2hex_char(kitchenState state);
    static QString k2o3_stae2hex_str(kitchenState state);
    static QString k2o3_name(kitchenState state);

    static QString model_data(QAbstractItemModel *m, int row, int column);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(w_bt_dish::kitchenStates)
#endif // W_BT_DISH_H
