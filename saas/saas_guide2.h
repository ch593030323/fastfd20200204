#ifndef SAAS_GUIDE2_H
#define SAAS_GUIDE2_H

#include <QDialog>
#include "saas_guide_pay.h"
#include <QState>

namespace Ui {
class saas_guide2;
}

class saas_guide2 : public QDialog
{
    Q_OBJECT

public:
    saas_guide2(QWidget *w,  QWidget *parent = 0);
    ~saas_guide2();

    void only_show_exit();
    void hide_next_btn();
signals:
    void signal_left();
    void signal_right();

private slots:
    void s2Assigned();

    void toNext();
    void toPre();
    void toHardTest();
    void toExit();
private:
    void init(QWidget *w);
    void enableBtn(bool f);
    Ui::saas_guide2 *ui;
    QState *s1, *s2, *s3;

    QPoint pos_cur;
    QPoint pos_right;
    QPoint pos_left;
    lds_bool_default_false b_only_show_exit;
};

#endif // SAAS_GUIDE2_H
