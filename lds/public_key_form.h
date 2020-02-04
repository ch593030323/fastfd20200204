#ifndef PUBLIC_KEY_FORM_H
#define PUBLIC_KEY_FORM_H

#include <QWidget>

namespace Ui {
class public_key_Form;
}

class public_key_Form : public QWidget
{
    Q_OBJECT

public:
    explicit public_key_Form(QWidget *parent = 0);
    ~public_key_Form();

private slots:
    void set_num();
private:
    Ui::public_key_Form *ui;
};

#endif // PUBLIC_KEY_FORM_H
