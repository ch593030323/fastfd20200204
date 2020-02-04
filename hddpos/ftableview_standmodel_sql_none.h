#ifndef FTABLEVIEW_STANDMODEL_SQL_NONE_H
#define FTABLEVIEW_STANDMODEL_SQL_NONE_H

#include  "ftableview_standmodel_virtual.h"
#include <QSet>
#include <QMap>
#include "lds_bool.h"


class ftableview_standmodel_sql_none : public ftableview_standmodel_virtual
{
    Q_OBJECT
public:

    class noneData{
    public:
        struct rowData{
            int role;
            QString field_name;
            QVariant value;
        };
        QList<rowData> content;
    public:
        noneData &append(int role, const QVariant &value);
        noneData &append(const QString &field_name, const QVariant &value);

        QVariant value(int role) const;
        QVariant value(const QString &field_name) const;
        QString keyvalue() const;
        QVariantList values() const;
        QMap<int, QVariant> map() const;
        void init(const QVariant &key_value);
        void print();

        //u1 是key
        noneData();
        noneData(const QVariant &u, const QVariant &u1);
        noneData(const QVariant &u, const QVariant &u1, const QVariant &u2) ;
        void setValue(int role, const QVariant &u);
        void setValue(const QString &field_name, const QVariant &u);
    };

    static QVariant list_data(const QList<noneData> &list, const QString &usr_1, int role);
    static void list_remove(QList<noneData> &list, const QString &usr_1);
    static int list_index(const QList<noneData> &list, const QString &usr_1);
    static QList<noneData> stringlist2datalist(const QStringList &list);
public:
    explicit ftableview_standmodel_sql_none();
    //!
    void setPrepareList(const QList<noneData> &list);
    void setPrepareList(const QList<noneData> &list, const QStringList &select_usr_1_list);
    void setModelList(const QList<noneData> &list);
    void select(const QString &usr_1);

    void set_list_data(const QVariant &u1, const QVariant &value, int role);

    const QList<noneData> & data_list();

    void initDelay(int mesc = 100);

    QVariantList get_value_unselect(int role);//u u1 u2 返回u2的拼接

    void enableEndAppendNew(bool enabled);

    QMap<int, QVariant>  getMapByKey(const QString &usr_1);

    static QVariant value(const QModelIndex &index, const QString &field_name);
    static QString keyvalue(const QModelIndex &index) ;
public slots:
    void toinit();

private:
    lds_bool_default_false b_endAppendNew;//用于最后新增按钮
    QList<noneData> _data_list;
    void generateNext();
    bool atEnd();
    //!~
public:
    virtual void togenerate(int index, int updownum, bool hasnext);
    virtual void togenerateNext();
    virtual void togeneratePre();
};

#endif // FTABLEVIEW_STANDMODEL_SQL_NONE_H
