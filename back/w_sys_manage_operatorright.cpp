#include "w_sys_manage_operatorright.h"
#include "ui_w_sys_manage_operatorright.h"
#include "lds_query_db.h"
#include "checkbox_phonestyle.h"
#include "lds.h"
#include "qheaderview.h"
#include "w_sys_manage_operatorright.h"
#include <QStandardItemModel>
#include "backheader.h"
#include "lds_query_hddpos.h"
#include <QSqlRecord>
#include <QSqlError>
#include "lds_messagebox.h"
#include "lds_tableview_delegate_check.h"
#include "n_func.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include "w_sys_manage_operatorright_rightcopy.h"
#include "w_sys_manage_operatorright_pwdchange.h"
#include "w_sys_manage_operatorright_dish_type_right.h"
#include "w_sys_manage_operatorright_dish_type_right_new_type.h"
#include "ui_w_bt_dish_unitset_dialog.h"
#include "ui_w_sys_manage_operatorright_rightcopy_dialog.h"
#include "checkbox_phonestyle.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include "ui_w_sys_manage_operatorright_pwdchange.h"
#include "w_sys_manage_operatorright_area_right.h"
#include "w_sys_manage_operatorright_type_right.h"


QList<w_sys_manage_operatorright::OperatorRight > w_sys_manage_operatorright::code_name_list;
int w_sys_manage_operatorright::stack_widget_index = 0;
const QList<w_sys_manage_operatorright::OperatorRight > &w_sys_manage_operatorright::get_code_name_list()
{
    if(code_name_list.isEmpty()) {
        //        基础资料
        code_name_list_append(OperatorRight{"0101", QT_TRANSLATE_NOOP("QObject", "商品资料"), "jczl", false});
        code_name_list_append(OperatorRight{"0104", QT_TRANSLATE_NOOP("QObject", "做法设置"), "jczl", false});
        code_name_list_append(OperatorRight{"0102", QT_TRANSLATE_NOOP("QObject", "组合商品"), "jczl", false});
        code_name_list_append(OperatorRight{"0105", QT_TRANSLATE_NOOP("QObject", "餐桌信息"), "jczl", false});
        code_name_list_append(OperatorRight{"0906", QT_TRANSLATE_NOOP("QObject", "基础资料导出"), "jczl", false});
        code_name_list_append(OperatorRight{"0907", QT_TRANSLATE_NOOP("QObject", "基础资料导入"), "jczl", false});
        code_name_list_append(OperatorRight{"0110", QT_TRANSLATE_NOOP("QObject", "退货原因"), "jczl", false});
        code_name_list_append(OperatorRight{"0111", QT_TRANSLATE_NOOP("QObject", "客户"), "jczl", false});

        //        会员设置
        code_name_list_append(OperatorRight{"0401", QT_TRANSLATE_NOOP("QObject", "会员参数设置"), "hysz", false});
        code_name_list_append(OperatorRight{"0407", QT_TRANSLATE_NOOP("QObject", "会员类型变更"), "hysz", false});
        code_name_list_append(OperatorRight{"0402", QT_TRANSLATE_NOOP("QObject", "会员资料维护"), "hysz", false});
        code_name_list_append(OperatorRight{"0406", QT_TRANSLATE_NOOP("QObject", "会员积分管理"), "hysz", false});
        code_name_list_append(OperatorRight{"0403", QT_TRANSLATE_NOOP("QObject", "会员状态管理"), "hysz", false});
        code_name_list_append(OperatorRight{"0404", QT_TRANSLATE_NOOP("QObject", "充值"), "hysz", false});
        code_name_list_append(OperatorRight{"0413", QT_TRANSLATE_NOOP("QObject", "芯片清除"), "hysz", false});
        code_name_list_append(OperatorRight{"0411", QT_TRANSLATE_NOOP("QObject", "会员报表"), "hysz", false});

        //        库存管理
        code_name_list_append(OperatorRight{"0607", QT_TRANSLATE_NOOP("QObject", "供应商"), "kcgl", false});
        code_name_list_append(OperatorRight{"0602", QT_TRANSLATE_NOOP("QObject", "采购入库"), "kcgl", false});
        code_name_list_append(OperatorRight{"0603", QT_TRANSLATE_NOOP("QObject", "采购退货"), "kcgl", false});
        code_name_list_append(OperatorRight{"0604", QT_TRANSLATE_NOOP("QObject", "商品报损"), "kcgl", false});
        code_name_list_append(OperatorRight{"0605", QT_TRANSLATE_NOOP("QObject", "库存盘点"), "kcgl", false});
        code_name_list_append(OperatorRight{"0609", QT_TRANSLATE_NOOP("QObject", "盘点查询"), "kcgl", false});
        code_name_list_append(OperatorRight{"0610", QT_TRANSLATE_NOOP("QObject", "进销存汇总"), "kcgl", false});
        code_name_list_append(OperatorRight{"0611", QT_TRANSLATE_NOOP("QObject", "单据汇总查询"), "kcgl", false});
        code_name_list_append(OperatorRight{"0606", QT_TRANSLATE_NOOP("QObject", "库存查询"), "kcgl", false});

        //        报表分析
        code_name_list_append(OperatorRight{"0709", QT_TRANSLATE_NOOP("QObject", "交班报表"), "bbfx", false});
        code_name_list_append(OperatorRight{"0710", QT_TRANSLATE_NOOP("QObject", "日营业情况统计"), "bbfx", false});
        code_name_list_append(OperatorRight{"0705", QT_TRANSLATE_NOOP("QObject", "收银流水"), "bbfx", false});
        code_name_list_append(OperatorRight{"0701", QT_TRANSLATE_NOOP("QObject", "商品销售汇总"), "bbfx", false});
        code_name_list_append(OperatorRight{"0203", QT_TRANSLATE_NOOP("QObject", "销售流水"), "bbfx", false});
        code_name_list_append(OperatorRight{"0714", QT_TRANSLATE_NOOP("QObject", "商品销售排行统计"), "bbfx", false});
        code_name_list_append(OperatorRight{"0201", QT_TRANSLATE_NOOP("QObject", "营业数据按单查询"), "bbfx", false});
        code_name_list_append(OperatorRight{"0227", QT_TRANSLATE_NOOP("QObject", "反结账查询"), "bbfx", false});
        code_name_list_append(OperatorRight{"0704", QT_TRANSLATE_NOOP("QObject", "退货情况报表"), "bbfx", false});
        code_name_list_append(OperatorRight{"0712", QT_TRANSLATE_NOOP("QObject", "品类销售营业分析"), "bbfx", false});
        code_name_list_append(OperatorRight{"0702", QT_TRANSLATE_NOOP("QObject", "促销报表"), "bbfx", false});
        code_name_list_append(OperatorRight{"0708", QT_TRANSLATE_NOOP("QObject", "日月周报表分析"), "bbfx", false});
        code_name_list_append(OperatorRight{"0715", QT_TRANSLATE_NOOP("QObject", "服务员提成查询"), "bbfx", false});
        code_name_list_append(OperatorRight{"0711", QT_TRANSLATE_NOOP("QObject", "原料销售报表"), "bbfx", false});
        code_name_list_append(OperatorRight{"0703", QT_TRANSLATE_NOOP("QObject", "桌台营业统计"), "bbfx", false});
        code_name_list_append(OperatorRight{"0706", QT_TRANSLATE_NOOP("QObject", "服务费与低消查询"), "bbfx", false});
        code_name_list_append(OperatorRight{"0713", QT_TRANSLATE_NOOP("QObject", "时段营业情况分析"), "bbfx", false});
        code_name_list_append(OperatorRight{"0717", QT_TRANSLATE_NOOP("QObject", "税收报表"), "bbfx", false});

        //系统设置
        code_name_list_append(OperatorRight{"0801", QT_TRANSLATE_NOOP("QObject", "外部设备"), "xtsz", false});
        code_name_list_append(OperatorRight{"0802", QT_TRANSLATE_NOOP("QObject", "收银设置"), "xtsz", false});
        code_name_list_append(OperatorRight{"0106", QT_TRANSLATE_NOOP("QObject", "收银方式"), "xtsz", false});
        code_name_list_append(OperatorRight{"0804", QT_TRANSLATE_NOOP("QObject", "促销设置"), "xtsz", false});
        code_name_list_append(OperatorRight{"0901", QT_TRANSLATE_NOOP("QObject", "公司信息"), "xtsz", false});
        code_name_list_append(OperatorRight{"0903", QT_TRANSLATE_NOOP("QObject", "操作员信息"), "xtsz", false});
        code_name_list_append(OperatorRight{"0807", QT_TRANSLATE_NOOP("QObject", "操作员权限"), "xtsz", false});
        code_name_list_append(OperatorRight{"0905", QT_TRANSLATE_NOOP("QObject", "数据库"), "xtsz", false});
        code_name_list_append(OperatorRight{"0809", QT_TRANSLATE_NOOP("QObject", "在线升级"), "xtsz", false});
        code_name_list_append(OperatorRight{"0814", QT_TRANSLATE_NOOP("QObject", "注册"), "xtsz", false});

        //云端连锁
        code_name_list_append(OperatorRight{"0810", QT_TRANSLATE_NOOP("QObject", "云设置"), "ydls", false});
        code_name_list_append(OperatorRight{"0811", QT_TRANSLATE_NOOP("QObject", "采购申请"), "ydls", false});
        code_name_list_append(OperatorRight{"0812", QT_TRANSLATE_NOOP("QObject", "门店入库"), "ydls", false});
        code_name_list_append(OperatorRight{"0813", QT_TRANSLATE_NOOP("QObject", "门店出库"), "ydls", false});
        //前台
        code_name_list_append(OperatorRight{"0209", QT_TRANSLATE_NOOP("QObject", "开钱箱"), "qtsz", true});
        code_name_list_append(OperatorRight{"0224", QT_TRANSLATE_NOOP("QObject", "商品销售统计"), "qtsz", false});
        code_name_list_append(OperatorRight{"0244", QT_TRANSLATE_NOOP("QObject", "后台"), "qtsz", false});
        code_name_list_append(OperatorRight{"0241", QT_TRANSLATE_NOOP("QObject", "前台"), "qtsz", false});
        code_name_list_append(OperatorRight{"0245", QT_TRANSLATE_NOOP("QObject", "交班"), "qtsz", false});

        code_name_list_append(OperatorRight{"0214", QT_TRANSLATE_NOOP("QObject", "付款"), "qtsz", false});
        code_name_list_append(OperatorRight{"0218", QT_TRANSLATE_NOOP("QObject", "退货"), "qtsz", true});
        code_name_list_append(OperatorRight{"0815", QT_TRANSLATE_NOOP("QObject", "反结账"), "qtsz", true});
        code_name_list_append(OperatorRight{"0816", QT_TRANSLATE_NOOP("QObject", "前台改价"), "qtsz", true});
    }

    return code_name_list;
}

