#include "lds_model_sqltablemodel_m.h"
#include "lds_query_db.h"
#include "lds_query_thread.h"
#include <QSqlRecord>
#include <QPixmap>
#include <QtDebug>
#include "lds_expression2rpn.h"
#include <QStack>
#include <QtDebug>
#include <QDateTime>
#include <QSqlError>
#include <QApplication>
#include "math.h"
#include <QFile>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlField>
#include <QtDebug>

const QString lds_model_sqltablemodel_m::SELECT_NOT_CLEAR_MAP_KEY = "SELECT_NOT_CLEAR_MAP_KEY";
const QString lds_model_sqltablemodel_m::SELECT_AUTO_REFRESH_KEY = "SELECT_AUTO_REFRESH_KEY";

class  ifData{
public:
    ifData(int start, int end, const QString &express, const QString &type) ;
    QString express;
    double value;

    int start_value() const;
    int end_value() const;
    QString express_org_value() const;
    QString type_value() const;
    void type_value_set(const QString &type);
private:
    int start;
    int end;
    QString express_org;
    QString type;
};
ifData::ifData(int start, int end, const QString &express, const QString &type)
{
    this->start = start;
    this->end = end;
    express_org = this->express = express;
    this->value = 0;
    this->type = type;
}

int ifData::start_value() const
{
    return start;
}

int ifData::end_value() const
{
    return end;
}

QString ifData::express_org_value() const
{
    return express_org;
}

QString ifData::type_value() const
{
    return type;
}

void ifData::type_value_set(const QString &type)
{
    this->type = type;
}


lds_model_sqltablemodel_m::lds_model_sqltablemodel_m(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{
    select_count = 0;
    tablename_enable_query_enable = true;
}

lds_model_sqltablemodel_m::~lds_model_sqltablemodel_m()
{
}

//qt 5 下 有这种情况
//若是flags是不可编辑，则setdata时，也是失败的
Qt::ItemFlags lds_model_sqltablemodel_m::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QSqlTableModel::flags(index);
    if(set_only_edited_when_OpInsert.contains(index.column())
            && opera(index.row()) != OpInsert) {
        f = f & ~Qt::ItemIsEditable;
    }

    if(!save_set_header_can_edited.contains(index.column())) {
        f = f & ~Qt::ItemIsEditable;
    }
    if(cell_uneditable_map.value(index, false))
        f = f & ~Qt::ItemIsEditable;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if(data_is_editing) {
        f = f | Qt::ItemIsEditable;
    }
#endif
    return f;
}

lds_model_sqltablemodel_m::ModelOpera lds_model_sqltablemodel_m::opera(int row) const
{
    if(QSqlTableModel::headerData(row, Qt::Vertical).toString() == "*") {
        return OpInsert;
    }
    if(QSqlTableModel::headerData(row, Qt::Vertical).toString() == "!") {
        return OpDelete;
    }
    if(row_where_sql_map.find(row) != row_where_sql_map.end()) {
        return OpUpdate;
    }
    return OpNone;
}

void lds_model_sqltablemodel_m::enable_vheaderdata_dirty_translate()
{
    b_enable_vheaderdata_dirty_translate = true;
}

void lds_model_sqltablemodel_m::enable_only_edited_when_OpInsert(int column)
{
    set_only_edited_when_OpInsert.insert(column);
}

void lds_model_sqltablemodel_m::disable_cell_editable(const QModelIndex &index, bool uneditable)
{
    cell_uneditable_map.insert(index, uneditable);
}

void lds_model_sqltablemodel_m::setFilter(const QString &filter)
{
    QSqlTableModel::setFilter(filter);
    if(!setFilter_select) {
        select();
    }
}

void lds_model_sqltablemodel_m::setLastData(const QString &field_name, const QVariant &value, int role)
{
    int k = this->rowCount() - 1;
    if(k >= 0) {
        this->model_data_set(k, field_name, value, role);
    }
}

void lds_model_sqltablemodel_m::appendTableRow(const lds_model_sqltablemodel_m *model, int row)
{
    int k = this->rowCount();
    this->insertRow(k);
    for(int column = 0; column < model->columnCount(); column ++) {
        this->model_data_set(k, column, model->model_data(row, column));
    }
}

double lds_model_sqltablemodel_m::model_data_NIBOLAN(int row, const QVector<QString> &expression) const
{
    return get_value_by_nibolan(expression, row);
}

//尼波兰
//若是非表达式，就会崩溃
double lds_model_sqltablemodel_m::model_data_NIBOLAN(int row, const QString &expression) const
{
    return model_data_NIBOLAN(row, lds_expression2RPN::get_rpn_by(expression));
}

