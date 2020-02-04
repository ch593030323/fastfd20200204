#ifndef FTABLEVIEW_STANDMODEL_SQL_TABLE_H
#define FTABLEVIEW_STANDMODEL_SQL_TABLE_H

#include  "ftableview_standmodel_virtual.h"
#include "lds_bool.h"

class ftableview_standmodel_sql_table : public ftableview_standmodel_virtual
{
    Q_OBJECT
public:
    struct checkData{
        checkData() {}
        checkData(const QString &t, const QString &c)
            : ch_tableno(t), checklist(c) {}
        QString ch_tableno;
        QString checklist;
    };
public:
    explicit ftableview_standmodel_sql_table();
    //!
    QList<checkData> get_value_select();//
    QList<QVariantList/*一个deep下的list*/>  get_value_select_deep(QList<int/*deep*/> deeps);

    void enableEndAppendNew(bool enabled);
public slots:
    void toinit();
    void toinitBySqlother(const QString &no);
public:
    QString key_sqlother;
    QString sql_cur;// select * from tablename
    QString sqlother;//   select * from tablename where x = '%1'
private:
    QString sqlargarg;// limit %1,%2
    lds_bool_default_false b_endAppendNew;//用于最后新增按钮
    void toinit0();
    void clearCheckStatus(int r, int c);
    //!~
public:
    virtual void togenerate(int index, int updownum, bool hasnext);
    virtual void togenerateNext();
    virtual void togeneratePre();

};

#endif // FTABLEVIEW_STANDMODEL_SQL_TABLE_H
