#ifndef W_INVENTORY_INPUT_NEW_H
#define W_INVENTORY_INPUT_NEW_H

#include <QDialog>
#include "lds_model_sqltablemodel.h"
#include <QDataWidgetMapper>
namespace Ui {
class w_inventory_input_new;
}

class w_inventory_input_new : public QDialog
{
    Q_OBJECT

public:
    explicit w_inventory_input_new(const QString &ch_sheetType, const QString &ch_sheetno, QWidget *parent = 0);
    ~w_inventory_input_new();

private slots:
    void to_detail_datachange(const QModelIndex &index);
    void tonew();
    void todel();

    void tosave();
    void toexit();
    void torefresh();
    void toaudit();
    void tobad();

    void toUpdateTax(int index);
private:
    bool saveData();
private:
    void detail_control(const QString &ch_state);//Y N O
private:
    Ui::w_inventory_input_new *ui;

    lds_model_sqltablemodel *tablemodel_detail;
    lds_model_sqltablemodel *tablemodel_master;
    QDataWidgetMapper *mapper;
    QString ch_sheetType;
};

#endif // W_INVENTORY_INPUT_NEW_H
