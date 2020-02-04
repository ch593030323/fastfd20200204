#ifndef TABLEVIEW_ITEM_H
#define TABLEVIEW_ITEM_H

#include <QString>
#include <QImage>
#include <QBitArray>
#include <QTableView>
#include <QItemDelegate>
#include <QTimer>

class tableview_item_text;

class tableview_item : public QTableView
{
    Q_OBJECT
public:
    typedef  QList<QBitArray> BG_RUN_SET;

public:
    tableview_item(QWidget *parent = 0);
    ~tableview_item();

    void initScene(const QString & imagepath);
    void showPixmap(const QString &mpath);//之前要手动停止timer
    void showPixmap(const QPixmap &m);//之前要手动停止timer
    void start(int msec);
    void start();
    void stop();

signals:
    void signal_pixmap_go();
    void signal_mouse_pressed();

protected:
    virtual void closeEvent(QCloseEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void hideEvent(QHideEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
private slots:
    void togo();
private:
    QAbstractTableModel *model;
    QTimer *timer;
};

class tableview_item_delegate : public QItemDelegate
{
public:
    tableview_item_delegate(QTableView *tableview, QObject *parent);
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const ;
    QTableView *tableview;

};

class tableview_item_model : public QAbstractTableModel{
    Q_OBJECT
public:
    tableview_item_model(QObject *parent = 0):QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
    QVariant headerData(int /*section*/, Qt::Orientation /*orientation*/, int role = Qt::DisplayRole) const ;

};
#endif // TABLEVIEW_ITEM_H
