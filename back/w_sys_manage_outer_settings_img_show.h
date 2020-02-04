#ifndef W_SYS_MANAGE_OUTER_SETTINGS_IMG_SHOW_H
#define W_SYS_MANAGE_OUTER_SETTINGS_IMG_SHOW_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class w_sys_manage_outer_settings_img_show;
}

class w_sys_manage_outer_settings_img_show : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_settings_img_show(QWidget *parent = 0);
    ~w_sys_manage_outer_settings_img_show();

    void showPixmap(const QPixmap &pixmap);
private slots:
    void took();
private:
    Ui::w_sys_manage_outer_settings_img_show *ui;
    QLabel *label;
};

#endif // W_SYS_MANAGE_OUTER_SETTINGS_IMG_SHOW_H
