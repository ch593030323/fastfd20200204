#ifndef QT_XIANGSHAN_PAY_H
#define QT_XIANGSHAN_PAY_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class qt_XIANGSHAN_pay;
}

class qt_XIANGSHAN_pay : public QDialog
{
    Q_OBJECT

public:
    explicit qt_XIANGSHAN_pay(const QString &PAYMENT, QWidget *parent = 0);
    ~qt_XIANGSHAN_pay();

    bool makeUrl();

public slots:
    void toCheckPaid();
private:
    Ui::qt_XIANGSHAN_pay *ui;
    QString ORDERID;
    QString PAYMENT;
    int pay_check_count;
};

#endif // QT_XIANGSHAN_PAY_H
