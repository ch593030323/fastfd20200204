#ifndef EN_HARDID_MAX_H
#define EN_HARDID_MAX_H

#include <QDialog>

namespace Ui {
class en_hardid_max;
}

class en_hardid_max : public QDialog
{
    Q_OBJECT

public:
    explicit en_hardid_max(QWidget *parent = 0);
    ~en_hardid_max();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_exit_clicked();

private:
    void togenerate2();
private:
    Ui::en_hardid_max *ui;
    QString MESSAGE_TITLE_VOID;
};

#endif // EN_HARDID_MAX_H
