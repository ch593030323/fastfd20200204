#include "lds_tableview_delegate_report.h"
#include "w_rpt_report_frame.h"
#include "lds.h"


QString lds_tableview_delegate_report::paint_transfer(const QVariant &dd) const
{
    QString hkey =  dd.toString();
    QString hvalue = lds::reportModelMapValue(hkey.toLower(), hkey);
    return hvalue;
}
