#ifndef W_SYS_MANAGE_CLOUDSYNC_CLOUD2_H
#define W_SYS_MANAGE_CLOUDSYNC_CLOUD2_H

#include <QDialog>
#include <QVariant>

namespace Ui {
class w_sys_manage_cloudsync_cloud2;
}

class w_sys_manage_cloudsync_cloud2 : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_cloudsync_cloud2(QWidget *parent = 0);
    ~w_sys_manage_cloudsync_cloud2();


    static void exec_keepdata_byconf(QWidget *parent);
private slots:
    void to_data_upload();
    void to_data_download();
    void to_data_updownload();
    void to_flow_upload();
    void to_timecheck();
    void to_exit();
    void to_saas_update();
    void tochangeHost();

private:
    static void data_updown(QWidget *parent);
    void AA_verify();
    void AA_saas_update();
private:
    Ui::w_sys_manage_cloudsync_cloud2 *ui;
};

#endif // W_SYS_MANAGE_CLOUDSYNC_CLOUD2_H
