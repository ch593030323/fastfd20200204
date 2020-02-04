#ifndef BACKHEADER_H
#define BACKHEADER_H

#include <QObject>
#include <QSize>
#include <QStandardItemModel>
#include "public_mainprinter.h"
#include "lds_tableview_repecify.h"
#include "lds_combobox.h"
#include "lds_query_hddpos.h"
#include "public_sql.h"
#include <QSqlQueryModel>
#include "printer_pos.h"

#define  qAppTr(text) qApp->translate("QObject", text.toLocal8Bit().data())

class backheader : public QObject
{
    Q_OBJECT

public:
    explicit backheader(QObject *parent = 0);

    static bool operate_right_check_code(QWidget *parent, const QString& code);
    static bool operate_right_check_zh(QWidget *parent, const QString& zh);
    static bool operate_right_check_index(QWidget *parent, int index);
    static bool operate_right_check(const QString &code, const QString &operid);

    static QString getinterdata(const QString &_data_);//xx[data]
    static int set_index_of_interdata(const QString &inter, QComboBox *c);
    static public_mainprinter printer;//异步打印机
    static QString print_def_path();
    static bool print_def_path_is_tty();
    static int print_paper_witdh(const QString &path);
    static void syn_dataprin_test_byresolveself(QWidget *parent, const QString &ch_kitchen_view, const QString &path, int instype, const QString & printtype, int paperwidth, int xinye_beep_count);//开始打印
    //

    static void reportPrint(blockIns ins);
    static bool reportPrint(QSharedPointer<Printer_POS> &printer, const QString &title, const QString &csv_data, QString &errstring);
    static QByteArray reportTranImage(blockIns &ins);
    static void reslove(blockIns &ins);
    static void reslove(blockIns &ins, const QString &path);

    static void write(blockIns &ins);
    static void write(blockIns &ins, const QString &path);
    static void reslove_write(blockIns &ins);
    static void reslove_write(blockIns &ins, const QString &path);

    static void writeDirect(const blockIns &ins);
    static void writeDirect(const blockIns &ins, const QString &path);
    static void writeByte(const QByteArray &byte);
    static void writeHex(const QByteArray &hex);

    static void sendError(const QString &err, const QString &title);

    static QImage createImage(const blockIns &ins, QString path = "");
public:
    static void modelSetHeader(QAbstractItemModel *model, const QStringList &headers);

    static QString SELECT_OPERID_NAME_FLAG;
    static QString SELECT_OPERID_NAME;

    static void backheader_printer_insert_company(blockIns &printins, const QByteArray &hex);

    static QString modelValue(QAbstractItemModel *m, const QString &key);
};

//商品检索的窗口

#endif // BACKHEADER_H
