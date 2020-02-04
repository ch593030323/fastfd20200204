#ifndef W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_H
#define W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_H

#include <QFrame>
#include <QDialog>
class QLineEdit;
class QComboBox;
class QLabel;
class lds_ComboBox;

class w_bt_dish_kitchenprinter_printerset_port : public QObject
{
    Q_OBJECT

public:
    explicit w_bt_dish_kitchenprinter_printerset_port(QObject *parent = 0);
    ~w_bt_dish_kitchenprinter_printerset_port();

    void setPrinterport(const QString &port);
    QString printerport();
    void refreshPort();
    void set_com_usb_enable(bool e);

    void onlyNetPort();

    void addControl(QFrame *frame_print);
public slots:
    void toporttype(int index);
private slots:
    void toselectvidpidlist();
private:
    QLineEdit * ui_lineEdit_ip;
    QComboBox * ui_comboBox_port_tty;
    QComboBox * ui_comboBox_port_usb;
    QLabel * ui_label_baud;
    QLabel * ui_label_flowcontrol;
    QLabel * ui_label_type;
    QLabel * ui_label_port;
    QPushButton * ui_pushButton_usb_vp;
    QLineEdit * ui_lineEdit_port;
    QComboBox * ui_comboBox_baud;
    lds_ComboBox * ui_comboBox_porttype;
    lds_ComboBox * ui_comboBox_flowcontrol;

    QFrame *frame_print;
};

#endif // W_BT_DISH_KITCHENPRINTER_PRINTERSET_PORT_H
