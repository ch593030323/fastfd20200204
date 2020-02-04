#ifndef W_SYS_MANAGE_BASICDATAIMPORT_H
#define W_SYS_MANAGE_BASICDATAIMPORT_H

#include <QDialog>
#include <QFile>
class Ui_w_sys_manage_basicdataimport_Dialog;

class w_sys_manage_basicdataimport : public QDialog
{
    Q_OBJECT
public:
    static QString FILE_READ_LINE_CODEC(QFile &file, const QByteArray &codec);
public:
    explicit w_sys_manage_basicdataimport(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *e);
protected:
    virtual void reset_tablenames_by_path(const QString &path);
public slots:
    virtual void togetfilepath();
    void tosave();
    void toexit();
    void tomemberimport();
    void tobaseimport();

    void toOtherInfoImport();

    void export_info_progress(const QString &info, int step=-1, int max=-1);
protected:
    Ui_w_sys_manage_basicdataimport_Dialog *ui;

    bool *isend;
    QStringList _tablenames;
    QString _confname;
};

class w_sys_manage_basicdataimport_sub : public w_sys_manage_basicdataimport
{
    Q_OBJECT
public:
    explicit w_sys_manage_basicdataimport_sub(const QStringList &tablenames, const QString &confname, QWidget *parent = 0);
protected:
    virtual void reset_tablenames_by_path(const QString &/*path*/);
private:
    void reset_export_tablenames(const QStringList &tablenames);//导出的文件唯一tablename.csv的方式命名
    void reset_export_confname(const QString &confname);//"w_sys_manage_basicdataexport/path"
};

#endif // W_SYS_MANAGE_BASICDATAIMPORT_H
