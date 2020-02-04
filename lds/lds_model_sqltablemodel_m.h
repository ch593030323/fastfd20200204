#ifndef LDS_MODEL_SQLTABLEMODEL_M_H
#define LDS_MODEL_SQLTABLEMODEL_M_H

#include <QSqlTableModel>
#include <QSet>
#include <QMapIterator>
#include <QStringList>
#include "lds_bool.h"
#include "lds_model_sqltablemodel_d.h"
#include "lds_query_variant.h"

class lds_model_sqltablemodel_m : public QSqlTableModel
{
    Q_OBJECT
public:
    enum ModelOpera{
        OpNone = 1,
        OpInsert = 2,
        OpUpdate = 4,
        OpDelete = 8,

        OpExceptDelete = OpNone|OpInsert |OpUpdate,
        OpExceptInsert  = OpNone|                OpUpdate|OpDelete,
        OpDirty               =               OpInsert |OpUpdate|OpDelete,
    };
    Q_DECLARE_FLAGS(ModelOperas, ModelOpera)

public:
    lds_model_sqltablemodel_m(QObject *parent, QSqlDatabase db);
    virtual ~lds_model_sqltablemodel_m();
    Qt::ItemFlags flags(const QModelIndex &index) const;
    ModelOpera opera(int row) const;

    void enable_vheaderdata_dirty_translate();
    void enable_only_edited_when_OpInsert(int column);
    void disable_cell_editable(const QModelIndex &index, bool uneditable);
    void setFilter(const QString &filter);

    void appendTableRow(const lds_model_sqltablemodel_m *model, int row);
    void setLastData(const QString & field_name, const QVariant &value, int role = Qt::EditRole);
private:
    lds_bool_default_false b_enable_vheaderdata_dirty_translate;
    lds_bool_default_false setFilter_select;
    QSet<int> set_only_edited_when_OpInsert;
    QMap<QModelIndex, bool> cell_uneditable_map;
public:
    double model_data_NIBOLAN(int row, const QVector<QString> &expression) const;//支持；+-*/()等运算
    double model_data_NIBOLAN(int row, const QString &expression) const;//支持；+-*/()等运算

    //如：num_price_ad, num_price 的情况，我只替换，num_price 而不影响num_price_ad
    QString model_expression_replace(const QString &expression, const QString &keyOld, const QString &keyNew) const;
    //if(a = 'Y', x, y)
    double model_data_IF(int row, const QString &expression) const;
    double model_data_IF_equalY(int row, const QString &expression) const;//支持 xx?dd:ss
    double model_data_IF_equalY(int row, const QString &condition, const QString &expression1 , const QString &expression2) const;//支持 xx?dd:ss
    double model_data_IF_equalY(int row, const QString &condition, const QVector<QString> &expression1 , const QVector<QString> &expression2) const;//支持 xx?dd:ss

    bool model_data_IF_equalY_reslove(const QString &expression, QString &condition, QString &expression1, QString &express2) const;//支持 xx?dd:ss
    bool model_data_IF_equalY_format(const QString &expression, QString *errstring = 0) const;//支持 xx?dd:ss

    QVariant model_data(int row, const QString &field_name, int role = Qt::DisplayRole) const;
    QVariant model_data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /**/QVariant model_data(int row, int column, int role = Qt::DisplayRole) const;

    qrtVariantPairList model_data_pairlist(int row) const;

