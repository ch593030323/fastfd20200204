#ifndef BACK_VIEW_DIALOG_H
#define BACK_VIEW_DIALOG_H

#include <QDialog>
#include <QTableView>
#include <QItemDelegate>
#include "public_dialog.h"
#include "lds_roundeddialog_rect.h"
#include <QStandardItem>
#include "lds_model_sqltablemodel_d.h"
#include <QSqlRecord>

class Ui_back_view_dialog;
class QStandardItemModel;
class ftableview_standmodel_sql_none;
namespace Ui {
class back_view_dialog;
}
class back_view_tableview : public QTableView {
    Q_OBJECT
public:
    //////////////////////////////////////样式表//////////////////////////////////////
    Q_PROPERTY(QColor bgColor READ getbgColor WRITE setbgColor DESIGNABLE true)
    Q_PROPERTY(QColor textColor READ gettextColor WRITE settextColor DESIGNABLE true)
    Q_PROPERTY(QColor borderColor READ getborderColor WRITE setborderColor DESIGNABLE true)

    void setbgColor(const QColor &color);
    QColor getbgColor() const;
    QColor bgColor;

    void settextColor(const QColor &color);
    QColor gettextColor() const;
    QColor textColor;

    void setborderColor(const QColor &color);
    QColor getborderColor() const;
    QColor borderColor;
public:
    back_view_tableview(QWidget *parent = 0);
};

class back_view_dialog : public lds_roundeddialog_rect0
{
    Q_OBJECT
public:
    class itemDelegate : public QItemDelegate{
    public:
        itemDelegate(QObject *parent = 0):QItemDelegate(parent) {}
        ~itemDelegate();
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
        void keepConnect(back_view_tableview *tableview);

        QColor *bgColor;
        QColor *textColor;
        QColor *borderColor;

    };

    Ui_back_view_dialog *ui;
public:
    explicit back_view_dialog(QWidget *parent = 0);
    ~back_view_dialog();
    void tableview_span_column(QTableView *t, int c);

    static QPixmap get_fastfdbacklogo();
    static QStringList get_fastfdbacktext();
    virtual void retranslateView();

    QString dialogClassName();
public slots:
    void tofirstGoIn();
private slots:
    void toupdateSection(const QModelIndex &index);
    void topop(const QModelIndex &index);
protected:
    void keyPressEvent(QKeyEvent *e);
private:
    QTableView *creat_tableview();
    QStandardItem *createItem(const QString &direct, const QString &image_path, const QString &text, const QString &usr);

private:
    QMap<int, int> index_key_map;//row->>stackwidget_index
    QDialog *dialog;
    QModelIndex tableview_list_pre_row;
    ftableview_standmodel_sql_none * backlist_model;
    static QMap<QString, QImage> name_image_map;
};

#endif // BACK_VIEW_DIALOG_H
