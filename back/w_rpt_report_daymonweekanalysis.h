#ifndef W_RPT_REPORT_DAYMONWEEKANALYSIS_H
#define W_RPT_REPORT_DAYMONWEEKANALYSIS_H

#include <QDialog>
#include <QMap>
#include <QDateTime>
#include "w_rpt_report.h"
#include <QStandardItemModel>

class Ui_w_rpt_report_daymonweekanalysis_Dialog;
class Ui_w_bt_dish_unitset_Dialog;
class QTableView;
class QMenu;
class w_rpt_report_daymonweekanalysis : public QDialog
{
    Q_OBJECT
public:
    explicit w_rpt_report_daymonweekanalysis(QWidget *parent = 0);
    ~w_rpt_report_daymonweekanalysis();
public:

private:
    void tab1sqlinit();
    void modeltranslate(const QString &sql, QStandardItemModel *model);//由于model格式不对应，所以需要对 model进行处理
    bool updateitems(QList<QStandardItem *> &items, const QString &text);

    void tab2sqlinit();
    void modeltranslate_2(const QString &sql, QStandardItemModel *model);
    void updateheaders(QStringList &headers, const QString &headername);
private slots:
    void toselect();
    void toexit();

    void tocomtype(const QString &text);

private:
    Ui_w_rpt_report_daymonweekanalysis_Dialog *ui;
    QMap<QString , w_rpt_report::TABTYPE> tab1info;
    QSqlQueryModel *querymodel;
    QStandardItemModel *standmodel;

    QMap<QString , w_rpt_report::TABTYPE> tab2info;
};

//=================================

#endif // W_RPT_REPORT_DAYMONWEEKANALYSIS_H
