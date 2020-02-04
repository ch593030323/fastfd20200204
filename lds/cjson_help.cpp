#include "cjson_help.h"
#include "cJSON.h"
#include <QtDebug>
#include <QDateTime>

static QString dateFormatStr="yyyy-MM-dd hh:mm:ss";

cJSON_help::cJSON_help()
{
    this->json = 0;
}

cJSON_help::cJSON_help(const QByteArray &d)
{
    json = 0;
    parse(d);
}

cJSON_help::cJSON_help(cJSON *json)
{
    this->json = json;
}

cJSON_help::cJSON_help(int type) : QObject(0)
{
    /* cJSON Types: */
    switch(type) {
    case cJSON_Array :
        this->json = cJSON_CreateArray();
        break;
    case cJSON_Object :
        this->json = cJSON_CreateObject();
        break;
    default:
        this->json = 0;
        break;
    }
}

void cJSON_help::parse(const QByteArray &d)
{
    if(json) {
        cJSON_Delete(json);
        json = 0;
    }
    json = cJSON_Parse(d);
}

QByteArray cJSON_help::jsonValue()
{
    return getcJSONprint(json);
}

QByteArray cJSON_help::jsonValueUnformatted()
{
    return getcJSONprint(json, true);
}

cJSON_help::~cJSON_help()
{
    if(json) {
        qDebug() << "delete json";
        cJSON_Delete(json);
        json = 0;
    }
}

bool cJSON_help::isNull()
{
    return json == 0;
}

void cJSON_help::arrayAppendObject(cJSON *obj)
{
    if(json) {
        cJSON_AddItemToArray(json, obj);
    }
}

cJSON *cJSON_help::getcJSON(cJSON *obj, const char *key)
{
    if(obj == 0) {
        qDebug() << "obj == 0";
        return 0;
    }
    return cJSON_GetObjectItem(obj, key);
}

cJSON *cJSON_help::getcJSON(cJSON_help &obj, const char *key)
{
    return getcJSON(obj.json, key);
}

cJSON *cJSON_help::getcJSON(QSharedPointer<cJSON> obj, const char *key)
{
    return getcJSON(obj.data(), key);
}

QVariant cJSON_help::getcJSONvalue(const cJSON_help &obj, const char *key)
{
    return getcJSONvalue(obj.json, key);
}

QVariant cJSON_help::getcJSONvalue(cJSON *obj, const char *key)
{
    if(obj == 0) {
        qDebug() << "root == 0";
        return QVariant();
    }

    cJSON *item = cJSON_GetObjectItem(obj,key);
    return getItemValue(item);
}

QVariant cJSON_help::getcJSONvalue(const QByteArray &obj, const char *key)
{
    cJSON_help p(obj);
    return getcJSONvalue(p, key);
}

QVariant cJSON_help::getcJSONvalue(QSharedPointer<cJSON> obj, const char *key)
{
    return getcJSONvalue(obj.data(), key);
}

QVariant cJSON_help::getArrayValue(cJSON *array, int item, const char *string)
{
    QVariant ret;
    if(array != 0) {
        cJSON *r = cJSON_GetArrayItem(array, item);
        if(r != 0) {
            ret = cJSON_help::getcJSONvalue(r, string);
        }
    }
    return ret;
}

QVariant cJSON_help::getItemValue(cJSON *item)
{
    return cJSON_path::item_value(item);
}

QVariant cJSON_help::getItemValue(QSharedPointer<cJSON> item)
{
    return getItemValue(item.data());
}

QVariant cJSON_help::getItemValue(QSharedPointer<cJSON> array, int k)
{
    return getItemValue(cJSON_GetArrayItem(array.data(), k));
}

QVariant cJSON_help::getItemValue(cJSON *array, int k)
{
    return getItemValue(cJSON_GetArrayItem(array, k));
}

void cJSON_help::replaceItemValue(cJSON *item, const char *key, const QVariant &data)
{
    if(cJSON_GetObjectItem(item, key)) {
        cJSON_DeleteItemFromObject(item, key);
        _cJSON_AddValueToObject(item, key, data);
    }
}

void cJSON_help::debug(cJSON *json, QString *ret) {
    if(json==NULL) {
        return;
    }

    char* arrayData = cJSON_Print(json);
    QString param=QString(arrayData);
    free(arrayData);
    if(ret) *ret = param;
}

