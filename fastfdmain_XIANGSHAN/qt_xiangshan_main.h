#ifndef QT_XIANGSHAN_MAIN_H
#define QT_XIANGSHAN_MAIN_H

#include <QDialog>
#include <QtGui>
#include "printer_pos_serial.h"
#include <QItemDelegate>
#include <QtNetwork>
#include "qt_xiangshan_pay_info.h"

namespace Ui {
class qt_XIANGSHAN_main;
}

class qt_XIANGSHAN_main : public QDialog
{
    Q_OBJECT
public:
    struct FileData{
        QByteArray type;
        qlonglong pic_size;
        QString pic_name;
        QByteArray pic_byte;
    };
    static void reboot();
public:
    explicit qt_XIANGSHAN_main(QWidget *parent = 0);
    ~qt_XIANGSHAN_main();

private slots:
    void updategetweight(const QString &w);

    void toScaleRead();

    void toTcpNewComing();
    void toTcpDataComing();
    void toTcpWriteFile(QTcpSocket *tcp);

    void toReboot();

    void to_adv_show_cur_0();

    void toSetting();
    void toShutDown();
private:
    QString ledNet(const QString &net, const QString &flag_net);
    QString ledPrice(const QString &price);
    QString ledTotal(const QString &total);

    void appendRow(const QString &state, const QString &name, const QString &num, const QString &price, const QString &total);
    void updateRow(int row, const QString &state, const QString &name, const QString &num, const QString &price, const QString &total);
    void updateData(int row, const QString key, const QString &value);
    QString getData(int row, const QString &key);
    void clearAllRows();

    void show_pay_info(const QString &payName, const QString &payAmount);

private:
    QStandardItemModel *model;
    Printer_POS_SERIAL printer;
    QByteArray scale_pool;

    int adv_show_cur;
    QString adv_show_save_weight;
private:
    Ui::qt_XIANGSHAN_main *ui;
    QTcpServer *tcp;
    QMap<QTcpSocket *, FileData> socket_file_map;

    qt_XIANGSHAN_pay_info *pay_info_widget;
};

class itemDelegate : public QStyledItemDelegate {

    Q_OBJECT
public:
    itemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

#endif // QT_XIANGSHAN_MAIN_H
