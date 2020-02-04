#ifndef LINSHI_COOK_H
#define LINSHI_COOK_H

#include <QDialog>
#include <QModelIndex>

#include <QPointer>
#include <QGridLayout>

namespace Ui {
class linshi_cook;
}
class ftableview_standmodel_sql;
class linshi_cook : public QDialog
{
    Q_OBJECT
    
public:
    explicit linshi_cook(QWidget *parent);
    ~linshi_cook();
    QString ch_dishno;
private slots:
    void tonew();
    void took();
    void tocancel();
    void tokitchen_plan();
    void tochange();

private:
    bool dataChecked();
    Ui::linshi_cook *ui;
    ftableview_standmodel_sql *model_dish;
};

#endif // LINSHI_COOK_H