QByteArray cJSON_help::getcJSONprint(cJSON *json, bool unformatted)
{
    if(json==NULL) {
        return "";
    }

    char* arrayData;
    if(unformatted)
        arrayData = cJSON_PrintUnformatted(json);
    else
        arrayData = cJSON_Print(json);

    QByteArray param=QByteArray(arrayData);
    free(arrayData);

    return param;
}

QByteArray cJSON_help::getcJSONprint(cJSON_help &json, bool unformatted)
{
    return getcJSONprint(json.json, unformatted);
}

void cJSON_help::jsonPrint(cJSON *json)
{
    qDebug() << getcJSONprint(json);
}

void cJSON_help::_cJSON_AddItemToObject(cJSON *obj, const char *key, cJSON *item)
{
    cJSON_AddItemToObject(obj, key, item);
}

void cJSON_help::_cJSON_AddItemToObject(cJSON_help &obj, const char *key, cJSON *item)
{
    _cJSON_AddItemToObject(obj.json, key, item);
}

void cJSON_help::_cJSON_AddItemToObject(QSharedPointer<cJSON> obj, const char *key, cJSON *item)
{
    return _cJSON_AddItemToObject(obj.data(), key, item);
}

void cJSON_help::_cJSON_AddItemToArray(cJSON *array, cJSON *item)
{
    cJSON_AddItemToArray(array, item);
}

void cJSON_help::_cJSON_AddItemToArray(cJSON_help &array, cJSON *item)
{
    _cJSON_AddItemToArray(array.json, item);
}

void cJSON_help::_cJSON_AddValueToArray(cJSON *array, const QVariant &data)
{
    cJSON_AddItemToArray(array, _cJSON_CreateValue(data));
}

void cJSON_help::_cJSON_AddValueToArray(cJSON_help &array, const QVariant &data)
{
    _cJSON_AddValueToArray(array.json, data);
}

void cJSON_help::_cJSON_AddValueToObject(cJSON *obj, const char *key, const QVariant &data)
{
    cJSON_AddItemToObject(obj, key, _cJSON_CreateValue(data));
}

void cJSON_help::_cJSON_AddValueToObject(cJSON_help &obj, const char *key, const QVariant &data)
{
    _cJSON_AddValueToObject(obj.json, key, data);
}

cJSON *cJSON_help::_cJSON_CreateValue(const QVariant &data)
{
    if(data.type() == QVariant::DateTime
            || data.type() == QVariant::Date
            || data.type() == QVariant::Time) {
        if(data.isNull()
                || false == data.toDateTime().isValid()
                || data.toString().isEmpty()) {
            return cJSON_CreateNull();
        }
        return cJSON_CreateString(data.toDateTime().toString(dateFormatStr).toLocal8Bit());
    }

    if(data.type() == QVariant::Double ) {
        return cJSON_CreateNumber(data.toDouble());
    }
    if(data.type() == QVariant::Int ) {
        return cJSON_CreateNumber(data.toInt());
    }
    if(data.type() == QVariant::LongLong ) {
        return cJSON_CreateNumber(data.toLongLong());
    }
    if(data.type() == QVariant::Bool) {
        return cJSON_CreateBool(data.toBool());
    }
    if(data.type() == QVariant::Invalid) {
        return cJSON_CreateNull();
    }
    return cJSON_CreateString(data.toString().toLocal8Bit());
}

int cJSON_help::array_count(cJSON *array)
{
    if(array == 0) return 0;
    return cJSON_GetArraySize(array);
}

int cJSON_help::array_count(cJSON_help &array)
{
    if(array.isNull())
        return 0;
    return cJSON_GetArraySize(array.json);
}

int cJSON_help::array_count(QSharedPointer<cJSON> json)
{
    return array_count(json.data());
}

cJSON *cJSON_help::_cJSON_GetArrayItem(cJSON *array, int k)
{
    if(array == 0)
        return 0;
    return cJSON_GetArrayItem(array, k);
}

cJSON *cJSON_help::_cJSON_GetArrayItem(cJSON_help &array, int k)
{
    if(array.isNull())
        return 0;

    return cJSON_GetArrayItem(array.json, k);
}

