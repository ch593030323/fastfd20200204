#ifndef SAAS_GUIDE_INIT_SOFT_H
#define SAAS_GUIDE_INIT_SOFT_H

#include <QFrame>

namespace Ui {
class saas_guide_init_soft;
}

class saas_guide_init_soft : public QFrame
{
    Q_OBJECT

public:
    explicit saas_guide_init_soft(QWidget *parent = 0);
    ~saas_guide_init_soft();

    bool MySQL_isConnected();
private slots:
    void on_pushButton_connect_MySQL_clicked();

    void on_pushButton_demo_clicked();

    void on_pushButton_reciept_clicked();

    void on_pushButton_labelprint_clicked();

    void on_pushButton_screen_clicked();

    void on_pushButton_done_clicked();

private:
    Ui::saas_guide_init_soft *ui;
};

#endif // SAAS_GUIDE_INIT_SOFT_H
