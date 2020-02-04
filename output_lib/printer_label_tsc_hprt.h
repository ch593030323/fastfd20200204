#ifndef PRINTER_LABEL_TSC_HPRT_H
#define PRINTER_LABEL_TSC_HPRT_H

#include "printer_label_tsc_ttp_244_plus.h"

class printer_label_tsc_HPRT : public printer_label_tsc_ttp_244_plus
{
public:
    printer_label_tsc_HPRT();

    // from printer_label
    virtual QString name();
    // from printer_label
    virtual  QStringList support_printer_type();
    // from printer_label
    virtual QMap<QString, QString> print_bar_code_map();
    //from printer_label_tsc_ttp_244_plus
    virtual QByteArray lineEnd();
    virtual QByteArray GAP(int vgap);
};

#endif // PRINTER_LABEL_TSC_HPRT_H
