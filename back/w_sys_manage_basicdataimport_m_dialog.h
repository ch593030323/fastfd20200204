#ifndef W_SYS_MANAGE_BASICDATAIMPORT_M_DIALOG_H
#define W_SYS_MANAGE_BASICDATAIMPORT_M_DIALOG_H

#include <QDialog>

namespace Ui {
class w_sys_manage_basicdataimport_m_Dialog;
}

class w_sys_manage_basicdataimport_m_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_basicdataimport_m_Dialog(QWidget *parent = 0);
    ~w_sys_manage_basicdataimport_m_Dialog();

private slots:
    void on_toolButton_clicked();

    void on_checkBox_2_toggled(bool checked);//导入有效期

    void on_pushButton_import_clicked();

    void on_checkBox_3_toggled(bool checked);//导入会员类型编号

    void topreview();
private:
    bool checkNoRepeat(const QString &filepath, const QString &codec, const QString &split, int column, bool skip_first_row, QString &errstring);
private:
    Ui::w_sys_manage_basicdataimport_m_Dialog *ui;
};

#endif // W_SYS_MANAGE_BASICDATAIMPORT_M_DIALOG_H
