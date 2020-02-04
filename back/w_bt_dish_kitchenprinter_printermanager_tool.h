#ifndef W_BT_DISH_KITCHENPRINTER_PRINTERMANAGER_TOOL_H
#define W_BT_DISH_KITCHENPRINTER_PRINTERMANAGER_TOOL_H

#include <QDialog>
class lds_ComboBox;
namespace Ui {
class w_bt_dish_Kitchenprinter_printermanager_tool;
}

class w_bt_dish_Kitchenprinter_printermanager_tool : public QDialog
{
    Q_OBJECT

public:
    explicit w_bt_dish_Kitchenprinter_printermanager_tool(const QString &printport, QWidget *parent = 0);
    ~w_bt_dish_Kitchenprinter_printermanager_tool();

private slots:
    void toapp_beeper();
    void toapp_chinese_mode();
    void toapp_code_page();
    void toexit();
private:
    void towriteHex(QObject *obj);
private:
    Ui::w_bt_dish_Kitchenprinter_printermanager_tool *ui;
    QString printport;
};

#endif // W_BT_DISH_KITCHENPRINTER_PRINTERMANAGER_TOOL_H
