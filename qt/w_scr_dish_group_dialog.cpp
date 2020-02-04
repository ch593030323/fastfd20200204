#include "w_scr_dish_group_dialog.h"
#include "ui_w_scr_dish_group_dialog.h"
#include "lds_query_hddpos.h"
#include "ftableview_delegate.h"
#include "lds.h"
#include "fexpandmain_2.h"
#include "lds_messagebox.h"
#include "w_scr_dish_main.h"
#include "public_printby_ch_billno.h"


w_scr_dish_group_dialog::w_scr_dish_group_dialog(const QString &ch_dishno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_scr_dish_group_Dialog)
{
    ui->setupUi(this);
    model_dish = new ftableview_standmodel_sql(ftableview_standmodel_sql::TYPE_icon);
    model_dish->enableEndAppendNew(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
    model_dish->imagedishdir = lds::sysconf->value("system_setting/bofanglujing").toString();
    ui->tableView_dish->setModel(model_dish);
    ui->tableView_dish->enableLongPress(n_func::f_get_sysparm("qt_forbide_change_new","0") == "0");
    /// ---------------delegate -------------------begin
    //修改delegate的某些样式单独修改
    ftableview_delegate *d = new ftableview_delegate(ui->tableView_dish);
    ui->tableView_dish->setProperty("outer_stylesheet", "tableView_dish");
    d->setColorLight(3);
    d->keepConnect(ui->tableView_dish);
    d->set_pay_unit_prefix(n_func::f_get_sysparm("pay_unit_prefix", " "));
    ui->tableView_dish->setItemDelegate(d);
    /// ---------------delegate -------------------end
    ui->tableView_dish->updateFixed_fillData_addAnimation(6, 3, 100, 100);
    ui->tableView_dish->setSelectionMode(QTableView::MultiSelection);

    //刷新 type subtype dish

    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->tableView_dish,SIGNAL(timeout(QModelIndex)),this,SLOT(toLongPressUpdateDishImage(QModelIndex)));
    connect(ui->tableView_dish,SIGNAL(signalclickNew(QModelIndex)),this,SLOT(toLongPressUpdateDishImage(QModelIndex)));

    refresh(ch_dishno);
}

w_scr_dish_group_dialog::~w_scr_dish_group_dialog()
{
    delete ui;
}

bool w_scr_dish_group_dialog::check_group(const QString &ch_dishno)
{
    lds_query_hddpos  query;
    QString ch_groupno;
    query.execSelect(QString("select ch_groupno from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(ch_dishno));
    query.next();
    ch_groupno = query.recordValue("ch_groupno").toString();
    query.execSelect(QString("select count(ch_groupno) from cey_bt_dish where ch_groupno = '%1' ")
                     .arg(ch_groupno));
    query.next();

    return query.recordValue(0).toInt() > 1;
}

void w_scr_dish_group_dialog::refresh(const QString &ch_dishno)
{
    QString vch_dishname;
    lds_query_hddpos  query;
    query.execSelect(QString("select ch_groupno, vch_dishname from cey_bt_dish where ch_dishno = '%1' ")
                     .arg(ch_dishno));
    query.next();
    ch_groupno = query.recordValue("ch_groupno").toString();
    vch_dishname = query.recordValue("vch_dishname").toString();

    QString dishname;
    QString dishname_2;
    public_printby_ch_billno::dishname_split(lds::sysconf->value("w_scr_dish_main/dishnameswitch", "vch_dishname").toString(), dishname, dishname_2, "a");//cey_bt_dish a

    model_dish->sql_cur =
            " select "
            " CONCAT(" + dishname + ",'\n', a.num_price) as name_price"//描述
            ", a.ch_dishno as dishno1"//品码
            ", a.ch_dishno as dishno2"//图片路径
            ", 0 group_count"
            ", if(a.item_flag = 'Y', ifnull( (select num_num from cey_st_material_num where ch_materialno = a.ch_dishno), 0),  '') as dish_st_remain"//库存剩余
            ", 0 as dishcount "
            ", '' as ischecked "
            ", num_item_warn "
            ", (select GROUP_CONCAT(concat(AA.ch_itemno, '/', ifnull(BB.num_num, ''), '/', CC.num_item_warn)) from material_bt_dish AA,  cey_st_material_num BB, cey_bt_dish CC where AA.ch_itemno = BB.ch_materialno and AA.ch_itemno = CC.ch_dishno and ifnull(CC.item_flag, '') = 'Y' and AA.ch_dishno = a.ch_dishno) as ingredient_st "
            ",a.vch_color"
            "," + dishname_2 + " as dishname_2"
            " from cey_bt_dish a "
            " where a.ch_groupno =  " + QString(" '%1' ").arg(ch_groupno) +
            " and a.ch_stopflag <> 'Y' and a.ch_tempflag <> 'Y' "
            ;

    this->setWindowTitle(vch_dishname + " "+ QObject::tr("组号"));
    ui->tableView_dish->clearSelection();
    model_dish->clearCache();//清除上一次选择的记录
    QTimer::singleShot(100, this, SLOT(torefresh()));
}

void w_scr_dish_group_dialog::retranslateView()
{
    ui->retranslateUi(this);
}

void w_scr_dish_group_dialog::took()
{
    QVariantList list = model_dish->currentSelected(1);//Qt::UserRole + 1
    if(list.count() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return;
    }
    ret_ch_dishno_list.clear();
    for(int k = 0; k < list.count(); k ++) {
        ret_ch_dishno_list << list[k].toString();
    }
    this->accept();
}

void w_scr_dish_group_dialog::toexit()
{
    this->reject();
}

void w_scr_dish_group_dialog::toLongPressUpdateDishImage(const QModelIndex &index)
{
    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;

    fexpandmain_2::updateDishImage(this, index, model_dish, "", ch_groupno);
}

void w_scr_dish_group_dialog::toLongPressUpdateDishImage_add()
{
    if(n_func::f_get_sysparm("qt_forbide_change_new","0") == "1")
        return;

    fexpandmain_2::updateDishImage_add(this, model_dish, "", ch_groupno);
}

void w_scr_dish_group_dialog::torefresh()
{
    model_dish->toinit();
    ui->tableView_dish->setCurrentIndex(model_dish->index(0, 0));
}
