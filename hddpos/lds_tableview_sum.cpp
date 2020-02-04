#include "lds_tableview_sum.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QDebug>
#include <QHeaderView>
#include <QStandardItemModel>
#include "lds_tableview.h"
#include "lds_model_sqlstandardmodel.h"

lds_tableview_sum::lds_tableview_sum(QWidget *parent) :
    QWidget(parent)
{
    //tableView
    horizontalScrollBar = new QScrollBar(Qt::Horizontal, this);
    verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    tableView = new lds_tableView(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//若不设置的话，tableView的默认高度正好少horizonal的高度
    tableView->setShowNest(true);

    lineView = new lds_tableView(this);
    lineView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lineView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lineView->setStyleSheet("color:#ff546b;");
    lineView->setFixedHeight(30+lineView->frameWidth() * 2);
    lineView->verticalHeader()->setDefaultSectionSize(lineView->height());
    lineView->horizontalHeader()->hide();

    //layout
    QGridLayout *gridLayout = new QGridLayout();
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    QHBoxLayout *hblayout=new QHBoxLayout;
    verticalLayout->addWidget(tableView);
    verticalLayout->addWidget(lineView);
    verticalLayout->addLayout(hblayout);
    gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);
    gridLayout->addWidget(verticalScrollBar, 0, 1, 1, 1);
    gridLayout->addWidget(horizontalScrollBar, 1, 0, 1, 1);

    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(2);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    hblayout->setSpacing(10);
    hblayout->setContentsMargins(0,0,0,0);
    setLayout(gridLayout);

    //lineView
    linemodel=new lds_model_sqlstandardmodel(this);
    linemodel->enableNoItemFlags();
    lineView->setModel(linemodel);
    lineView->hide();

    horizontalScrollBar->hide();
    verticalScrollBar->hide();

    //connect
    connect(tableView->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(updateHorizontalScrollBarRange(int,int)));
    connect(tableView->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(updateVerticalScrollBarRange(int, int)));

    connect(tableView->horizontalScrollBar(), SIGNAL(valueChanged(int)), horizontalScrollBar, SLOT(setValue(int)));
    connect(tableView->horizontalScrollBar(), SIGNAL(valueChanged(int)), lineView->horizontalScrollBar(), SLOT(setValue(int)));
    connect(tableView->verticalScrollBar(), SIGNAL(valueChanged(int)), verticalScrollBar, SLOT(setValue(int)));

    connect(horizontalScrollBar, SIGNAL(valueChanged(int)), tableView->horizontalScrollBar(), SLOT(setValue(int)));
    connect(verticalScrollBar, SIGNAL(valueChanged(int)), tableView->verticalScrollBar(), SLOT(setValue(int)));
}

void lds_tableview_sum::setModel(QAbstractItemModel *model)
{
    parentmodel=model;
    tableView->setModel(model);

    //更新lineview的数据，只保留一行
    linemodel_remove_and_filldata();
    lineView->verticalHeader()->setHidden(0 == parentmodel->rowCount());
}

QStringList lds_tableview_sum::updateSumRange(int from, int to)
{
    QStringList ret_list;
    for(;from <= to; from ++) {
        ret_list << updateSum(from);
    }
    return ret_list;
}

QString lds_tableview_sum::updateSum(int column, const QVariant &value)
{
    updatelinemodel();
    //更新lineview的数据，只保留一行
    if(linemodel->rowCount() <= 0
            ||linemodel->columnCount() <= column) {
        return "";
    }

    linemodel->item(0, column)->setData(value, Qt::DisplayRole);
    return lineView->hh_paint_transfer(column);

}

void lds_tableview_sum::setEditTriggers(QAbstractItemView::EditTriggers triggers)
{
    tableView->setEditTriggers(triggers);
}

QList<int> lds_tableview_sum::rePecifyHeader(const QStringList &tos)
{
    lineView->rePecifyHeader(tos);
    update_total_str();
    return tableView->rePecifyHeader(tos);
}

QList<int> lds_tableview_sum::rePecifyHeader(const QList<int> &tos)
{
    lineView->rePecifyHeader(tos);
    update_total_str();
    return tableView->rePecifyHeader(tos);
}

void lds_tableview_sum::setSelectionBehavior(QAbstractItemView::SelectionBehavior behavior)
{
    tableView->setSelectionBehavior(behavior);
}