QString lds_model_sqltablemodel_m::model_expression_replace(const QString &expression, const QString &keyOld, const QString &keyNew) const
{
    QVector<int> key_begin_vector;
    QString str = expression;
    str = str.replace(" ", "");
    for(int k = 0; k < str.count(); k++) {
        if(str.mid(k, keyOld.length()) == keyOld) {
            if((k + keyOld.length()) >= str.length()
                    || lds_expression2RPN::containOpFlag(str.mid(k + keyOld.length(), 1))) {
                key_begin_vector.prepend(k);
            }
        }
    }
    for(int k = 0 ;k < key_begin_vector.count(); k++) {
        str = str.replace(key_begin_vector[k], keyOld.length(), keyNew);
    }
    return str;
}

double lds_model_sqltablemodel_m::model_data_IF(int row, const QString &expression) const
{
    QList<ifData> if_data_list;
    QString str = expression;
    str = str.replace(" ", "");

    ///对表达式进行分组，if组和非if组
    int P_start = -1;
    int P_end = -1;
    for(int y  = 0; y < str.count(); y++) {
        if(str.mid(y, 2) == "if") {
            int kuohao = 0;
            int y2 = y + 2;
            for(; y2 < str.count(); y2++) {
                if(str.mid(y2, 1) == "(") kuohao++;
                if(str.mid(y2, 1) == ")") kuohao--;
                if(0 == kuohao) {
                    y2 ++;
                    break;
                }
            }
            if(kuohao != 0) {
                qDebug("kuohao must equal 0!");
            }

            ifData d(y, y2, str.mid(y, y2-y), "Y");
            if(P_start < d.start_value() && P_end > d.end_value() ) {
                if_data_list.prepend(d);
            } else {
                d.type_value_set("P");
                if_data_list.prepend(d);
                P_start = d.start_value();
                P_end = d.end_value();
            }
        }
    }

    ///对if组里的内容进行细分，同时赋值
    P_start = 0;
    for(int k = 0; k < if_data_list.count(); k++) {
        int k0 = P_start;
        while(k0 < k && if_data_list[k].express.count("if") > 1) {
            if_data_list[k].express =
                    if_data_list[k].express.replace(if_data_list[k0].express, QString::number(if_data_list[k0].value));
            k0 ++;
        }
        if(1 == if_data_list[k].express.count("if")) {
            if_data_list[k].value = model_data_IF_equalY(row, if_data_list[k].express);//rand()%100;
        }

        if(if_data_list[k].type_value() == "P") {
            P_start = k;
        }
    }
    ///对非if组进行细分，同时赋值
    for(int k = 0; k < if_data_list.count(); k++) {
        if(if_data_list[k].type_value() == "P") {
            str = str.replace(if_data_list[k].express_org_value(), QString::number(if_data_list[k].value));
        }
    }

    return model_data_NIBOLAN(row, str);
}

double lds_model_sqltablemodel_m::model_data_IF_equalY(int row, const QString &expression) const
{
    QString expression1, expression2;
    QString condition;
    if(model_data_IF_equalY_reslove(expression, condition, expression1, expression2)) {
        return model_data_IF_equalY(row, condition, expression1, expression2);
    }

    return 0;
}

double lds_model_sqltablemodel_m::model_data_IF_equalY(int row, const QString &condition, const QString &expression1, const QString &expression2) const
{
    return model_data_IF_equalY(row, condition, lds_expression2RPN::get_rpn_by(expression1), lds_expression2RPN::get_rpn_by(expression2));
}

double lds_model_sqltablemodel_m::model_data_IF_equalY(int row, const QString &condition, const QVector<QString> &expression1, const QVector<QString> &expression2) const
{
    QVariant c1, c2;
    QStringList list = condition.split("=");
    if(fieldIndex(list[0]) >= 0) {
        c1 = model_data(row, list[0]);
    } else {
        QString p = list[0];
        c1 = p.replace("'", "");// 'Y' --> Y
    }

    if(fieldIndex(list[1]) >= 0) {
        c2 = model_data(row, list[1]);
    } else {
        QString p = list[1];
        c2 = p.replace("'", "");// 'Y' --> Y
    }


    if(c1 == c2) {
        return get_value_by_nibolan(expression1, row);
    } else {
        return get_value_by_nibolan(expression2, row);
    }
    return 0;
}

bool lds_model_sqltablemodel_m::model_data_IF_equalY_reslove(const QString &expression, QString &condition, QString &expression1, QString &expression2) const
{
    QString p = expression;
    QString errstring;
    p = p.replace(" ", "");

    if(false == model_data_IF_equalY_format(expression, &errstring)) {
        qDebug() << errstring << expression;
        QObject *b = 0;b->children();
        return false;
    }


    p = p.remove(0, 3);//IF(
    p.chop(1);//)
    QStringList list = p.split(",");//q
    condition = list[0];//Y
    expression1 = list[1];//2+8
    expression2 = list[2];//2+8

    return true;
}

