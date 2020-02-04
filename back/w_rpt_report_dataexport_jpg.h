#ifndef W_RPT_REPORT_DATAEXPORT_JPG_H
#define W_RPT_REPORT_DATAEXPORT_JPG_H

#include <QDialog>

namespace Ui {
class w_rpt_report_dataExport_JPG;
}

class w_rpt_report_dataExport_JPG : public QDialog
{
    Q_OBJECT

public:
    explicit w_rpt_report_dataExport_JPG(const QString &savePath, QWidget *parent = 0);
    ~w_rpt_report_dataExport_JPG();

    void setLabelPixmap(const QPixmap &pixmap);
private slots:
    void toprint();
    void tocancel();
    void toexport();
private:
    Ui::w_rpt_report_dataExport_JPG *ui;
    QPixmap pixmap;
    QString savePath;
};

#endif // W_RPT_REPORT_DATAEXPORT_JPG_H
