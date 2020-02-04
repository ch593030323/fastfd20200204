#ifndef CJSON_HELP_H
#define CJSON_HELP_H

#include <QVariant>
#include <QObject>
#include "ldsvariantmap.h"
#include <QSharedPointer>

class cJSON;

class cJSON_help : public QObject{
    Q_OBJECT
public:
    cJSON_help();
    cJSON_help(const QByteArray &d);
    cJSON_help(cJSON *json);
    cJSON_help(int type);

    void parse(const QByteArray &d);
    QByteArray jsonValue();
    QByteArray jsonValueUnformatted();
    ~cJSON_help();

    bool isNull();
    void arrayAppendObject(cJSON *obj);
private:
    inline cJSON_help &operator=(const cJSON_help &) { return *this; }

    cJSON *json;

public:
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    static cJSON *getcJSON(cJSON *obj, const char *key);
    static cJSON *getcJSON(cJSON_help &obj, const char *key);
    static cJSON *getcJSON(QSharedPointer<cJSON> obj, const char *key);

    static QVariant getcJSONvalue(const cJSON_help &obj, const char *key);
    static QVariant getcJSONvalue(cJSON *obj, const char *key);
    static QVariant getcJSONvalue(const QByteArray &obj, const char *key);
    static QVariant getcJSONvalue(QSharedPointer<cJSON> obj, const char *key);

    static QVariant getArrayValue(cJSON *array, int item, const char *string);

    static QVariant getItemValue(cJSON *item);
    static QVariant getItemValue(QSharedPointer<cJSON> item);
    static QVariant getItemValue(QSharedPointer<cJSON> array, int k);
    static QVariant getItemValue(cJSON *array, int k);
    static void replaceItemValue(cJSON *item, const char *key, const QVariant &data);

    static void debug(cJSON *json, QString *ret = 0);

    static QByteArray getcJSONprint(cJSON *json, bool unformatted = false);
    static QByteArray getcJSONprint(cJSON_help &json, bool unformatted = false);

    static void jsonPrint(cJSON *json);

    static void _cJSON_AddItemToObject(cJSON *obj, const char *key, cJSON *item);
    static void _cJSON_AddItemToObject(cJSON_help &obj, const char *key, cJSON *item);
    static void _cJSON_AddItemToObject(QSharedPointer<cJSON> obj, const char *key, cJSON *item);

    static void _cJSON_AddItemToArray(cJSON *array,  cJSON *item);
    static void _cJSON_AddItemToArray(cJSON_help &array,  cJSON *item);

    static void _cJSON_AddValueToObject(cJSON *obj, const char *key, const QVariant &data);
    static void _cJSON_AddValueToObject(cJSON_help &obj, const char *key, const QVariant &data);

    static void _cJSON_AddValueToArray(cJSON *array, const QVariant &data);
    static void _cJSON_AddValueToArray(cJSON_help &array, const QVariant &data);

    static cJSON *_cJSON_CreateValue(const QVariant &data);

    static int array_count(cJSON *array);
    static int array_count(cJSON_help &array);
    static int array_count(QSharedPointer<cJSON> json);

    static cJSON *_cJSON_GetArrayItem(cJSON *array, int k);
    static cJSON *_cJSON_GetArrayItem(cJSON_help &array, int k);
    static cJSON *_cJSON_GetArrayItem(QSharedPointer<cJSON> array, int k);

    static QByteArray mapTocJSONByteArray(const QString &key1, const QVariant &value1);
    static QByteArray mapTocJSONByteArray(const QString &key1, const QVariant &value1, const QString &key2, const QVariant &value2);
    static QByteArray mapTocJSONByteArray(const ldsVariantMap &map, bool unformatted = false);
    static cJSON *mapTocJSON(const ldsVariantMap &map);

    static QSharedPointer<cJSON> createSharedPtr(const QByteArray &byte);
    static QSharedPointer<cJSON> createSharedPtr(cJSON *json);
    static void deleteJSON(cJSON *json);
};

class cJSON_path {
public:
    static QVariant obj_value(cJSON *json, const char *string);
    static QVariant item_value(cJSON *item);
public:
    cJSON_path();
    cJSON_path(int type);
    cJSON_path(const QByteArray &byte);
    ~cJSON_path();

    bool parse(const QByteArray &byte);
    bool isValid();
    bool isNull();

    QVariant obj_value(const char *string);

    cJSON* path_root();
    cJSON* path_json(const QString &path, cJSON* from = 0);
    QVariant path_value(const QString &path, cJSON* from = 0);

    int array_count(const QString &path, cJSON* from = 0);
    QVariant array_value(const QString &path, int index, cJSON* from = 0);

    QByteArray toByteArray(bool unformatted = false);
    void print();

    void replaceItemValue(const QString &path, const char *key, const QVariant &data, cJSON* from = 0);
    static QByteArray replaceItemValue(const QByteArray &json, const QString &path, const char *key, const QVariant &data, cJSON* from = 0);

    void deleteArray(const QString &array, int which);
private:
    cJSON *json;

public:
    void select_array(const QString &path, int select_k = -1);
    bool select_previous();
    bool select_next();
    bool select_has_next();
    bool select_seek(int select_k);
    QVariant select_value(const char *key);
    QString select_path(const char *key);
private:
    QString _select_path;
    int select_k;
};
#endif // CJSON_HELP_H
