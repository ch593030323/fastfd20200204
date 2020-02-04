#ifndef PRINTER_NORMAL_H
#define PRINTER_NORMAL_H

#include "printer_0.h"
class Printer_POS;
class Printer_Normal
{
public:
    Printer_Normal();
    virtual void tran(blockIns &bins, int xinye_beep);
    virtual QString name();
    //def 80 增强型打印
    virtual QByteArray init();//1b40
    virtual QByteArray scale(char c);//0x00默认 0x01倍高 0x10倍宽 x11倍高倍宽
    virtual QByteArray align(char c);//0x00居中 0x10左对齐 0x01右对齐
    virtual QByteArray bold(char c);//0x01-0x05 打印并走纸
    virtual QByteArray gonum(char c);//0x01-0x05 打印并走纸

    virtual QByteArray pic_code(const QString &filepath);//
    virtual QByteArray bar_code(const QString &code);//
    virtual QByteArray qr_code(const QString &code);//获取二维码，打印指令

    /////////////////////////////////////
    static QstandarditemmodelManager class_name_model;
    static Printer_Normal *byName(const QString &fname, QString &class_name);
    static QByteArray static_qr_code(const QString &print_class_name, const QString &qr_code);

    /////////////////////////////////////
public:
    enum Normal_INS_TYPE{
        Enhance = 0,
        Standard = 1,
    };
public:
    /**
      i 打印初始化
      a 对齐方式  00【居中】、01【右对齐】、10【左对齐】
      s 倍高倍宽  00【默认】、01【倍高】、10【倍宽】、11【倍高倍宽】
      - 分割线 根据打印机的纸张宽度, 有“-”时，打的second将无效，如<"-", "nihao"> "nihao"将打不出来
      b 加粗 01【加粗】、 00【取消加粗】

      逗号分隔，如<"i,a00,s10,-,b01",  "你好">

*/
//    resolve::Enhance, Printer_Normal_58,32,ins
    static void resolve(int print_instype, QString print_class_name, int paperwidth, int xinye_beep_count,
                        blockIns &ins
                        );
    static const QMap<QString/*print_class_name, name*/, QVariant> &get_normal_print_map_ref();
    static const QMap<QString/*Normal_INS_TYPE*/, QVariant/*name*/> &get_normal_ins_map_ref();
    //
    //解析后，打印，关闭接口
    static bool toprint(int print_instype, QString print_class_name, int paperwidth, int xinye_beep_count,
                        const QString &path,
                        blockIns ins,
                        int times = 1, QString *errstring = 0);
    //打印，关闭接口
    static bool toprint(const QString &path, const blockIns &ins, int times = 1, QString *errstring = 0);
    //打印，不关闭接口
    static bool toprint(QSharedPointer<Printer_POS> printer, const blockIns &ins, int times = 1);

    static void reSort(blockIns &ins);

    static void reTran(blockIns &ins, const QString &printer_class_name, int xinye_beep_count);
private:
    //
    static bool ins_first_has_image(const QByteArray &byte);
};
//注册

template <typename T>
class Printer_Normal_virtual_Register{
public:
    static void register_class_name_info(QList<QStandardItem *> items)
    {
        Printer_Normal::class_name_model.mCreator()->appendRow(items);
    }

public:
    Printer_Normal_virtual_Register(QList<QStandardItem *> items)
    {
        register_class_name_info(items);
        QByteArray typeName = items[0]->index().data(Qt::EditRole).toByteArray();
        int typeId = QMetaType::type(typeName);
        if (typeId == 0)
            qRegisterMetaType<T>(typeName);
    }
};
class  Printer_Normal_auto_destroy{
public:
    Printer_Normal_auto_destroy(Printer_Normal *p) {this->p = p;}
    ~Printer_Normal_auto_destroy() {delete p; p = 0;}
    Printer_Normal *p;
};
#endif // PRINTER_NORMAL_H