//if(a=b,c,d)
//if(a=b,c,(e+4)*d)
bool lds_model_sqltablemodel_m::model_data_IF_equalY_format(const QString &expression, QString *errstring) const
{
    QString p = expression;
    p = p.replace(" ", "");
    if(!p.startsWith("IF(") && !p.startsWith("if(")) {
        if(errstring) *errstring  = "model_data_IF_equalY: must start with if or IF( ";
        return false;
    }
    if(!p.endsWith(")")) {
        if(errstring) *errstring  = "model_data_IF_equalY: must end with ) ";
        return false;
    }
    if(!expression.contains("=")) {
        if(errstring) *errstring  = "model_data_IF_equalY: must contain =";
        return false;
    }

    if(expression.count(",") != 2) {
        if(errstring) *errstring  = "model_data_IF_equalY: count(,) must = 2";
        return false;
    }
    if(expression.count("(") != expression.count(")")) {
        if(errstring) *errstring  = "model_data_IF_equalY:  count '(' <> count ')' ";
        return false;
    }
    return true;
}

QVariant lds_model_sqltablemodel_m::model_data(int row, const QString &field_name, int role) const
{
#ifdef QT_DEBUG
    if(debug_model_crash_when_field_not_valid && this->fieldIndex(field_name) < 0) {
        qDebug() << __FUNCTION__<< row << field_name << this->fieldIndex(field_name);
        QObject *x = 0;
        x->children();
    }
#endif
    return model_data(row, this->fieldIndex(field_name), role);
}

QVariant lds_model_sqltablemodel_m::model_data(const QModelIndex &index, int role) const
{
    return model_data(index.row(), index.column(), role);
}

QVariant lds_model_sqltablemodel_m::model_data(int row, int column, int role) const
{
    return this->data(this->index(row, column), role);
}

qrtVariantPairList lds_model_sqltablemodel_m::model_data_pairlist(int row) const
{
    qrtVariantPairList p;
    for(int k = 0;  k < this->columnCount(); k ++) {
        p << qrtVariantPair(this->record().fieldName(k), model_data(row, k));
    }
    return p;
}

bool lds_model_sqltablemodel_m::model_data_set(const QModelIndex &index, const QVariant &value, int role)
{
    return setData(index,  value, role);
}

bool lds_model_sqltablemodel_m::model_data_set(int row, const QString &field_name, const QVariant &value, int role)
{
#ifdef QT_DEBUG
    if(debug_model_crash_when_field_not_valid && this->fieldIndex(field_name) < 0) {
        qDebug() << __FUNCTION__<< this->tableName()<< row << field_name << this->fieldIndex(field_name);
        QObject *x = 0;
        x->children();
    }
#endif
    return model_data_set(row, this->fieldIndex(field_name), value, role);
}

bool lds_model_sqltablemodel_m::model_data_set(int row, int column, const QVariant &value, int role)
{
    return model_data_set(this->index(row, column), value, role);
}

QModelIndex lds_model_sqltablemodel_m::model_index(int row, const QString &field_name)
{
    return this->index(row, this->fieldIndex(field_name));
}

QModelIndex lds_model_sqltablemodel_m::model_index(int row, int column)
{
    return this->index(row, column);
}

bool lds_model_sqltablemodel_m::model_is_dirty() const
{
    return rowCountOp(OpDirty) > 0;
}

bool lds_model_sqltablemodel_m::model_is_dirty(int row) const
{
    return opera(row) != OpNone;
}

bool lds_model_sqltablemodel_m::model_find(const QString &field_name, const QString &cmp_value)
{
    for(int r=0,r_count=this->rowCount();r<r_count;r++) {
        if(cmp_value==model_data(r, field_name).toString()) {
            return true;
        }
    }
    return false;
}

QString lds_model_sqltablemodel_m::max_column_add_1_bysql(const QString &field_name, const char *cformat, int min)
{
    return QString().sprintf(cformat, max_column_add_1_bysql(field_name, min));
}

int lds_model_sqltablemodel_m::max_column_add_1_bysql(const QString &field_name, int min)
{
    lds_query_db query(this->database());
    query.execSelect(QString("select max(cast(%1 as decimal))+1 from %2").arg(field_name).arg(this->tableName()));
    query.next();
    int max_1 = query.recordValue(0).toInt();
    return qMax(max_1, min);
}

int lds_model_sqltablemodel_m::max_column_add_1_dec(const QString &field_name, int min)
{
    int value=0;
    int cmp_value;
    for(int r=0,r_count=this->rowCount();r<r_count;r++) {
        cmp_value=this->model_data(r, field_name).toInt();
        if(value<cmp_value) {
            value=cmp_value;
        }
    }
    return qMax(min, value+1);
}

QString lds_model_sqltablemodel_m::max_column_add_1__max_sql_add_1_dec(const QString &field_name, const char *cformat, int min)
{
    return QString().sprintf(cformat, max_column_add_1__max_sql_add_1_dec(field_name, min));
}

