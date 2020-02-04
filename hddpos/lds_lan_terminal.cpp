#include "lds_lan_terminal.h"

lds_lan_terminal::lds_lan_terminal(QObject *parent) :
    QObject(parent)
{
}

bool lds_lan_terminal::regedit(const QString &terminalCode, QString &errstring)
{
    lds_query_db query;
    if(terminalCode.count() == 0) {
        errstring = "terminalCode is empty!";
        return false;
    }

    //联机多台及其根据机器号不重单付款
    query.execSelect(QString(" select vch_terminal from cey_sys_lan_terminal where vch_terminal = '%1' ")
                     .arg(terminalCode));
    if(false == query.next()) {
        query.execSelect(QString(" select max(int_id) from cey_sys_lan_terminal "));
        query.next();
        int int_id = query.recordValue(0).toInt();
        int_id ++;
        if(int_id > 255) {
            errstring = QObject::tr("局域网最多支持255台机器");//255 = FF
            return false;
        }
        if(!query.execInsert("cey_sys_lan_terminal", qrtVariantPairList()
                             << qrtVariantPair("vch_terminal", terminalCode)
                             << qrtVariantPair("int_id", int_id)
                             ) ){
            errstring = query.errorString();
            return false;
        }
    }
    return true;
}
