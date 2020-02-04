#ifndef LDS_SCR_QT_H
#define LDS_SCR_QT_H

#include "lds_query_hddpos.h"
#include <QString>
#include <QComboBox>
#include "lds_dd.h"
#include "lds.h"

class lds_scr_qt
{
public:
    lds_scr_qt();
    static bool material_insert(lds_query_hddpos  &query, const QString &ch_billno, int int_id);

    static bool guqing_update(const QString &ch_dishno, double off/*可以为负数*/);

    static void doublescreensenddataByconf(const lds::DOUBLE_DATA &ddata);
};

#endif // LDS_SCR_QT_H
