#ifndef LDS_DIALOG_INPUT_DOUBLE_H
#define LDS_DIALOG_INPUT_DOUBLE_H

#include <QDialog>
class Ui_lds_dialog_input_dialog_double;

class lds_dialog_input_double : public QDialog
{
    Q_OBJECT
public:
    lds_dialog_input_double(QWidget *parent, bool show_second_line = false);
    ~lds_dialog_input_double();

    Ui_lds_dialog_input_dialog_double *ui;

    void showLabelDesc(const QString &desc);
    void disableDot();
private slots:
    void took();
    void toexit();
    void delay_set_focus();
    void lineselectall();
    //
public:
    void connect_discount_amount_transfer(double total);
private slots:
    void update_discount(double value);
    void udpate_amount(double value);
private:
    double total;
    bool update_discount_running;
    bool udpate_amount_running;
};

#endif // LDS_DIALOG_INPUT_DOUBLE_H
