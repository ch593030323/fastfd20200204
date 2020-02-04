#ifndef KDS_TABLEVIEW_H
#define KDS_TABLEVIEW_H

#include <QTableView>

class kds_tableview : public QTableView
{
    Q_OBJECT
public:
    explicit kds_tableview(QWidget *parent = 0);

    void emitClicked(const QModelIndex &index);

};

#endif // KDS_TABLEVIEW_H
