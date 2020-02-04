#ifndef W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_DIALOG_H
#define W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_DIALOG_H

#include <QDialog>
#include <QFrame>
class w_bt_dish_kitchenprinter_printerset_port;
namespace Ui {
class w_bt_dish_kitchenprinter_printerset_port_dialog;
}

class w_bt_dish_kitchenprinter_printerset_port_help : public QFrame
{
    Q_OBJECT
public:
    explicit w_bt_dish_kitchenprinter_printerset_port_help(QWidget *parent = 0);

};

class w_bt_dish_kitchenprinter_printerset_port_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchenprinter_printerset_port_dialog(QWidget *parent = 0);
    ~w_bt_dish_kitchenprinter_printerset_port_dialog();

    QString printerPort();
    void setPrinterport(const QString &path);

private slots:
    void took();
    void tocancel();
    void totest();

private:
    Ui::w_bt_dish_kitchenprinter_printerset_port_dialog *ui;
    w_bt_dish_kitchenprinter_printerset_port *print_help;
    QString printer_port;
};

#endif // W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_DIALOG_H
