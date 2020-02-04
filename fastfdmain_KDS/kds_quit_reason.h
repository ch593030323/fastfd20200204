#ifndef KDS_QUIT_REASON_H
#define KDS_QUIT_REASON_H

#include <QDialog>
#include <QStyledItemDelegate>
#include"ftableview_standmodel_sql_none.h"

namespace Ui {
class kds_quit_reason;
}

class kds_quit_reason : public QDialog
{
    Q_OBJECT

public:
    explicit kds_quit_reason(QWidget *parent = 0);
    ~kds_quit_reason();

    QString reason();
private slots:
    void took();
    void tocancel();
    void totableclick(const QModelIndex &index);
private:
    Ui::kds_quit_reason *ui;
    ftableview_standmodel_sql_none * model;

    class kds_quit_reason_delegate : public QStyledItemDelegate {
    public:
        kds_quit_reason_delegate(QObject *parent) : QStyledItemDelegate(parent) {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

};

#endif // KDS_QUIT_REASON_H