    bool model_data_set(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool model_data_set(int row, const QString &field_name, const QVariant &value, int role = Qt::EditRole);
    /**/bool model_data_set(int row, int column, const QVariant &value, int role = Qt::EditRole);

    QModelIndex model_index(int row, const QString &field_name);
    QModelIndex model_index(int row, int column);
    bool model_is_dirty() const;//model是否有脏数据
    bool model_is_dirty(int row) const;//model是否有脏数据

    bool model_find(const QString &field_name, const QString &cmp_value);
    //query来获取最大值，不存在返回10
    QString max_column_add_1_bysql(const QString &field_name, const char * cformat, int min = 10);
    int max_column_add_1_bysql(const QString &field_name, int min = 10);

    int max_column_add_1_dec(const QString &field_name, int min = 10);//返回+1后的str

    QString max_column_add_1__max_sql_add_1_dec(const QString &field_name, const char * cformat, int min = 10);
    int max_column_add_1__max_sql_add_1_dec(const QString &field_name, int min = 10);

    void enable_debug_model_not_crash_when_field_not_valid();

#ifdef QT_DEBUG
    lds_bool_default_false debug_model_crash_when_field_not_valid;
#endif
public:
    void save_set_header_clear();
    bool save_set_header(const QString &field_name, int iseditable = 0);
    bool save_set_header(int section, int iseditable = 0);
    bool save_set_header(int section, const QString &hheaderStr,  int iseditable = 0);
    bool save_set_header(const QString &field_name, const QString &hheaderStr, int iseditable=0);//配合tableview的 rePecifyHeader 函数使用的

    QList<int> get_save_set_header_saver_column_list();

    int select_count;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCountOp(ModelOperas ops) const;

    bool trySubmitAll(QString *errstring = 0);
    bool trySubmitAllUpdateLoading(QString *errstring, QObject *object, void (*updateProgress)(QObject *, qint64, qint64));

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
public slots:
    bool select();
private:
    double get_value_by_nibolan(const QVector<QString>& nibolan_str, int row) const;
protected:
    //外部不可调用
    QVariant data(const QModelIndex &idx, int role = Qt::EditRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QSqlError lastError() const;
    bool submitAll();
    //~外部不可调用
    bool submitRow(int row, QString *errstring);
private:
    QSet<int> save_set_header_can_edited;//可以编辑的set
    QList<int> save_set_header_saver_column_list;//中文的列名的list
    QMap<int, QVariant> save_section_value_map;
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    lds_bool_default_false data_is_editing;
#endif
public:
    static const QString SELECT_NOT_CLEAR_MAP_KEY;
    static const QString SELECT_AUTO_REFRESH_KEY;
private:
    ///tablename_kvmap
    ///tablename可以为任意值，不一定为数据库表名
    struct SELECT_AUTO_REFRESH_data{
        inline SELECT_AUTO_REFRESH_data() {}

        //
        inline SELECT_AUTO_REFRESH_data(const QString &sql_keyname, const QString &sql_valuename, const QString &sql_filter
                                        , bool firstEmpty = false) {
            this->sql_filter = sql_filter;
            this->sql_keyname = sql_keyname;
            this->sql_valuename = sql_valuename;
            this->firstEmpty = firstEmpty;
        }
        QString sql_keyname;
        QString sql_valuename;
        QString sql_filter;
        lds_bool_default_false firstEmpty;

        //
        inline SELECT_AUTO_REFRESH_data(const QString &key_value_sql) {
            this->key_value_sql = key_value_sql;
        }
        QString key_value_sql;
    };

    mutable QSet<QString> SELECT_NOT_CLEAR_MAP_KEY_set;//当map在select中清空时，这些需要保留
    QMap<QString/*tablename*/, SELECT_AUTO_REFRESH_data> SELECT_AUTO_REFRESH_map;//当map在select中清空时，这些需要保留
    void SELECT_AUTO_REFRESH_refresh(const QString &tablename) const;
public:
    void tablename_kvmap_insert_field(const QString &tablename
                                      , const QString &key
                                      , const QString &field_name
                                      , const QVariant &field_value) const;//直接插入
    void tablename_kvmap_insert_field(const QString &tablename
                                      , const QString &key
                                      , const QVariantMap &map) const;//直接插入
    void tablename_kvmap_insert_field(const QString &tablename
                                      , const QString &key
                                      , const qrtVariantPairList &pairs) const;//直接插入
    void tablename_kvmap_insert_field(const QString &tablename
                                      , const QString &key
                                      , const QSqlRecord &record) const;//直接插入
    //插入获取map(tablename, 空的map(key, value))
    void tablename_kvmap_insert_table(const QString &tablename) const;//直接插入
    //插入获取map(tablename, 的map(keyname, valuename))
    ///插入固定的key和value
    void tablename_kvmap_insert_value(const QString &tablename
                                      , const QString &key
                                      , const QVariant &value) const;//直接插入
    //返回map<key, value>， 若map为空则更新数据
    ///插入sql的检索条件，select时，会自动刷新，要求，sql的检索结果要尽可能的少
    void tablename_kvmap_insert_sql(const QString &tablename
                                    , const QString &sql_keyname
                                    , const QString &sql_valuename
                                    , const QString &sql_filter
                                    , bool firstEmpty
                                    , const QString &SELECT_AUTO_REFRESH_KEY) ;//直接插入
    void tablename_kvmap_insert_sql(const QString &tablename
                                    , const QString &key_value_sql
                                    , bool firstEmpty
                                    , const QString &SELECT_AUTO_REFRESH_KEY) ;//直接插入
    //返回map<key, value>
    const lds_model_sqltablemodel_d * tablename_kvmap_d(const QString &tablename) const;
    void tablename_kvmap_print() const;
    void tablename_kvmap_print(const QString &tablename) const;

    bool tablename_key_existed(const QString &tablename, const QString &key) const;
    bool tablename_kvmap_field_existed(const QString &tablename, const QString &key, const QString &field_name) const;

    void tablename_enable_query(bool enable);

    int tablename_kvmap_count(const QString &tablename) const;

    QString tablename_kvmap_indexof(const QString &tablename, const qrtVariantPairList &select_arg, bool *ok = 0) const;

    //tablename_kvmap_select
public:
    void tablename_kvmap_select(const QString &tablename) const;
    bool tablename_kvmap_select_next() const;
    QVariant tablename_kvmap_select_value(const QString &field_name) const;
private:
    mutable QString tablename_kvmap_select_tablename;
    mutable int tablename_kvmap_select_index;
    mutable const lds_model_sqltablemodel_d* tablename_kvmap_select_item;
    //~tablename_kvmap_select
signals:
    void signal_select_and_autoselect();
public:
    //清除map<tablename, map>
    void tablename_kvmap_delete_for_select_and_autoselect();
    int tablename_kvmap_create(const QString &tablename) const;
    void tablename_kvmap_delete(const QString &tablename) const;
    void tablename_kvmap_drop(const QString &tablename) const;

public:
    QVariant tablename_kvmap_field(const QString &tablename,
                                   const QString &key,
                                   const QString &field_name,
                                   const QVariant &default_value = QVariant()) const;
    qrtVariantPair tablename_kvmap_pair(const QString &tablename,
                                        const QString &key,
                                        const QString &field_name) const;
    QVariant tablename_kvmap_value(const QString &tablename,
                                   const QString &key,
                                   const QVariant &default_value = QVariant()) const;
    QVariant tablename_kvmap_value(const QString &tablename,
                                   const QString &sql_keyvalue,
                                   const QString &sql_keyname,
                                   const QString &sql_valuename,
                                   const QString &sql_filter = "",
                                   const QVariant &default_value = QVariant(),
                                   bool default_fill = false) const;
    QVariant tablename_kvmap_value_NIBOLAN(const QString &tablename,
                                   const QString &sql_keyvalue,
                                   const QString &sql_keyname,
                                   const QString &sql_expression ) const;

private:
    QVariant sql_value(const QString &tablename, const QString &sql_keyvalue,
                       const QString &sql_keyname, const QString &sql_valuename,
                       const QString &sql_filter = "") const;
private:
    //删除全部
    mutable QList<QPair<QString, lds_model_sqltablemodel_d> > tablename_kvmap;

    int kvmap_index(const QString &tablename) const;
    int kvmap_insert_unique(const QString &tablename) const;

    QMap<int , QString> row_where_sql_map;
    void insert_row_where_sql_map(int row);
    void insert_row_where_sql_map(int row, const QString &sql);
    QString insert_row_where_sql(int row);
    bool tablename_enable_query_enable;

    QList<QMap<int, QVariant> > rowRoleDataList;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(lds_model_sqltablemodel_m::ModelOperas)


#endif // LDS_MODEL_SQLTABLEMODEL_M_H
