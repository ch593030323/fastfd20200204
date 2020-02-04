#include "w_m_member_list_select.h"
#include "ui_w_m_member_list_select.h"
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QTimer>
#include "lds_messagebox.h"
w_m_member_list_select::w_m_member_list_select(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_m_member_list_select)
{
    ui->setupUi(this);
    //
    tablemodel = new lds_model_sqltablemodel(this);

    tablemodel->setEditStrategy(lds_model_sqltablemodel::OnManualSubmit);
    tablemodel->setTable("t_m_member");
    tablemodel->save_set_header("vch_memberno", QObject::tr("会员号"));
    tablemodel->save_set_header("vch_member", QObject::tr("会员"));
    tablemodel->save_set_header("ch_typeno", QObject::tr("类型"));
    tablemodel->save_set_header("vch_handtel", QObject::tr("手机号"));

    ui->tableView->setModel(tablemodel);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->rePecifyHeader(tablemodel->get_save_set_header_saver_column_list());
    ui->tableView->setItemDelegateForColumn(tablemodel->fieldIndex("ch_typeno"), new lds_model_sqltablemodel_delegate_com(
                                                this, tablemodel, "t_m_member_type","ch_typeno","vch_typename"
                                                ));
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    //
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));

    this->setWindowTitle(QObject::tr("会员"));
    //
    QTimer::singleShot(0, this, SLOT(torefresh()));
}

w_m_member_list_select::~w_m_member_list_select()
{
    delete ui;
}

void w_m_member_list_select::torefresh()
{
    tablemodel->select();
    ui->tableView->clearSelection();
}

void w_m_member_list_select::took()
{
    ret_m = "";
    int row = ui->tableView->currentIndex().row();
    if(row >=  0) {
        ret_m = tablemodel->model_data(row, "vch_memberno").toString();
    }
    this->accept();
}

void w_m_member_list_select::tocancel()
{
    this->reject();
}
