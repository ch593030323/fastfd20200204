#ifndef W_SYS_MANAGE_BASICDATAIMPORT_PREVIEW_H
#define W_SYS_MANAGE_BASICDATAIMPORT_PREVIEW_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class w_sys_manage_basicdataimport_preview;
}

class w_sys_manage_basicdataimport_preview : public QDialog
{
    Q_OBJECT

public:
    explicit w_sys_manage_basicdataimport_preview(const QString &filepath, const QString &def_codec, QWidget *parent = 0);
    ~w_sys_manage_basicdataimport_preview();

    QString curCodec();
private slots:
    void took();
    void tocancel();
    void toFillModel();
private:
    int file_row_count(const QString &filepath, bool skip_empt_row = true);
private:
    Ui::w_sys_manage_basicdataimport_preview *ui;
    QString filepath;
    QStandardItemModel *model;
};

#endif // W_SYS_MANAGE_BASICDATAIMPORT_PREVIEW_H
