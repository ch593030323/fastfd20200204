#ifndef W_SCR_DISH_PAY_WIDGET_SERVICE_MANUAL_H
#define W_SCR_DISH_PAY_WIDGET_SERVICE_MANUAL_H

#include <QDialog>

class Ui_w_scr_dish_pay_widget;
class Ui_w_scr_dish_pay_widget_service_manual;
class w_scr_dish_pay_widget;
namespace Ui {
class w_scr_dish_pay_widget_service_manual;
}

class w_scr_dish_pay_widget_service_manual_obj : public QObject
{
    Q_OBJECT

public:
    w_scr_dish_pay_widget_service_manual_obj(QObject *parent = 0);

    struct PayInterData {
        double num_cost = 0;//in
        bool enable_service = false;//in
        bool enable_lowcost = false;//in
        double num_pay_low = 0;//in
        double num_paid = 0;//in

        double num_service = 0;//out
        double num_lowcost = 0;//out
        double num_realreceive = 0;//out
        double num_money = 0;//out
        double num_blotout = 0;//out
        void print(int line);
        double curService();
        double curLow();
        bool isCorrect();

        void writeUi(Ui_w_scr_dish_pay_widget *ui);
        void readUi(Ui_w_scr_dish_pay_widget *ui, w_scr_dish_pay_widget *obj);
    };
    bool setService(bool enable, double service);
    bool setService(double service);
    bool setMoney(double money);
    void print();
    void writeUi(Ui_w_scr_dish_pay_widget *ui);
    void writeUi(Ui::w_scr_dish_pay_widget_service_manual *ui);
    void readUi(Ui_w_scr_dish_pay_widget *ui, w_scr_dish_pay_widget *obj);
    bool isServiceEnable();
private:
    bool updateMoney(PayInterData &d, double money);
    bool updateService(PayInterData &d, bool enable, double service);
    PayInterData d;
};

class w_scr_dish_pay_widget_service_manual : public QDialog
{
    Q_OBJECT

public:
    explicit w_scr_dish_pay_widget_service_manual(QWidget *parent = 0);
    ~w_scr_dish_pay_widget_service_manual();

    void writeUi(Ui_w_scr_dish_pay_widget *ui);
    void readUi(Ui_w_scr_dish_pay_widget *ui, w_scr_dish_pay_widget *obj);

private slots:
    void took();
    void tocancel();

    void toservicechange(double value);
    void topaychange(double value);
private:
    Ui::w_scr_dish_pay_widget_service_manual *ui;
    w_scr_dish_pay_widget_service_manual_obj obj;

};

#endif // W_SCR_DISH_PAY_WIDGET_SERVICE_MANUAL_H
