#include "lds_roundeddialog.h"
#include "lds.h"

lds_roundeddialog::lds_roundeddialog(QDialog *buddy, bool show_title, bool click_empty_reject)
    : lds_roundeddialog_rect_align(buddy, NULL, show_title, click_empty_reject, QSize(600, 300))
{

}
