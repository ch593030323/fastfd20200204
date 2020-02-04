#include "printer_label_tsc_hprt.h"
////注册
static printer_label_Virtual_Register<printer_label_tsc_HPRT>
r(QList<QStandardItem*>() << new QStandardItem("printer_label_tsc_HPRT"));

printer_label_tsc_HPRT::printer_label_tsc_HPRT()
{
}

QString printer_label_tsc_HPRT::name()
{
    return "HPRT";
}

QStringList printer_label_tsc_HPRT::support_printer_type()
{
        return QStringList() << "HPRT";
}

QMap<QString, QString> printer_label_tsc_HPRT::print_bar_code_map()
{
    static QMap<QString, QString> map;
    if(map.isEmpty()) {
        map.insert("Code 93", "93");
        map.insert("EAN 13", "EAN13");
        map.insert("Interleaved 2 of 5", "25");
        map.insert("Interleaved 2 of 5 with check digits", "25C");
        map.insert("Code 39 full ASCII for TSPL2 printers", "39");
        map.insert("Code 128, switching code subset A, B, C automatically", "128");
    }
    return map;
}

QByteArray printer_label_tsc_HPRT::lineEnd()
{
    return "\r\n";
}

QByteArray printer_label_tsc_HPRT::GAP(int vgap)
{
    return "";
}
