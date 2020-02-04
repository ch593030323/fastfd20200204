#ifndef BARSCALE_NUM_INPUT_H
#define BARSCALE_NUM_INPUT_H

#include <QDialog>

namespace Ui {
class barscale_num_input;
}

class barscale_num_input : public QDialog
{
    Q_OBJECT

public:
    explicit barscale_num_input(QWidget *parent = 0);
    ~barscale_num_input();

    QString retvalue();
    int mode;
private slots:
    void set_num();
    void tomodelswitch();
private:
    void updatem(int m);
private:
    Ui::barscale_num_input *ui;
};

#endif // BARSCALE_NUM_INPUT_H
