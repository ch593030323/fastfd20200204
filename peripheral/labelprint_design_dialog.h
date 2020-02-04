#ifndef LABELPRINT_DESIGN_DIALOG_H
#define LABELPRINT_DESIGN_DIALOG_H

#include <QDialog>
#include "s_mainwindow.h"
class printer_label_postek;
class printer_label_tsc_ttp_244_plus;
class printer_label_argox_PPLA;
class printer_label_argox_PPLB;
class Printer_POS_SERIAL;
class printer_label_gainscha;
class CommonHeader;
class QComboBox;
class S_MainWindow;
class Ui_TagprintSet;
class labelprint_set_tagbarcode_dialog;
namespace Ui {
class labelprint_design_dialog;
}

class labelprint_set_dialog;
class labelprint_design_dialog : public QWidget
{
    Q_OBJECT
public:
    enum DIRECT{
        LEFTTOP,
        RIGHTBOTTOM
    };
public:
    explicit labelprint_design_dialog(QWidget *parent = 0);
    ~labelprint_design_dialog();
    S_MainWindow *design;
    labelprint_set_tagbarcode_dialog *setdialog;
    labelprint_set_dialog *setting_printer;
    void designdialog_save();
    void tosave();//保存默认的
    void toload();//导入默认的

    void designprint(QWidget *parent, bool istest=false, bool pop=true);

    void setcurrentIndex(int index);
public slots:
    void toprint(bool istest=false, bool pop=true);
private slots:
    void toswitch();
    void todefbill();
    void toimportbill();
    void tosavebill();

    void totestprint();
    void on_pushButton_importbill_clicked();

private:
    /*    fsline->addItem("标准", "11");
    fsline->addItem("倍高", "12");
    fsline->addItem("倍宽", "21");
    fsline->addItem("倍高倍宽", "22");*/
    QString print_piece_POSTEK(printer_label_postek &printer, int rr, int start_x = 0);
    QString print_piece_TSC(printer_label_tsc_ttp_244_plus &printer, int rr, int start_x = 0);
    QString print_piece_ARGOX_PPLA(printer_label_argox_PPLA &printer, int rr, int start_x = 0);
    QString print_piece_ARGOX_PPLB(printer_label_argox_PPLB &printer, int rr, int start_x = 0);
    QString print_piece_GAINSCHA(printer_label_gainscha &printer, int rr, int start_x = 0);

    QSize getsizeby14(const QString &size_str);
    float valueTranslate(float value, const QString &unit, const QString &rate);
//    SSRPairMap ssrmapTranslate(const QString &unit, const QString &rate, DIRECT origin);
    void rectDiv(QRect &rect, double factor, DIRECT origin, const QSize &p1);
private:
    Ui::labelprint_design_dialog *ui;
};

#endif // LABELPRINT_DESIGN_DIALOG_H
