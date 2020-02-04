#include "label_img_header.h"
#include <QBuffer>
#include "QJsonDocument.h"
#include "QJsonArray.h"
#include "QJsonObject.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include "QJsonValueRef.h"
#endif
#include <QtDebug>
#include "label_img_design_view.h"
#include <QPrinterInfo>

label_img_header::label_img_header(QObject *parent) : QObject(parent)
{

}

label_img_header::LabelTable label_img_header::resolveLabel(const QString &text)
{
    LabelTable d;
    QBuffer buf;
    buf.setData(text.toLocal8Bit());
    buf.open(QBuffer::ReadOnly);
    while(!buf.atEnd()) {
        QString line = buf.readLine().trimmed();
        QString value;
        if(startKey(line, "printerName:", value)) {
            d.printerName = value;
        } else if(startKey(line, "size:", value)) {
            d.MMwidth = value.split("x").value(0).toInt();
            d.MMheight = value.split("x").value(1).toInt();
        } else {
            d.content << LabelLine::reslove(line);
        }
    }
    return d;
}

bool label_img_header::startKey(const QString &line, const QString &key, QString &value)
{
    if(line.startsWith(key)) {
        value = line.mid(key.length());
        return true;
    }
    return false;
}

void label_img_header::print(const QString &text)
{
    print(resolveLabel(text));
}

void label_img_header::print(const LabelTable &table)
{
    label_img_design_view d;
    d.setTable(table);
    d.printAll();
}

QStringList label_img_header::printerNames()
{
    QStringList list;
#ifdef Q_OS_WIN
    list = QPrinterInfo::availablePrinterNames();
#endif
    return list;
}

QPixmap label_img_header::pixmapFirst(const label_img_header::LabelTable &table)
{
    label_img_design_view d;
    d.setTable(table);
    return d.pixmapFirst();
}

label_img_header::LabelLine label_img_header::LabelLine::reslove(const QString &line)
{
    LabelLine d;
    QJsonDocument json = QJsonDocument::fromJson(line.toLocal8Bit());
    if(!json.isObject()) {
        return d;
    }
    QVariantMap map = json.object().toVariantMap();
    d.type = map.value("type").toString();
    d.x = map.value("x").toDouble();
    d.y = map.value("y").toDouble();
    d.font_pixel = map.value("font_pixel").toInt();
    d.name = map.value("name").toString();
    d.desc = map.value("desc").toString();
    d.list = map.value("list").toStringList();
    return d;
}

void label_img_header::LabelLine::print()
{
    qDebug()
            << "type:" << type
            << "x:" << x
            << "y:" << y
            << "font_pixel:" << font_pixel

            << "name:" << name
            << "list:" << list
               ;
}

QString label_img_header::LabelLine::toString() const
{
    QVariantMap map;
    map.insert("type", type);
    map.insert("x", x);
    map.insert("y", y);
    map.insert("font_pixel", font_pixel);
    map.insert("name", name);
    map.insert("desc", desc);
    map.insert("list", list);
    return QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
}

void label_img_header::LabelTable::appendData(const QString &name, const QString &value)
{
    for(int k = 0; k < content.count(); k ++) {
        if(content[k].name == name) {
            content[k].list << value;
        }
    }
}

void label_img_header::LabelTable::clearContent()
{
    for(int k = 0; k < content.count(); k ++)
        content[k].list.clear();
}

QString label_img_header::LabelTable::toString() const
{
    QString d =
            QString() +
            "printerName:" + printerName + "\n" +
            "size:" + QString::number(MMwidth) + "x" + QString::number(MMheight) + "\n";
    for(int k = 0; k < content.count(); k ++) {
        d += content[k].toString() + "\n";
    }
    return d;
}
