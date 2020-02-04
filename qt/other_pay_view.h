#ifndef OTHER_PAY_VIEW_H
#define OTHER_PAY_VIEW_H

#include <QDialog>

namespace Ui {
class other_pay_view;
}

class other_pay_view : public QDialog
{
    Q_OBJECT
    
public:
    explicit other_pay_view(const QString& str,QWidget *parent);
    ~other_pay_view();
    void init_control();
    bool btn_t_f;
    int curindex_id;
    QString beizhu;
    QStringList payfanshi;
    QStringList payname;
private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::other_pay_view *ui;
    QString type;
};

#endif // OTHER_PAY_VIEW_H
