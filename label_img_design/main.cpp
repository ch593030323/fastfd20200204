#include "label_img_header_dialog.h"
#include <QApplication>
#include <QTextCodec>
#include "label_img_header.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //设置编号
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif

    label_img_header_dialog w;
    w.show();


    QString text =
            "printerName:Gprinter GP-3150TN\n"
            "size:40x30\n"
            "text,110,320,-90,3.1,item_name:[\"apple\",\"peach\"]\n"
            "bar,10,290,-90,2.3,item_code:[\"11001\",\"1002\"]\n"
            "text,100,180,-90,2,item_price:[\"PRICE12.36\",\"RMB3.60\"]\n"
            "text,140,178,-90,2,item_num:[\"NUM3.50\",\"NUM6.00\"]\n"
            "text,180,320,-90,2,serial_no:[\"No.1002\",\"No.1202\"]\n"
            "text,180,190,-90,2,bill_time:[\"2019-12-10\",\"2019-12-11\"]\n"
            "text,220,320,-90,2.8,shop_name:[\"EPI VERISON\",\"EPI VERISON\"]\n";
//    label_img_header::print(text);

    return a.exec();
}


/*


*/
