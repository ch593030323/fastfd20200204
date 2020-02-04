#include "printer_label.h"
#include <QtDebug>

QstandarditemmodelManager printer_label::class_name_model;
int printer_label::line_char_height = -1;

printer_label::printer_label()
{
}

printer_label::~printer_label()
{

}

int printer_label::value_mm_2_dpi200(double value)
{
    return valueTranslate(value, mm, DPI200, dots, DPI200);
}

const QMap<QString, QVariant> &printer_label::get_label_print_map_ref()
{
    static QMap<QString, QVariant> map_label_pint;
    if(map_label_pint.isEmpty()) {
        for(int row = 0, rowcount = printer_label::class_name_model.mCreator()->rowCount();
            row < rowcount ;row++) {
            QByteArray typeName = printer_label::class_name_model.mCreator()->index(row, 0).data().toByteArray();
            int typeId = QMetaType::type(typeName);
            if (typeId != 0) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
                printer_label *p =  static_cast<printer_label *>(QMetaType::create(typeId));
#else
                printer_label *p =  static_cast<printer_label *>(QMetaType::construct(typeId));
#endif
                if(p) {
                    printer_label_auto_destroy d(p);
                    map_label_pint.insert(typeName, p->name());
                }
            }
        }
    }
    return map_label_pint;
}

QStringList printer_label::getLabelPrinterNameList()
{
    QStringList list;
    for(int row = 0; row < printer_label::class_name_model.mCreator()->rowCount(); row ++) {
        QByteArray printer_label_type = printer_label::class_name_model.mCreator()->item(row)->data(Qt::EditRole).toByteArray();
        int typeId = QMetaType::type(printer_label_type);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        printer_label *p =  static_cast<printer_label *>(QMetaType::create(typeId));
#else
        printer_label *p =  static_cast<printer_label *>(QMetaType::construct(typeId));
#endif
        if(p) {
            printer_label_auto_destroy d(p);
            foreach(const QString &ptype, p->support_printer_type()) {
                list.append(ptype);
            }
        }
    }
    return list;
}

printer_label *printer_label::get_print_class_by_printer_name(const QString &printer_name)
{
    for(int row = 0; row < printer_label::class_name_model.mCreator()->rowCount(); row ++) {
        QByteArray printer_label_type = printer_label::class_name_model.mCreator()->item(row)->data(Qt::EditRole).toByteArray();

        int typeId = QMetaType::type(printer_label_type);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        printer_label *p =  static_cast<printer_label *>(QMetaType::create(typeId));
#else
        printer_label *p =  static_cast<printer_label *>(QMetaType::construct(typeId));
#endif
        if(p) {
            if(p->support_printer_type().contains(printer_name)) {
                return p;
            }
            printer_label_auto_destroy d(p);
        }
    }
    return 0;
}

double printer_label::valueTranslate(double value, unitType value_unit, dpiType value_dpi, unitType result_unit, dpiType result_dpi)
{
    double f1 = 1.0;
    if(value_unit == mm) { //to [dots 200dpi]
        f1 = 8.0;
    } else  if(value_unit == dots && value_dpi == DPI200) {
        f1 = 1.0;
    } else  if(value_unit == dots && value_dpi == DPI300) {
        f1 = 2.0 / 3.0;
    }

    double f2 = 1.0;
    if(result_unit == mm) {//[dots 200dpi] to [unit rate]
        f2 = 1.0 / 8.0;
    } else if(result_unit == dots && result_dpi == DPI200) {
        f2 = 1.0;
    } else if(result_unit == dots && result_dpi == DPI300) {
        f2 = 3.0 / 2.0;
    }
    return f1 * f2 * value;
}

void printer_label::set_line_char_height(int line_char_height)
{
    printer_label::line_char_height = line_char_height;
}

int printer_label::get_line_char_height()
{
    return line_char_height;
}


printer_label_auto_destroy::printer_label_auto_destroy(printer_label *printer)
{
    this->printer = printer;
}

printer_label_auto_destroy::~printer_label_auto_destroy()
{
    delete printer;
}
