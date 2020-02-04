#ifndef W_RPT_REPORT_PRINTFORMAT_H
#define W_RPT_REPORT_PRINTFORMAT_H

#include <QDialog>
#include <QSettings>
#include <QTableView>
#include "lds_tableview.h"
#include "lds_tableview_sum.h"
#include "backheader.h"

class QStandardItemModel;
class QAbstractItemModel;
namespace Ui {
class w_rpt_report_printFormat;
}

class w_rpt_report_printFormat : public QDialog
{
    Q_OBJECT
public:
    enum showMode{
        show_model_quick,
        show_model_more,
    };
private:
    enum printSize{
        print_size_58 = 0,
        print_size_80,
    };
public:
    //print_type_index : 1
    //    w_rpt_report_printFormat(lds_tableView *content_tableview, lds_tableView *tail_tableview,
    //                             const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path,
    //                             QWidget *parent );

    //print_type_index : 2
    //model 打印内容、汇总、
    w_rpt_report_printFormat(showMode mode, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path,
                             const QString &dt_from,  const QString &dt_to,
                             lds_tableView *content_tableview, lds_tableView *tail_tableview,
                             QWidget *parent );
    //print_type_index : 3
    //sql    打印分页内容、汇总、
    w_rpt_report_printFormat(showMode mode, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path,
                             const QString &dt_from,  const QString &dt_to,
                             lds_tableView *content_tableview, const QString &sql, lds_tableView *tail_tableview,
                             QWidget *parent );
    //print_type_index : 4
    //    打印内容、汇总、
    w_rpt_report_printFormat(showMode mode, const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path,
                             const QString &dt_from,  const QString &dt_to,
                             lds_tableview_sum *tableview_sum,
                             QWidget *parent );
    ~w_rpt_report_printFormat();

    void pop(QWidget *basewidget, QWidget *parent);
signals:
    void signal_printmore();
protected:
    void keyPressEvent(QKeyEvent *e);

private:
    void showQuickMode();
    void showMoreMode();

private slots:
    void toReportPrint();
    void toReportPrintImg();
    void toReportPrintIns();
    void toexit();

    void toPrintFormatSaveAndLoad(int index);
    void toPrintFormatSave();

    void toquickprintmore();
    void toprint58();
    void toprint80();

private:
    void modelappRow(const QString &checked, const QString &header, int width, int line);
    void initData(showMode mode, lds_tableView *content_tableview, lds_tableView *tail_tableview,
                  const QString &title, const QString &operid, const QString &opertime, const QString &_conf_path);

private:
    Ui::w_rpt_report_printFormat *ui;
    QString conf_path;
    QString sql;

    QStandardItemModel *standmodel;
    QSettings *print_format_conf;
    lds_tableView *content_tableview;
    lds_tableView *tail_tableview;

    struct printFormat{
        int index;//行号
        int width;//打印宽度
        int line;//打印行
    };

    int print_type_index;
    QSharedPointer<Printer_POS> printer;
};

#endif // W_RPT_REPORT_PRINTFORMAT_H
