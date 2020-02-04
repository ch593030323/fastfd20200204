#include "public_odbcconf.h"

#include <QFile>

public_odbcconf::public_odbcconf(const QString &odbcpath):path(odbcpath)
{
}

QVariant public_odbcconf::value(const QString &key, const QVariant &defaultValue) const
{
    return QSettings(path, QSettings::IniFormat).
            value(key, defaultValue);
}

void public_odbcconf::setValue(const QString &key, const QVariant &value)
{
    QFile f(path);
    QSettings c(path, QSettings::NativeFormat);

    c.setValue(key, value);
    c.sync();

    f.open(QFile::ReadOnly);
    QByteArray b = f.readAll();
    f.close();

    //若是第一次修改，则需要替换下面数据
    QByteArray text = QByteArray::fromPercentEncoding(b);
    text.replace(" = ", "=");
    text.replace("=", " = ");

    f.open(QFile::WriteOnly);
    f.write(text);
    f.close();
}
