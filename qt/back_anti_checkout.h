#ifndef BACK_ANTI_CHECKOUT_H
#define BACK_ANTI_CHECKOUT_H

#include <QDialog>
#include <QStandardItemModel>
//反结账查询
namespace Ui {
class back_anti_checkout;
}

class back_anti_checkout : public QDialog
{
    Q_OBJECT
    
public:
    explicit back_anti_checkout(QWidget *parent);
    ~back_anti_checkout();
    void init_control();
private slots:
    void on_pushButton_12_clicked();
    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_10_clicked();//查询

    void on_pushButton_11_clicked();

private:
    Ui::back_anti_checkout *ui;
    QStandardItemModel *model_top;
    QStandardItemModel *model_left;
    QStandardItemModel *model_right;
};

#endif // BACK_ANTI_CHECKOUT_H
