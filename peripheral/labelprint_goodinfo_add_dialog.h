#ifndef LABELPRINT_GOODINFO_ADD_DIALOG_H
#define LABELPRINT_GOODINFO_ADD_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>

class Ui_labelprint_goodinfo_add_dialog;
class labelprint_goodinfo_add_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit labelprint_goodinfo_add_dialog(QWidget *parent = 0);
    Ui_labelprint_goodinfo_add_dialog *ui;
    QStandardItemModel *model;
private slots:
    void clearselected();

};

#endif // LABELPRINT_GOODINFO_ADD_DIALOG_H
