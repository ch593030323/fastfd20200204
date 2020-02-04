#ifndef W_BT_DISH_QUIT_H
#define W_BT_DISH_QUIT_H

#include <QDialog>
class Ui_w_bt_dish_unitset_Dialog;
class lds_model_sqltablemodel;
class lds_tableView;
class w_bt_dish_quit : public QDialog
{
    Q_OBJECT
public:
    explicit w_bt_dish_quit(QWidget *parent = 0);
private slots:
    void tonew();
    void todel();
    bool tosave();
    void torefresh();
    void toexit();
private:
    Ui_w_bt_dish_unitset_Dialog *ui;
    lds_model_sqltablemodel *tablemodel;
    
};


#endif // W_BT_DISH_QUIT_H
