#ifndef LABELPRINT_MAIN_DIALOG_H
#define LABELPRINT_MAIN_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "s_mainwindow.h"
#include "labelprint_design_dialog.h"
#include "cjson_help.h"
//3需要判断下端口是否有重复
//4扫描条码是发送的信号 void signal_scanbar(const QString &bar);//当有条码扫描是，需要接受这个信号，对model进行添加
//5点击新增时发送的信号 void signal_add();//当点击新增时，需要接受这个信号，对model进行添加


class Ui_labelprint_main_dialog;
class labelprint_set_dialog;
class labelprint_goodinfo_dialog;
class labelprint_design_dialog;
class printer_label_postek;
class printer_label_tsc_ttp_244_plus;
class printer_label_argox_PPLA;
class printer_label_argox_PPLB;
class Printer_POS_SERIAL;
class printer_label_gainscha;
class printer_label_tsc_HPRT;
class printer_label;

class labelprint_main_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit labelprint_main_dialog(QWidget *parent = 0);
    ~labelprint_main_dialog();
    QStandardItemModel *printmodel;//开放printmodel方便外部及时修改
    void inputfinished(bool label = false);
    void toprint(QWidget *parent);
    void set_label_enabled();
protected:
    void goodsadd(const QList<QStringList> &itemslist);
protected slots:
    virtual void togoodadd();//添加
protected:
    bool is_label_mode;
private slots:
    void toexit();
    void togoodremove();
    void tooverview();
    void toset();

    void toswitch();
    void tosave();

private:
    Ui_labelprint_main_dialog *ui;

    labelprint_set_dialog *setdialog;
    labelprint_goodinfo_dialog*gooddialog;
    labelprint_design_dialog*designdialog;
};


class labelprint_main_dialog_object : public QObject{
    Q_OBJECT
public:
    struct prit_piece_data{
        bool select(QStandardItemModel *model,
                    const QString &print_json,
                    printer_label *printer,
                    int rr);
        QString textCut(int k);
        QString text(int k);
        int x(int k);
        int y(int k);
        int width(int k);
        int height(int k);
        int type(int k);
        int xscale(int k);
        int yscale(int k);
        QSize xyscale(int k);
        int rotate(int k);

        cJSON_path path;
        int wide_width;
        int narrow_width;
        QString onebartype_key ;
        QString font_key;
        QString onebartype;
        QString font;
        SSMap ssmap;
        QString ins;
    };

public:
    explicit labelprint_main_dialog_object(QObject *parent = 0);
    bool printLabel(QStandardItemModel *model, bool istest, QString *errstring = 0);
    bool printLabel(QStandardItemModel *model, const SSRPairMap &ssrmap, bool istest, QString *errstring = 0);
    bool printLabel(QStandardItemModel *model, const QString &print_json, bool istest, QString *errstring = 0);

    //
    static QString print_piece_POSTEK(QStandardItemModel *model, const QString &print_json, printer_label_postek &printer, int rr, int start_x = 0);
    static QString print_piece_TSC(QStandardItemModel *model, const QString &print_json, printer_label_tsc_ttp_244_plus &printer, int rr, int start_x = 0);
    static QString print_piece_ARGOX_PPLA(QStandardItemModel *model, const QString &print_json, printer_label_argox_PPLA &printer, int rr, int start_x = 0);
    static QString print_piece_ARGOX_PPLB(QStandardItemModel *model, const QString &print_json, printer_label_argox_PPLB &printer, int rr, int start_x = 0);
    static QString print_piece_GAINSCHA(QStandardItemModel *model, const QString &print_json, printer_label_gainscha &printer, int rr, int start_x = 0);
    static QString print_piece_HPRT(QStandardItemModel *model, const QString &print_json, printer_label_tsc_HPRT &printer, int rr, int start_x = 0);

    //
    static float valueTranslate(float value, const QString &unit, const QString &rate);
    static SSRPairMap ssrmapTranslate(const SSRPairMap &printMap, const QString &unit, const QString &rate, labelprint_design_dialog::DIRECT origin);
    static QByteArray ssrmapTranslate(const QString &print_json, const QString &unit, const QString &rate, labelprint_design_dialog::DIRECT origin);
    static void rectDiv(QRect &rect, double factor, labelprint_design_dialog::DIRECT origin, const QSize &p1);
    static QSize getsizeby14(const QString &size_str);

    //
    static bool getPrintMapFromDef(SSRPairMap &map, QString *errstring = 0);
    static bool getPrintMap(const QString &filepath, SSRPairMap &map, QString *errstring = 0);
    static bool savePrintMapToDef(const SSRPairMap &map, QString *errstring = 0);
    static bool savePrintMap(const QString &filepath, const SSRPairMap &map, QString *errstring = 0);

    static QString textCutN(bool istext, const QString &key, QString text);
};

#endif // LABELPRINT_MAIN_DIALOG_H
