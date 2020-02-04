#ifndef READCARD_MWRF32_H
#define READCARD_MWRF32_H

#include <QObject>
#include "mwrf32.h"

//密码模式为0，扇区为1

class ReadCard_mwrf32 : public QObject
{
    Q_OBJECT
public:
    explicit ReadCard_mwrf32(QObject *parent = 0);

    bool init();

    bool writeSqr(int block_id, const QString &part);//将part 写到卡内block_id块
    bool writecard(const QString &part0,const  QString &part1,const  QString &part2);
    bool readcard(QString &part0,QString &part1,QString &part2);

    bool setDefPwd(const QString &defPwd/*ffffffffffff*/);
    bool pwdChange(const QString &oldPwd/*ffffffffffff*/,
                   const QString &newPwd/*ffffffffffff*/);
    bool cardisready();
    void beep();
    QString lastError();
signals:

private:
    static HANDLE handle;
    QString pwd;

    int m_pwdMode;
    int m_sec;        //扇区
};

#endif // READCARD_MWRF32_H
