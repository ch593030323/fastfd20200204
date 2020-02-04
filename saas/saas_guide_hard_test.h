#ifndef SAAS_GUIDE_HARD_TEST_H
#define SAAS_GUIDE_HARD_TEST_H

#include <QFrame>

namespace Ui {
class saas_guide_hard_test;
}

class saas_guide_hard_test : public QFrame
{
    Q_OBJECT

public:
    explicit saas_guide_hard_test(QWidget *parent = 0);
    ~saas_guide_hard_test();


private slots:
    void tobeep();
    void tocdrawer_default();
    void tocdrawer_test();
    void todisplay_test();
    void toprinter_manage();
    void toprinter_test();
    void toexit();
private:
    Ui::saas_guide_hard_test *ui;
};

#endif // SAAS_GUIDE_HARD_TEST_H