int lds_model_sqltablemodel_m::max_column_add_1__max_sql_add_1_dec(const QString &field_name, int min)
{
    return qMax(max_column_add_1_bysql(field_name, min),
                max_column_add_1_dec(field_name, min));
}

void lds_model_sqltablemodel_m::enable_debug_model_not_crash_when_field_not_valid()
{
#ifdef QT_DEBUG
    debug_model_crash_when_field_not_valid = false;
#endif
}

void lds_model_sqltablemodel_m::save_set_header_clear()
{
    save_set_header_saver_column_list.clear();
    save_set_header_can_edited.clear();
}

bool lds_model_sqltablemodel_m::save_set_header(const QString &field_name, int iseditable)
{
    return save_set_header(this->fieldIndex(field_name), iseditable);
}

bool lds_model_sqltablemodel_m::save_set_header(int section, int iseditable)
{
    //header save
    save_set_header_saver_column_list << section;
    if(iseditable > 0) {
        save_set_header_can_edited.insert(section);
        //        if(iseditable == 1) {
        //            this->setHeaderData(section, Qt::Horizontal,QPixmap(":/image/widgets_pic/model_pen"),Qt::DecorationRole);
        //        }
        //        if(iseditable == 2) {
        //            this->setHeaderData(section, Qt::Horizontal,QPixmap(":/image/widgets_pic/model_pen2"),Qt::DecorationRole);
        //        }
        //        if(iseditable == 3) {
        //            this->setHeaderData(section, Qt::Horizontal,QPixmap(":/image/widgets_pic/model_pen3"),Qt::DecorationRole);
        //            enable_only_edited_when_OpInsert(section);
        //        }
    }
    return true;
}

bool lds_model_sqltablemodel_m::save_set_header(int section, const QString &hheaderStr, int iseditable)
{
    save_section_value_map.insert(section, hheaderStr);
    return save_set_header(section, iseditable);
}

bool lds_model_sqltablemodel_m::save_set_header(const QString &field_name, const QString &hheaderStr, int iseditable)
{
    return save_set_header(fieldIndex(field_name), hheaderStr, iseditable);
}

QList<int> lds_model_sqltablemodel_m::get_save_set_header_saver_column_list()
{
    return save_set_header_saver_column_list;
}

bool lds_model_sqltablemodel_m::select()
{
    if(!setFilter_select)
        setFilter_select = true;

    select_count ++;
    qDebug() << "SELECT_COUNT:" << this << select_count;
    bool f = false;
    f = QSqlTableModel::select();
    //清除缓存
    tablename_kvmap_delete_for_select_and_autoselect();
    row_where_sql_map.clear();

    return f;
}

double lds_model_sqltablemodel_m::get_value_by_nibolan(const QVector<QString> &nibolan_str, int row) const
{
    QStack<double> stack;
    bool ok;
    double p0;
    double p1;
    foreach(const QString &k, nibolan_str) {
        if("*" == k) {
            p0 = stack.pop();
            p1 = stack.pop();
            stack.push(p1 * p0);
        } else if("/" == k) {
            p0 = stack.pop();
            p1 = stack.pop();
            stack.push(p1 / p0);
        } else if("+" == k) {
            p0 = stack.pop();
            p1 = stack.pop();
            stack.push(p1 + p0);
        } else if("-" == k) {
            p0 = stack.pop();
            p1 = stack.pop();
            stack.push(p1 - p0);
        } else {
            p1 = k.toDouble(&ok);
            if(ok) stack.push(p1);
            else stack.push( this->model_data(row, k).toDouble());
        }
    }
    return stack[0];
}

QVariant lds_model_sqltablemodel_m::data(const QModelIndex &idx, int role) const
{
    QVariant val;
    if(role < Qt::UserRole)
        val = QSqlTableModel::data(idx, role);
    else
        val = rowRoleDataList[idx.row()].value(role);

    //Qt 下 bindValue(name, true)数据库会插入'true';
    return val.type() == QVariant::Bool ? (val.toBool() ? 1 : 0) : val;
}

bool lds_model_sqltablemodel_m::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QVariant old = this->data(index, role);
    if(old.type() == QVariant::String
            &&old == value) {
        return true;
    }
    QString where_sql = insert_row_where_sql(index.row() );
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    data_is_editing = true;
#endif
    bool f = QSqlTableModel::setData(index, value, role);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    data_is_editing = false;
#endif
    if(isDirty(index)) {
        insert_row_where_sql_map(index.row(), where_sql);
    }
    //
    if(role >= Qt::UserRole)
        rowRoleDataList[index.row()].insert(role, value);
    //
    return f;
}

bool lds_model_sqltablemodel_m::submitAll()
{
    return QSqlTableModel::submitAll();
}

