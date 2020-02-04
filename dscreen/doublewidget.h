#ifndef DOUBLEWIDGET_H
#define DOUBLEWIDGET_H

#include <QDialog>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QStandardItemModel>
#include <QUdpSocket>
#include <QStyledItemDelegate>
#include "lds.h"

//当有新的连接是，才会有心跳
class QSettings;
class QFrame;
class fexpandmain_table;
class lds_picshow_item;
class Ui_doublewidget;
class tableview_item;

class doublewidget : public QDialog
{
    Q_OBJECT

public:
    static void showPayView(QWidget *tableview,  QFrame *&pay, const lds::DOUBLE_DATA &d);

    static  QString parentPID();

    static QString _dishpath();
    static QString _advpath();
    static QString _showdish();
    static QString _skinname();
    static QSize _primaryscreensize();
    static QSize _secondaryscreensize();

    static QString _type();
    static QString _layout();
    static QString _secondaryscreenx();

    static QSize xstringToSize(const QString &sizestr, const QString &defSize);
    static QString sizeToxstring(const QSize &size);
public:
    explicit doublewidget(QSize dsize ,QWidget *parent = 0);
    ~doublewidget();
protected slots:
    virtual void adc_timer_start() = 0;
    virtual void adc_timer_out() = 0;
    virtual void standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4) = 0;
    virtual void standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4) = 0;
    virtual void qr_code_adv_hold() = 0;

    virtual void total_settext(const QString &text);
    virtual void num_settext(const QString &text);

    virtual void scale_update(const QString &keys, const QString titles, const QString &suffixs);
    virtual void scale_update_numbers(const QString &numbers);
    virtual bool hlabels_will_update();

    virtual void update_pay_request(const lds::DOUBLE_DATA &data);
    virtual void update_skin_request(const lds::DOUBLE_DATA &data);
    virtual void update_paying_request(const lds::DOUBLE_DATA &data);
protected slots:
    void trytoclose();
    void processPendingDatagrams();
    void start();
    void toquit();
    void toAutoQuit();
protected:
//    tableview_item*expand;
    Ui_doublewidget *ui;
    QTimer *dishtimeouttimer;
    QString heartbeatresponse;
    QUdpSocket *udpSocket;
    QFrame *paywidget;
    QStandardItemModel *standmodel;
    QString FONT_DIGIT_Family;
};


class doublewidget_delegate : public QStyledItemDelegate {
public:
    doublewidget_delegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
    void keepConnect(fexpandmain_table *tableview);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;


    int *border;
    QColor *divColor;
    QColor *nameColor;
    QColor *nameSubColor;
    QColor *defColor;
    QColor *itemBgColor;
};


#endif // DOUBLEWIDGET_H
