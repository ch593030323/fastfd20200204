#include "lds_getdishavailable.h"
#include <QProcess>

lds_getdishAvailable::lds_getdishAvailable()
{
}

int lds_getdishAvailable::availableSpaceMb(const QString &path)
{
    //du -sh path查看一个文件夹的size
    QProcess df;
    df.start("df", QStringList() <<"-h"<<path);
    if (!df.waitForStarted())
        return 0;

    if (!df.waitForFinished())
        return 0;

    df.readLine();
    QString res( df.readLine() );
    QStringList data = res.split(' ', QString::SkipEmptyParts);
    QString free = data.value(3).toUpper();
    if(free.endsWith("M")) {
        free.chop(1);
    }
    if(free.endsWith("G")) {
        free.chop(1);
        free = QString::number(free.toFloat() * 1024);
    }
    return (int)free.toFloat();
}
