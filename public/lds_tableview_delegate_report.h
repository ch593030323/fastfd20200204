#ifndef LDS_TABLEVIEW_DELEGATE_REPORT_H
#define LDS_TABLEVIEW_DELEGATE_REPORT_H

#include "lds_tableview_delegate_virtual.h"

class lds_tableview_delegate_report :public lds_tableview_delegate_virtual
{
public:
    lds_tableview_delegate_report(QObject *parent = 0)
        : lds_tableview_delegate_virtual(parent) {}

    virtual QString paint_transfer(const QVariant &dd) const ;
};

#endif // LDS_TABLEVIEW_DELEGATE_REPORT_H
