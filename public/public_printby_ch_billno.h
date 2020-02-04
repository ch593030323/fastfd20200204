#ifndef PUBLIC_PRINTBY_CH_BILLNO_H
#define PUBLIC_PRINTBY_CH_BILLNO_H

#include <QObject>
#include "commonheader.h"
#include "public_mainprinter.h"
#include "lds_query_hddpos.h"
#include "public_ins_maker.h"
#include <QDateTime>
#include "lds_model_sqltablemodel.h"
#include "lds_model_sqltablemodel.h"

class txt_ins_bill_print;
class public_printby_ch_billno : public QObject
{
    Q_OBJECT
public:
    enum payPrintMode{
        payNone = 0,
        payRefund = 1, //退单
        payNormal = 2,//结算单
        payDrawer = 4,//钱箱
        payLabel = 8,//标签
        payReprint = 16, //补打
    };
    Q_DECLARE_FLAGS(payPrintModes, payPrintMode)
public:
    static QString model_bp_fmt_value(lds_model_sqltablemodel *model_bp_fmt,  const QString &as_parameter, const QVariant &as_default = QVariant());
public:
    static QString getfixedlen(const QString &value, int len, int isCut);
    
    static void txt_print_fill_sale_head(lds_model_sqltablemodel *m, const QString &ch_billno);
    static void txt_print_fill_sale_detail(lds_model_sqltablemodel *m, const QString &ch_billno, const QString &and_sql_filter = "");
    static void txt_print_fill_pay_detail(lds_model_sqltablemodel *m, const QString &ch_billno);
    static void txt_print_fill_pay_detail_preprint(lds_model_sqltablemodel *m, const QString &ch_billno, const QString &flowid_sql_filter);
    static void txt_print_fill_company_info(lds_model_sqltablemodel *m, const QString &ch_billno);

    static void txt_print_bill_set_data(lds_model_sqltablemodel *m, const QString &table_name, const QString &key_name, const QVariant &value);
    static void txt_print_bill_create_field(lds_model_sqltablemodel *m, const QString &key, const QString &table_name, const QString &key_name, const QString &explain, const QVariant &value);
    static void txt_print_bill_create_table(lds_model_sqltablemodel *m);

    static QString model_operator(lds_model_sqltablemodel *m, const QString &operid);
    static QString model_tablename(lds_model_sqltablemodel *m, const QString &ch_tableno);
    static QString septNumberCurrency(lds_model_sqltablemodel *m, double value, int decimal, bool fillCurrency = true);


    static QString interSelectSql(lds_model_sqltablemodel *m, bool pay_exit, const QString &and_sql_filter);

    static void dishname_split(const QString &dishname_show, QString &dishname, QString &dishname_2);
    static void dishname_split(const QString &dishname_show, QString &dishname, QString &dishname_2, const QString &tablename);

    static void payPrint(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList & label_insert_int_flowid_list, int pay_print_mode,
                         const QImage &image);
    static bool openDrawer(QString *errstring = 0);
private:
    static QString stringListToJsonArray(const QStringList &list);
    static QString zeroEmpty(lds_model_sqltablemodel *m, double value, int decimal, bool fillCurrency = true);
    static QVariant cey_sys_parameterValue(const lds_model_sqltablemodel *m, const QString &parameter, const QVariant &def = QVariant() );

public:
    explicit public_printby_ch_billno(const QString &ch_billno, const QString &languageKey = "",  QObject *parent = 0);
    public_printby_ch_billno();
    ~public_printby_ch_billno();

    //payRefund pay drawer label
    void toPayPrint(const QString &bp_path, int bp_count, const QStringList & label_insert_int_flowid_list, payPrintModes m);
    void toprint(const QString &path, int manual_bp_count = -1, payPrintMode pay_mode = payNormal);
    void toprint_dish_return(const QString &path, const QStringList &int_quit_flow_list);
    static void toprint_dish_return(const QString &ch_billno, const QString &path, const QStringList &int_quit_flow_list);

    void toMakeFormIns(const QString &path, payPrintMode pay_mode = payNormal);
    QImage toMakeFormImg();

    QImage getPayPrintImg(const QString &bp_path, payPrintModes m);

    //外卖
    void toprintwaimai(const QString &path);//打印外卖
    void toprintwaimai_pay(const QString &path, int manual_bp_count);//打印外卖

    //没有表头，没有表尾，不切刀,包含分单号
    void toprint_preprint(const QString &path, const QString &title,  const QString &flowid_sql_filter = "");
    blockIns toprint_preprint_div(const QString &path, double &ret_total, int sqr = -1);//预打单
    blockIns toprint_preprint_div_title(const QString &path, const QString &title);//预打单
    blockIns toprint_preprint_div_tail(const QString &path, double subtotal, int idvcount);//预打单

    void makeIns(int paperwidth);
    void clear_ins();
    void clear_model_bp_fmt();
    static QString defpath();
    //    static void print_label(QWidget *parent, const QString &ch_billno, int print_int_flowid_max = -1);
//    static void print_label(const QString &ch_billno, int print_int_flowid_max = -1);
    static void print_label(const QString &ch_billno);
    static void print_label(const QString &ch_billno, const QString &and_where_sql);
    static void print_label(const QString &ch_billno, const QStringList &insert_int_flowid_list);
    static void print_label_test(QWidget *parent);

