#ifndef FTABLEVIEW_STANDMODEL_SQL_H
#define FTABLEVIEW_STANDMODEL_SQL_H

#include  "ftableview_standmodel_virtual.h"
#include <QElapsedTimer>
#include "lds_bool.h"

class QItemSelectionModel;
class QTableView;
class ftableview_standmodel_sql : public ftableview_standmodel_virtual{
    Q_OBJECT
public:
    explicit ftableview_standmodel_sql(TYPE t = TYPE_list);

    void enableEndAppendNew(bool enabled);
    void initDelay(int mesc = 10);
public slots:
    void toinit();
    void toinitBySqlother(const QString &no);
    void toinitBySqlother2(const QString &no);
    void toinitBySqlother3(const QString &no);
    void toinitBySqlother4();

    void clearCache();
public:
    virtual void togenerate(int index, int updownum, bool hasnext);
    virtual void togenerateNext();
    virtual void togeneratePre();
private:
    QString sqlargarg;// limit %1,%2
    lds_bool_default_false b_endAppendNew;//用于最后新增按钮
    void toinit0();
public:
    QString sql_cur;// select * from tablename

    QStringList sql_field_list;
    QString sqlother;//   select * from tablename where x = '%1'
    QString sqlother2;//  select * from tablename where x = '%1'
    QString sqlother3;//  select * from tablename where x = '%1'
    QString sqlother4;//  select * from tablename where x = '%1'

private:
    QString curgoing;
    int generateCount;
};

#endif // FTABLEVIEW_STANDMODEL_SQL_H
