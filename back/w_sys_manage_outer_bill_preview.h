#ifndef W_SYS_MANAGE_OUTER_BILL_PREVIEW_H
#define W_SYS_MANAGE_OUTER_BILL_PREVIEW_H

#include <QDialog>
#include <QModelIndex>
#include "lds_model_sqltablemodel.h"
#include <QDataWidgetMapper>
#include "lds_query_hddpos.h"
#include "lds_model_mapwidget_vertical.h"

namespace Ui {
class w_sys_manage_outer_bill_preview;
}

class w_sys_manage_outer_bill_preview : public lds_model_mapwidget_vertical
{
    Q_OBJECT
public:
    explicit w_sys_manage_outer_bill_preview(QWidget *parent = 0);
    ~w_sys_manage_outer_bill_preview();

    static void if_bill_empty_import_58_def();
    static void import_bp_fmt_m_default();
public:
    bool billexport(QWidget *parent);
    bool billimport(QWidget *parent);
    static bool import(const QString &filepath);
public slots:
    void took();
    void tocancel();
    void toupdateSection(const QModelIndex &index);
    void togetlogo();
    void tobillexport();
    void tobillimport();
    void to_rb_input();


    void toHideExSet();
    void toShowExSet();

    void to_print_prepare_ins_58receipt_center_in_80paper();

    void tobilltest();

    void to_content_hheader_order();

    void to_billpreview_connect();

    void to_bill_preview();
protected:
    virtual bool insert_sql_objectname(const QString &objectname);
    virtual void mappedSetCurrentIndex(int index);
private:
    bool saveData();

    void pushButton_fmt_setText(const QString &text);
    QPushButton * pushButton_fmt_index(const QString &text);
    void bp_fmt_hheader_move(const QString &key, int index);
    void bp_fmt_hheader_order_updateGeometry();
    void bp_fmt_hheader_order_setText(const QString &bp_fmt_hheader_order);
private:
    Ui::w_sys_manage_outer_bill_preview *ui;
    QList<QList<QRect> > bp_fmt_hheader_rect;
    int billpreview_paperwidth;

//    lds_bool_default_false stack1_adjusted;
};

#endif // W_SYS_MANAGE_OUTER_BILL_PREVIEW_H
