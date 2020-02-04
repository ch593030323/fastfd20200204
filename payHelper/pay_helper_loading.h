#ifndef PAY_HELPER_LOADING_H
#define PAY_HELPER_LOADING_H

#include <QDialog>

namespace Ui {
class pay_helper_loading;
}

class pay_helper_loading : public QDialog
{
    Q_OBJECT
public:
    explicit pay_helper_loading(QWidget *parent, const QString &text);
    ~pay_helper_loading();

    void showdelay();
private:
    Ui::pay_helper_loading *ui;
};

#endif // PAY_HELPER_LOADING_H
