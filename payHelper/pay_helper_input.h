#ifndef PAY_HELPER_INPUT_H
#define PAY_HELPER_INPUT_H

#include <QDialog>

namespace Ui {
class pay_helper_input;
}

class pay_helper_input : public QDialog
{
    Q_OBJECT

public:
    explicit pay_helper_input(QWidget *parent = 0);
    ~pay_helper_input();

private slots:
    void took();
    void tocancel();
    void toset();
    void toexit();
    void tonum();
    void tobackspace();

private:
    void updateOkText();
    void pay();

private:
    Ui::pay_helper_input *ui;
};

#endif // PAY_HELPER_INPUT_H
