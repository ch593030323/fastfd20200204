#ifndef W_SYS_MANAGE_BASICDATAEXPORT_H
#define W_SYS_MANAGE_BASICDATAEXPORT_H

#include <QDialog>
class Ui_w_sys_manage_basicdataimport_Dialog;
class w_sys_manage_basicdataexport : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_basicdataexport(QWidget *parent = 0);
    void reset_export_tablenames(const QStringList &tablenames);//导出的文件唯一tablename.csv的方式命名
    void reset_export_confname(const QString &confname);//"w_sys_manage_basicdataexport/path"

protected:
    void closeEvent(QCloseEvent *e);
public slots:
    void togetfilepath();
    void toumount_upan();
    void tosave();
    void toexit();

    void export_info_progress(const QString &info, int step=-1, int max=-1);
private:
    Ui_w_sys_manage_basicdataimport_Dialog *ui;
    QStringList _tablenames;
    QString _confname;
};

#endif // W_SYS_MANAGE_BASICDATAEXPORT_H
