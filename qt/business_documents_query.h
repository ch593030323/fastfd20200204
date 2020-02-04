#ifndef BUSINESS_DOCUMENTS_QUERY_H
#define BUSINESS_DOCUMENTS_QUERY_H
#include <QDialog>
#include <QStandardItemModel>
#include "public_mainprinter.h"
#include "w_rpt_report_frame.h"
//营业单据查询
class Ui_business_documents_query;
class business_documents_query : public QDialog
{
    Q_OBJECT
public:
    static bool doUnCheck(const QString &ch_tableno, const QString &ch_billno);
public:
    explicit business_documents_query(QWidget *parent, bool just_quitbill = false);
    ~business_documents_query();
    void showLoginerContent();

private slots:
    void right_check_and_init();

    void totableview(const QModelIndex &index);
    void toexit();//退出
    void torefresh();//检索
    void toprint2();//补打
    void toexport();
    void toprint();
    void touncheck();//反结账
    void torefund();//退单
private:
    int f_member_delete(const QString & as_payno,const QString & as_operid);
    int f_member_delete_y(const QString & as_payno,const QString & as_operid);
    bool prepareUnCheck(const QString &ch_payno, const QString &ch_billno);
    bool doWarin();
    bool insert_cey_u_table(const QString &ch_billno_old, const QString &ch_billno_new, const QString &cur_dt_str, const QString &vch_memo);
    int getDishSelected(QList<qlonglong> &ret_int_flowID_list);
private:
    report_querymodel *model_top;
    report_querymodel *model_left;
    report_querymodel *model_right;
    Ui_business_documents_query *ui;
    class data_refresh {
    public:
        data_refresh(business_documents_query *d): _d(d) {}
        ~data_refresh() {_d->torefresh();}
    private:
        business_documents_query *_d;
    };
    lds_bool_default_false is_show_login_data;
};

#endif // BUSINESS_DOCUMENTS_QUERY_H
