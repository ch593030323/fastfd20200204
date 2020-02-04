#ifndef QT_XIANGSHAN_COPY_FILE_H
#define QT_XIANGSHAN_COPY_FILE_H

#include <QObject>
#include <QtGui>

class qt_XIANGSHAN_copy_file : public QObject
{
    Q_OBJECT
public:
    explicit qt_XIANGSHAN_copy_file(QObject *parent = 0);

signals:
    void information(bool success, const QString &msg);
public slots:
    void copyFile(const QStringList &fileInfoFrom, const QString &dirTo);
    void copyFile(const QString &filepathFrom, const QString &dirTo, const QString &baseNameTo);

};

#endif // QT_XIANGSHAN_COPY_FILE_H
