#include "bmp_info.h"
#include <QtDebug>
#include <QColor>

bool Bmp_info::Pic2printIns(const QString &filename)
{
    return Pic2printIns(QImage(filename));
}

bool Bmp_info::Pic2printIns(const QImage &fileimage)
{
    //  50 * 50
    /*  50 * 24
        50 * 24
        50 * 24 */
    image = fileimage;
    if(image.isNull()) {
        qDebug() <<"image.isNull";
        return false;
    }

    /*获得合适高度*/
    int h = image.height();
    int row_sum = h%24 > 0 ? h/24 + 1 : h/24;

    while(row_sum >= 1 && row_sum--) pic_matrix.push_back(Picrow(image.width()));

    for(int y = 0; y < pic_matrix.size(); y++) {
        for(int x = 0; x < pic_matrix[0].size(); x++) {
            every24_row(x, y,pic_matrix[y][x]);//item会自动清零
        }//item
    }//row
    flag = true;
    return true;
}

void Bmp_info::every24_row(int x, int y, int &item)// (x,y) 24 -> item
{
    item = 0;
    for(int i = 0; i < 24; i++)
    {
        if(y*24 + i < image.height()) {//越界
            QRgb m = image.pixel(x,y*24 + i);
            if(m <= QColor(IMAGE_GRAY_DIV).rgb())//黑色
                item |= 0x01;
            //白色默认
        }
        if(i >= 23)break;//若是到底则跳出，避免最后一位多0的情况
        item <<= 1;
    }
}

void Bmp_info::makeInsByPath(const QString &imagePath, QByteArray &ins)
{
    makeInsByImg(QImage(imagePath), ins);
}

void Bmp_info::makeInsByImg(const QImage &image, QByteArray &ins)
{
    int w = image.width();
    int h = image.height();
    h = h / 0x18 * 0x18 + 0x18 * (h % 0x18 > 0 ? 1 : 0);

    int size = 4 + (5 + 3 * w + 1) * (h / 0x18);
    ins.fill(0, size);

    int k = 0;
    //fillHead
    fillHead(ins, k);
    //fillRow
    for(int row = 0; row < h; row += 0x18) {
        fillRow(image, ins, k, row);
    }
    ins.chop(1);
}

void Bmp_info::fillHead(QByteArray &ins, int &k)
{
    //1b 33 18 0a
    //设置行高
    ins[k ++] = 0x1b;
    ins[k ++] = 0x33;
    ins[k ++] = 0x18;
    ins[k ++] = 0x0a;
}

void Bmp_info::fillRow(const QImage &image, QByteArray &ins, int &k, int y)
{
    //1b 2a 21 01 00
    //39---ＣＲＴ三倍密度
    ins[k ++] = 0x1b;
    ins[k ++] = 0x2a;
    ins[k ++] = 0x21;
    ins[k ++] = image.width() & 0xff;
    ins[k ++] = image.width() >> 8 & 0xff;

    //body 278 241
    for(int x = 0; x < image.width(); x ++) {
        for(int y24 = y + 24, y1 = y; y1 < y24; y1 += 8, k ++) {
            for(int y8 = y1 + 8, y0 = y1; y0 < y8; y0 ++) {
                ins[k] = 2 * ins[k] + blackBit(image, x, y0);
            }
        }
    }
    //tail
    ins[k ++] = 0x0a;
}

int Bmp_info::blackBit(const QImage &image, int x, int y)
{
    if(x < image.width()
            && y < image.height()
            && image.pixel(x,  y) <= QColor(IMAGE_GRAY_DIV).rgb()) {
        return 1;
    }
    return 0;
}

void Bmp_info::makeIns(QByteArray &in)
{
    /*处理图片*/
    if(!isrun())return;//pic load error

    /*获得开始打印的表头*/
    QByteArray start_head;
    //    start_head.append("1d 50 00 c8 1b 33 18");//设置行高
    start_head.append("1b 33 18");//设置行高

    /*获得每段打印的表头*/
    QByteArray p_head;
    /*0a 1b 2a  [21]
                32---单密度
                33---双密度
                38---图形二
                39---ＣＲＴ三倍密度
                40---六倍密度,每个像素有空格
        */

    p_head.append("0a 1b 2a 21 ");//        39---ＣＲＴ三倍密度
    {
        int w = image_w();
        /*像素宽度,最大255 * 256 个像素*/
        p_head.append(QString().sprintf("%02x ",(int)(w & 0x00ff)));
        p_head.append(QString().sprintf("%02x\n",(int)(w & 0xff00)>>8));
    }

    /*对bmp图片转换成打印内容*/
    QByteArray &printer_ins = in;
    printer_ins.clear();
    QByteArray part_body;
    printer_ins.append(start_head);
    for(int i = 0; i < body_size(); i++) {
        part_body.clear();
        part_body.append(p_head);
        /*获得part数量*/
        const Picrow &part =pic_matrix[i];
        /*将part转换成qbytearray*/
        for(int t = 0; t < part.size(); t++) {
            part_body.append(QString().sprintf("%02x",(part[t] & 0xff0000)>>16));
            part_body.append(QString().sprintf("%02x",(part[t] & 0x00ff00)>>8));
            part_body.append(QString().sprintf("%02x",(part[t] & 0x0000ff)));
        }
        printer_ins.append(part_body);
        //        printer_ins.append("0a");
    }
}

//bool Bmp_info::pic2Ins(const QImage &image, QByteArray &ins)
//{
//    makeInsByImg(image, ins);
//    return true;
//}

bool Bmp_info::pic2InsList(const QImage &image, QList<QByteArray> &ins, int height)
{
    for(int y = 0; y < image.height(); y += height) {
        ins.append(QByteArray());
        makeInsByImg(image.copy(0, y, image.width(), qMin(image.height() - y, height) ), ins.last());
    }
    return true;
}

bool Bmp_info::pic2Ins(const QImage &image, QByteArray &ins)
{
    makeInsByImg(image, ins);
    return true;
}

//bool Bmp_info::pic2InsList(const QImage &image, QList<QByteArray> &ins, int height)
//{
//    for(int y = 0; y < image.height(); y += height) {
//        Bmp_info info;
//        ins.append(QByteArray());
//        if(false == info.Pic2printIns(image.copy(0, y, image.width(), qMin(image.height() - y, height) ))) {
//            return false;
//        }
//        info.makeIns(ins.last());
//    }
//    return true;
//}
