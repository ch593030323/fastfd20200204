#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "vch_print_memodata.h"
#include "w_bt_dish_flag.h"
#include "w_bt_dish_back.h"
#include <QtNetwork>

class cJSON_path;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool dishOrder(const QString &json, QString &errstring);
    bool dishReturn(const QString &json, QString &errstring, QTcpSocket *tcp);

    QList<num_backData> fillBackinfo(cJSON_path *p);
private slots:
    void toTcpConnect();

    void toTcpDataAppend();

    void on_pushButton_kitchen_settings_clicked();

    void on_pushButton_kitchen_dish_settings_clicked();

    void on_pushButton_exit_clicked();

private:
    qint64 tcpWriteString(QTcpSocket *tcp, const QString &d);
    void showStatusMessage(const QString &message);
private:
    Ui::MainWindow *ui;
    QTcpServer *tcpserver;
    QMap<QTcpSocket *, QByteArray> tcpDataMap;

};

#endif // MAINWINDOW_H
