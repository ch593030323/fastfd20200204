#include "transparentcenterdialog.h"
#include "saas_pay_detail.h"
#include "lds.h"

transparentCenterDialog::transparentCenterDialog(QDialog *buddy, bool show_title, bool click_empty_reject)
    : lds_roundeddialog_rect_align(buddy, saas_pay_detail::paintLeftTop, show_title, click_empty_reject, QSize(600, 300))
{

}


transparentCenterDialog_void::transparentCenterDialog_void(QDialog *buddy) : QObject(0), buddy(buddy)
{
    buddy->move(0,0);
    lds::setwflagFrameless(buddy);
}

int transparentCenterDialog_void::exec()
{
    return buddy->exec();
}
