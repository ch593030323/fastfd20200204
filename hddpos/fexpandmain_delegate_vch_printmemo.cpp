#include "fexpandmain_delegate_vch_printmemo.h"
#include "vch_print_memodata.h"


fexpandmain_delegate_vch_printmemo::fexpandmain_delegate_vch_printmemo(QObject *parent)
    : lds_tableview_delegate_virtual(parent)
{
    model_cache = new lds_model_sqltablemodel(this);
}

QString fexpandmain_delegate_vch_printmemo::paint_transfer(const QVariant &dd, const QString &field_name) const
{
    QString key = dd.toString();
    if(false == b_paint_transfer)
        return key;

    return vch_print_memoData(dd.toString()).jsonToString();
}
