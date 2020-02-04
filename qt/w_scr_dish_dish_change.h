#ifndef W_SCR_DISH_DISH_CHANGE_H
#define W_SCR_DISH_DISH_CHANGE_H

#include <QDialog>
#include <QSqlQueryModel>


namespace Ui {
class w_scr_dish_dish_change;
}

class w_scr_dish_dish_change : public QDialog
{
    Q_OBJECT
public:
    explicit w_scr_dish_dish_change(const QString &ch_dishno_cur, float num_price_cur, const QString &ch_suitno, QWidget *parent);
    ~w_scr_dish_dish_change();

    QString ch_dishno_new;
    float rate;
    bool suitreplace;//可替换商品
private slots:
    void tocancel();
    void took();
    void totableclicked(int row);

    void toselectequal();
    void toSelectAll();
    void tosuitchange();
    void toselect();
private:
    Ui::w_scr_dish_dish_change *ui;
    QSqlQueryModel *querymodel;
    float _num_price_cur;
    QString _ch_dishno_cur;
    QString wtitle;
    QString _ch_suitno;
    QString _sql;
    /////////
protected:
    bool eventFilter(QObject *o, QEvent *e);
};

#endif // W_SCR_DISH_DISH_CHANGE_H
