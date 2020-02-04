#include "lds_selectgoods.h"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QToolButton>
#include "lds_query_hddpos.h"
#include <QSqlRecord>
#include "backheader.h"
#include <QPushButton>
#include "lds_menu.h"
#include "backheader.h"
#include "lds_messagebox.h"
#include "lds_toolbar_button.h"
#include <QTimer>
#include "lds_model_sqltablemodel.h"
#include "lds_model_sqltablemodel_delegate_com.h"

lds_selectgoods::lds_selectgoods(QWidget *parent) :
    QWidget(parent)
{
    //layout
    line=new QLineEdit(this);
    lds_toolbar_button *tool=new lds_toolbar_button(this);
    tool->setText("...");
    QHBoxLayout *hlayout=new QHBoxLayout;

    tool->setMaximumSize(60,30);
    tool->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tool->setFocusPolicy(Qt::NoFocus);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hlayout->addWidget(line,1);
    hlayout->addWidget(tool);
    hlayout->setMargin(0);
    hlayout->setSpacing(0);

    setLayout(hlayout);
    connect(tool,SIGNAL(clicked()),this,SLOT(show_Selectgoods()));

    this->setFixedHeight(30);
}

QString lds_selectgoods::gettext()
{
    return line->text();
}

void lds_selectgoods::settext(const QString &text)
{
    line->setText(text);
}

void lds_selectgoods::show_Selectgoods()
{
    Selectgoods dialog(this);
    connect(&dialog,SIGNAL(okstr(QString)),line,SLOT(setText(QString)));
    connect(&dialog,SIGNAL(okstr1(QString)),this,SIGNAL(okstr1(QString)));
    dialog.setWindowTitle(_TEXT_SLOT(this));
    transparentCenterDialog(&dialog).exec();
}


//=========================================
#include "ui_selectgoods_dialog.h"

Selectgoods::Selectgoods(QWidget *parent):
    QDialog(parent),
    ui(new Ui_selectgoods_Widget)
{
    ui->setupUi(this);

    //! tableview
    tablemodel=new lds_model_sqltablemodel(this);
    tablemodel->setTable("cey_bt_dish");
    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_dish_typeno"),new  lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish_type", "ch_dish_typeno","vch_dish_typename"));
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_sub_typeno"),new  lds_model_sqltablemodel_delegate_com(this, "cey_bt_dish_subtype", "ch_sub_typeno","vch_sub_typename"));

    //    ui->tableView->setSortingEnabled(true);
    //! toselect
    toselect();
    //重新布局表头和隐藏相关表头
    QList<int> virtualheades;
    tablemodel->save_set_header(0,QObject::tr("品码"));
    tablemodel->save_set_header(1,QObject::tr("品名"));
    tablemodel->save_set_header(4,QObject::tr("大类"));
    tablemodel->save_set_header(14,QObject::tr("小类"));
    virtualheades=ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());

    //! resize
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(toselect(QString)));

    ui->pushButton->setProperty("outer_stylesheet",QVariant("pushbutton"));
    this->setFixedSize(600, 400);

}

void Selectgoods::setselectmultimode()
{
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
}

void Selectgoods::setitem_flag(const QChar &flag)
{
    _item_flag=QString(flag);
    this->setWindowTitle(this->windowTitle() + "," + QObject::tr("直销商品"));
    toselect(ui->lineEdit->text());
}

void Selectgoods::setstcheck_sheetno()
{
    setitem_flag('Y');//直销商品
    _check_sheetno =
            " ("
            "not exists (select cey_st_check_detail.ch_materialno "
            " from cey_st_check_master, cey_st_check_detail"
            " where cey_st_check_master.ch_sheetno = cey_st_check_detail.ch_sheetno"
            " and cey_st_check_master.ch_state = 'N'  and cey_st_check_detail.ch_materialno = cey_bt_dish.ch_dishno)"
            ")"
            ;

    this->setWindowTitle(this->windowTitle() + "," + QObject::tr("盘点商品"));
    toselect(ui->lineEdit->text());
}

void Selectgoods::toselect(const QString &filter)
{
    QString sqlselect;

    if(filter.isEmpty()) {

    } else {
        sqlselect +=QString(" (ch_dishno like '%1%' or vch_dishname like '%2%' or vch_dish_typename like '%3%' or vch_sub_typename like '%4%')and")
                .arg(filter)
                .arg(filter)
                .arg(filter)
                .arg(filter)
                ;
    }

    //是否停用
    sqlselect +=QString(" ch_stopflag like 'N' ");

    //外部直销商品过滤
    if(_item_flag.isEmpty()) {

    } else {
        sqlselect +=QString(" and item_flag='%1' ")
                .arg(_item_flag);
    }

    //外部盘点过滤
    if(_check_sheetno.isEmpty()) {

    } else {
        sqlselect +=QString(" and %1 ")
                .arg(_check_sheetno);
    }

    //检索
    tablemodel->setFilter(sqlselect);
    QTimer::singleShot(0, ui->tableView, SLOT(resizeColumnsToContents()));
}

void Selectgoods::took()
{
    QString linestr;
    QString vch_name;

    if(ui->tableView->currentIndex().row()>=0) {
        linestr=tablemodel->model_data(ui->tableView->currentIndex().row(), "ch_dishno").toString();
        vch_name=tablemodel->model_data(ui->tableView->currentIndex().row(), "vch_dishname").toString();
        emit okstr(linestr);
        emit okstr1(vch_name);
    }

    if(ui->tableView->selectionMode()&QAbstractItemView::MultiSelection) {
        QStringList infos;
        QList<int> rows=ui->tableView->getSelectedRowList();
        foreach(int row, rows) {
            infos.append(tablemodel->model_data(row, "ch_dishno").toString());
        }
        emit okstrs(infos);
    }

    ui->tableView->clearSelection();
    this->accept();
}
