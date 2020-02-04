#ifndef MODIFY_PASSWORD_H
#define MODIFY_PASSWORD_H

#include <QDialog>

namespace Ui {
class modify_password;
}

class modify_password : public QDialog
{
    Q_OBJECT
    
public:
    explicit modify_password(QWidget *parent);
    ~modify_password();
    
private slots:
    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::modify_password *ui;
};

#endif // MODIFY_PASSWORD_H
