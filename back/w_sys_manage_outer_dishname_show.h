#ifndef W_SYS_MANAGE_OUTER_DISHNAME_SHOW_H
#define W_SYS_MANAGE_OUTER_DISHNAME_SHOW_H

#include <QDialog>
#include <QFrame>
class lds_CheckBox;

namespace Ui {
class w_sys_manage_outer_dishname_show;
}

class w_sys_manage_outer_dishname_show : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_dishname_show(QWidget *parent = 0);
    ~w_sys_manage_outer_dishname_show();
private slots:
    void took();
    void tocancel();
private:
    bool savedata(bool save = true);
private:
    Ui::w_sys_manage_outer_dishname_show *ui;

    class NCombobox : public QFrame {
    public:
        NCombobox(QWidget *parent = 0);
        void setValue(const QString &value);
        QString value();
    private:
        lds_CheckBox *b1;
        lds_CheckBox *b2;
    };
};

#endif // W_SYS_MANAGE_OUTER_DISHNAME_SHOW_H
