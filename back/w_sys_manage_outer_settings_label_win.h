#ifndef W_SYS_MANAGE_OUTER_SETTINGS_LABEL_WIN_H
#define W_SYS_MANAGE_OUTER_SETTINGS_LABEL_WIN_H

#include <QDialog>
#include "label_img_header.h"
namespace Ui {
class w_sys_manage_outer_settings_label_win;
}

class w_sys_manage_outer_settings_label_win : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_settings_label_win(QWidget *parent = 0);
    ~w_sys_manage_outer_settings_label_win();

private slots:
    void took();
    void toexit();
    void totest();
    void topreview();
private:
    bool getPrintTable(label_img_header::LabelTable &table, QString &errstring);
private:
    Ui::w_sys_manage_outer_settings_label_win *ui;
};

#endif // W_SYS_MANAGE_OUTER_SETTINGS_LABEL_WIN_H