void lds_tableview_sum::setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate)
{
    tableView->setItemDelegateForColumn(column, delegate);
}

void lds_tableview_sum::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    tableView->setSelectionMode(mode);
}

void lds_tableview_sum::setItemDelegate(QAbstractItemDelegate *delegate)
{
    tableView->setItemDelegate(delegate);
}

QHeaderView *lds_tableview_sum::horizontalHeader() const
{
    return tableView->horizontalHeader();
}

QHeaderView *lds_tableview_sum::verticalHeader() const
{
    return tableView->verticalHeader();
}

void lds_tableview_sum::resizeColumnsToContents()
{
    tableView->resizeColumnsToContents();

    for(int c = 0, c_count = tableView->horizontalHeader()->count(); c < c_count; c ++) {
        if(tableView->isColumnHidden(c)) {
            lineView->setColumnHidden(c, tableView->isColumnHidden(c));
        } else {
            tableView->setColumnWidth(c, qMax(lineView->columnWidth(c), tableView->columnWidth(c)));
            lineView->setColumnWidth(c, tableView->columnWidth(c));
        }
    }
}

void lds_tableview_sum::setTransferHheader()
{
    tableView->setTransferHheader();
    lineView->setTransferHheader();
    lineView->horizontalHeader()->setFixedHeight(30);//新增的lineViewheight也必须是30
}

QModelIndex lds_tableview_sum::currentIndex() const
{
    return tableView->currentIndex();
}

QAbstractItemDelegate *lds_tableview_sum::itemDelegateForColumn(int column) const
{
    return tableView->itemDelegateForColumn(column);
}

void lds_tableview_sum::restore(int defrow)
{
    tableView->restore(defrow);
}

void lds_tableview_sum::lineView_show()
{
    lineView->show();
}

QString lds_tableview_sum::updateSum(int column)
{
    updatelinemodel();
    //更新lineview的数据，只保留一行
    if(linemodel->rowCount() <= 0
            ||linemodel->columnCount()<=column) {
        return "";
    }

    QVariant::Type sum_type = QVariant::Invalid;
    double f_sum = 0.0;
    for(int r = 0,r_count = parentmodel->rowCount(); r < r_count; r++) {
        QVariant d = parentmodel->index(r, column).data();
        f_sum += d.toDouble();
        if(sum_type == QVariant::Invalid)
            sum_type = qMax(sum_type, d.type());
    }
    QVariant sum;
    if(sum_type >= QVariant::Double) {
        sum = f_sum;
    } else {
        sum = qlonglong(f_sum);
    }

    linemodel->model_data_set(0, column, sum);
    return lineView->hh_paint_transfer(column);
}

void lds_tableview_sum::linemodel_remove_and_filldata()
{
    linemodel->removeRows(0, linemodel->rowCount());
    linemodel->removeColumns(0, linemodel->columnCount());
    for(int c = 0, c_count=parentmodel->columnCount(); c < c_count; c++) {
        linemodel->setItem(0, c, new QStandardItem);
        linemodel->setHeaderData(c, Qt::Horizontal, parentmodel->headerData(c, Qt::Horizontal));
    }
    update_total_str();
}

void lds_tableview_sum::updateVerticalScrollBarRange(int min, int max)
{
    verticalScrollBar->setRange(min,max);
    verticalScrollBar->setVisible(max > 0);
}

void lds_tableview_sum::updateHorizontalScrollBarRange(int min, int max)
{
    horizontalScrollBar->setRange(min, max);
    horizontalScrollBar->setVisible(max>0);
}

void lds_tableview_sum::updatelinemodel()
{
    if(linemodel->rowCount() == 0 || linemodel->columnCount() == 0 || parentmodel->columnCount() != linemodel->columnCount()) {
        linemodel_remove_and_filldata();
    }
    lineView->verticalHeader()->setHidden(0 == parentmodel->rowCount());
    linemodel->setHeaderData(0, Qt::Vertical, parentmodel->rowCount() );
    lineView->show();
}

void lds_tableview_sum::update_total_str()
{
    if(linemodel->rowCount() > 0)
        linemodel->model_data_set(0, lineView->rePecifyHeaderList.value(0, 0), QObject::tr("合计"));
}
