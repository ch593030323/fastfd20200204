#ifndef LDS_ROUNDEDDIALOG_H
#define LDS_ROUNDEDDIALOG_H

#include "lds_roundeddialog_rect_align.h"

class lds_roundeddialog : public lds_roundeddialog_rect_align
{
    Q_OBJECT
public:
    lds_roundeddialog(QDialog *buddy,
                            bool show_title=true, bool click_empty_reject = false);

};

#endif // LDS_ROUNDEDDIALOG_H