int w_sys_manage_operatorright::index_zh(const QString &value)
{
    for(int k = 0; k < get_code_name_list().count(); k ++) {
        if(get_code_name_list()[k].value == value)
            return k;
    }
    return -1;
}

int w_sys_manage_operatorright::index_code(const QString &key)
{
    for(int k = 0; k < get_code_name_list().count(); k ++) {
        if(get_code_name_list()[k].key == key)
            return k;
    }
    return -1;
}

void w_sys_manage_operatorright::code_name_list_append(const OperatorRight &right)
{
#ifdef QT_DEBUG
    for(int k = 0; k < code_name_list.count(); k ++) {
        if(code_name_list[k].value == right.value) {
            qDebug() << __FILE__ << __LINE__ <<  right.value << ", value is duliple";
            QObject *o = 0;
            o->children();
        }
        if(code_name_list[k].key == right.key) {
            qDebug() << __FILE__ << __LINE__ <<  right.value << ", key is duliple";
            QObject *o = 0;
            o->children();
        }
    }
#endif
    code_name_list.append(right);
}

w_sys_manage_operatorright::w_sys_manage_operatorright(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_sys_manage_operatorright)
{
    ui->setupUi(this);
    resize(lds::MAIN_WINDOW_SIZE);
    ui->comboBox_operator->addItemsBySql(" SELECT vch_operID, vch_operator FROM cey_sys_operator; ");

    QList<ftableview_standmodel_sql_none::noneData> list_data;
    list_data<<ftableview_standmodel_sql_none::noneData(QObject::tr("基础资料"), "jczl");
    list_data<<ftableview_standmodel_sql_none::noneData(QObject::tr("会员设置"), "hysz");
    list_data<<ftableview_standmodel_sql_none::noneData(QObject::tr("库存管理"), "kcgl");
    list_data<<ftableview_standmodel_sql_none::noneData(QObject::tr("报表分析"), "bbfx");
    list_data<<ftableview_standmodel_sql_none::noneData(QObject::tr("系统设置"), "xtsz");

    list_data<<ftableview_standmodel_sql_none::noneData(QObject::tr("云端连锁"), "ydls");

    list_data<<ftableview_standmodel_sql_none::noneData(QObject::tr("其他"), "qtsz");

    backlist_model = new ftableview_standmodel_sql_none;
    backlist_model->set_row_column(list_data.count(), 1);
    backlist_model->setModelList(list_data);
    ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
    d->keepConnect(ui->tableView_list);
    d->indexisEmptyDrawGrad = true;
    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type");
    ui->tableView_list->setModel(backlist_model);
    ui->tableView_list->setItemDelegate(d);
    ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
    ui->tableView_list->verticalHeader()->setDefaultSectionSize(50);
    ui->tableView_list->setCurrentIndex(backlist_model->index(stack_widget_index, 0));
    //
    const QList<w_sys_manage_operatorright::OperatorRight > & right_list = get_code_name_list();
    for(int k = 0, index = 0; k < right_list.count(); k ++)  if(right_list[k].type == "jczl")    createItem(ui->page, right_list[k], index ++);
    for(int k = 0, index = 0; k < right_list.count(); k ++)  if(right_list[k].type == "hysz")    createItem(ui->page_2, right_list[k], index ++);
    for(int k = 0, index = 0; k < right_list.count(); k ++)  if(right_list[k].type == "kcgl")    createItem(ui->page_3, right_list[k], index ++);
    for(int k = 0, index = 0; k < right_list.count(); k ++)  if(right_list[k].type == "bbfx")    createItem(ui->page_4, right_list[k], index ++);
    for(int k = 0, index = 0; k < right_list.count(); k ++)  if(right_list[k].type == "xtsz")    createItem(ui->page_5, right_list[k], index ++);

    for(int k = 0, index = 0; k < right_list.count(); k ++)  if(right_list[k].type == "ydls")    createItem(ui->page_6, right_list[k], index ++);
    for(int k = 0, index = 0; k < right_list.count(); k ++)  if(right_list[k].type == "qtsz")    createItem(ui->page_7, right_list[k], index ++);

    ui->stackedWidget->setCurrentIndex(stack_widget_index);

    connect(ui->pushButton_selectall, SIGNAL(clicked()), this, SLOT(toselectall()));
    connect(ui->pushButton_clearall, SIGNAL(clicked()), this, SLOT(toclearall()));
    connect(ui->pushButton_itemright, SIGNAL(clicked()), this, SLOT(toitemright()));
    connect(ui->pushButton_arearight, SIGNAL(clicked()), this, SLOT(toarearight()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_rightcopy, SIGNAL(clicked()), this, SLOT(torightcopy()));
    connect(ui->pushButton_changepwd, SIGNAL(clicked()), this, SLOT(tochangepwd()));
    connect(ui->comboBox_operator, SIGNAL(currentIndexChanged(int)), this, SLOT(toupdaterights()));
    connect(ui->tableView_list, SIGNAL(clicked(QModelIndex)), this, SLOT(updateSection(QModelIndex)));
    connect(ui->pushButton_com_down, SIGNAL(clicked()), ui->comboBox_operator, SLOT(toScrollDown()));
    connect(ui->pushButton_com_up, SIGNAL(clicked()), ui->comboBox_operator, SLOT(toScrollUp()));

    if(ui->comboBox_operator->currentIndex() >= 0) {
        setCheckedSelect(ui->comboBox_operator->curusrdata().toString());
    }
}

w_sys_manage_operatorright::~w_sys_manage_operatorright()
{
    delete ui;
}


void w_sys_manage_operatorright::toselectall()
{
    setCheckedAll(true);
}
void w_sys_manage_operatorright::toclearall()
{
    setCheckedAll(false);
}

void w_sys_manage_operatorright::toitemright()
{
    w_sys_manage_operatorright_type_right dialog( this);
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_operatorright::toarearight()
{
    w_sys_manage_operatorright_area_right dialog(this);
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_operatorright::took()
{
    lds_query_hddpos::tran_saction();
    lds_query_hddpos query;
    for(int row = 0; row < ui->comboBox_operator->count(); row ++) {
        QString rights = ui->comboBox_operator->model_data(row, 0, Qt::UserRole +1).toString();
        if(rights.isNull())
            continue;
        QStringList rightlist = rights.split(",", QString::SkipEmptyParts);
        QString vch_operID = ui->comboBox_operator->model_data(row, 0, Qt::UserRole).toString();

        if(false == query.execDelete("cey_sys_oper_purview", QString("vch_operID='%1' ").arg(vch_operID))) {
            lds_query_hddpos::roll_back();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
            return;
        }
        foreach(const QString &ch_purviewno, rightlist) {
            if(false == query.execInsert("cey_sys_oper_purview", qrtVariantPairList()
                                         << qrtVariantPair("vch_operID", vch_operID)
                                         << qrtVariantPair("ch_purviewno", ch_purviewno))) {
                lds_query_hddpos::roll_back();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, query.recordError());
                return;
            }
        }
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    this->accept();
}
void w_sys_manage_operatorright::tocancel()
{
    this->reject();
}
void w_sys_manage_operatorright::torightcopy()
{
    w_sys_manage_operatorright_rightcopy dialog(ui->comboBox_operator->currentText(), ui->comboBox_operator->curusrdata().toString(), this);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        QString vch_operID = dialog.target;
        int row = ui->comboBox_operator->findData(vch_operID);
        ui->comboBox_operator->model_data_set(row, 0, lds_query_hddpos::selectValue(QString("SELECT group_concat(ch_purviewno) FROM cey_sys_oper_purview where vch_operID = '%1' ").arg(vch_operID)), Qt::UserRole + 1);
    }
}
void w_sys_manage_operatorright::tochangepwd()
{
    QString cur_operID;
    cur_operID = ui->comboBox_operator->curusrdata().toString();
    if(lds::gs_operid == "0000") {
        switch(lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("修改密码"), QObject::tr("管理员账号"), QObject::tr("其他账号"), QObject::tr("取消"))) {
        case 0:
            cur_operID = lds::gs_operid;
            break;
        case 1:
            cur_operID = ui->comboBox_operator->curusrdata().toString();
            break;
        case 2:
            return;
        }
    }
    w_sys_manage_operatorright_pwdchange dialog(this);
    //默认选择是登录账号
    dialog.ui->lineEdit_cur_operator->setText(lds::gs_operid);
    dialog.ui->lineEdit_login_operator->setText(lds::gs_operid);
    //有选中 则修改账号
    dialog.ui->lineEdit_cur_operator->setText(cur_operID);
    //如果admin 来修改非admin 密码， 则旧密码不需要填写
    if(lds::gs_operid == "0000" && dialog.ui->lineEdit_cur_operator->text() !="0000") {
        lds::hideWidget(dialog.ui->label_oldpwd);
        lds::hideWidget(dialog.ui->lineEdit_oldpwd);
    }
    dialog.ui->lineEdit_login_operator->setEnabled(false);
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}

void w_sys_manage_operatorright::toupdaterights()
{
    int row = ui->comboBox_operator->currentIndex();
    QString rights = ui->comboBox_operator->model_data(row, 0, Qt::UserRole + 1).toString();
    QString vch_operID = ui->comboBox_operator->model_data(row, 0, Qt::UserRole).toString();
    if(rights.count() == 0)
        setCheckedSelect(vch_operID);
    else
        setCheckedRights(rights);
}

void w_sys_manage_operatorright::toSaveCheckbox()
{
    int row = ui->comboBox_operator->currentIndex();
    QString rights = ui->comboBox_operator->model_data(row, 0, Qt::UserRole + 1).toString();
    checkbox_phonestyle *b = qobject_cast<checkbox_phonestyle *>(this->sender());
    QString bright = b->getData(Qt::UserRole).toString();
    if(b->isChecked() && false == rights.contains(bright)) {
        if(rights.count() > 0)
            rights += ",";
        rights += bright;
    }
    if(false == b->isChecked()) {
        int index = rights.indexOf(bright);
        if(index >= 0) {
            rights.remove(index, bright.count() + 1);//111,
        }
    }
    ui->comboBox_operator->model_data_set(row, 0, rights, Qt::UserRole + 1);
}

void w_sys_manage_operatorright::updateSection(const QModelIndex &index)
{
    stack_widget_index = index.row();
    ui->stackedWidget->setCurrentIndex(index.row());
}

void w_sys_manage_operatorright::setCheckedAll(bool check)
{
    int row = ui->comboBox_operator->currentIndex();
    QString rights;
    for(int k = 0; k < checkbox_list.count(); k ++) {
        checkbox_phonestyle *b = checkbox_list[k];
        b->setChecked(check);
        if(check) {
            rights += b->getData(Qt::UserRole).toString() + ",";
        }
    }
    rights.chop(1);
    ui->comboBox_operator->model_data_set(row, 0, rights, Qt::UserRole + 1);
}

void w_sys_manage_operatorright::setCheckedSelect(const QString &vch_operID)
{
    int row = ui->comboBox_operator->currentIndex();
    ui->comboBox_operator->model_data_set(row, 0, lds_query_hddpos::selectValue(QString("SELECT group_concat(ch_purviewno) FROM cey_sys_oper_purview where vch_operID = '%1' ").arg(vch_operID)), Qt::UserRole + 1);
    setCheckedRights(ui->comboBox_operator->model_data(row, 0, Qt::UserRole + 1).toString());
}

void w_sys_manage_operatorright::setCheckedRights(const QString &rights)
{
    for(int k = 0; k < checkbox_list.count(); k ++) {
        checkbox_list[k]->setChecked(rights.contains(checkbox_list[k]->getData(Qt::UserRole).toString()));
    }
}

void w_sys_manage_operatorright::createItem(QWidget *parent, const OperatorRight &right, int index)
{
    checkbox_phonestyle *b = new checkbox_phonestyle(parent);
    b->setText(qAppTr(right.value));
#ifdef QT_DEBUG
    b->setText("(" + right.key + ")" + b->text() );
#endif
    b->setData(Qt::UserRole, right.key);
    b->setUnderline(right.authorized);
    connect(b, SIGNAL(clicked()), this, SLOT(toSaveCheckbox()));
    checkbox_list.append(b);

    b->setGeometry(5 + (index % 2) * (305 + 5), (index / 2) * (30 + 5) + 5, 305, 30);
}
