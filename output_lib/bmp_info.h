#ifndef BMP_INFO_H
#define BMP_INFO_H

#include <QByteArray>
#include <QFile>
#include <QImage>
#include <QRgb>
#include <QVector>
#define IMAGE_GRAY_DIV "#b1b1b1" //(rgb) 黑白分水岭， f白>, 0黑<=;fefefe
//#define PRINTER_CHAR_WIDTH_PIXELS 11.7
#define PRINTER_CHAR_WIDTH_PIXELS 11.7
#define PRINTER_58MM_WIDTH_CHARS 32
#define PRINTER_58MM_WIDTH_PIXELS (PRINTER_CHAR_WIDTH_PIXELS * PRINTER_58MM_WIDTH_CHARS)
#define PRINTER_80MM_WIDTH_CHARS 48
#define PRINTER_80MM_WIDTH_PIXELS (PRINTER_CHAR_WIDTH_PIXELS * PRINTER_80MM_WIDTH_CHARS)
#define PRINTER_A4_WIDTH_PIXELS 950.0
/**
exmaple:
    QByteArray b0;//生成的打印十六进制指令
    Bmp_info bmpmaker;
    bmpmaker.Pic2printIns(logopath);
    bmpmaker.makeIns(b0);
*/
class Bmp_info
{
private:
    //用于热敏打印机打图片生成
    typedef  QVector <int> Picrow;//每行多少个int
    typedef  QVector <Picrow> Picmatrix;
public:
    Bmp_info():flag(false) {}

    bool Pic2printIns(const QString &filename = QString("0"));
    bool Pic2printIns(const QImage &fileimage);

    void makeIns(QByteArray &in);

    static bool pic2Ins(const QImage &image, QByteArray &ins);
    static bool pic2InsList(const QImage &image, QList<QByteArray> &ins, int height = 240);
private:

    bool isrun() {return flag;}

    int image_w() { return image.width();}

    Picrow & part(int i) {return pic_matrix[i];}

    int  body_size() {return pic_matrix.size();}

    void setimage(const QImage &img) {image = img;}

    void every24_row(int x, int y, int &item);//将一行转成[int]

    bool flag;//获取图片结束

    Picmatrix pic_matrix;//将图片转成对应到[int]/[int]/[int]矩阵

    QImage image;//存放原图

    ////////////////////////////////////////////////////
    /**
    打印顺序为：0000->0001->...->0023->0100->0101->...->0123，0023表示x:00,y:23
    一次打印纵向的24个点：1111 1111 1111 1111 1111 1111即char[3]
    若打印则1，不打印则0
    结构为：
    head:1b 33 [18] 0a  ; [18]:行高
    body head:1b 2a 21 [01 00]   ;[01 00]:宽度
    body: char[3] x n + 0x0a
    */
    static void makeInsByPath(const QString &imagePath, QByteArray &ins);
    static void makeInsByImg(const QImage &image, QByteArray &ins);
    static void fillHead(QByteArray &ins, int &k);
    static void fillRow(const QImage &image, QByteArray &ins, int &k, int y);
    static int blackBit(const QImage &image, int x, int y);

};

#endif // BMP_INFO_H
