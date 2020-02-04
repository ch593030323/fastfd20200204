#ifndef W_SYS_MANAGE_operatorright_H
#define W_SYS_MANAGE_operatorright_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class w_sys_manage_operatorright;
}

class checkbox_phonestyle;
class ftableview_standmodel_sql_none;
class w_sys_manage_operatorright : public QDialog
{
    Q_OBJECT

public:
    struct OperatorRight{
        QString key;//0801
        QString value;//前台改价
        QString type;//系统设置区域
        bool authorized;//权限不够时,临时赋权限
    };

    static const QList<OperatorRight > &get_code_name_list();

    static int index_zh(const QString &value);
    static int index_code(const QString &key);

private:
    static QList<OperatorRight > code_name_list;
    static void code_name_list_append(const OperatorRight &right);
    static int stack_widget_index;
public:
    explicit w_sys_manage_operatorright(QWidget *parent = 0);
    ~w_sys_manage_operatorright();
private slots:
    void toselectall();
    void toclearall();
    void toitemright();
    void toarearight();
    void took();
    void tocancel();
    void torightcopy();
    void tochangepwd();

    void toupdaterights();
    void toSaveCheckbox();

    void updateSection(const QModelIndex &index);
private:
    void setCheckedAll(bool check);

    void setCheckedSelect(const QString &vch_operID);
    void setCheckedRights(const QString &rights);

    void createItem(QWidget *parent, const OperatorRight &right, int index);
private:
    Ui::w_sys_manage_operatorright *ui;
    QList<checkbox_phonestyle*> checkbox_list;
    ftableview_standmodel_sql_none *backlist_model;
};

#endif // W_SYS_MANAGE_operatorright_H
