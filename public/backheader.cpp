#include "backheader.h"
#include <qsqlrecord.h>
#include <QApplication>
#include <QDesktopWidget>
#include "printer_0.h"
#include "lds_messagebox.h"
#include "lds.h"
#include "public_ins_maker.h"
#include "printer_normal.h"
#include "printer_pos_net.h"
#include "public_printby_ch_billno_over_view.h"
#include "bmp_info.h"
#include <QTimer>
#include "w_sys_manage_outer_pay_set_locale_wizard.h"
#include "n_func.h"
#include "w_rpt_report_dataexport.h"
#include "w_sys_manage_operatorright.h"
#include "lds_dialog_input.h"
#include "ui_lds_dialog_input_dialog.h"

public_mainprinter backheader::printer;
QString backheader::SELECT_OPERID_NAME_FLAG =
        "Select '0000' as vch_operID,'system' as vch_operator union Select vch_operID, ifnull(vch_operator, '') as vch_operator from cey_sys_operator where ch_flag = 'Y'  ";
QString backheader::SELECT_OPERID_NAME =
        "Select '0000' as vch_operID,'system' as vch_operator union Select vch_operID, ifnull(vch_operator, '') as vch_operator from cey_sys_operator ";

backheader::backheader(QObject *parent) : QObject(parent)
{
}

bool backheader::operate_right_check_code(QWidget *parent, const QString &code)
{
    return operate_right_check_index(parent, w_sys_manage_operatorright::index_code(code));
}

bool backheader::operate_right_check_zh(QWidget *parent, const QString &zh)
{
    return operate_right_check_index(parent, w_sys_manage_operatorright::index_zh(zh));

}

bool backheader::operate_right_check_index(QWidget *parent, int index)
{
    w_sys_manage_operatorright::OperatorRight r = w_sys_manage_operatorright::get_code_name_list().value(index);
#ifdef QT_DEBUG
    if(r.key == "") {
        qDebug() << __FILE__ << __LINE__ <<  "right not exsited";
        QObject *o = 0;
        o->children();
    }
#endif
    QString vch_operId = lds::gs_operid;
    QString vch_operator;
    QString errstring;

    while(!operate_right_check(r.key, vch_operId)) {
        vch_operator = lds_query_hddpos::selectValue(QString(" SELECT vch_operator FROM hddpos.cey_sys_operator where vch_operID = '%1' ").arg(lds::gs_operid)).toString();
        if(!r.authorized) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,
                                    QObject::tr("操作员") + ":" + vch_operator + "(" + lds::gs_operid + ")\n" +
                                    QObject::tr("操作员权限不足") + ":" + r.value + "(" + r.key + ")");
            return false;
        }
        //authorized
        lds_dialog_input inputdialog(parent);
        inputdialog.showSecondLineEdt();
        inputdialog.setWindowTitle(QObject::tr("提升权限"));
        inputdialog.ui->label->setText(QObject::tr("用户名") + ":");
        inputdialog.ui->label_2->setText(QObject::tr("密码") + ":");
        inputdialog.ui->lineEdit_2->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
        inputdialog.ui->lineEdit->setText("0000");
        inputdialog.ui->lineEdit_2->setText(n_func::f_get_godpassword());
#endif
        if(1 == lds_messagebox::question(parent, MESSAGE_TITLE_VOID,
                                         QObject::tr("操作员") + ":" + vch_operator  + "(" + lds::gs_operid + ")\n" +
                                         QObject::tr("操作员权限不足") + ":" + r.value + "(" + r.key + ")",
                                         QObject::tr("提升权限"),
                                         QObject::tr("取消"))) {
            return false;
        }
        forever {
            if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
                return false;
            }
            errstring = n_func::f_confirm_user_pwd(inputdialog.ui->lineEdit->text().trimmed(),
                                                   inputdialog.ui->lineEdit_2->text().trimmed());
            if(errstring == "")
                break;
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
        }
        vch_operId = inputdialog.ui->lineEdit->text().trimmed();
    }
    return true;
}

