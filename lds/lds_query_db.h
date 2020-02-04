#ifndef LDS_QUERY_DB_H
#define LDS_QUERY_DB_H

#include "lds_query_variant.h"
#include <QSqlQuery>

class lds_query_db:public QSqlQuery
{
public:
    struct TablePairlistData{
        int type;//-1,0,1
        QString table;
        qrtVariantPairList list;
        QString where_sql;
    };

    struct TablePairlistInsert{
        QString table;
        qrtVariantPairList list;
    };
    struct TablePairlistUpdate{
        QString table;
        qrtVariantPairList list;
        QString where_sql;
    };
    struct TablePairlistDelete{
        QString table;
        QString where_sql;
    };

public:
    explicit lds_query_db(QSqlDatabase db = QSqlDatabase::database());
    //
    QSqlDatabase db;
    QDateTime curDateTime();
    QString curdatestr();
    QVariant pathValue(QString path);
    QVariant pathValue(QString path, const QString &arg);
    bool pathExisted(QString path);
    bool pathExisted(QString path, const QString &arg);
    static QVariant pathValue(QString path, QSqlDatabase db);
    static QVariant pathValue(QString path, const QString &arg, QSqlDatabase db);
    ///
    bool execUpdate(const TablePairlistUpdate &d, QString *errstring = 0);
    bool execUpdate(const QList<TablePairlistUpdate> &list, QString *errstring = 0);
    bool execUpdate(const QString &table,
                    const qrtVariantPairList &list,
                    const QString &where_sql,
                    QString *errstring = 0);
    bool execUpdate(const QString &table,
                    const QString &key1, const qrtVariant &value1,
                    const QString &where_sql,
                    QString *errstring = 0);

    bool execUpdate(const QString &table,
                    const QString &key1, const qrtVariant &value1,
                    const QString &key2, const qrtVariant &value2,
                    const QString &where_sql,
                    QString *errstring = 0);
    bool execUpdate(const QString &table,
                    const QStringList &unique_key_list,
                    const QString &selectStr_selectKeyInTable,
                    QString *errstring = 0);
    //insert duplicate update
    bool execInsertDuplicateUpdate(const QString &table,
                                   const QStringList &unique_key_list,
                                   const qrtVariantPairList &key_value_list,
                                   QString *errstring = 0);
    bool execInsertDuplicateUpdate(const QString &table,
                                   const QString &unique_key1,
                                   const qrtVariantPairList &key_value_list,
                                   QString *errstring = 0);
    bool execInsertDuplicateUpdate(const QString &table,
                                   const QString &unique_key1,
                                   const QString &unique_key2,
                                   const qrtVariantPairList &key_value_list,
                                   QString *errstring = 0);
    ///
    bool execInsert(const TablePairlistInsert &d, QString *errstring = 0);
    bool execInsert(const QList<TablePairlistInsert> &list, QString *errstring = 0);
    bool execInsert(const QString &table,
                    const qrtVariantPairList &list,
                    QString *errstring = 0);
    bool execInsert(const QString &table,
                    const QString &keys_str, //a, b, c
                    const QString &values_str, //a, b, c
                    const QString &split,
                    QString *errstring = 0);
    bool execInsert(const QString &table,
                    const QString &values_str,//a, b, c
                    const QString &split,
                    QString *errstring = 0);
    bool execInsert(const QString &table,
                    const QString &selectStr_selectKeyInTable,//select sql
                    QString *errstring = 0);

    ///
    bool execDelete(const TablePairlistDelete &d, QString *errstring = 0);
    bool execDelete(const QList<TablePairlistDelete> &list, QString *errstring = 0);
    bool execDelete(const QString &table,
                    QString *errstring = 0);
    bool execDelete(const QStringList &table_list,
                    QString *errstring = 0);
    bool execDelete(const QString &table,
                    const QString &where_sql,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QStringList &tableList,
                    const QString &selectStr_tableListKeyForColumnIndex,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QString &table,
                    const QString &selectStr_tableKeyForColumn0,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QString &table,
                    const QString &table2,
                    const QString &selectStr_tableKeyForColumn0_table2KeyForColumn1,
                    QString *errstring = 0);
    bool execDeleteSelectKey(const QString &table,
                    const QString &table2,
                    const QString &table3,
                    const QString &selectStr_tableKeyForColumn0_table2KeyForColumn1_table3KeyForColumn2,
                    QString *errstring = 0);
    ///
    bool execSelect(const QString &sql, QString *errstring = 0);
    ///
    bool execCreate(const QString &sql, QString *errstring = 0);
    void tryCreate(const QString &sql);
    ///
    bool execDirect(const QString &sql, QString *errstring = 0);
    ///
    bool execAlter(const QString &sql, QString *errstring = 0);
    ///
    bool execTrigger(const QString &sql, QString *errstring = 0);
    ///
    bool execDrop(const QString &table, QString *errstring = 0);

    ///
    int recordCount();
    QVariant recordValue(const QString& name);
    QVariant recordValue(int i);

    QString errorString();
    QString recordError();
    QString insertSql(const QString &table, const qrtVariantPairList &key_value_list);
    QString updateSql(const QString &table, const qrtVariantPairList &key_value_list, const QString &where_sql);
protected:
    bool exec(const QString &query, QString *errstring = 0);
    bool exec(QString *errstring = 0);
    bool prepare(const QString &query, QString *errstring = 0);
    QSqlError lastError() const;
    bool emptyDataIsDateTime(const QString &table, const qrtVariantPair &pair);
    QString getWhereSql(const QStringList &unique_key_list, const qrtVariantPairList &key_value_list);

    //更新的触发器
    virtual void execUpdateTrigger(QList<TablePairlistData> &list);
    //插入的触发器
    virtual void execInsertTrigger(QList<TablePairlistData> &list);
private:
    bool doExecRun(const QList<TablePairlistUpdate> &list, QString *errstring);
    bool doInsert(const QString &table, const qrtVariantPairList &key_value_list, QString *errstring);
    bool doUpdate(const QString &table, const qrtVariantPairList &key_value_list, const QString &where_sql, QString *errstring);
};

#endif // LDS_QUERY_DB_H
