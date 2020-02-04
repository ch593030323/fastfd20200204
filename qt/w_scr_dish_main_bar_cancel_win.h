#ifndef W_SCR_DISH_MAIN_BAR_CANCEL_WIN_H
#define W_SCR_DISH_MAIN_BAR_CANCEL_WIN_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"

namespace Ui {
class w_scr_dish_main_bar_cancel_win;
}

class w_scr_dish_main_bar_cancel_win : public QDialog
{
    Q_OBJECT

protected:
    explicit w_scr_dish_main_bar_cancel_win(const QString &vch_handle, QWidget *parent = 0);
    ~w_scr_dish_main_bar_cancel_win();
private slots:
    void took();
    void tocancel();
    void todel();
    void toadd();
    void torefresh();

    void totakewine();
    void toprint();
private:
    bool saveData();
protected:
    Ui::w_scr_dish_main_bar_cancel_win *ui;

    class sqlTableModel : public lds_model_sqltablemodel{
    public:
        sqlTableModel(QObject *parent = 0) ;
        virtual ~sqlTableModel();
        QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    };
    sqlTableModel *tablemodel;
    QString vch_handle;
};

class w_scr_dish_main_bar_cancel_win_pend : public w_scr_dish_main_bar_cancel_win {
    Q_OBJECT
public:
    w_scr_dish_main_bar_cancel_win_pend(const QString &vch_handle, const QString &vch_guest, QWidget *parent = 0) ;
};

class w_scr_dish_main_bar_cancel_win_take : public w_scr_dish_main_bar_cancel_win {
    Q_OBJECT
public:
    w_scr_dish_main_bar_cancel_win_take(const QString &vch_handle, QWidget *parent = 0) ;
};

#endif // W_SCR_DISH_MAIN_BAR_CANCEL_WIN_H
