#ifndef W_SCR_DISH_GROUP_DIALOG_H
#define W_SCR_DISH_GROUP_DIALOG_H

#include <QDialog>
class Ui_w_scr_dish_group_Dialog;
#include "ftableview_standmodel_sql.h"

class w_scr_dish_group_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_group_dialog(const QString &ch_dishno, QWidget *parent = 0);
    ~w_scr_dish_group_dialog();

    static bool check_group(const QString &ch_dishno);
    void retranslateView();

    Ui_w_scr_dish_group_Dialog *ui;

    QStringList ret_ch_dishno_list;
public slots:
    void took();
    void toexit();

    void toLongPressUpdateDishImage(const QModelIndex &index);
    void toLongPressUpdateDishImage_add();
private slots:
    void torefresh();
private:
    void refresh(const QString &ch_dishno);
    ftableview_standmodel_sql *model_dish;
    QString ch_groupno;
};

#endif // W_SCR_DISH_GROUP_DIALOG_H