    const blockIns &getPintIns() const;

    lds_model_sqltablemodel *model_bp_fmt;
    void initData(const QString &languageKey);
private:
    void model_bp_fmt_fill_orderdish(bool pay_exit = false, const QString & flowid_sql_filter = "");
    void model_bp_fmt_fill_orderdish_return(const QStringList &int_quit_flow_list);
    //    void print_content_dishreturn(const QStringList &int_flow_id_list);//中餐退货总单
    void print_run();

    void model_bp_fmt_fill_pay_detail();
    static QSharedPointer<lds_model_sqltablemodel> make_pay_detail_preprint(const lds_model_sqltablemodel*m,  const QString &ch_billno, const QString &flowid_sql_filter = "");
    double model_bp_fmt_fill_pay_detail_preprint(const QString &flowid_sql_filter = "");
    double model_bp_fmt_fill_pay_detail_preprint_qr_code(int int_flowid_greater_than = -1);

    bool can_print_sale_detail();
private:
    QString get_dish_typename(const QString &ch_dish_typeno);
    QString get_sub_typename(const QString &ch_sub_typeno);
    QString get_dish_typeno_sum(const lds_model_sqltablemodel_d *dish_printd, const QString &ch_dish_typeno);
    QString get_sub_typeno_sum(const lds_model_sqltablemodel_d *dish_printd, const QString &ch_sub_typeno);
    QString get_sum(const lds_model_sqltablemodel_d *dish_printd);
    QString getfixedlen(const QString &value, int len);
    void piecedishprint(int row, const lds_model_sqltablemodel_d *dish_printd, const QByteArray &pre, bool isheader = false);

    int format_Negative(const QString &field_name);
    QString format_amount(float value, const QString &field_name);
    QString format_num(float value, const QString &field_name);
    QString format_price(float value, const QString &field_name);

    QString name_of_operid(const QString &operid);
    QString key_value_com(const QString &k, const QString &v, int paperwidth, int xscale);
    int scale_h_by(const QByteArray &s00);
    QString get_bp_fmt_table(const QString &vch_tablename, const QString &ch_tableno);
    QString get_bp_fmt_table();

    QString get_bp_fmt_operid(const QString &vch_operator, const QString &vch_operid);
    QString get_bp_fmt_operid();

    QVariant get_cey_u_table(const QString &sql_value_name);
    QVariant get_cey_u_togo(const QString &sql_value_name);

    bool sale_head_existed(const QString &key);
    QVariant get_sale_head(const QString &key);

    bool receipt_head_existed(const QString &key);
    QVariant get_receipt_head(const QString &key);

    bool pay_detail_existed(const QString &key);
    QVariant get_pay_detail(const QString &key);

    bool receipt_tail_existed(const QString &key);
    QVariant get_receipt_tail(const QString &key);

    QString model_bp_fmt_value(const QString &as_parameter, const QVariant &as_default = QVariant());

    QString pay_fixed_service_suffix();
    QString pay_fixed_tax_suffix();

    void model_bp_fmt_append_query(const QString &tablename, lds_query_hddpos  &query);
    void model_bp_fmt_append_sale_detail(lds_query_hddpos  &query);
    void model_bp_fmt_fill_receipt_tail();

    void print_format_init_path(const QString &path);
private:
    static bool emptyOrN(const QString &d);
private:
    QString litterblank;
    QString ch_billno;
    int paperwidth;
    int column_count;
    int bp_count;

    QByteArray bp_content_len;
    //    QString ch_payno;
    //    QString int_id;
    //    QByteArray vch_print_memo;
    //    QString vch_reason;
    //    QString ch_suitflag;
    //    QString bp_series_group;//按大类分组
    //    QString ch_dish_typeno;
    //    QString ch_sub_typeno;
    //    QString ch_tableno;
    //    QString vch_tablename;
    //    int int_person;

    QString bp_dishname;
    QString bp_unit;
    QString bp_num;
    QString bp_price;
    QString bp_fmt_price_with_org;
    QString bp_amount;
    QString bp_disdetail;
    QString bp_total;
    QString bp_fmt_table_fmts;
    QString bp_fmt_serial_no_desc;
    QString bp_fmt_operid_fmts;

    int bp_amount_left;
    int bp_fmt_disdetail_left ;
    int bp_fmt_total_left ;
    int bp_dishname_left ;
    int bp_num_left ;
    int bp_price_left ;
    int bp_unit_left;
    int bp_num_dec;
    int bp_price_dec;
    int bp_amount_dec;

    int bp_dishname_row;
    int bp_unit_row;
    int bp_num_row;
    int bp_price_row;
    int bp_amount_row;
    int bp_disdetail_row;
    int bp_total_row;

    int bp_dishname_len;
    int bp_unit_len;
    int bp_num_len;
    int bp_price_len;
    int bp_amount_len;
    int bp_disdetail_len;
    int bp_total_len;

    public_ins_maker print_format;
    int rowno;

};

#endif // PUBLIC_PRINTBY_CH_BILLNO_H
