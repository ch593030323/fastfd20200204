#ifndef LABELPRINT_PROJECT_SELF_DIALOG_H
#define LABELPRINT_PROJECT_SELF_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class labelprint_project_self_Dialog;
}

class labelprint_project_self_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit labelprint_project_self_Dialog(QWidget *parent = 0);
    ~labelprint_project_self_Dialog();


private slots:
    void toadd();
    void tosub();
    void took();
    void tocancel();
private:
    Ui::labelprint_project_self_Dialog *ui;
public:
    QStandardItemModel *standmodel;
};

#endif // LABELPRINT_PROJECT_SELF_DIALOG_H
