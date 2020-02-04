#ifndef PUBLIC_ODBCCONF_H
#define PUBLIC_ODBCCONF_H

#include <QObject>
#include <QVariant>
#include <QSettings>

class public_odbcconf{
public:
    explicit public_odbcconf(const QString &odbcpath);
    QVariant value(const QString & key, const QVariant & defaultValue = QVariant())const;
    void setValue(const QString & key, const QVariant & value );
private:
    QString path;
};

#endif // PUBLIC_ODBCCONF_H
