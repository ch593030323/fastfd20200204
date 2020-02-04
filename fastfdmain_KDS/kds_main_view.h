#ifndef KDS_MAIN_VIEW_H
#define KDS_MAIN_VIEW_H

#include <QDialog>
#include <QStyledItemDelegate>
#include <QTcpServer>
#include <QTcpSocket>
#include "lds_model_sqltablemodel_m.h"
#include"kds.h"
#include"ftableview_standmodel_sql_none.h"
#include"kds_standmodel_sql.h"

namespace Ui {
class kds_main_view;
}
class kds_main_info_brief;
class kds_tableview;
class kds_main_view : public QDialog
{
    Q_OBJECT
private:
    QStringList uniqueStrList(QStringList list, const QString &str);
    QString strList2jsonArray(const QStringList &list);
public:
    explicit kds_main_view(QWidget *parent = 0);
    ~kds_main_view();

private slots:
    void toinit();
    void toservice();
    void toexit();
    void torefreshprepare();
    void torefreshpreparewarn();
    void torefreshcooking();

    void torefresh();
    void torefreshsingle();
    void toshowtablebrief(const QModelIndex &index);
    void toserviceclick(const QModelIndex &index);
    void togroupclick(const QModelIndex &index);
    void totake_tableclick(const QModelIndex &index);

    void tonum();
    void tostateback();
    void toloopChecknew();
private:
    void refresh();
    void refreshView();
    void refreshView(const QString &state);
    void resetGeometry();
    void resetGeometry(const QString &state);

    void servicetoken(const QModelIndex &index);
    void servicetoken(const QString &ch_billno, const QString &vch_plan_id);
    QVariant varMin(const QVariant &p1, int p2);
    QVariant varMin(const QVariant &p1, QDateTime p2);
    void model_tablename_kvmap_init(const QString &tablename);
    void setGeometrys(QWidget *w1, QWidget *w2, const QRect &rect);

    QString refresh_state_push(const QString &refresh_state);
    QString refresh_state_cur();
    QString refresh_state_old();
    QString refresh_state_left();

    QString setUpdateTime(const QString &dt);
    QString curUpdateTime();
    QString setParamerTime(const QString &dt);
    QString curParamerTime();
    QVariant conf_parameter_data(const QString &field_name);
    void conf_parameter_set(const QString &field_name, const QVariant &value);

    void viewPrepareHide();
    void viewShowWidget(QWidget *w, bool show = true);
    void viewExecHide();

    void fileAppend(const QString &str);
    void fileClear();

    void tableview_select_index_default_first(const QModelIndex &index, kds_tableview *tableview);
    void tableview_select_first(const QAbstractItemModel *model, kds_tableview *tableview);

    void kdsddsend();
protected:
    void keyPressEvent(QKeyEvent *e);
public:
    Ui::kds_main_view *ui;
    kds_standmodel_sql * model_group;
    kds_standmodel_sql * model_service;
    kds_standmodel_sql * model_take_table;
    QLineEdit * barcode_scan;
    QMap<QWidget*, bool> view_show_widget_map;
private:
    class kds_main_view_delegate : public QStyledItemDelegate    {
    public:
        kds_main_view_delegate(QObject *parent) : QStyledItemDelegate(parent) {}
    private:
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
    class kds_main_service_delegate : public QStyledItemDelegate {
    public:
        kds_main_service_delegate(QObject *parent) : QStyledItemDelegate(parent) {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
    class kds_main_group_delegate : public QStyledItemDelegate {
    public:
        kds_main_group_delegate(QObject *parent) : QStyledItemDelegate(parent) {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;
        int textWidth(QPainter *p, const QString &text) const;
        QFont pixelFont(QPainter *p, int pixel) const;
    };
    class kds_main_take_table_delegate : public QStyledItemDelegate {
    public:
        kds_main_take_table_delegate(QObject *parent) : QStyledItemDelegate(parent) {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
};

class kds_main_view_table_brief : public QDialog {

    Q_OBJECT
public:
    kds_main_view_table_brief(const QByteArray &json, QWidget *parent);
    ~kds_main_view_table_brief();
    void paintEvent(QPaintEvent *);
    QByteArray json;
};

class kds_main_view_timer : QObject {

    Q_OBJECT
public:
    kds_main_view_timer(QObject *parent = 0) : QObject(parent){}
    QSet<QString> set;
};

#endif // KDS_MAIN_VIEW_H
