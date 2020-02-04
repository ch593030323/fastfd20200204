#ifndef BARSCALE_NET_SET_DIALOG_H
#define BARSCALE_NET_SET_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "barscale_main_dialog.h"

namespace Ui {
class barscale_net_set_dialog;
}

class barscale_net_set_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit barscale_net_set_dialog(barscale_main_dialog *parent);
    ~barscale_net_set_dialog();
    barscale_main_dialog *_parent;
public slots:
    void on_pushButton_clicked();//新增

    void on_pushButton_2_clicked();//修改

    void on_pushButton_3_clicked();//删除

    void on_pushButton_4_clicked();//退出
public:
    void load_setting();
    void save_setting();
public:
    Ui::barscale_net_set_dialog *ui;
    QStandardItemModel *model;
};

//修改
class Ui_barscale_net_set_change_dialog;
class barscale_net_set_change:public QDialog{
    Q_OBJECT
public:
    barscale_net_set_change(QWidget *parent = 0);
    Ui_barscale_net_set_change_dialog *ui;
public slots:
    virtual void toexit();
    virtual void took();
    void update_port(const QString&scaletype);
};

//新增
class barscale_net_set_new:public barscale_net_set_change{
    Q_OBJECT
public:
    barscale_net_set_new(barscale_net_set_dialog *parent = 0);
private slots:
    virtual void toexit();
    virtual void took();
};
#endif // BARSCALE_NET_SET_DIALOG_H
