#ifndef QT_XIANGSHAN_PAY_INFO_H
#define QT_XIANGSHAN_PAY_INFO_H

#include <QWidget>

namespace Ui {
class qt_XIANGSHAN_pay_info;
}

class qt_XIANGSHAN_pay_info : public QWidget
{
    Q_OBJECT

public:
    explicit qt_XIANGSHAN_pay_info(QWidget *parent = 0);
    ~qt_XIANGSHAN_pay_info();


    void upatePay(const QString &payName, const QString &payAmount);
private:
    Ui::qt_XIANGSHAN_pay_info *ui;
};

#endif // QT_XIANGSHAN_PAY_INFO_H
