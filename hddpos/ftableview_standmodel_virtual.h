#ifndef FTABLEVIEW_STANDMODEL_VIRTUAL_H
#define FTABLEVIEW_STANDMODEL_VIRTUAL_H

#include <QStandardItemModel>
#include <QSqlRecord>
#include <QSet>
#include "lds_bool.h"

class QItemSelectionModel;
class QTableView;
class ftableview_standmodel_virtual : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(int step READ step WRITE setstep USER true)

public:
    enum TYPE{
        TYPE_icon,
        TYPE_list
    };
    typedef QMap<QString /*user + 1*/,  QVariantList /*user + 0->n*/>  usrVarMap;
    typedef QMapIterator<QString /*user + 1*/,  QVariantList /*user + 0->n*/>  usrVarMapIterator;
public:
    //model中默认Qt::UserRole+1是主键
    explicit ftableview_standmodel_virtual(TYPE t);

    QString imagedishdir;//  D:/Qt/projects/untitled151/image_dish_dir

    bool model_data_set(int row, int column, const QVariant &value, int role);
    bool model_data_set(int row, int column, const QMap<int, QVariant> &map);
    QVariant model_data(int row, int column, int role = Qt::DisplayRole) const;
    void set_row_column(int row_count, int column_count);

    void setSelectionModel(QItemSelectionModel *m);
public:
    virtual void togenerate(int index, int updownum, bool hasnext) = 0;
    virtual void togenerateNext() = 0;
    virtual void togeneratePre() = 0;
public slots:
    void setstep(int value);
    void refreshcur();
signals:
    void signalstepstart();
    void signalstepstop();
    void signalFinished();
protected:

    int step() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual void updatedata(int r, int c, const QMap<int, QVariant> &map);
    virtual void updatedata(int r, int c, const QVariant &u, const QVariant &u1, const QVariant &u2);
    virtual void updatedata(int r,int c, const QSqlRecord &record);
    void updatep2pdelay();//path2pixmap
    void indexBegin();
public:
    void clearData(int r, int c);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
protected:
    int curindex;
    int preindex;
    TYPE _type;

    //////////////////////////////1//////////////////////////////
public:
    void disablePageRecord();
    void removePageRecordDeep(int deep);

    bool hasSelected();
    void clearSelected();
    QVariantList currentSelected(int deep);
    const QMap<QString /*user + 1*/,  QVariantList /*user + 0->n*/> & currentSelected();
    QStringList currentSelected_usr_1();
public:
    bool page_record_set_data(const QString &usr_1, const QVariant &value, int role);
    bool page_record_remove(const QString &usr_1);
protected:
    QVariantList page_record_value(int deep);
    void page_record_refresh();
    void page_record_restore();

    usrVarMap  page_record;
    lds_bool_default_true b_page_record;
    QSet<int> page_record_skip_deep;
    //////////////////////////////1//////////////////////////////
private:
    int valuestep;
    int valuestep_pre;
    int user_role_equal_max;
    QItemSelectionModel *selectionmodel;
};

#endif // FTABLEVIEW_STANDMODEL_VIRTUAL_H
