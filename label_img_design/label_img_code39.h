#ifndef LABEL_IMG_CODE39_H
#define LABEL_IMG_CODE39_H

#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QFont>
#include <QBitmap>
#include <QPainter>

class label_img_code39
{
public:
    label_img_code39(QString BarCode, QString SavePath);

private:
    QString barcode;

private:
    QString path;

private:
    QMap<QString,QString> map;

public:
    void start();

private:
    void draw(QString BinaryNum,QString barcode);

private:
    QString CodeBinary(QString barcode);

private:
    void InitMap();

public:
    void reset(QString BarCode, QString SavePath);
};

#endif // LABEL_IMG_CODE39_H
