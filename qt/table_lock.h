#ifndef TABLE_LOCK_H
#define TABLE_LOCK_H

#include <QDialog>
#include <QStandardItemModel>
class lds_model_sqltablemodel;
namespace Ui {
class table_lock;
}

class table_lock : public QDialog
{
    Q_OBJECT
    
public:
    explicit table_lock(QWidget *parent);
    ~table_lock();
private slots:
    void on_pushButton_7_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_allunlock_clicked();

    void toselect();

private:
    bool saveData();
private:
    Ui::table_lock *ui;
    lds_model_sqltablemodel *tablemodel;
};

#endif // TABLE_LOCK_H
