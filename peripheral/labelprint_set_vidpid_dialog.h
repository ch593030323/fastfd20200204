#ifndef LABELPRINT_SET_VIDPID_DIALOG_H
#define LABELPRINT_SET_VIDPID_DIALOG_H

#include <QDialog>
#include "labelprint_set_dialog.h"
#include <QItemDelegate>
class QStandardItemModel;
namespace Ui {
class labelprint_set_vidpid_dialog;
}

class labelprint_set_vidpid_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit labelprint_set_vidpid_dialog(labelprint_set_dialog *parent = 0);
    ~labelprint_set_vidpid_dialog();
    QString ret_value;
private slots:
    void on_pushButton_exit_clicked();

    void on_pushButton_app_clicked();
private:
    Ui::labelprint_set_vidpid_dialog *ui;
    labelprint_set_dialog *_parent;
    QStandardItemModel *model;
};


#endif // LABELPRINT_SET_VIDPID_DIALOG_H