bool backheader::operate_right_check(const QString &code, const QString &operid)
{
    if(operid == "0000")
        return true;
    if(code == "")
        return true;

    if(lds_query_hddpos::selectHasNext(QString(" SELECT ch_purviewno FROM cey_sys_oper_purview where vch_operID = '%1' and ch_purviewno = '%2'; ").arg(operid).arg(code))) {
        return true;
    }

    return false;
}

QString backheader::getinterdata(const QString &_data_)
{
    QString ret;
    if(_data_.contains("[") && _data_.contains("]")) {
        ret=_data_;
        ret=ret.split("[")[1];
        ret.chop(1);
    } else if(_data_.contains("(") && _data_.contains(")")) {
        ret=_data_;
        ret=ret.split("(")[1];
        ret.chop(1);
    }

    return ret;
}
int backheader::set_index_of_interdata(const QString &inter, QComboBox *c)
{
    int ret = -1;
    if(c) {
        for(int row = 0, rowcount = c->count(); row < rowcount; row++) {
            if(inter == getinterdata(c->itemText(row))) {
                ret = row;
                return ret;
            }
        }
    }
    return ret;
}

QString backheader::print_def_path()
{
    return lds::sysconf->value("system_setting/printerpath").toString();
}

bool backheader::print_def_path_is_tty()
{
    QString printpath = backheader::print_def_path().toLower();
    if(printpath.contains("tty")
            || printpath.contains("com")) {
        return true;
    }
    return false;
}

