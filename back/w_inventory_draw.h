#ifndef W_INVENTORY_DRAW_H
#define W_INVENTORY_DRAW_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include <QDataWidgetMapper>
#include "w_rpt_report_frame.h"

class Ui_w_inventory_draw;
class Ui_w_inventory_input_new;

class w_inventory_draw : public QDialog
{
    Q_OBJECT
public:
    static bool detail_control_transaction(const QString &ch_state, const QString &ch_sheetno, QString *errstring = 0);

public:
    explicit w_inventory_draw(QWidget *parent = 0);
    ~w_inventory_draw();
private slots:
    void torefresh();
    void toexit();
    void toaudit();
    void tobad();
    void tochange();
    void todel();
    void tonew();
private:
    void detail_control(const QString &ch_state);//Y N O
private:
    Ui_w_inventory_draw *ui;
    report_querymodel *querymodel;
    lds_model_sqltablemodel_d cey_sys_operator_d;
};

class w_inventory_draw_new : public QDialog {
    Q_OBJECT

public:
    explicit w_inventory_draw_new(const QString &ch_sheetno, QWidget *parent = 0);
    ~w_inventory_draw_new();
private slots:
    void to_detail_datachange(const QModelIndex &index);
    void tonew();
    void todel();

    void tosave();
    void toexit();
    void torefresh();
    void toaudit();
    void tobad();
private:
    bool saveData();
private:
    void detail_control(const QString &ch_state);//Y N O

private:
    Ui_w_inventory_input_new *ui;
    lds_model_sqltablemodel *tablemodel_detail;
    lds_model_sqltablemodel *tablemodel_master;
    QDataWidgetMapper *mapper;
};

#endif // W_INVENTORY_DRAW_H
