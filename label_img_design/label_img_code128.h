#ifndef LABEL_IMG_CODE128_H
#define LABEL_IMG_CODE128_H

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QFont>
#include <QBitmap>
#include <QPainter>

class label_img_code128
{
public:
    label_img_code128(QString text,QString p);

private:
    QString barcode;

private:
    QString path;

public:
    void start();

private:
    static QString BuildBarString(QString tex);

private:
    static QString AddSimpleTag(int CodeIndex);

private:
    void KiCode128C(QString BarString);

public:
    void reset(QString text,QString p);

    static void paintBar(QPainter *painter, const QString &bar);
};
#endif // LABEL_IMG_CODE128_H
