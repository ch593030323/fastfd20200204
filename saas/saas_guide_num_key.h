#ifndef SAAS_GUIDE_NUM_KEY_H
#define SAAS_GUIDE_NUM_KEY_H

#include <QDialog>

namespace Ui {
class saas_guide_num_key;
}

class saas_guide_num_key : public QDialog
{
    Q_OBJECT

public:
    explicit saas_guide_num_key(const QString &num, QWidget *parent = 0);
    ~saas_guide_num_key();
    QString retNum();
private slots:
    void tonum();
private:
    Ui::saas_guide_num_key *ui;
};

#endif // SAAS_GUIDE_NUM_KEY_H
