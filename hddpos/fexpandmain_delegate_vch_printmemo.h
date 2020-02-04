#ifndef FEXPANDMAIN_DELEGATE_VCH_PRINTMEMO_H
#define FEXPANDMAIN_DELEGATE_VCH_PRINTMEMO_H

#include "lds_tableview_delegate_virtual.h"
#include "lds_model_sqltablemodel.h"

class fexpandmain_delegate_vch_printmemo : public lds_tableview_delegate_virtual
{
public:
    fexpandmain_delegate_vch_printmemo(QObject *parent);
    virtual QString paint_transfer(const QVariant &dd, const QString &field_name) const ;
    lds_model_sqltablemodel *model_cache;
};

#endif // FEXPANDMAIN_DELEGATE_VCH_PRINTMEMO_H
