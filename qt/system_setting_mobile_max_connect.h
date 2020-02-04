#ifndef SYSTEM_SETTING_MOBILE_MAX_CONNECT_H
#define SYSTEM_SETTING_MOBILE_MAX_CONNECT_H

#include <QDialog>

namespace Ui {
class system_setting_mobile_max_connect;
}

class system_setting_mobile_max_connect : public QDialog
{
    Q_OBJECT

public:
    explicit system_setting_mobile_max_connect(QWidget *parent);
    ~system_setting_mobile_max_connect();
    QString get_id_value(const QString &hardid, const QString &value);
private slots:
    void took();
    void tocancel();
    void tocopy();
    void toread();

private:
    Ui::system_setting_mobile_max_connect *ui;
};

#endif // SYSTEM_SETTING_MOBILE_MAX_CONNECT_H