cJSON *cJSON_help::_cJSON_GetArrayItem(QSharedPointer<cJSON> array, int k)
{
    if(array.isNull())
        return 0;
    return cJSON_GetArrayItem(array.data(), k);
}

QByteArray cJSON_help::mapTocJSONByteArray(const QString &key1, const QVariant &value1)
{
    cJSON_help obj(cJSON_Object);
    _cJSON_AddValueToObject(obj, key1.toLocal8Bit(), value1);

    return getcJSONprint(obj);
}

QByteArray cJSON_help::mapTocJSONByteArray(const QString &key1, const QVariant &value1, const QString &key2, const QVariant &value2)
{
    cJSON_help obj(cJSON_Object);
    _cJSON_AddValueToObject(obj, key1.toLocal8Bit(), value1);
    _cJSON_AddValueToObject(obj, key2.toLocal8Bit(), value2);

    return getcJSONprint(obj);
}

QByteArray cJSON_help::mapTocJSONByteArray(const ldsVariantMap &map, bool unformatted)
{
    cJSON_help obj(cJSON_Object);
    QMapIterator<QString, QVariant> mapitr(map.d);
    while(mapitr.hasNext()) {
        mapitr.next();
        _cJSON_AddValueToObject(obj, mapitr.key().toLocal8Bit(), mapitr.value());
    }

    return getcJSONprint(obj, unformatted);
}

cJSON *cJSON_help::mapTocJSON(const ldsVariantMap &map)
{
    cJSON *obj = cJSON_CreateObject();
    QMapIterator<QString, QVariant> mapitr(map.d);
    while(mapitr.hasNext()) {
        mapitr.next();
        _cJSON_AddValueToObject(obj, mapitr.key().toLocal8Bit(), mapitr.value());
    }

    return obj;
}

QSharedPointer<cJSON> cJSON_help::createSharedPtr(const QByteArray &byte)
{
    return createSharedPtr(cJSON_Parse(byte));
}

QSharedPointer<cJSON> cJSON_help::createSharedPtr(cJSON *json)
{
    return QSharedPointer<cJSON>(json, cJSON_help::deleteJSON);
}

void cJSON_help::deleteJSON(cJSON *json)
{
    qDebug() << __FUNCTION__;
    cJSON_Delete(json);
}

//////////////////////////
QVariant cJSON_path::obj_value(cJSON *json, const char *string)
{
    if(0 == json) {
        qDebug() << "json == 0";
        return QVariant();
    }
    cJSON *item = cJSON_GetObjectItem(json, string);
    return item_value(item);
}

QVariant cJSON_path::item_value(cJSON *item)
{
    if(0 == item) {
        qDebug() << "item == 0";
        return QVariant();
    }

    if(cJSON_NULL == item->type) {
        return QVariant();
    }

    if(cJSON_String == item->type) {
        return item->valuestring;
    }

    if(cJSON_Number == item->type) {
        if(item->valuedouble != double(item->valueint)) {
            return item->valuedouble;
        }
        return item->valueint;
    }

    char *string = cJSON_Print(item);
    QByteArray body(string);
    if(string) {
        delete string;
        string = 0;
    }
    return body;
}

cJSON_path::cJSON_path()
{
    json = 0;
}

cJSON_path::cJSON_path(int type)
{
    json = 0;
    if(type == cJSON_Object)
        json = cJSON_CreateObject();
    if(type == cJSON_Array)
        json = cJSON_CreateArray();
}

cJSON_path::cJSON_path(const QByteArray &byte)
{
    json = cJSON_Parse(byte);
}

cJSON_path::~cJSON_path()
{
    if(0 != json) {
        cJSON_Delete(json);
        json = 0;
    }
}

bool cJSON_path::parse(const QByteArray &byte)
{
    if(0 != json) {
        cJSON_Delete(json);
        json = 0;
    }
    //
    json = cJSON_Parse(byte);
    //
    return json != 0;
}

bool cJSON_path::isValid()
{
    return json != 0;
}

bool cJSON_path::isNull()
{
    return json == 0;
}

QVariant cJSON_path::obj_value(const char *string)
{
    return obj_value(json, string);
}

cJSON *cJSON_path::path_root()
{
    return json;
}

