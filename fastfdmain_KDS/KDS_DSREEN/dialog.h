#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QStyledItemDelegate>
#include <QUdpSocket>
#include <QLabel>
#include "ftableview_standmodel_sql_none.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void processPendingDatagrams();
private:
    void datagram2model(const QByteArray &gram);
    void readLabel();
private:
    Ui::Dialog *ui;
    ftableview_standmodel_sql_none *model_service;
    ftableview_standmodel_sql_none *model_cooking;
    QUdpSocket *udpSocket;

private:
    class kds_dcreen_service_delegate : public QStyledItemDelegate {
    public:
        kds_dcreen_service_delegate(QObject *parent) : QStyledItemDelegate(parent) {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
};

#endif // DIALOG_H
