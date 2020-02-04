#ifndef FEXPANDMAIN_TABLE_H
#define FEXPANDMAIN_TABLE_H

#include <QTableView>
#include <QMouseEvent>
#include "lds_tableview_timeout.h"

class fexpandmain_table : public lds_tableview_timeout
{
    Q_OBJECT
    //////////////////////////////////////样式表//////////////////////////////////////
    Q_PROPERTY(QColor divColor READ getdivColor WRITE setdivColor DESIGNABLE true)
    Q_PROPERTY(QColor nameColor READ getnameColor WRITE setnameColor DESIGNABLE true)
    Q_PROPERTY(QColor nameSubColor READ getnameSubColor WRITE setnameSubColor DESIGNABLE true)
    Q_PROPERTY(QColor defColor READ getdefColor WRITE setdefColor DESIGNABLE true)
    Q_PROPERTY(QColor itemBgColor READ getitemBgColor WRITE setitemBgColor DESIGNABLE true)
public:
    static const int tableRowHeight;
public:
    QColor getdivColor();
    void setdivColor(const QColor &c);
    QColor divColor;

    QColor getnameColor();
    void setnameColor(const QColor &c);
    QColor nameColor;

    QColor getnameSubColor();
    void setnameSubColor(const QColor &c);
    QColor nameSubColor;

    QColor getdefColor();
    void setdefColor(const QColor &c);
    QColor defColor;

    QColor getitemBgColor();
    void setitemBgColor(const QColor &c);
    QColor itemBgColor;
public:
    int border;
signals:
    void signal_dishname_rect_release(const QModelIndex &index);
    void signal_suit_fold(const QModelIndex &index);

    void signal_add(const QModelIndex &index);
    void signal_sub(const QModelIndex &index);
    void signal_change(const QModelIndex &index);
    void signal_detail(const QModelIndex &index);
    void signal_delete(const QModelIndex &index);

public:
    explicit fexpandmain_table(QWidget *parent = 0) ;
    ~fexpandmain_table();

};

#endif // FEXPANDMAIN_TABLE_H
