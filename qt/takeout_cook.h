#ifndef TAKEOUT_COOK_H
#define TAKEOUT_COOK_H

#include <QDialog>
#include "public_mainprinter.h"
#include "w_scr_dish.h"
#include "fexpandmain_model_sqltablemodel_data.h"

class frontViewDialog_virtual;
namespace Ui {
class takeout_cook;
}

class takeout_cook : public QDialog
{
    Q_OBJECT
    
public:
    takeout_cook(const fexpandmain_model_sqltablemodel_data *tablemodel, const QString &ch_tableno, QWidget *parent);
    ~takeout_cook();
    Ui::takeout_cook *ui;
public slots:
    void took();
    void tocancel();
    void tosendman();
private:
    const fexpandmain_model_sqltablemodel_data *_tablemodel;
    QString ch_tableno;
};

#endif // TAKEOUT_COOK_H