int backheader::print_paper_witdh(const QString &path)
{
    int int_paperwidth = lds_query_hddpos::selectValue(QString(" select int_paperwidth from cey_bt_kitchen_print where vch_printip = '%1' ").arg(path)).toInt();
    if(int_paperwidth == 0)
        int_paperwidth = 32;
    return int_paperwidth;
}
void backheader::syn_dataprin_test_byresolveself(QWidget *parent, const QString &ch_kitchen_view, const QString &path, int instype, const QString &printtype, int paperwidth, int xinye_beep_count)
{
    //#ifdef QT_DEBUG
    //    blockIns printdata;
    /*
        TurkishMap.insert("İ", "\xDD");
        TurkishMap.insert("ı", "\xFD");


        TurkishMap.insert("Ç", "\xC7");
        TurkishMap.insert("ç", "\xE7");


        TurkishMap.insert("Ğ", "\xD0");
        TurkishMap.insert("ğ", "\xF0");


        TurkishMap.insert("Ö", "\xD6");
        TurkishMap.insert("ö", "\xF5");


        TurkishMap.insert("Ş", "\xDE");
        TurkishMap.insert("ş", "\xFE");


        TurkishMap.insert("Ü", "\xDC");
        TurkishMap.insert("ü", "\xFC");
*/

    //    printdata += pieceIns("",     "merhaba dÇnya\n"); //252 fc - 3ü

    //    printdata += pieceIns("",     "merhaba dİİİİİİnya\n"); //252 fc - 3ü
    //    printdata += pieceIns("",     "merhaba dıııııınya\n"); //252 fc - 3üFD");


    //    printdata += pieceIns("",     "merhaba dÇÇÇÇÇÇnya\n"); //252 fc - 3üC7");
    //    printdata += pieceIns("",     "merhaba dççççççnya\n"); //252 fc - 3üE7");


    //    printdata += pieceIns("",     "merhaba dĞĞĞĞĞĞnya\n"); //252 fc - 3üD0");
    //    printdata += pieceIns("",     "merhaba dğğğğğğnya\n"); //252 fc - 3üF0");


    //    printdata += pieceIns("",     "merhaba dÖÖÖÖÖÖnya\n"); //252 fc - 3üD6");
    //    printdata += pieceIns("",     "merhaba döööööönya\n"); //252 fc - 3üF5");


    //    printdata += pieceIns("",     "merhaba dŞŞŞŞŞŞnya\n"); //252 fc - 3üDE");
    //    printdata += pieceIns("",     "merhaba dşşşşşşnya\n"); //252 fc - 3üFE");


    //    printdata += pieceIns("",     "merhaba dÜÜÜÜÜÜnya\n"); //252 fc - 3üDC");
    //    printdata += pieceIns("",     "merhaba düüüüüünya\n"); //252 fc - 3üFC");

    //    printdata += pieceIns("",             "A Ā B C Č D E Ē F G Ģ H I Ī J K Ķ L Ļ M N Ņ O P R S Š T U Ū V Z Ž\n");
    //    printdata += pieceIns("",             "a ā b c č d e ē f g ģ h i ī j k ķ l ļ m n o p r s š t u ū v z ž\n");


    //    printdata += pieceIns("", "A língua portuguesa, também designada português, é uma língua românica flexiva ocidental originada no galego-português falado no Reino da Galiza e no norte de Portugal. Com a criação do Reino de Portugal em 1139 e a expansão para o sul como parte da Reconquista deu-se a difusão da língua pelas terras conquistadas e mais tarde, com as descobertas portuguesas, para o Brasil, África e outras partes do mundo. Português foi usado, naquela época, não somente nas cidades conquistadas pelos portugueses, mas também por muitos governantes locais nos seus contatos com outros estrangeiros poderosos. Especialmente nessa altura a língua portuguesa também influenciou várias línguas. \n");
    //    printdata += pieceIns("", "üzüxücüvübünümü,ü.üaüsüdüfüg\n");
    //    Printer_Normal::resolve(instype, printtype, paperwidth, xinye_beep_count, printdata);

    //    QByteArray printd;

    //    QList<QPair<QByteArray, uchar> > map;
    //    map.append(qMakePair(QByteArray("á"),	uchar(160)));
    //    map.append(qMakePair(QByteArray("ā"),	uchar(133)));
    //    map.append(qMakePair(QByteArray("â"),	uchar(131)));
    //    map.append(qMakePair(QByteArray("ã"),	uchar(227)));
    //    map.append(qMakePair(QByteArray("Á"),	uchar(193)));
    //    map.append(qMakePair(QByteArray("Ā"),	uchar(192)));
    //    map.append(qMakePair(QByteArray("Â"),	uchar(194)));
    //    map.append(qMakePair(QByteArray("Ã"),	uchar(199)));
    //    map.append(qMakePair(QByteArray("é"),	uchar(130)));
    //    map.append(qMakePair(QByteArray("ę"),	uchar(136)));
    //    map.append(qMakePair(QByteArray("É"),	uchar(144)));
    //    map.append(qMakePair(QByteArray("Ę"),	uchar(210)));
    //    map.append(qMakePair(QByteArray("í"),	uchar(161)));
    //    map.append(qMakePair(QByteArray("Í"),	uchar(214)));
    //    map.append(qMakePair(QByteArray("ó"),	uchar(162)));
    //    map.append(qMakePair(QByteArray("ō"),	uchar(149)));
    //    map.append(qMakePair(QByteArray("ô"),	uchar(147)));
    //    map.append(qMakePair(QByteArray("õ"),	uchar(245)));
    //    map.append(qMakePair(QByteArray("Ó"),	uchar(224)));
    //    map.append(qMakePair(QByteArray("Ō"),	uchar(227)));
    //    map.append(qMakePair(QByteArray("Ô"),	uchar(226)));
    //    map.append(qMakePair(QByteArray("Õ"),	uchar(229)));
    //    map.append(qMakePair(QByteArray("ú"),	uchar(163)));
    //    map.append(qMakePair(QByteArray("ü"),	uchar(129)));
    //    map.append(qMakePair(QByteArray("Ú"),	uchar(233)));
    //    map.append(qMakePair(QByteArray("Ü"),	uchar(154)));
    //    map.append(qMakePair(QByteArray("į"),	uchar(135)));
    //    map.append(qMakePair(QByteArray("Į"),	uchar(128)));
    //    map.append(qMakePair(QByteArray("á"),	uchar(160)));
    //    for(int k = 0; k < map.count(); k++) {
    //        printd += map[k].first + "  " + QByteArray::number(map[k].second) + ":" + map[k].second + "n\n";
    //    }
    //    printdata += pieceIns(printd.toHex(), "");
    //    printer.synchPrintWithoutresolve(printdata, path);
    //    printer.synchPrint(printdata, path);
    //#else
    if(ch_kitchen_view == "Y") {
        Printer_POS_NET printer;
        if(false == printer.tryOpen(path)) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("操作失败") + "\n" + printer.lastError());
            return;
        }
        printer.writedirect(
                    "{\"type\":	\"10\",\"type_cmd\":	\"\", \"serial_no\":	\"008\", \"table_no\":	\"1015\", \"bill_no\":	\"B1805090300008\",\"operid\":	\"0000\",\"time\":	\"2018-05-09 16:43:24\", \"people_num\":	1,\"memo\":	\"\", \"dishDetail\":	[]} "
                    );
        if(false == printer.waitForReadyRead()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("操作失败") + "\n" + "TIME_OUT");
            return;
        }
        lds_messagebox::information(parent, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
    } else {
        blockIns printdata;
        printdata += backheader::printer.drawText("i,a00,s11", QObject::tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
        printdata += backheader::printer.drawText("i,a00,s00", QObject::tr("打印测试成功")+"\n");//清除缓存 居中 2x2x
        printdata += backheader::printer.drawText("i,a00,s00", "الإنجليزية""\n");//清除缓存 居中 2x2x
        printdata += backheader::printer.drawText("-", "");//清除缓存
        printdata += backheader::printer.drawText("i", "");//清除缓存

        Printer_Normal::resolve(instype, printtype, paperwidth, xinye_beep_count, printdata);
        printer.write(printdata, path);
        //        printer.synchPrintWithoutresolve(printdata, path);
        //#endif
    }
}

