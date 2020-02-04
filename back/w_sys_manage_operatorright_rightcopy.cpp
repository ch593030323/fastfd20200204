#include "w_sys_manage_operatorright_rightcopy.h"
#include "ui_w_sys_manage_operatorright_rightcopy_dialog.h"
#include <QSqlRecord>

w_sys_manage_operatorright_rightcopy::w_sys_manage_operatorright_rightcopy(const QString &vch_operID, const QString &vch_operator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui_w_sys_manage_operatorright_rightcopy_Dialog)
{
    ui->setupUi(this);
    //! tableview
    querymodel=new report_querymodel;
    this->vch_operID = vch_operID;

    querymodel->setQuery(QString("select vch_operID, vch_operator  from cey_sys_operator where vch_operID <> '%1' ")
                         .arg(vch_operID)
                         );
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setTransferHheader();
    ui->tableView->setModel(querymodel);
    ui->label_2->setText(vch_operator+"("+vch_operID+")");

    //! toolbar
    ui->widget->setup(QStringList() << QObject::tr("权限copy") << QObject::tr("退出"), Qt::AlignRight);
    connect(ui->widget->index_widget(QObject::tr("权限copy")), SIGNAL(clicked()),this,SLOT(tocopy()));
    connect(ui->widget->index_widget(QObject::tr("退出")), SIGNAL(clicked()),this,SLOT(toexit()));

}

void w_sys_manage_operatorright_rightcopy::tocopy()
{
    if(ui->tableView->currentIndex().row()>=0) {
        QString vch_operidto = querymodel->record(ui->tableView->currentIndex().row()).value("vch_operID").toString();
        QString errstring;
        lds_query_hddpos  query;
        //! <--transaction begin-->
        lds_query_hddpos::tran_saction();
        if(query.execDelete("cey_sys_oper_purview", QString("vch_operID = '%1'").arg(vch_operidto),  &errstring)) {
            if(query.execInsert("cey_sys_oper_purview",
                                QString(" select '%1' as vch_operID, ch_purviewno from  cey_sys_oper_purview where vch_operID = '%2' ")
                                .arg(vch_operidto)
                                .arg(vch_operID), &errstring)) {
                target = vch_operidto;
                lds_query_hddpos::com_mit();
                lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
                this->accept();
                return;
            }
        }
        lds_query_hddpos::roll_back();
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
        //! <--transaction end-->
    }
}

void w_sys_manage_operatorright_rightcopy::toexit()
{
    this->reject();
}
