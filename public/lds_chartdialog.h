#ifndef LDS_CHARTDIALOG_H
#define LDS_CHARTDIALOG_H

#include <QDialog>

namespace Ui {
class lds_chartDialog;
}
class QAbstractItemModel;
class lds_chartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit lds_chartDialog(QWidget *parent = 0);
    ~lds_chartDialog();
private slots:
    void toswitch();
    void toexit();
private:
    Ui::lds_chartDialog *ui;
};

#endif // LDS_CHARTDIALOG_H