void backheader::reportPrint( blockIns ins)
{
    if(w_sys_manage_outer_pay_set_locale_wizard::bp_country_print_img()) {
        QByteArray pHex = backheader::reportTranImage(ins);
        ins.clear();
        ins << pieceIns("1B40", pHex);
        ins << pieceIns("", QByteArray(n_func::f_get_sysparm("bp_fmt_tail_block").toInt() + 3, '\n'));
        ins << pieceIns("1B6405 1D5601", "");
        backheader::writeDirect(ins);
    } else {
        ins << pieceIns("", QByteArray(n_func::f_get_sysparm("bp_fmt_tail_block").toInt(), '\n'));
        ins << pieceIns("1B6405 1D5601", "");
        backheader::reslove_write(ins);
    }
}

bool backheader::reportPrint(QSharedPointer<Printer_POS> &printer, const QString &title, const QString &csv_data, QString &errstring)
{
    QString printpath = backheader::print_def_path();
    if(printer.isNull()) {
        printer = Printer_POS::get_pos_printer_by_path(printpath);
    }
    if(printer.isNull()) {
        errstring = "PRINTER_IS_NULL";
        return false;
    }
    if(!printer->isOpen()) {
        if(!printer->tryOpen(printpath)) {
            errstring = printer->lastError();
            return false;
        }
    }

    int paperwidth = lds_query_hddpos::selectValue(QString("SELECT int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1' ").arg(printpath)).toInt();
    QPixmap pixmap;
    w_rpt_report_dataExport::getExportImage(title, csv_data, pixmap, PRINTER_CHAR_WIDTH_PIXELS * paperwidth, false);
    //
    QByteArray pHex;
    Bmp_info::pic2Ins(pixmap.toImage(), pHex);
    printer->writedirect(pHex);
    printer->writedirect(QByteArray(3, '\n'));
    printer->writeHex("1B6405 1D5601");//走纸5x， 切纸

    return true;
}

QByteArray backheader::reportTranImage(blockIns &ins)
{
    QString printpath = print_def_path();
    public_printby_ch_billno_over_view_data d;
    d.setPaperWidth(lds_query_hddpos::selectValue(QString("SELECT int_paperwidth FROM cey_bt_kitchen_print where vch_printip = '%1';").arg(printpath)).toInt());//print_format.paperwidth);
    d.ins = ins;
    QImage img = d.getImage();
#ifdef QT_DEBUG
    img.save("print1.png");
#endif
    QByteArray pHex;
    Bmp_info::pic2Ins(img, pHex);
    return pHex;
}

