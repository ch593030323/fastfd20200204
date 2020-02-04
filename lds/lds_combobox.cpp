#include "lds_combobox.h"
#include <QAbstractItemView>
#include <QTableView>
#include <QStandardItemModel>
#include <qdebug.h>
#include "lds_query_db.h"
#include <QTableView>
#include <QHeaderView>
#include "lds_query_db.h"
#include <QSqlDatabase>

lds_ComboBox *lds_ComboBox::createCom(const QString &key_value_sql)
{
    lds_ComboBox *c = new lds_ComboBox;
    c->addItemsBySql(key_value_sql);
    return c;
}

lds_ComboBox::lds_ComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

void lds_ComboBox::resetTableView(int verticalHeight)
{
    QTableView *table = new QTableView;
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
    table->verticalHeader()->setDefaultSectionSize(verticalHeight);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setShowGrid(false);

    setView(table);
}

void lds_ComboBox::addItemsByMap(const QMap<QString, QVariant> &texts)
{
    for(QMap<QString, QVariant>::const_iterator k = texts.begin(); k != texts.end(); k++) {
        this->addItem(k.value().toString(), k.key());
    }
}

void lds_ComboBox::addItemsBySql(const QString &key_value_sql)
{
    lds_query_db query(QSqlDatabase::database());//ui一定主线程的
    query.execSelect(key_value_sql);
    while(query.next()) {
        this->addItem(query.recordValue(1).toString(), query.recordValue(0));
        for(int k = 2; k < query.recordCount(); k ++) {
            model_data_set(this->count() - 1, 0, query.recordValue(k), Qt::UserRole + k - 1);
        }
    }
}

void lds_ComboBox::addItemsByModel(const QAbstractItemModel *key_value_model)
{
    for(int row = 0; row < key_value_model->rowCount(); row ++) {
        this->addItem(key_value_model->data(key_value_model->index(row, 1)).toString(),
                      key_value_model->data(key_value_model->index(row, 0)));
    }
}

void lds_ComboBox::addItemsByPairList(const QList<QPair<QString, QVariant> > &pair_list)
{
    for(int k = 0; k < pair_list.count(); k++) {
        this->addItem(pair_list[k].second.toString(), pair_list[k].first);
    }
}

void lds_ComboBox::setcomtext(const QString &text)
{
    for(int k = 0; k < this->count(); k ++) {
        if(itemText(k) == text) {
            this->setCurrentIndex(k);
            return;
        }
    }
    this->setCurrentIndex(-1);
}
/**
 * @brief lds_ComboBox::setcomusrdata
 * @param usrtext
 * desc 针对model和mapper的格式制定的函数，当model并没有relation时，可以对齐进行直接的修改
 *model-》com key；com-》model key;显示的value
 */

void lds_ComboBox::setcomusrdata(const QVariant &usrtext)
{
    QAbstractItemModel *model=this->model();
    if(model) {
        for(int r=0,r_count=model->rowCount();r<r_count;r++) {
            if(model->data(model->index(r, 0), Qt::UserRole)==usrtext) {
                setCurrentIndex(r);
                return;
            }
        }
    }
    setCurrentIndex(-1);
}

QVariant lds_ComboBox::comusrdata(int comindex)
{
    if(comindex == -1) {
        comindex = this->currentIndex();
    }
    QAbstractItemModel *model=this->model();
    if(model
            &&model->rowCount()>0
            &&comindex >=0
            &&comindex < model->rowCount()) {
        return model->data(model->index(comindex, 0), Qt::UserRole);
    }
    return QVariant();
}

QVariant lds_ComboBox::curusrdata()
{
    return comusrdata(this->currentIndex());
}

void lds_ComboBox::setEitherUsrOrText(const QVariant &text)
{
    setcomusrdata(text);
    if(-1 == currentIndex())
        setEditText(text.toString());
}

QVariant lds_ComboBox::eitherUsrOrText()
{
    QVariant d = curusrdata();
    if(currentText() != comText(d)) {
        return currentText();
    }
    return d;
}

QString lds_ComboBox::comText(const QVariant &d)
{
    for(int k = 0, count = this->count(); k < count; k++) {
        if(itemData(k) == d) {
            return itemText(k);
        }
    }
    return "";
}


void lds_ComboBox::enableEmitColumn(int emitColumn)
{
    this->emitColumn = emitColumn;
    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(toEmitColumn(int)));
}

QVariant lds_ComboBox::model_data(int row, int column, int role)
{
    QAbstractItemModel *model=this->model();
    if(model == 0
            || row >= model->rowCount()
            || column >= model->columnCount()) return QVariant();

    return model->data(model->index(row, column), role);
}

bool lds_ComboBox::model_data_set(int row, int column, const QVariant &value, int role)
{
    QAbstractItemModel *model=this->model();
    if(model == 0
            || row >= model->rowCount()
            || column >= model->columnCount()) return false;
    return model->setData(model->index(row, column), value, role);
}

void lds_ComboBox::copyItemsFrom(QComboBox *com)
{
    clear();
    for(int row = 0; row < com->count(); row ++) {
        addItem(com->itemText(row),  com->itemData(row));
    }
}

void lds_ComboBox::copyItemsTo(QComboBox *com)
{
    com->clear();
    for(int row = 0; row < this->count(); row ++) {
        com->addItem(this->itemText(row),  this->itemData(row));
    }
}

void lds_ComboBox::toEmitColumn(int index)
{
    QAbstractItemModel *model=this->model();
    if(!model) return;
    emit signalEmitColumn(model->index(index, emitColumn).data().toString());
}

void lds_ComboBox::toScrollDown()
{
    this->setCurrentIndex(this->currentIndex() + 1);
}

void lds_ComboBox::toScrollUp()
{
    this->setCurrentIndex(this->currentIndex() - 1);
}
