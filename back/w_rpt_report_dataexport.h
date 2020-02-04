#ifndef W_RPT_REPORT_DATAEXPORT_H
#define W_RPT_REPORT_DATAEXPORT_H

#include <QDialog>
#include <QTableView>
#include "lds_tableview.h"
#include <QStyledItemDelegate>

class QAbstractItemModel;
namespace Ui {
class w_rpt_report_dataExport;
}

//windowtitle 即为导出文件的文件名
class w_rpt_report_dataExport : public QDialog
{
    Q_OBJECT
public:
    enum ExportType{
        ExportTypeTABLE,
        ExportTypeSQL,
        ExportTypeMASTER
    };
    static QFont fontPixelSize(QFont font, int pixelSize);
    static void getExportImage(const QString &title, QString write_byte, QPixmap &pixmap, int pixmap_width, bool hasGrid = true);
    class ItemDelegateForArabic : public QStyledItemDelegate{
    public:
        ItemDelegateForArabic(QObject *parent = 0);
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    };

public:
    //////////TABLE
    w_rpt_report_dataExport(lds_tableView *content_tableview,
                            lds_tableView *tail_tableview,QWidget *parent = 0);
    //////////SQL
    w_rpt_report_dataExport(lds_tableView *content_tableview,
                            const QString &content_sql, lds_tableView *tail_tableview, QWidget *parent = 0);
    //////////MASTER
    w_rpt_report_dataExport(const QString & head_sql,
                            lds_tableView *content_tableview, lds_tableView *tail_tableview,QWidget *parent = 0);

    ~w_rpt_report_dataExport();
    void defFilepath(const QString &tabtext);
public slots:
    void toexit();
    void toexport();
    void toexport_JPG();
    void toumount_upan();
    void export_info_progress(const QString &info, int v, int total);
    void togetfilepath();
private:
    void getExportData(QString &write_byte);
    ExportType export_type;
    void initData();
private:
    Ui::w_rpt_report_dataExport *ui;

    QString head_sql;
    QString content_sql;
    lds_tableView *content_tableview;
    lds_tableView *tail_tableview;
};

#endif // W_RPT_REPORT_DATAEXPORT_H
