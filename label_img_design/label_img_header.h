#ifndef LABEL_IMG_HEADER_H
#define LABEL_IMG_HEADER_H

#include <QObject>
#include <QStringList>
#include <QVariantList>

class label_img_header : public QObject
{
public:
    struct LabelLine{
        QString type;
        qreal x;
        qreal y;
        int font_pixel;

        QString name;
        QString desc;
        QStringList list;
        static LabelLine reslove(const QString &line);

        void print();
        QString toString() const;
    };
    struct LabelTable{
        QString printerName;
        int MMwidth;
        int MMheight;
        QList<LabelLine> content;

        void appendData(const QString &name, const QString &value);
        void clearContent();
        QString toString() const;
    };
public:
    label_img_header(QObject *parent = 0);

    static LabelTable resolveLabel(const QString &text);

    static bool startKey(const QString &line, const QString &key, QString &value);

    static void print(const QString &text);

    static void print(const LabelTable &table);

    static QStringList printerNames();

    static QPixmap pixmapFirst(const LabelTable &table);
};

#endif // LABEL_IMG_HEADER_H