cJSON *cJSON_path::path_json(const QString &path, cJSON *from)
{
    //    {data[0{Keys
    cJSON *rjson = 0 ;
    if(from == 0)
        rjson = json;
    else
        rjson = from;
    //
    for(int k = 0; k < path.count(); k ++) {
        if(0 == rjson) {
            qDebug() << "rjson == 0";
            break;
        }
        QString arg = path.mid(k, 1);
        if("{" == arg) {
            QString item_str;
            for(k++; k < path.count(); k ++) {
                if(QString("{[").contains(path.mid(k, 1))) {
                    k --;
                    break;
                } else {
                    item_str += path.mid(k, 1);
                }
            }
            rjson = cJSON_GetObjectItem(rjson, item_str.toLocal8Bit());
        } else if("[" == arg) {
            QString item_str;
            for(k++; k < path.count(); k ++) {
                if(QString("]").contains(path.mid(k, 1))) {
                    break;
                } else if(QString("{[").contains(path.mid(k, 1))) {
                    k --;
                    break;
                } else {
                    item_str += path.mid(k, 1);
                }
            }
            rjson = cJSON_GetArrayItem(rjson, item_str.toInt());
        } else {
            rjson = cJSON_GetObjectItem(rjson, path.mid(k).toLocal8Bit());
        }
    }

    return rjson;
}

QVariant cJSON_path::path_value(const QString &path, cJSON *from)
{
    //string double obj arr
    return item_value(path_json(path, from));
}

int cJSON_path::array_count(const QString &path, cJSON *from)
{
    cJSON *rjson = path_json(path, from);
    if(0 == rjson)
        return 0;
    //string double obj arr
    if(cJSON_Array != rjson->type)
        return 0;

    return cJSON_GetArraySize(rjson);
}

QVariant cJSON_path::array_value(const QString &path, int index, cJSON *from)
{
    //string double obj arr
    return item_value(path_json(path.arg(index), from));
}

QByteArray cJSON_path::toByteArray(bool unformatted)
{
    if(json == 0)
        return "";
    char* arrayData = 0;
    if(unformatted)
        arrayData = cJSON_PrintUnformatted(json);
    else
        arrayData = cJSON_Print(json);

    QByteArray param=QByteArray(arrayData);
    free(arrayData);

    return param;
}

void cJSON_path::print()
{
    qDebug() << toByteArray().data();
}

void cJSON_path::replaceItemValue(const QString &path, const char *key, const QVariant &data, cJSON *from)
{
    cJSON *item = path_json(path, from);
    if(item == 0)  {
        qDebug() << "item = 0";
        return;
    }
    cJSON_help::replaceItemValue(item, key, data);
}

QByteArray cJSON_path::replaceItemValue(const QByteArray &json, const QString &path, const char *key, const QVariant &data, cJSON *from)
{
    /*
     * {"row_begin":1}
     * replaceItemValue("", "row_begin", row_begin);
*/
    cJSON_path p(json);
    if(p.isValid()) {
        p.replaceItemValue(path, key, data, from);
    }
    qDebug() << p.toByteArray(true);
    return p.toByteArray(true);
}

void cJSON_path::deleteArray(const QString &array, int which)
{
    cJSON* j = path_json(array);
    if(j != 0) {
        cJSON_DeleteItemFromArray(j, which);
    }
}

void cJSON_path::select_array(const QString &path, int select_k)
{
    _select_path = path;
    this->select_k = select_k;
}

bool cJSON_path::select_previous()
{
    select_k --;
    return select_k >= 0 && select_k < array_count(_select_path);
}

bool cJSON_path::select_next()
{
    select_k ++;
    return select_k >= 0 && select_k < array_count(_select_path);
}

bool cJSON_path::select_has_next()
{
    int k = select_k + 1;
    return k >= 0 && k < array_count(_select_path);
}

bool cJSON_path::select_seek(int select_k)
{
    this->select_k = select_k;
    int k = select_k;
    return k >= 0 && k < array_count(_select_path);
}

QVariant cJSON_path::select_value(const char *key)
{
    return path_value(select_path(key));

//    if(strcmp(key, "") == 0)
//        return array_value(_select_path + "[%1", select_k);
//    return array_value(_select_path + "[%1{" + key, select_k);
}

QString cJSON_path::select_path(const char *key)
{
    if(strcmp(key, "") == 0)
        return (_select_path + "[%1").arg( select_k);
    return (_select_path + "[%1{" + key).arg(select_k);
}
