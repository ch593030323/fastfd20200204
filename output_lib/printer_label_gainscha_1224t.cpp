#include "printer_label_gainscha_1224t.h"
#include "printer_label_gainscha.h"

////注册
static printer_label_Virtual_Register<printer_label_gainscha_1224t>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_gainscha_1224t"));
////


printer_label_gainscha_1224t::printer_label_gainscha_1224t()
{
}

printer_label_gainscha_1224t::~printer_label_gainscha_1224t()
{

}

QString printer_label_gainscha_1224t::name()
{
    return "Gainscha-1224t";
}

QMap<QString, QString> printer_label_gainscha_1224t::print_bar_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map = printer_label_tsc_ttp_244_plus().print_bar_code_map();
    }
    return map;
}

QMap<QString, QString> printer_label_gainscha_1224t::print_qr_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map = printer_label_tsc_ttp_244_plus().print_qr_code_map();
    }
    return map;
}

QMap<QString, QString> printer_label_gainscha_1224t::print_text_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map = printer_label_gainscha().print_text_map();
    }
    return map;
}

QStringList printer_label_gainscha_1224t::support_printer_type()
{
    return QStringList() << "GP-1224T";
}

double printer_label_gainscha_1224t::value_translate(double value, unitType value_unit, dpiType value_dpi)
{
    Q_UNUSED(value_unit)
    Q_UNUSED(value_dpi)
    return value;
}

QString printer_label_gainscha_1224t::drawText(const QString &data, int x, int y, const QString &font, const QSize &size)
{
    return printer_label_tsc_ttp_244_plus::drawText(data, x, y, font, size);
}
