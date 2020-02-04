#ifndef W_RPT_REPORT_FRAME_H
#define W_RPT_REPORT_FRAME_H

#include <QDialog>
#include "w_rpt_report_frame_btn_pageindex.h"
#include "w_rpt_report_frame_select.h"
#include <QStandardItemModel>
#include "w_rpt_report.h"
#include "w_rpt_report_printformat.h"
#include "w_rpt_report_dataexport.h"
#include "lds_model_sqltablemodel.h"
#include "lds_tableview_delegate_report.h"
#include "pointerautodel.h"
#include "lds_roundeddialog_rect_pop.h"

class QTableView;
namespace Ui {
class w_rpt_report_frame;
}

class w_rpt_report_frame : public QDialog
{
    Q_OBJECT
public:
    enum mapIndex{
        mapKey = 0,
        mapValue,
        mapTablename,
    };

public:
    explicit w_rpt_report_frame(const QStringList &title_list, QWidget *parent = 0);
    ~w_rpt_report_frame();
    static void clearCache();

    pointerAutoDel<Ui::w_rpt_report_frame> ui;
    void sum_filldata(const QList<w_rpt_report_frame_btn_pageindex::total_desc_value_Data> &d_list);

private slots:
    void sum_hscroll_set_value_1();
public slots:
    void toup();
    void todown();
    void tochoosepage();
    void toupdate_limitbtn(int index);

    void toselect_page();
    virtual void toexport();
    virtual void toprint();


    void toprintmore();
    void toprint58();
    void toprint80();

    void toexit();

    void toPageUp();
    void toPageDown();
protected:
    void exportSql();
    void printSql();

    void exportModel();
    void printModel();
protected:
    void resize_column_to_content_t();
    virtual void select_page_after();
    ///
    /// \brief tableview_setpan
    /// \param cmp_column                 相同的cmp_column则合并
    /// \param span_column_list         需要合并的列span_column
    ///
    void tableview_setpan(const QString &cmp_column, const QStringList & span_column_list);

    int tableviewPageCells(lds_tableView *tableview);
private slots:
    void to_resize_column_to_content();
protected:
    QString  sql_count_arg1;
protected:
    class pushbutton_auto_enable{
    public:
        pushbutton_auto_enable(QAbstractButton *b) : _b(b) {b->setEnabled(false);b->repaint();if(b->parentWidget())b->parentWidget()->repaint();}
        ~pushbutton_auto_enable() {_b->setEnabled(true);}
    private:
        QAbstractButton *_b;
    };
private:
    static QMap<QString, QString> map;
};

class report_querymodel : public QSqlQueryModel{
public:
    report_querymodel(QObject *parent = 0) :
        QSqlQueryModel(parent),
        querycount(0) {}

    void setQuery(const QString &query, const QSqlDatabase &db = QSqlDatabase());
    QVariant model_data(int row, const QString &field_name) const;
    int fieldIndex(const QString &fieldname) const;
    int querycount;

    QString arg_query;
};

//有专门用途
// num_back 12.09   num_total = 1999.0
//的column key 为column - 1
class report_standmodel : public QStandardItemModel{
public:
    report_standmodel(QObject *parent) :QStandardItemModel(parent) {}
    QVariant data(const QModelIndex &index, int role) const;
    static QStandardItem *newItem(const QVariant &value);
    void setup(lds_tableView *tableview);
    void disable_data_transfer();

    void appendVarList(const QVariantList &var_list);
private:
    lds_bool_default_true b_data_transfer;
};


#endif // W_RPT_REPORT_FRAME_H