QVariant lds_model_sqltablemodel_m::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant d = QSqlTableModel::headerData(section, orientation, role);
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Vertical) {
            if(b_enable_vheaderdata_dirty_translate) {
                if(opera(section) == OpDelete) {
                    return "-";
                }
                if(opera(section) == OpInsert) {//new
                    return "+";
                }
                if(opera(section) == OpUpdate) {//update
                    return "#";
                }
            }
        }

        if(orientation == Qt::Horizontal) {
            return save_section_value_map.value(section, d);
        }
    }
    return d;
}

int lds_model_sqltablemodel_m::rowCountOp(ModelOperas ops) const
{
    int ret_count_insert = 0;
    int ret_count_update = 0;
    int ret_count_delete = 0;
    int ret_count_none = 0;

    int row_count = rowCount();
    int insert_row_count = 0;
    for(int rowcount = row_count - 1; rowcount >= 0; rowcount --)  {
        if(opera(rowcount) == OpInsert) {
            insert_row_count ++;
            continue;
        }
        break;
    }
    //
    if(ops & OpInsert) {
        ret_count_insert = insert_row_count;
    }

    QMapIterator<int,  QString> mapitr(row_where_sql_map);
    while(mapitr.hasNext()) {
        mapitr.next();
        if(opera(mapitr.key()) == OpUpdate) {
            if(ops & OpUpdate) ret_count_update ++;
        } else {
            if(ops & OpDelete) ret_count_delete ++;
        }
    }

    if(ops & OpNone) {
        ret_count_none = row_count/*total*/ - (row_where_sql_map.count()/*delete update*/ + insert_row_count/*insert*/);
    }
    return ret_count_none + ret_count_insert + ret_count_update + ret_count_delete;
}

bool lds_model_sqltablemodel_m::submitRow(int row, QString *errstring)
{
    lds_query_db query(this->database());
    QString table_name = tableName();
    QString where_sql;
    //OpUpdate
    if(opera(row) == OpUpdate) {
        where_sql = row_where_sql_map.value(row);
        qrtVariantPairList key_value_list;
        for(int k = 0; k < columnCount(); k++) {
            if(isDirty(index(row, k))) {
                key_value_list << qrtVariantPair(this->record().fieldName(k), model_data(row, k));
            }
        }
        if(!query.execUpdate(table_name, key_value_list, where_sql, errstring)) {
            return false;
        }
    }
    //OpInsert
    if(opera(row) == OpInsert) {
        qrtVariantPairList key_value_list;
        for(int k = 0; k < columnCount(); k ++) {
            key_value_list << qrtVariantPair(this->record().fieldName(k),  model_data(row, k));
        }
        if(!query.execInsert(table_name, key_value_list, errstring)) {
            return false;
        }
    }
    //OpDelete
    if(opera(row) == OpDelete) {
        where_sql = row_where_sql_map.value(row);
        if(where_sql.length() > 0) {
            if(!query.execDelete(table_name, where_sql, errstring)) {//OpDelete
                return false;
            }
        }
    }
    return true;
}

QSqlError lds_model_sqltablemodel_m::lastError() const
{
    return QSqlTableModel::lastError();
}

bool lds_model_sqltablemodel_m::trySubmitAll(QString *errstring)
{
    //insert
    for(int row = rowCount() - 1; row >= 0; row--) {
        if(opera(row) == OpInsert) {
            if(false == submitRow(row, errstring)) {
                return false;
            }
        } else {
            break;
        }
    }
    //update delete
    QMapIterator<int,  QString> mapitr(row_where_sql_map);
    while(mapitr.hasNext()) {
        mapitr.next();
        if(false == submitRow(mapitr.key(), errstring)) {
            return false;
        }
    }
    //select
    select();
    return true;
}

bool lds_model_sqltablemodel_m::trySubmitAllUpdateLoading(QString *errstring, QObject *object, void (*updateProgress)(QObject *, qint64, qint64))
{
    //bytesTotal
    qint64 bytesStep = 0;
    qint64 bytesTotal = this->rowCountOp(OpInsert) + row_where_sql_map.count();
    //insert
    for(int row = rowCount() - 1; row >= 0; row--) {
        if(opera(row) == OpInsert) {
            updateProgress(object, ++bytesStep, bytesTotal);
            if(false == submitRow(row, errstring)) {
                return false;
            }
        } else {
            break;
        }
    }
    //update delete
    QMapIterator<int,  QString> mapitr(row_where_sql_map);
    while(mapitr.hasNext()) {
        mapitr.next();
        updateProgress(object, ++bytesStep, bytesTotal);
        if(false == submitRow(mapitr.key(), errstring)) {
            return false;
        }
    }
    //select
    select();
    return true;
}

