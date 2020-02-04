#ifndef TRANSPARENTCENTERDIALOG_H
#define TRANSPARENTCENTERDIALOG_H

#include "lds_roundeddialog_rect_align.h"

class transparentCenterDialog : public lds_roundeddialog_rect_align
{
public:
    transparentCenterDialog(QDialog *buddy,
                            bool show_title=true, bool click_empty_reject = false);
};

class transparentCenterDialog_void : public QObject {
    Q_OBJECT
public:
    transparentCenterDialog_void(QDialog *buddy);
    int exec();
private:
    QDialog *buddy;
};

#endif // TRANSPARENTCENTERDIALOG_H
