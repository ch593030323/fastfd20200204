#ifndef LDS_LAN_TERMINAL_H
#define LDS_LAN_TERMINAL_H

#include <QObject>
#include "lds_query_db.h"

class lds_lan_terminal : public QObject
{
    Q_OBJECT
public:
    explicit lds_lan_terminal(QObject *parent = 0);

    static bool regedit(const QString &terminalCode, QString &errstring);

};

#endif // LDS_LAN_TERMINAL_H
