#ifndef LDS_QUERY_VARIANT_H
#define LDS_QUERY_VARIANT_H

#include <QDateTime>
#include <QVariant>

class lds_query_variant
{
public:
    lds_query_variant() {}
};


class qrtVariant;
#define qrtEqual(key, value)		QString("%1 = '%2'").arg(key).arg(QString("%1").arg(value).replace("'", "\\'"))
#define qrtFunc(value)  qrtVariant(value, qrtVariant::FUNC)
#define yyyyMMddhhmmss "yyyy-MM-dd hh:mm:ss"
#define yyyyMMdd "yyyy-MM-dd"

typedef QPair<QString, qrtVariant> qrtVariantPair;
typedef QList<qrtVariantPair> qrtVariantPairList;
typedef QPair<QString, QString> QStringPair;
typedef QList<QStringPair> QStringPairList;

class qrtVariant {
public:
    static QVariant pairListValue(const qrtVariantPairList&pairlist, const QString &field_name);
    static QVariant pairListTake(qrtVariantPairList &pairlist, const QString &field_name);
    static int pairListIndex(const qrtVariantPairList &pairlist, const QString &field_name);
    static void pairSetValue(qrtVariantPairList &pairlist, const QString &field_name, const QVariant &value);
public:
    enum qrtVariantType{
        NONE = 0,
        FUNC = 1,
        VAL = 2,
    };

    inline qrtVariant(int i, qrtVariantType type = VAL):_value(i),_type(type) {}
    inline qrtVariant(uint ui, qrtVariantType type = VAL):_value(ui),_type(type) {}
    inline qrtVariant(qlonglong ll, qrtVariantType type = VAL):_value(ll),_type(type) {}
    inline qrtVariant(qulonglong ull, qrtVariantType type = VAL):_value(ull),_type(type) {}
    //Qt 下 bindValue(name, true)数据库会插入'true';
    inline qrtVariant(bool b, qrtVariantType type = VAL):_value(b ? 1 : 0),_type(type) {}
    inline qrtVariant(double d, qrtVariantType type = VAL):_value(d),_type(type) {}
    inline qrtVariant(float f, qrtVariantType type = VAL):_value(f),_type(type) {}

#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN qrtVariant(const char *str, qrtVariantType type = VAL):_value(str),_type(type) {}
#endif
    inline qrtVariant(const QByteArray &bytearray, qrtVariantType type = VAL):_value(bytearray),_type(type) {}
    inline qrtVariant(const QBitArray &bitarray, qrtVariantType type = VAL):_value(bitarray),_type(type) {}
    inline qrtVariant(const QString &string, qrtVariantType type = VAL):_value(string),_type(type) {}
    inline qrtVariant(const QDate &date, qrtVariantType type = VAL):_value(date),_type(type) {}
    inline qrtVariant(const QTime &time, qrtVariantType type = VAL):_value(time),_type(type) {}
    inline qrtVariant(const QDateTime &datetime, qrtVariantType type = VAL):_value(datetime),_type(type) {}

    //Qt 下 bindValue(name, true)数据库会插入'true';
    inline qrtVariant(const QVariant &val, qrtVariantType type = VAL):_value(val.type() == QVariant::Bool ? (val.toBool() ? 1 : 0) : val),_type(type) {}

    inline operator QVariant () const{ return _value;}//对于debug和qvarint d = qrtVariant() 起作用
    inline const QVariant & getValue() const{ return _value;}
    inline qrtVariantType getType() const {return _type;}

    void setType(qrtVariantType type) const;
    QString toString()const;
    static QString toString(const QVariant &d);
protected:
    QVariant _value;
    mutable qrtVariantType _type;
};

#endif // LDS_QUERY_VARIANT_H
