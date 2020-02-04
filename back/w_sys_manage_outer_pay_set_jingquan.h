#ifndef W_SYS_MANAGE_OUTER_PAY_SET_JINGQUAN_H
#define W_SYS_MANAGE_OUTER_PAY_SET_JINGQUAN_H

#include <QDialog>
#include "w_scr_dish_pay_widget.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_model_mapwidget_vertical.h"
#include "lds_messagebox.h"
#include <QEventLoop>

namespace Ui {
class w_sys_manage_outer_pay_set_jingquan;
}

/*
 * 现金的默认编号为1
*/
class w_sys_manage_outer_pay_set_jingquan : public lds_model_mapwidget_vertical
{
    Q_OBJECT
public:
    static bool postSale(lds_messagebox_loading_show *loading, const QString &ch_payno, w_scr_dish_pay_widget::operatModes flag, QString &errstring);

    static bool postIGood(lds_messagebox_loading_show *loading,  QString &errstring);

private:
    static QString paymodenoFilter(const QString &ch_paymodeno);

    static bool mapHasEmptyValue(const QString &title,const ldsVariantMap &map, QString &errstring);

    static bool httpPost(const QString &title,
                         const ldsVariantMap &map,
                         QNetworkAccessManager &m,
                         QNetworkRequest &request,
                         QEventLoop &loop,
                         QString &errstring);
public:
    explicit w_sys_manage_outer_pay_set_jingquan( QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_jingquan();

private slots:
    void took();
    void tocancel();
    void todataupload();
private:
    Ui::w_sys_manage_outer_pay_set_jingquan *ui;
};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_JINGQUAN_H
