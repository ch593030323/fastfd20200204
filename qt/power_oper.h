#ifndef POWER_OPER_H
#define POWER_OPER_H

#include <QDialog>

namespace Ui {
class power_oper;
}

class power_oper : public QDialog
{
    Q_OBJECT
    
public:
    explicit power_oper(QWidget *parent);
    ~power_oper();
    bool btn_t_f;
    QString operid;
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

private:
    Ui::power_oper *ui;
};

#endif // POWER_OPER_H