bool lds_model_sqltablemodel_m::removeRows(int row, int count, const QModelIndex &parent)
{
    for(int k = 0; k < count; k++) {
        insert_row_where_sql_map(k + row);
    }
    //
    for(int k = 0; k < count; k++) {
        rowRoleDataList.removeAt(row +k);
    }
    //
    bool f =  QSqlTableModel::removeRows(row, count, parent);

    return f;
}

bool lds_model_sqltablemodel_m::insertRows(int row, int count, const QModelIndex &parent)
{
    //
    for(int k = 0; k < count; k++) {
        rowRoleDataList.insert( row + k, QMap<int, QVariant>() );
    }
    //
    return QSqlTableModel::insertRows(row, count, parent);
}

void lds_model_sqltablemodel_m::SELECT_AUTO_REFRESH_refresh(const QString &tablename) const
{
    //d
    SELECT_AUTO_REFRESH_data  sqls = SELECT_AUTO_REFRESH_map.value(tablename);
    int k = kvmap_insert_unique(tablename);
    lds_model_sqltablemodel_d &d = tablename_kvmap[k].second;
    //key_value_sql
    QString key_value_sql;
    {
        QString sql_filter = sqls.sql_filter;
        QString sql_keyname = sqls.sql_keyname;
        QString sql_valuename = sqls.sql_valuename;

        if(sqls.key_value_sql.length() > 0) {
            key_value_sql = sqls.key_value_sql;
        } else {
            key_value_sql = QString("select %1, %2 from %3 %4")
                    .arg(sql_keyname)
                    .arg(sql_valuename)
                    .arg(tablename)
                    .arg(sql_filter.isEmpty() ? "": (" where "+sql_filter));
        }
    }

    //clear
    d.clear();
    //query

    lds_query_db query(this->database());
    query.execSelect(key_value_sql);
    int recordcount = query.record().count();
    //firstEmpty
    if(sqls.firstEmpty) {
        d.setData("", query.record().fieldName(1), "");
    }
    while(query.next()) {
        for(int k = 1; k < recordcount; k++) {
            d.setData(query.recordValue(0).toString(), query.record().fieldName(k), query.recordValue(k));
        }
    }
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_table(const QString &tablename) const
{
    kvmap_insert_unique(tablename);
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_value(const QString &tablename, const QString &key, const QVariant &value) const
{
    tablename_kvmap_insert_field(tablename, key, "", value);
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_field(const QString &tablename, const QString &key, const QString &field_name, const QVariant &field_value) const
{
    //1
    int k = tablename_kvmap_create(tablename);
    //2
    tablename_kvmap[k].second.setData(key, field_name, field_value);
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_field(const QString &tablename, const QString &key, const QVariantMap &map) const
{
    QMapIterator<QString, QVariant> mapitr(map);
    while(mapitr.hasNext()) {
        mapitr.next();
        tablename_kvmap_insert_field(tablename, key, mapitr.key(), mapitr.value());
    }
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_field(const QString &tablename, const QString &key, const qrtVariantPairList &pairs) const
{
    for(int k = 0; k < pairs.count(); k++) {
        tablename_kvmap_insert_field(tablename, key, pairs[k].first, pairs[k].second);
    }
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_field(const QString &tablename, const QString &key, const QSqlRecord &record) const
{
    for(int k = 0; k < record.count(); k++) {
        tablename_kvmap_insert_field(tablename, key, record.fieldName(k), record.value(k));
    }
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_sql(const QString &tablename, const QString &sql_keyname, const QString &sql_valuename, const QString &sql_filter, bool firstEmpty, const QString &SELECT_AUTO_REFRESH_KEY)
{
    QString key_value_sql = QString("select %1, %2 from %3 %4")
            .arg(sql_keyname)
            .arg(sql_valuename)
            .arg(tablename)
            .arg(sql_filter.isEmpty() ? "": (" where "+sql_filter));
    tablename_kvmap_insert_sql(tablename, key_value_sql, firstEmpty, SELECT_AUTO_REFRESH_KEY);
}

void lds_model_sqltablemodel_m::tablename_kvmap_insert_sql(const QString &tablename, const QString &key_value_sql, bool firstEmpty, const QString &SELECT_AUTO_REFRESH_KEY)
{
    if(this->SELECT_AUTO_REFRESH_KEY == SELECT_AUTO_REFRESH_KEY) {
        if(SELECT_AUTO_REFRESH_map.find(tablename) == SELECT_AUTO_REFRESH_map.end()) {
            SELECT_AUTO_REFRESH_data d(key_value_sql);
            d.firstEmpty = firstEmpty;
            SELECT_AUTO_REFRESH_map.insert(tablename, d);
        }
        SELECT_AUTO_REFRESH_refresh(tablename);
    }
}

QVariant lds_model_sqltablemodel_m::sql_value(const QString &tablename, const QString &sql_keyvalue, const QString &sql_keyname, const QString &sql_valuename, const QString &sql_filter) const
{
    if(tablename_enable_query_enable == false)
        return QVariant();

    //拼接filter
    QString filter;
    filter = sql_keyname + "= '" +sql_keyvalue+"'";
    if(!sql_filter.isEmpty()) {
        filter += " and "+sql_filter;
    }

    //查询
    lds_query_db query(this->database());
    QVariant retd;
    if(!query.execSelect("select "+sql_valuename + " from " + tablename + " where " +filter)) {
#ifdef QT_DEBUG
        if(debug_model_crash_when_field_not_valid) {
            qDebug() << __FILE__ << __LINE__ << "check param is uncorrect!" << query.lastQuery();
            QObject *o = 0;
            o->children();
        }
#endif
    }

    if(query.next()) {
        retd = query.recordValue(0);
    }
    return retd;
}

const lds_model_sqltablemodel_d *lds_model_sqltablemodel_m::tablename_kvmap_d(const QString &tablename) const
{
    int k = kvmap_index(tablename);
    if(k >= 0) {
        return &tablename_kvmap[k].second;
    }
    return 0;
}

void lds_model_sqltablemodel_m::tablename_kvmap_print() const
{
    for(int k = 0; k < tablename_kvmap.count(); k ++) {
        qDebug() << "============== == " << tablename_kvmap[k].first << "============== == ";
        tablename_kvmap[k].second.print();
        qDebug() << "";
        qDebug() << "";
        qDebug() << "";
    }
}

void lds_model_sqltablemodel_m::tablename_kvmap_print(const QString &tablename) const
{
    const lds_model_sqltablemodel_d *d = tablename_kvmap_d(tablename);
    if(d) {
        qDebug() << tablename;
        d->print();
    }
}

bool lds_model_sqltablemodel_m::tablename_key_existed(const QString &tablename, const QString &key) const
{
    const lds_model_sqltablemodel_d *d = tablename_kvmap_d(tablename);
    if(d != 0) {
        return d->isExistedRow(key);
    }
    return false;
}

bool lds_model_sqltablemodel_m::tablename_kvmap_field_existed(const QString &tablename, const QString &key, const QString &field_name) const
{
    const lds_model_sqltablemodel_d *d = tablename_kvmap_d(tablename);
    if(d != 0) {
        return d->isFill(key, field_name);
    }
    return false;
}

void lds_model_sqltablemodel_m::tablename_enable_query(bool enable)
{
    tablename_enable_query_enable = enable;
}

int lds_model_sqltablemodel_m::tablename_kvmap_count(const QString &tablename) const
{
    const lds_model_sqltablemodel_d *d = tablename_kvmap_d(tablename);
    if(d != 0) {
        return d->rowCount();
    }
    return 0;
}

QString lds_model_sqltablemodel_m::tablename_kvmap_indexof(const QString &tablename, const qrtVariantPairList &select_arg, bool *ok) const
{
    const lds_model_sqltablemodel_d *d = tablename_kvmap_d(tablename);
    int row = -1;
    if(ok) *ok = false;

    if(d != 0) {
        row = d->indexOf(select_arg);
        if(row != -1) {
            if(ok) *ok = true;
            return d->key(row);
        }
    }
    return "";
}

void lds_model_sqltablemodel_m::tablename_kvmap_select(const QString &tablename) const
{
    tablename_kvmap_select_tablename = tablename;
    tablename_kvmap_select_index = -1;
    tablename_kvmap_select_item = tablename_kvmap_d(tablename);
}

bool lds_model_sqltablemodel_m::tablename_kvmap_select_next() const
{
    if(tablename_kvmap_select_item == 0)
        return false;
    tablename_kvmap_select_index ++;
    return tablename_kvmap_select_index < tablename_kvmap_select_item->rowCount();
}

QVariant lds_model_sqltablemodel_m::tablename_kvmap_select_value(const QString &field_name) const
{
    return tablename_kvmap_select_item->data(tablename_kvmap_select_index, field_name);
}

void lds_model_sqltablemodel_m::tablename_kvmap_delete(const QString &tablename) const
{
    int k = kvmap_index(tablename);
    if(k >= 0)
        tablename_kvmap[k].second.clear();
}

void lds_model_sqltablemodel_m::tablename_kvmap_drop(const QString &tablename) const
{
    for(int k = 0; k < tablename_kvmap.count(); k ++) {
        if(tablename_kvmap[k].first == tablename) {
            tablename_kvmap.removeAt(k);
            break;
        }
    }
}

void lds_model_sqltablemodel_m::tablename_kvmap_delete_for_select_and_autoselect()
{
    for(int k = 0; k < tablename_kvmap.count(); k ++) {
        if(SELECT_NOT_CLEAR_MAP_KEY_set.contains(tablename_kvmap[k].first))
            continue;
        tablename_kvmap_delete(tablename_kvmap[k].first);
    }
    QMapIterator<QString/*tablename*/, SELECT_AUTO_REFRESH_data> mapitr(SELECT_AUTO_REFRESH_map);//当map在select中清空时，这些需要保留
    while(mapitr.hasNext()) {
        mapitr.next();
        SELECT_AUTO_REFRESH_refresh(mapitr.key());
    }

    emit signal_select_and_autoselect();
}

int lds_model_sqltablemodel_m::tablename_kvmap_create(const QString &tablename) const
{
    int k = kvmap_index(tablename);
    if(k < 0) {
         k = kvmap_insert_unique(tablename);
        SELECT_NOT_CLEAR_MAP_KEY_set.insert(tablename);
    }
    return k;
}

QVariant lds_model_sqltablemodel_m::tablename_kvmap_value(const QString &tablename, const QString &sql_keyvalue, const QString &sql_keyname, const QString &sql_valuename, const QString &sql_filter, const QVariant &default_value, bool default_fill) const
{
    int k = kvmap_insert_unique(tablename);
    lds_model_sqltablemodel_d &d = tablename_kvmap[k].second;

    QVariant retv;
    if(!d.isFill(sql_keyvalue, sql_valuename)) {//列表中不存在
        retv = sql_value(tablename, sql_keyvalue, sql_keyname, sql_valuename, sql_filter);
        if(retv.isValid()) {
            d.setData(sql_keyvalue, sql_valuename, retv);
        }
    } else {
        retv = d.data(sql_keyvalue, sql_valuename);
    }
    if(!retv.isValid() && default_value.isValid()) {//存在但为null
        if(default_fill) {
            d.setData(sql_keyvalue, sql_valuename, default_value);
        }
        retv = default_value;
        //        return default_value;//d.setData(sql_keyvalue, sql_valuename, default_value);
    }
    return retv;
}

QVariant lds_model_sqltablemodel_m::tablename_kvmap_value_NIBOLAN(const QString &tablename, const QString &sql_keyvalue, const QString &sql_keyname, const QString &sql_expression) const
{
    return QVariant();
}

QVariant lds_model_sqltablemodel_m::tablename_kvmap_value(const QString &tablename, const QString &key, const QVariant &default_value) const
{
    return tablename_kvmap_field(tablename, key, "", default_value);
}

QVariant lds_model_sqltablemodel_m::tablename_kvmap_field(const QString &tablename, const QString &key, const QString &field_name, const QVariant &default_value) const
{
    QVariant ret;
    const lds_model_sqltablemodel_d *d = tablename_kvmap_d(tablename);
    if(0 != d) {
        ret = d->data(key, field_name);
    }
    if(!ret.isValid())
        return default_value;
    return ret;
}

qrtVariantPair lds_model_sqltablemodel_m::tablename_kvmap_pair(const QString &tablename, const QString &key, const QString &field_name) const
{
    const lds_model_sqltablemodel_d *d = tablename_kvmap_d(tablename);
    if(0 != d) {
        return qrtVariantPair(field_name, d->data(key, field_name));
    }
    return qrtVariantPair("", qrtVariant("", qrtVariant::NONE));
}

int lds_model_sqltablemodel_m::kvmap_index(const QString &tablename) const
{
    for(int k = 0; k < tablename_kvmap.count(); k ++) {
        if(tablename_kvmap[k].first == tablename)
            return k;
    }
    return -1;
}

int lds_model_sqltablemodel_m::kvmap_insert_unique(const QString &tablename) const
{
    int k = kvmap_index(tablename);
    if(k >= 0)
        return k;

    tablename_kvmap.append(QPair<QString, lds_model_sqltablemodel_d>(tablename, lds_model_sqltablemodel_d()));
    k = tablename_kvmap.count() - 1;
    return k;
}

//只记录，非插入的数据
void lds_model_sqltablemodel_m::insert_row_where_sql_map(int row)
{
    QString sql;
    for(int p = 0; p < primaryKey().count(); p ++) {
        QString kname = primaryKey().fieldName(p);
        QString kvalue = model_data(row, kname).toString();
        sql += qrtEqual(kname, kvalue) + " and ";
    }
    sql.chop(5);
    insert_row_where_sql_map(row, sql);
}

void lds_model_sqltablemodel_m::insert_row_where_sql_map(int row, const QString &sql)
{
    if(opera(row) == OpInsert) return;
    if(row_where_sql_map.find(row) != row_where_sql_map.end()) return;
    row_where_sql_map.insert(row, sql);
}

QString lds_model_sqltablemodel_m::insert_row_where_sql(int row)
{
    QString where_sql;
    for(int p = 0; p < primaryKey().count(); p ++) {
        QString kname = primaryKey().fieldName(p);
        QString kvalue = model_data(row, kname).toString();
        where_sql += qrtEqual(kname, kvalue) + " and ";
    }
    where_sql.chop(5);

    return where_sql;
}


