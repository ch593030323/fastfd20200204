#ifndef PRINTER_LABEL_H
#define PRINTER_LABEL_H

#include "printer_0.h"

class printer_label
{
public:
    enum unitType{
        mm,
        dots,
    };
    enum dpiType{
        DPI200,
        DPI300,
    };

public:
    printer_label();
    virtual ~printer_label();
    virtual QString print_form(const QString &content, double width, double height, double gap, int printcount) = 0;
    virtual QString print_text(const QString &content, double x, double y, const QString &font, int xbigger, int ybigger) = 0;
    virtual QString print_bar_code(const QString &content, double x, double y, double height, const QString &bar_type, int wide_width, int narrow_width) = 0;
    virtual QString print_qr_code(const QString &content, double x, double y) = 0;
    virtual QString name() = 0;

    //所有的key value  都是反的
    virtual QMap<QString, QString> print_bar_code_map() = 0;
    virtual  QMap<QString, QString> print_qr_code_map() = 0;
    virtual  QMap<QString, QString> print_text_map() = 0;
    virtual  QStringList support_printer_type() = 0;
protected:
    virtual double value_translate(double value, unitType value_unit = mm, dpiType value_dpi = DPI200)= 0;
    virtual int value_mm_2_dpi200(double value);
    /////////////////////////////////////
public:
    static const QMap<QString/*print_class_name, name*/, QVariant> &get_label_print_map_ref();

    static QStringList getLabelPrinterNameList();

    static printer_label *get_print_class_by_printer_name(const QString &printer_name);

    /////////////////////////////////////

    static double valueTranslate(double value, unitType value_unit, dpiType value_dpi, unitType result_unit, dpiType result_dpi);

    ////////////////////////////////////
    static QstandarditemmodelManager class_name_model;

    static void set_line_char_height(int line_char_height);
    static int get_line_char_height();
protected:
    static int line_char_height;
};

//注册
template <typename T>
class printer_label_Virtual_Register{
public:
    static void register_class_name_info(QList<QStandardItem *> items)
    {
        printer_label::class_name_model.mCreator()->appendRow(items);
    }

public:
    printer_label_Virtual_Register(QList<QStandardItem *> items)
    {
        register_class_name_info(items);
        QByteArray typeName = items[0]->index().data(Qt::EditRole).toByteArray();

        //Returns a handle to the type called typeName, or 0 if there is no such type.
        int typeId = QMetaType::type(typeName);
        if (0 == typeId)
            qRegisterMetaType<T>(typeName);
    }
};

class printer_label_auto_destroy{
public:
    printer_label_auto_destroy(printer_label *printer);
    ~printer_label_auto_destroy();
    printer_label *printer;
};
#endif // PRINTER_LABEL_H
