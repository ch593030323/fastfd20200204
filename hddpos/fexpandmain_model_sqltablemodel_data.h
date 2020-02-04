#ifndef FEXPANDMAIN_MODEL_SQLTABLEMODEL_DATA_H
#define FEXPANDMAIN_MODEL_SQLTABLEMODEL_DATA_H

#include "fexpandmain_model_sqltablemodel.h"
#include "lds_dd.h"
#include "lds_tableview_delegate_virtual.h"


///双屏同步数据
///select时会先清空所有的keepdata,并且在from db的同时重新生成keepdata
///remove keepdata +- 一行的数据
///
/// model当cur_billno的值和数据库里的ch_billno不同时,只计算cur_billno对应的keepdata
///
/// 数据库里的ch_payno不为空的时候,不计算对应的keepdata
class fexpandmain_model_sqltablemodel_data : public fexpandmain_model_sqltablemodel{
    Q_OBJECT
public:
    //kuaican_quit = false, bool only_update
    enum commitType{
        commitAll = 0,
        commitKuaicanQuit = 1,
        commitUpdate = 2,
    };

public:
    fexpandmain_model_sqltablemodel_data(QObject *parent = 0);

    ///数量和金额的实时修改-------------------------------------------begin

    void keepdata_insert_prepare(int rowbegin = -1);
    void keepdata_insert_submit(int  rowEnd = -1);//会更具dd_lock的值来同步副屏数据，插入
    void keepdata_insert_del(int  rowEnd = -1);//会更具dd_lock的值来同步副屏数据，插入

    void keepdata_Y_dish_fold(QTableView *tableview, int rowP);

    //model 允许存在多个ch_billn在commit的时候,会按照主ch_billno来提交
    QString cur_ch_billno() const;
    QString cur_ch_tableno() const;
    /// 副屏
    ///
public:
    bool insertRow(int arow, const QModelIndex &aparent=QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
protected:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    double model_expression_value(int row, QString expression, bool num_price_is_m = false) const;

    virtual void senddd(const lds::DOUBLE_DATA  &data, float dish_num_old = 0, const QString &SENDD_DATA_FROM_DATABASE = "");
    virtual void senddd_update_insert(int row,
                                      const QString &udp_request_type,
                                      const fexpandmain_keepData &keepd,
                                      double dish_num_old ,
                                      const QString &SENDD_DATA_FROM_DATABASE = "");
    virtual void emit_num_total_change_DD(const ldsVariantMap &num, const ldsVariantMap &total) const;
public:
    void emit_num_total_change_label(float num, float total) const;
    void emit_num_total_change_label_pay(float num, float total) const;

    QString dish_total_str(float t) const;
    QString dish_total_str(float t, const QString &prefix) const;
    QString dish_num_str(float n) const;
    QString dish_num_str(float n, const QString &prefix) const;

    void cey_u_orderdish_append(lds_query_hddpos  &queryRect, const QDateTime &dt, const QString &ch_tableno, const vch_print_memoData &vch_print_memo);
    void cey_u_orderdish_insert(int row, lds_query_hddpos  &queryRect, const QDateTime &dt, const QString &ch_tableno, const vch_print_memoData &vch_print_memo);

    int upper_bound_rowP(int rowY);
    int upper_bound_rowPN(int row, const QSet<int> & skip = QSet<int>());
protected:
    bool kd_enable;//双屏锁
    int kd_enable_row_begin;
    void get_keep_data(int row, fexpandmain_keepData &kdata);
    static void get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, int row, fexpandmain_keepData &kdata, bool pay_is_m_price = false, int pay_int_discount = -1/*-1 not run*/);
    static QString expression_replace(const fexpandmain_model_sqltablemodel_data *tablemodel, QString expression, const QString &min_num_m_price/*empty not run*/, int pay_int_discount = -1/*-1 not run*/);
    static const QString SENDD_DATA_FROM_DATABASE;
public:
    static fexpandmain_keepData get_keep_data_pay_by_isMPrice_intDiscount(const fexpandmain_model_sqltablemodel_data *tablemodel, bool pay_is_m_price = false, int pay_int_discount = -1/*-1 not run*/);

    //副屏
public:
    bool select(const QString &ch_billno, const QString &select_filter = "");
    bool select(const QString &ch_billno, QWidget *parent);
public:
    ///提交数据库
    bool try_commit_data(QString &errstring, commitType type = commitAll, QStringList *insert_int_flowid_list = 0) const;
    bool try_update_data(QString &errstring) const;
    QString msg_mid_by_N(const QString &msg, int pos, int length, bool show_dot_when_not_complete = true);
signals:
    void signals_num_total_change(QString, QString) const;
    void signals_total(double) const;
public:
    const fexpandmain_keepData& getKeepdata() const;//汇总信息
    fexpandmain_keepData getKeepdata(int row) const;//汇总信息
protected:
    fexpandmain_keepData _keepdata;//汇总信息
protected:
    QString _ch_billno;
    //
public:
    vch_membernoData  vch_membernod;
    bool vch_membernod_m_update(QString &errstring);
protected:
    //
    QList<int> n_m_filter_index_list;
    QStringList n_m_filter_name_list;

public:
    ///数量和金额的实时修改-------------------------------------------end
    ///ch_dishno_set_row_map
    /// 底部
public:
    QSet<int> ch_dishno_set_row_map_GET(const QString &ch_dishno) const;
    QSet<int> ch_dishno_set_row_map_GET(int row) const;
    const QMap<QString, QSet<int> > &ch_dishno_set_row_map_GET() const;
    //    bool can_change_num(int row) const ;
    void ch_dishno_set_row_map_VOID(int row, int count);
protected:
    void ch_dishno_set_row_map_PRINT();
    void ch_dishno_set_row_map_INSERT(int row, const QString &SENDD_DATA_FROM_DATABASE);
    void ch_dishno_set_row_map_UPDATE(int row, const QString &SENDD_DATA_FROM_DATABASE);
    void ch_dishno_set_row_map_CLEAR();
    void ch_dishno_set_row_map_DEL(int row, int count, const QString &SENDD_DATA_FROM_DATABASE);
    QMap<QString, QSet<int> >  ch_dishno_set_row_map_get;
    //
public:
    void row_fold_map_RELEASE(QTableView *tableView);
    void row_fold_map_FOLD(QTableView *tableView, const QModelIndex &index);
    QMap<int/*row*/, bool /*isfold*/> row_fold_map;
    ///数量
protected:
    void ch_dishno_dish_num_map_DEL(int row, int count, const QString &SENDD_DATA_FROM_DATABASE);
    void ch_dishno_dish_num_map_UPDATE(int row, double off/*可以为负数*/ , const QString &SENDD_DATA_FROM_DATABASE);
    void ch_dishno_dish_num_map_PRINT();
    QMap<QString, double> ch_dishno_dish_num_map;

    bool ch_dishno_guqing_UPDATE(int row, double off, const QString &SENDD_DATA_FROM_DATABASE);
    bool ch_dishno_guqing_DEL(int row, int count , const QString &SENDD_DATA_FROM_DATABASE);

    void delegate_row_INSERT(int row);
    void delegate_row_DEL(int row, int count);
private:
    void keepdata_init();
    void keepdata_init(fexpandmain_keepData &kd) const;
public:
    const QMap<QString, double> &ch_dishno_dish_num_map_GET() const;
    void print_dish_info();
};

#endif // FEXPANDMAIN_MODEL_SQLTABLEMODEL_DATA_H
