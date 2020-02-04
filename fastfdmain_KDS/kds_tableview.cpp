#include "kds_tableview.h"

kds_tableview::kds_tableview(QWidget *parent) :
    QTableView(parent)
{
}

void kds_tableview::emitClicked(const QModelIndex &index)
{
    emit clicked(index);
}