void backheader::reslove(blockIns &ins)
{
    reslove(ins, print_def_path());
}

void backheader::reslove(blockIns &ins, const QString &path)
{
    lds_query_hddpos query;
    public_ins_maker::resolve(path, query, ins);
}

void backheader::write(blockIns &ins)
{
    write(ins, print_def_path());
}

void backheader::writeHex(const QByteArray &hex)
{
    blockIns ins;
    ins << pieceIns(hex, "");
    writeDirect(ins);
}

void backheader::writeByte(const QByteArray &byte)
{
    blockIns ins;
    ins << pieceIns("", byte);
    writeDirect(ins);
}

void backheader::write(blockIns &ins, const QString &path)
{
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(path);
    if(printer.isNull()) {
        sendError(path + ":" + "PRINTER_IS_NULL", QObject::tr("打印信息"));
        return;
    }
    if(false == printer->tryOpen(path)) {
        sendError(path + ":" + printer->lastError(), QObject::tr("打印信息"));
        return;
    }
    foreach(const pieceIns &p, ins) {
        printer->writeHex(p.first);
        printer->write(p.second, lds::get_soft_curent_language_first());
    }
}

void backheader::writeDirect(const blockIns &ins, const QString &path)
{
    QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(path);
    if(printer.isNull()) {
        sendError(path + ":" + "PRINTER_IS_NULL", QObject::tr("打印信息"));
        return;
    }
    if(false == printer->tryOpen(path)) {
        sendError(path + ":" + printer->lastError(), QObject::tr("打印信息"));
        return;
    }
    foreach(const pieceIns &p, ins) {
        printer->writeHex(p.first);
        printer->writedirect(p.second);
    }
}

void backheader::reslove_write(blockIns &ins)
{
    reslove_write(ins, print_def_path());
}

void backheader::reslove_write(blockIns &ins, const QString &path)
{
    reslove(ins, path);
    write(ins, path);
}

void backheader::writeDirect(const blockIns &ins)
{
    writeDirect(ins, print_def_path());
}

void backheader::sendError(const QString &err, const QString &title)
{
    QMetaObject::invokeMethod(public_sql::save_login_object(),
                              "showprinterr",
                              Qt::QueuedConnection,
                              Q_ARG(QString, err),
                              Q_ARG(QString, title)
                              );
}

QImage backheader::createImage(const blockIns &ins, QString path)
{
    if(path == "") {
        path = print_def_path();
    }
    public_printby_ch_billno_over_view_data d;
    d.setPaperWidth(print_paper_witdh(path));
    d.ins = ins;
    QImage img = d.getImage();// /home/cz/图片/Lebanon.jpg
#ifdef QT_DEBUG
    img.save("print.png");
#endif
    return img;
}

void backheader::modelSetHeader(QAbstractItemModel *model, const QStringList &headers)
{
    for(int r=0,r_count=headers.count(); r < r_count; r++)
        model->setHeaderData(r, Qt::Horizontal, headers[r]);
}

void backheader::backheader_printer_insert_company(blockIns &printins, const QByteArray &hex)
{
    //公司信息
    lds_query_hddpos  query;
    QString vch_company;
    QString vch_address;
    query.execSelect("select vch_company, vch_address from cey_sys_company");
    query.next();
    vch_company = query.recordValue("vch_company").toString().trimmed();
    vch_address = query.recordValue("vch_address").toString().trimmed();
    //
    if(vch_company.length() != 0) {
        printins.append(backheader::printer.drawText("a00", vch_company +"\n"));
    }
    if(vch_address.length() != 0) {
        printins.append(backheader::printer.drawText("a00", vch_address +"\n\n"));
    }
    printins.append(backheader::printer.drawText(hex));
}

QString backheader::modelValue(QAbstractItemModel *m, const QString &key)
{
    for(int k = 0; k < m->rowCount(); k ++) {
        if(m->data(m->index(k, 0)).toString() == key) {
            return m->data(m->index(k, 1)).toString();
        }
    }
    return "";
}
