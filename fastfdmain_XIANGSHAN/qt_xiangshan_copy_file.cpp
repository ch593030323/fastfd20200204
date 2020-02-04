#include "qt_xiangshan_copy_file.h"

qt_XIANGSHAN_copy_file::qt_XIANGSHAN_copy_file(QObject *parent) :
    QObject(parent)
{
}

void qt_XIANGSHAN_copy_file::copyFile(const QStringList &fileInfoFrom, const QString &dirTo)
{
    int copyFileNum = 0;
    foreach(const QString &filepath, fileInfoFrom) {
        QFileInfo f(filepath);

//        rm 2018-03-07\ 16\:41\:27屏幕截图
        system(QString("rm %1/%2.*").arg(dirTo).arg(f.baseName().replace(" ", "\\ ").replace(":", "\\:").replace("(", "\\(").replace(")", "\\)").replace("*", "\\*")).toLocal8Bit());

        QFile file(filepath);
        if(!file.copy(dirTo + "/" +f.fileName())) {
            emit information(false, file.errorString());
            return;
        }
        copyFileNum ++;
    }
    emit information(true, QObject::tr("操作成功") + ":" + QString::number(copyFileNum));
}

void qt_XIANGSHAN_copy_file::copyFile(const QString &filepathFrom, const QString &dirTo, const QString &baseNameTo)
{
    int copyFileNum = 0;
    QFileInfo f(filepathFrom);
    QString baseName = baseNameTo;
    system(QString("rm %1/%2.*").arg(dirTo).arg(baseName.replace(" ", "\\ ").replace(":", "\\:").replace("(", "\\(").replace(")", "\\)").replace("*", "\\*").replace("(", "\\(").replace(")", "\\)")).toLocal8Bit());

    QFile file(filepathFrom);
    if(!file.copy(dirTo + "/" +baseNameTo + "." + f.suffix())) {
        emit information(false, file.errorString());
        return;
    }
    copyFileNum ++;
    emit information(true, QObject::tr("操作成功") + ":" + QString::number(copyFileNum));
}
