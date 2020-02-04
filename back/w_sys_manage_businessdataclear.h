#ifndef W_SYS_MANAGE_BUSINESSDATAclear_H
#define W_SYS_MANAGE_BUSINESSDATAclear_H

#include <QDialog>
class Ui_w_sys_manage_businessdataclear_Dialog;
class w_sys_manage_businessdataclear : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_businessdataclear(QWidget *parent = 0);
protected:
    void closeEvent(QCloseEvent *e);
private slots:
    void togetfilepath();
    void tosave();
    void toexit();
    void toclearinventorynum();//判断是否清除库存数量

public:
    void export_info_progress(const QString &info, int step=-1);
private:
    Ui_w_sys_manage_businessdataclear_Dialog *ui;

    bool *isend;
};

#endif // W_SYS_MANAGE_BUSINESSDATAclear_H
