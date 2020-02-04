#ifndef LABELPRINT_GOODINFO_DIALOG_H
#define LABELPRINT_GOODINFO_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class labelprint_goodinfo_dialog;
}

class labelprint_goodinfo_dialog : public QWidget
{
    Q_OBJECT

public:
    explicit labelprint_goodinfo_dialog(QWidget *parent = 0);
    ~labelprint_goodinfo_dialog();


public slots:
    void tocommvalue(int column);
    void togoodremove();
    void printmodel_has_newrow(const QModelIndex &parent, int first, int last);
    void tosave();
    void toprint();
signals:
    void signal_add();//当点击新增时，需要接受这个信号，对model进行添加
    void signal_print();
private slots:
    void on_pushButton_switch_mode_clicked();

    void on_pushButton_allcontrol_clicked();

    void on_pushButton_clearsection_clicked();

    void on_pushButton_print_project_self_clicked();

private:
    Ui::labelprint_goodinfo_dialog *ui;
    int switch_mode;
public:
    QStandardItemModel *printmodel;
    QStringList printmodel_defh_List;
    QStringList printmodel_selfh_List;
    QStringList printmodel_selfh_conttent_List;
    QStandardItemModel *inputallgoods_model;
    bool isfinishinput;

};

#endif // LABELPRINT_GOODINFO_DIALOG_H
