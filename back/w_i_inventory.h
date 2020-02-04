#ifndef W_I_INVENTORY_H
#define W_I_INVENTORY_H

#include <QObject>
#include "lds_query_hddpos.h"

#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QtDebug>
#include "backheader.h"

//w_i_inventory::MODEL_INVENTORY_INIT();

class w_i_inventory : public QObject
{
    Q_OBJECT
private:
    explicit w_i_inventory(QObject *parent = 0);
    //会员状态对照表
    static QMap<QString, QVariant> MODEL_INVENTORY_TYPE;
    //会员状态对照表
    static QMap<QString, QVariant> MODEL_INVENTORY_SHEET_TYPE;
    //会员状态对照表
    static QMap<QString, QVariant> MODEL_INVENTORY_SHEET_NAME;
    //报表状态，审核，作废之类
    static QMap<QString, QVariant> MODEL_INVENTORY_STATE_DO;
    //报表状态，审核，作废之类
    static QMap<QString, QVariant> MODEL_INVENTORY_STATE_TYPE;
public:

    static void clearCache();
    //会员状态对照表
    static QMap<QString, QVariant>& get_MODEL_INVENTORY_TYPE();
    //会员状态对照表
    static QMap<QString, QVariant>& get_MODEL_INVENTORY_SHEET_TYPE();
    //报表状态，审核，作废之类
    static QMap<QString, QVariant>& get_MODEL_INVENTORY_STATE_DO();
    //报表状态，审核，作废之类
    static QMap<QString, QVariant>& get_MODEL_INVENTORY_STATE_TYPE();
    //报表状态，审核，作废之类
    static QMap<QString, QVariant>& get_MODEL_INVENTORY_SHEET_NAME();

    static QStringList inventory_queryfilter(const QString &sql);

    //获取操作员的 name 更具id
    static QString inventory_getopername(const QString &operid);

    static QString SHEET_NAME(const QString &ch_sheetType);
    static int provide_PAY_FLAG(const QString &ch_sheetType);
};


#endif // W_I_INVENTORY_H
