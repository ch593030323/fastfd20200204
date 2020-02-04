#ifndef FREE_VIEW_H
#define FREE_VIEW_H

#include <QDialog>

namespace Ui {
class free_view;
}

class free_view : public QDialog
{
    Q_OBJECT
    
public:
    explicit free_view(const QString & str, const QString &ch_paymodeno, QWidget *parent);
    ~free_view();
    bool btn_t_f;
    QString oper_id;
    double line2_num;
    QString beizhu;
    QString time;
    void init_control();
    void setinputenabled(bool f);
    QString m;

public slots:
    void on_pushButton_ok_clicked();

    void todelayfocus();

    void tokeepamount();
private slots:
    void on_pushButton_5_clicked();
private:
    Ui::free_view *ui;
    QString num;
    QString ch_paymodeno;
};

#endif // FREE_VIEW_H
