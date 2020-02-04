#ifndef W_SYS_MANAGE_OUTER_PAY_SET_H
#define W_SYS_MANAGE_OUTER_PAY_SET_H

#include <QDialog>
#include <QModelIndex>
#include "lds_model_sqltablemodel.h"
#include <QDataWidgetMapper>
#include "lds_query_hddpos.h"
#include "public_dialog.h"
#include <QPair>
#include <QTextEdit>
#include <QTableView>
#include <QStandardItemModel>
#include "lds_model_sqlstandardmodel.h"
#include "lds_menu.h"
#include "lds_model_mapwidget_vertical.h"

class Ui_w_sys_manage_outer_pay_set;
class Ui_w_sys_manage_outer_pay_set_ftp;
class Ui_w_sys_manage_outer_pay_set_kechuan;
class Ui_w_sys_manage_outer_pay_set_isoft;
class Ui_w_sys_manage_outer_pay_set_xiexin;
class ftableview_standmodel_sql_none;
class public_pushbuton_number;
class QLabel;

namespace Ui {
class w_sys_manage_outer_pay_set;
}

class w_sys_manage_outer_pay_set : public public_Dialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_outer_pay_set(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set();

    static void dataCheck(QWidget *parent, bool ispop = true);
    static void addLanguagesWithDefault(QComboBox *com);
    static const QList<QPair<QString, QString> >& getLanguagelist();
    static QString getLanguageValue(const QString &key);
    static QString getLanguageKeyLike(const QString &key_or_value);
    static void clearCache();
private:
    static QList<QPair<QString, QString> > languagelist;
public:
    virtual void retranslateView();
private slots:
    void took();
    void tocancel();
    void toupdateSection(const QModelIndex &index);
    void toskinapp();
    void toshowround(int index);

    void tofunction();//一键会员价同步销售价
//    void tofunction_2();//一键设置服务率
    void tofunction_3();//数据库强制升级
//    void tofunction_4();//一键设置税率
    void tofunction_5();//一建付款方式英文
    void tofunction_7();//业务数据清除
    void tofunction_8();//本地数据校对
    void tofunction_9();//一键库存预警下限
    void tofunction_10();//组号同编号
    void tofunction_11();//开业请库
    void toDishNo2barcode();
    void toAlldish2invent();
    void toAlldish2weight();

    void totax_settax();
    void totax_settax_2();
    void totax_settax_3();
    void tax_settax(public_pushbuton_number *b, const QString &field_name, const QString &pay_fixed_tax_name);

    void totax_setservice();
    void totax_type();

    void toupload1(bool f);//ftp
    void toupload2(bool f);//科传
    void toupload3(bool f);//协信
    void toupload4(bool f);//星摩尔
    void torestful_interface(bool f);//Malaysia 的 restful API
    void tohourly_pos_interface(bool f);//Malaysia 的 restful API
    void toliaoning_inventory_keep(bool f);//顶有鸣
    void toupload_jingquan(bool f);
    void toupload_upload_fengze(bool f);

    void touploadaware(const QString &arg);

    void toSaaSCheck(bool check);

    void to_liaoning_inventory_upload();
    void to_liaoning_inventory_look();

    void to_language_settings();

    void to_app_locale_setttings();
private:
    bool saveData(int index, bool issave = false);
private:
    Ui_w_sys_manage_outer_pay_set *ui;
    QSet<int> index_set;
    ftableview_standmodel_sql_none * backlist_model;
    QString tax_service_form_explain;
    QDialog *frame_upload_dialog;
};
class w_sys_manage_outer_pay_set_ftp : public lds_model_mapwidget_vertical{
    Q_OBJECT
public:
    static QString FtpAddress;
public:
    explicit w_sys_manage_outer_pay_set_ftp(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_ftp();
private slots:
    void took();
    void todetail();
    void toexample();
    void totest();
    void toaddress_def();
private:
    void saveData();
private:
    Ui_w_sys_manage_outer_pay_set_ftp *ui;
};

class w_sys_manage_outer_pay_set_kechuan : public lds_model_mapwidget_vertical{
    Q_OBJECT
public:
    explicit w_sys_manage_outer_pay_set_kechuan(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_kechuan();
private slots:
    void took();
    void todetail();
    void toexample();
protected:
    virtual bool insert_sql_objectname(const QString &objectname);
private:
    Ui_w_sys_manage_outer_pay_set_kechuan *ui;
};

class w_sys_manage_outer_pay_set_xiexin : public lds_model_mapwidget_vertical{
    Q_OBJECT
public:
    explicit w_sys_manage_outer_pay_set_xiexin(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_xiexin();
private slots:
    void took();
    void tocancel();
    void touploadyesterday();
private:
    bool saveData();
protected:
    virtual bool insert_sql_objectname(const QString &objectname);
private:
    Ui_w_sys_manage_outer_pay_set_xiexin *ui;
};

class w_sys_manage_outer_pay_set_isoft : public lds_model_mapwidget_vertical{
    Q_OBJECT
public:
    explicit w_sys_manage_outer_pay_set_isoft(QWidget *parent = 0);
    ~w_sys_manage_outer_pay_set_isoft();
    void makeCache();
private slots:
    void took();
    void tocancel();
    void toshowcontent();
    void to_reset();
protected:
    virtual bool insert_sql_objectname(const QString &objectname);
private:
    Ui_w_sys_manage_outer_pay_set_isoft *ui;
};

class w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad : public QDialog{
    Q_OBJECT
public:
    static QByteArray car_name();
public:
    explicit w_sys_manage_outer_pay_set_liaoning_inventory_delayUpLoad(QWidget *parent = 0);
    lds_model_sqlstandardmodel *standmodel;
    QPushButton *btn_exit;
    QLabel *label_desc;
public slots:
    void toUpLoad();
};

#endif // W_SYS_MANAGE_OUTER_PAY_SET_H
