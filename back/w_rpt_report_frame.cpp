#include "w_rpt_report_frame.h"
#include "ui_w_rpt_report_frame.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include <QStandardItemModel>
#include <QtDebug>
#include "w_rpt_report_dataexport.h"
#include "w_rpt_report_printformat.h"
#include "public_sql.h"
#include "n_func.h"
#include <QTableView>
#include "lds_model_sqltablemodel_delegate_com.h"
#include <QScrollBar>
#include "lds_tableview.h"
#include "w_rpt_report.h"
#include "lds.h"
#include "lds_model_sqltablemodel.h"
#include <QSqlRecord>
#include "lds_tableview_header.h"
#include "lds_menu.h"
#include <QTableWidget>

QMap<QString, QString> w_rpt_report_frame::map;

static QSqlQueryModel *get_model_default_insert_one_row(QObject *parent) {
    QSqlQueryModel *m = new report_querymodel(parent);
    return m;
}

static lds_tableView *get_tableview(QSqlQueryModel *m) {
    lds_tableView*tableview = new lds_tableView;
    tableview->setTransferHheader();
    tableview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableview->setSelectionMode(QTableView::SingleSelection);
    tableview->setSelectionBehavior(QTableView::SelectRows);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setModel(m);
    return tableview;
}

w_rpt_report_frame::w_rpt_report_frame(const QStringList &title_list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_rpt_report_frame)
{
    ui->setupUi(this);
    //
    report_standmodel *m_sum = new report_standmodel(this);
    m_sum->setHorizontalHeaderLabels(QStringList() << QObject::tr("项目") << QObject::tr("合计"));
    ui->tableView_sum->verticalHeader()->hide();
    ui->tableView_sum->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView_sum->setModel(m_sum);
    ui->tableView_sum->setSelectionMode(QTableView::SingleSelection);
    ui->tableView_sum->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView_sum->setShowGrid(false);
    ui->tableView_sum->setFocusPolicy(Qt::NoFocus);
    ui->tableView_sum->resize_decimal_enable();
    QFont f = ui->tableView_sum->font();
    f.setPointSize(14);
    ui->tableView_sum->setFont(f);
    //dt
    QDateTime dt = n_func::f_get_sysdatetime();
    w_rpt_report::reportBeginDtSet(ui->dateTimeEdit);
    ui->dateTimeEdit_2->setDateTime(dt);
    //
    for(int k = 0; k < title_list.count(); k++) {
        ui->stackedWidget->insertWidget(0, get_tableview(get_model_default_insert_one_row(this)));
        ui->pushButton_step->insert_d(0);
    }
    ui->comboBox->addItems(title_list);
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushButton_step->update_map_widget_visiable(0);
    //
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)),this,SLOT(toupdate_limitbtn(int)));
    connect(ui->pushButton_step, SIGNAL(signal_step_change()),this,SLOT(toselect_page()));
    connect(ui->pushButton_up, SIGNAL(clicked()),this, SLOT(toup()));
    connect(ui->pushButton_down, SIGNAL(clicked()),this, SLOT(todown()));
    connect(ui->pushButton_step, SIGNAL(clicked()),this,SLOT(tochoosepage()));
    //
    sql_count_arg1 = "select count(0) from (select 0 %1 )t";
}

w_rpt_report_frame::~w_rpt_report_frame()
{
}

void w_rpt_report_frame::clearCache()
{
    map.clear();
}

void w_rpt_report_frame::sum_filldata(const QList<w_rpt_report_frame_btn_pageindex::total_desc_value_Data> &d_list)
{
    QStandardItemModel *m = static_cast<QStandardItemModel *>(ui->tableView_sum->model());
    m->removeRows(0,  m->rowCount());
    foreach(const w_rpt_report_frame_btn_pageindex::total_desc_value_Data &d, d_list) {
        QString fieldname = d.fieldname;
        m->appendRow(QList<QStandardItem *> ()
                     << new QStandardItem(fieldname.replace("`", "")) << report_standmodel::newItem(d.value));
        m->item(m->rowCount() - 1, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m->item(m->rowCount() - 1, 1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    ui->tableView_sum->resizeColumnsToContents();
    ui->tableView_sum->resize_decimal_evencolumn();

    QTimer::singleShot(10, this, SLOT(sum_hscroll_set_value_1()));
}

void w_rpt_report_frame::sum_hscroll_set_value_1()
{
    ui->tableView_sum->horizontalScrollBar()->setValue(1);
}

void w_rpt_report_frame::toup()
{
    if(false == ui->pushButton_step->up(ui->stackedWidget->currentIndex())) {
        toPageUp();
    }
}

void w_rpt_report_frame::todown()
{
    if(false == ui->pushButton_step->down(ui->stackedWidget->currentIndex())) {
        toPageDown();
    }
}

void w_rpt_report_frame::tochoosepage()
{
    lds_dialog_input inputdialog(this);
    inputdialog.ui->label->setText(QObject::tr("页数"));
    inputdialog.setWindowTitle(QObject::tr("选择页数"));
    inputdialog.setDotEnabled(false);
    inputdialog.ui->lineEdit->setText(QString::number(ui->pushButton_step->get_cur_page(ui->stackedWidget->currentIndex())));
    if(QDialog::Accepted==transparentCenterDialog(&inputdialog).exec()) {
        ui->pushButton_step->setpage(ui->stackedWidget->currentIndex(), inputdialog.ui->lineEdit->text().toInt() - 1);
    }
}

void w_rpt_report_frame::toupdate_limitbtn(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    ui->pushButton_step->setText(ui->pushButton_step->get_desc(index));
    ui->pushButton_step->update_map_widget_visiable(index);
    sum_filldata(ui->pushButton_step->get_sum_list_total_data(index));
}

void w_rpt_report_frame::toselect_page()
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());
    model->setQuery(ui->pushButton_step->get_limit_sql_update_desc(index));
    select_page_after();
    tableview->resizeColumnsToContents2();

    qDebug() << __FUNCTION__<< __LINE__<< model->query().lastQuery();
}

void report_querymodel::setQuery(const QString &query, const QSqlDatabase &db)
{
    qDebug() << this << ++querycount;
    return QSqlQueryModel::setQuery(query, db);
}

QVariant report_querymodel::model_data(int row, const QString &field_name) const
{
#ifdef QT_DEBUG
    if(this->fieldIndex(field_name) < 0) {
        qDebug() << __FUNCTION__<<row << field_name << this->fieldIndex(field_name);
        QObject *x = 0;
        x->children();
    }
#endif
    return data(this->index(row, this->fieldIndex(field_name)));
}

int report_querymodel::fieldIndex(const QString &fieldname) const
{
    for(int column = 0, columncount =  this->query().record().count(); column < columncount; column ++) {
        if(0 == this->query().record().fieldName(column).compare(fieldname, Qt::CaseInsensitive)) {
            return column;
        }
    }
    qWarning("fieldName not found") ;
    return -1;
}

void w_rpt_report_frame::toexport()
{
    exportSql();
}

void w_rpt_report_frame::toprint()
{
    printSql();
}

void w_rpt_report_frame::toprintmore()
{

}

void w_rpt_report_frame::toprint58()
{

}

void w_rpt_report_frame::toprint80()
{

}


void w_rpt_report_frame::toexit()
{
    this->reject();
}

void w_rpt_report_frame::toPageUp()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    tableview->toPageUp();
}

