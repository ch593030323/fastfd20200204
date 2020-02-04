#ifndef KDS_STANDMODEL_SQL_H
#define KDS_STANDMODEL_SQL_H

#include  "ftableview_standmodel_virtual.h"

class kds_standmodel_sql : public ftableview_standmodel_virtual
{
    Q_OBJECT
public:
    explicit kds_standmodel_sql(TYPE t = TYPE_list);

public slots:
    void toinit();

public:
    virtual void togenerate(int index, int updownum, bool hasnext);
    virtual void togenerateNext();
    virtual void togeneratePre();
public:
    QString sql_cur;// select * from tablename

private:
    QString sqlargarg;// limit %1,%2
    QString curgoing;
    int generateCount;
};

#endif // KDS_STANDMODEL_SQL_H
