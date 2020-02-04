#include "lds_query_variant.h"

QVariant qrtVariant::pairListValue(const qrtVariantPairList &pairlist, const QString &field_name)
{
    QVariant r;
    int k = pairListIndex(pairlist, field_name);
    if(k >= 0) {
        r = pairlist[k].second;
    }
    return r;
}

QVariant qrtVariant::pairListTake(qrtVariantPairList &pairlist, const QString &field_name)
{
    QVariant r;
    int k = pairListIndex(pairlist, field_name);
    if(k >= 0){
        r = pairlist[k].second;
        pairlist.removeAt(k);
    }
    return r;
}

int qrtVariant::pairListIndex(const qrtVariantPairList &pairlist, const QString &field_name)
{
    for(int k = 0; k < pairlist.count(); k++) {
        if(pairlist[k].first == field_name)
            return k;
    }
    return -1;
}

void qrtVariant::pairSetValue(qrtVariantPairList &pairlist, const QString &field_name, const QVariant &value)
{
    int k = pairListIndex(pairlist, field_name);
    if(k >= 0) {
        pairlist[k].second = value;
    }
}

void qrtVariant::setType(qrtVariant::qrtVariantType type) const
{
    _type = type;
}

QString qrtVariant::toString() const
{
    return toString(_value);
}

QString qrtVariant::toString(const QVariant &d)
{
    QVariant::Type datatype = d.type();
    if(QVariant::Date == datatype) {
        return d.toDate().toString("yyyy-MM-dd");
    }
    if(QVariant::Time == datatype) {
        return d.toTime().toString("hh:mm:ss");
    }
    if(QVariant::DateTime == datatype) {
        return d.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if(QVariant::Double == datatype) {
        return QString("%1").arg(d.toDouble());
    }
    return d.toString();
}
