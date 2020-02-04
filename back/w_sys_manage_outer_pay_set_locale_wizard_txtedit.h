#ifndef W_SYS_MANAGE_OUTER_PAY_SET_LOCALE_WIZARD_TXTEDIT_H
#define W_SYS_MANAGE_OUTER_PAY_SET_LOCALE_WIZARD_TXTEDIT_H

#include <QDialog>
class QPlainTextEdit;
namespace Ui {
class w_sys_manage_outer_pay_set_locale_wizard_txtedit;
}

class w_sys_manage_outer_pay_set_locale_wizard_txtedit : public QDialog
{
    Q_OBJECT
public:
    static QStringList country_receipt_pay_path_list(const QString &country);
public:
    explicit w_sys_manage_outer_pay_set_locale_wizard_txtedit(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_locale_wizard_txtedit();

    bool open(const QString &country, QString &errstring);
signals:
    void signal_receipt_print(const QString &country, const QByteArray &byte);
private slots:
    void took();
    void tocancel();
    void toprint();
    void toimport();
    void toexport();
    void toinit();
    void topreview();
    void tologo();

private:
    bool saveData();
private:
    Ui::w_sys_manage_outer_pay_set_locale_wizard_txtedit *ui;
    QString country;
};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_LOCALE_WIZARD_TXTEDIT_H
