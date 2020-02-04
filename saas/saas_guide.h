#ifndef SAAS_GUIDE_H
#define SAAS_GUIDE_H

#include <QDialog>
#include <QItemDelegate>
#include <QStandardItemModel>
#include "lds_messagebox.h"
#include "cjson_help.h"
#include "w_sys_manage_cloudsync.h"

class QState;
class QLineEdit;

class saas_guide : public QDialog
{
    Q_OBJECT

public:
    explicit saas_guide(QWidget *parent = 0);
    ~saas_guide();

    static httpDataAutoDel getTerminal(int mesc = 10 * 1000);
};



#endif // SAAS_GUIDE_H
