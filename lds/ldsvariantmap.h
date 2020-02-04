#ifndef LDSVARIANTMAP_H
#define LDSVARIANTMAP_H

#include <QVariantMap>

class ldsVariantMap{
public:
    ldsVariantMap() {}
    ldsVariantMap &append(const QString &key, const QVariant &value) {d.insert(key, value); return *this; }
    ldsVariantMap(const QString &k1, const QVariant &v1) {d.insert(k1, v1);}
    ldsVariantMap(const QString &k1, const QVariant &v1, const QString &k2, const QVariant &v2) {d.insert(k1, v1);d.insert(k2, v2);}
    ldsVariantMap(const QString &k1, const QVariant &v1, const QString &k2, const QVariant &v2, const QString &k3, const QVariant &v3) {d.insert(k1, v1);d.insert(k2, v2);d.insert(k3, v3);}
    QVariantMap d;
};

class ldsStringMap{
public:
    ldsStringMap() {}
    ldsStringMap &append(const QString &key, const QString &value) {d.insert(key, value); return *this; }
    QMap<QString, QString> d;
};

#endif // LDSVARIANTMAP_H
