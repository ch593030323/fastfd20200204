#ifndef PAY_HELPER_MAIN_H
#define PAY_HELPER_MAIN_H

#include <QDialog>

namespace Ui {
class pay_helper_main;
}

class pay_helper_main : public QDialog
{
    Q_OBJECT

public:
    explicit pay_helper_main(QWidget *parent = 0);
    ~pay_helper_main();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::pay_helper_main *ui;
    bool ispressed;
    bool ismoved;
    QPoint pressPos; //移动的距离
};

#endif // PAY_HELPER_MAIN_H