void w_rpt_report_frame::toPageDown()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    tableview->toPageDown();
}

void w_rpt_report_frame::exportSql()
{
    int index  = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_dataExport dialog(tableview, ui->pushButton_step->get_limit_sql(index), ui->tableView_sum, this);
    dialog.setWindowTitle(this->windowTitle());
    dialog.defFilepath(this->windowTitle());
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame::printSql()
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat dialog(w_rpt_report_printFormat::show_model_more, this->windowTitle(), lds::gs_operid, n_func::f_get_sysdatetime_str(),
                                    QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                                    ui->dateTimeEdit->dateTimeStr(), ui->dateTimeEdit_2->dateTimeStr(),
                                    tableview, ui->pushButton_step->get_limit_sql(index), ui->tableView_sum,
                                    this);
    dialog.setWindowTitle(MESSAGE_TITLE_VOID);
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame::exportModel()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    w_rpt_report_dataExport dialog(tableview, ui->tableView_sum, this);
    dialog.setWindowTitle(this->windowTitle());
    dialog.defFilepath(this->windowTitle());
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame::printModel()
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));

    w_rpt_report_printFormat dialog(w_rpt_report_printFormat::show_model_more,  this->windowTitle(),
                lds::gs_operid,
                n_func::f_get_sysdatetime_str(),
                QString("%1_%2").arg(this->metaObject()->className()).arg(index),
                ui->dateTimeEdit->dateTimeStr(),
                ui->dateTimeEdit_2->dateTimeStr(),
                tableview,
                ui->tableView_sum,
                this );
    dialog.setWindowTitle(MESSAGE_TITLE_VOID);
    transparentCenterDialog(&dialog).exec();
}

void w_rpt_report_frame::resize_column_to_content_t()
{
    QTimer::singleShot(0, this, SLOT(to_resize_column_to_content()));
}

void w_rpt_report_frame::select_page_after()
{

}

void w_rpt_report_frame::tableview_setpan(const QString &cmp_column, const QStringList &span_column_list)
{
    int index = ui->stackedWidget->currentIndex();
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->widget(index));
    report_querymodel *model = static_cast<report_querymodel *>(tableview->model());

    QList<int> column_list;
    foreach(const QString &c, span_column_list) {
        column_list << model->fieldIndex(c);
    }

    lds_tableview_virtual::tableviewSetpan(tableview, model->fieldIndex(cmp_column), column_list);
}

int w_rpt_report_frame::tableviewPageCells(lds_tableView *tableview)
{
    QTableWidget t;
    t.setColumnCount(1);
    return ( tableview->height() - tableview->horizontalScrollBar()->height()- t.horizontalHeader()->height() ) / tableview->verticalHeader()->defaultSectionSize();
}

void w_rpt_report_frame::to_resize_column_to_content()
{
    lds_tableView *tableview = static_cast<lds_tableView *>(ui->stackedWidget->currentWidget());
    tableview->resizeColumnsToContents();
}

QVariant report_standmodel::data(const QModelIndex &index, int role) const
{
    QVariant d = QStandardItemModel::data(index, role);

    if(b_data_transfer && role == Qt::TextColorRole) {
        if(index.column() % 2 == 1) {
            return QColor("#ff546b");
        }
    }

    return d;
}

QStandardItem *report_standmodel::newItem(const QVariant &value)
{
    QStandardItem *item = new QStandardItem;
    item->setData(value, Qt::EditRole);
    return item;
}

void report_standmodel::setup(lds_tableView *tableview)
{
    tableview->resize_decimal_enable();
    tableview->setModel(this);
}

void report_standmodel::disable_data_transfer()
{
    b_data_transfer = false;
}

void report_standmodel::appendVarList(const QVariantList &var_list)
{
    QList<QStandardItem*> items;
    foreach(const QVariant &d, var_list) {
        items << newItem(d);
    }
    appendRow(items);
}

