#ifndef LABEL_IMG_HEADER_DIALOG_H
#define LABEL_IMG_HEADER_DIALOG_H

#include <QDialog>
class label_img_checkbox;

namespace Ui {
class label_img_header_dialog;
}

class label_img_header_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit label_img_header_dialog(QWidget *parent = 0);
    ~label_img_header_dialog();


private slots:
    void on_pushButton_printPage_clicked();

    void on_pushButton_design_clicked();

    void on_pushButton_left_clicked();

    void on_pushButton_down_clicked();

    void on_pushButton_top_clicked();

    void on_pushButton_right_clicked();

    void on_pushButton_scale_minus_clicked();

    void on_pushButton_scale_plus_clicked();

    void updatePrinter();

    void updatePrinter(const QString &printerName);

    void on_pushButton_printer_MMwidth_sub_clicked();

    void on_pushButton_printer_MMheight_sub_clicked();

    void on_pushButton_printer_MMheight_add_clicked();

    void on_pushButton_printer_MMwidth_add_clicked();

    void on_pushButton_selectAll_clicked();

private:
    void updateCheck();

    void checkTransfer(const QString &member, int op);

    void checkInit(const QString &text);

    void updatePrinter(const QString &printerName, int MMwidth, int MMheight);

    QList<label_img_checkbox *> checkList();

private:
    Ui::label_img_header_dialog *ui;
};

#endif // LABEL_IMG_HEADER_DIALOG_H
