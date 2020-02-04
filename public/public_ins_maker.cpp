#include "public_ins_maker.h"
#include "printer_0.h"
#include "qextserialenumerator.h"
#include "lds.h"
#include <QApplication>
#include <QDebug>
#include "bmp_info.h"
#include "n_func.h"
#include "printer_normal.h"
#include "public_sql.h"
#include "lds.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "printer_pos.h"
#include "public_printby_ch_billno_over_view.h"


public_ins_maker::public_ins_maker(QObject *parent)
    : translatorObj(parent)
{
    clear();
}

void public_ins_maker::clear()
{
    is_column_print = false;
    column_count = 0;
    step = 0;
    paperwidth = 0;
    row = 0;
    print_ins.clear();
    print_path = defpath();
}

void public_ins_maker::initAndHeadPrepare(lds_model_sqltablemodel *cache)
{
    fexpandmain_model_sqltablemodel  *m = qobject_cast<fexpandmain_model_sqltablemodel  *>(cache);
    drawText("i");
    if(m->get_cey_sys_parameter_value("bp_fmt_print_prepare").toString() == "1") {
        drawText(m->get_cey_sys_parameter_value("bp_fmt_print_prepare_ins").toByteArray());
    }
}

QByteArray public_ins_maker::textAlign(const QString &text, Qt::Alignment textflag, int textwidth, char c)
{
    //    pieceIns pret;
    QByteArray ret;
    //【0】    文本长度过长会破坏格式
    ret = text.toLocal8Bit();
    int actualtextwidth = lds::size_g_of_u(text);//actualtextwidth;
    switch(textflag) {
    case Qt::AlignCenter://居中
        ret = QByteArray((textwidth-actualtextwidth)/2, c) + ret + QByteArray((textwidth-actualtextwidth)/2, c);
        break;
    case Qt::AlignRight://右对齐
        ret = QByteArray((textwidth-actualtextwidth), c) + ret;
        break;
    case Qt::AlignLeft://左对齐
        ret = ret + QByteArray((textwidth-actualtextwidth), c);
        break;
    }
    //最后统一按左对齐处理
    return ret;
}

QString public_ins_maker::defpath()
{
    return lds::sysconf->value("system_setting/printerpath").toString();
}

void public_ins_maker::drawText(const QByteArray &hex, const QString &text, Qt::Alignment textflag)
{
    insertText(currentIndex(), hex, text, textflag);
}

void public_ins_maker::drawText(const QByteArray &hex, const QString &text, int textwidth, Qt::Alignment textflag)
{
    insertText(currentIndex(), hex, text, textwidth, textflag);
}

void public_ins_maker::insertText(int index, const QByteArray &hex, const QString &text, Qt::Alignment textflag)
{
    int  textwidth = -1;
    if(is_column_print) {
        if(column_count == 0) {
            textwidth = paperwidth;
        } else {
            textwidth = paperwidth/column_count;
        }
    }

    int scale_h = 1;
    QList<QByteArray> hexs = hex.split(',');
    foreach(const QByteArray &h,  hexs) {
        if(h.startsWith('s')) {
            int s0 = h.mid(1,1).toInt();
            if(s0 != 0) {
                scale_h = s0 + 1;
                textwidth = textwidth / scale_h;// s00, s11, s22, s33 -> 不放大、x2、x3、x4
            }
            break;
        }
    }

    //    pieceIns pret;
    QString ret;
    //【0】    文本长度过长会破坏格式
    ret = text.toLocal8Bit();
    int actualtextwidth = lds::size_g_of_u(text);//actualtextwidth;
    char blank = ' ';
    //之前的词条+当前的词条超过一行，则当前词条前加换行
    if(is_column_print && textwidth != 0) {
        int columnadd = actualtextwidth / textwidth + ((actualtextwidth % textwidth) > 0 ? 1 : 0);
        column += columnadd;
        if(row > 0 && column > column_count) {
            if(drawN()) {
                index ++;
            }
            column = 0;
            column += columnadd;
        }
        textwidth = textwidth * (actualtextwidth / (textwidth + 1) + 1);
    }

    switch(textflag) {
    case Qt::AlignCenter://居中
        ret = QByteArray((textwidth-actualtextwidth)/2, blank) + ret + QByteArray((textwidth-actualtextwidth)/2, blank);
        break;
    case Qt::AlignRight://右对齐
        ret = QByteArray((textwidth-actualtextwidth), blank) + ret;
        break;
    case Qt::AlignLeft://左对齐
        ret = ret + QByteArray((textwidth-actualtextwidth), blank);
        break;
    }
    //最后统一按左对齐处理
    //若当前词条本身就大于一行，则无需换行
    if(is_column_print
            && step != 0
            && (step +lds::size_g_of_u(ret) * scale_h) > paperwidth) {
        print_ins += pieceIns("", "\n");
        step = 0;
        column = 0;
        row ++;
        index ++;
    }

    print_ins.insert(index, pieceIns(hex, ret.toLocal8Bit()));

    step += lds::size_g_of_u(ret) *scale_h;

    if(is_column_print) {
        if(column >= column_count) {
            drawN();
        }
    }
}

void public_ins_maker::insertText(int index, const QByteArray &hex, const QString &text, int textwidth, Qt::Alignment textflag)
{
    int scale_h = 1;
    QList<QByteArray> hexs = hex.split(',');
    foreach(const QByteArray &h,  hexs) {
        if(h.startsWith('s')) {
            int s = h.mid(1,1).toInt();
            if(s != 0) {
                scale_h = s + 1;// s00, s11, s22, s33 -> 不放大、x2、x3、x4
                textwidth = textwidth / scale_h;// s00, s11, s22, s33 -> 不放大、x2、x3、x4

            }
            break;
        }
    }
    QString ret;
    //【0】    文本长度过长会破坏格式
    ret = text.toLocal8Bit();
    int actualtextwidth = lds::size_g_of_u(text);
    char blank = ' ';
    if(actualtextwidth < textwidth) {//文本实际宽度，小于文本预宽度
        switch(textflag) {
        case Qt::AlignCenter://居中
            ret = QByteArray((textwidth-actualtextwidth)/2, blank) + ret + QByteArray((textwidth-actualtextwidth)/2, blank);
            break;
        case Qt::AlignRight://右对齐
            ret = QByteArray((textwidth-actualtextwidth), blank) + ret;
            break;
        case Qt::AlignLeft://左对齐
            ret = ret + QByteArray((textwidth-actualtextwidth), blank);
            break;
        }
        //最后统一按左对齐处理
    }

    print_ins.insert(index, pieceIns(hex, ret.toLocal8Bit()));

    step += lds::size_g_of_u(ret) *scale_h;
}

int public_ins_maker::currentIndex()
{
    return print_ins.count();
}

bool public_ins_maker::drawN()
{
    if(print_ins.count() > 1) {//初始化有个i
        if(!print_ins.last().second.endsWith("\n")
                && false == print_ins.last().first.contains("-")
                && (false == is_column_print || (true == is_column_print && step < paperwidth))) {
            print_ins += pieceIns("", "\n");
            step = 0;
            column = 0;
            row ++;
            return true;
        }
    }
    return false;
}

void public_ins_maker::drawLogo(const QByteArray &hex, const QString &logopath)
{
    QByteArray b0;
    Bmp_info bmpmaker;
    bmpmaker.Pic2printIns(logopath);
    bmpmaker.makeIns(b0);
    print_ins += pieceIns(hex+b0, "");
}

void public_ins_maker::drawQR_Code(const QByteArray &hex, const QString &code)
{
    print_ins  += draw_qr_code(hex, code);
}

void public_ins_maker::drawoneBarcode(const QByteArray &hex, const QString &code)
{
    print_ins += oneBarcode(hex, code);
}

pieceIns public_ins_maker::oneBarcode(const QByteArray &hex, const QString &code)
{
    QByteArray b0;
    QByteArray b1=QByteArray::number(public_ins_maker::stringToHex(code).length()/2,16);
    //    b0.append("1D6B45");
    b0.append("1D6B47");//条码太长时，打不全，改成47
    b0.append(b1.count()==1?"0"+b1:b1);
    b0.append(public_ins_maker::stringToHex(code));
    b0.append("0A");

    return pieceIns(hex+b0, "");
}

void public_ins_maker::drawTextEnd(const QByteArray &hex)
{
    print_ins += pieceIns(hex, "");
    if(n_func::f_get_sysparm("yun_bill_bottom_enable") == "1") {
        drawText("", n_func::f_get_sysparm("yun_bill_bottom"));
    }
}

void public_ins_maker::setPrintCount(int printcount)
{
    blockIns b = print_ins;
    printcount--;
    while(printcount >=1 && printcount--) {
        print_ins +=b;
    }
}

//bool public_ins_maker::synchPrint()
//{
//    return asynchPrint(print_path, true);
//}

//bool public_ins_maker::asynchPrint(const QString &path, bool isblock)
//{
//    lds_query_hddpos  query;
//    resolve(path, query, print_ins, getLanguageKey() );
//    return asynchPrintWithoutresolve(path, isblock);
//}

//bool public_ins_maker::asynchPrintWithoutresolve(const QString &path, bool isblock)
//{
//    //path 192.168.1.21:9100
//    //path /dev/ttyS0,9600,N,8,1   / port,9600,0,8,0,0,100[端口，波特率，]
//    /**
//    port,9600      , 0       , 8           , 0            , 0           , 100
//    port,baudrate,parity,databits,stopbits, flowtype,timeout
//    port              :/dev/ttyS0
//    baudrate      :50 - 115200
//    parity           :PAR_NONE = 0,PAR_ODD,PAR_EVEN,PAR_SPACE
//    databits       :DATA_5=5, DATA_6, DATA_7, DATA_8
//    stopbits       :STOP_1=0,STOP_2
//    flowtype      :FLOW_OFF=0,FLOW_HARDWARE,FLOW_XONXOFF
//    timeout       :100
//    */
//    QString error;
//    //
//    //    public_ins_maker::resolve(path, query, print_ins);
//    //2
//    if(Printer_Normal::toprint(path, print_ins, 1, &error)) {
//        return true;
//    }
//    if(!error.isEmpty()) sendError(path + ":" +error, QObject::tr("打印信息"));
//    return false;
//}

//bool public_ins_maker::synchPrintWithoutresolve(const QString &path)
//{
//    return asynchPrintWithoutresolve(path, true);
//}

bool public_ins_maker::write_bylanguage()
{
    QString errstring;
    return Printer_POS::printer_write_bylanguage(print_path, print_ins, getLanguageKey(), errstring);
}

void public_ins_maker::begin_column_print(int columnCount)
{
    is_column_print = true;
    column_count = columnCount;
    column = 0;
    step = 0;
}

void public_ins_maker::end_column_print()
{
    is_column_print = false;
}

int public_ins_maker::column_width()
{
    if(column_count != 0)
        return paperwidth / column_count;
    return 0;
}

int public_ins_maker::left()
{
    if(paperwidth != 0)
        return step % paperwidth;
    return step;
}

void public_ins_maker::resolve()
{
    lds_query_hddpos  query;
    resolve(print_path, query, print_ins);
}

bool public_ins_maker::get_is_column_print()
{
    return is_column_print;
}

QByteArray public_ins_maker::print_ins_2_pic()
{
    QByteArray ins;
    public_printby_ch_billno_over_view_data d;
    d.setPaperWidth(paperwidth);
    d.ins = print_ins;
    //所有正常的字体都倍高
//    for(int k = 0; k < d.ins.count(); k ++) {
//        d.ins[k].first.replace("s00", "s01");
//        d.ins[k].first.replace("s01", "s01");
//        d.ins[k].first.replace("s11", "s11");
//        d.ins[k].first.replace("s11", "s11");
//    }
    QImage img = d.getImage();// /home/cz/图片/Lebanon.jpg
    Bmp_info::pic2Ins(img, ins);
#ifdef QT_DEBUG
    img.save("kitchen.png");
#endif

    return ins;
}


void public_ins_maker::resolve(const QString &path, lds_query_hddpos  &query, blockIns &ins)
{
    int vch_instype = 0;
    QString vch_printtype = 0;
    int int_paperwidth = 32;
    int xinye_beep_count = 4;

    //1
    //打印机类型， 打印类型
    query.execSelect(QString("select  vch_printtype, vch_instype,int_paperwidth,int_xinye_beep_count  from cey_bt_kitchen_print where vch_printip = '%1'")
                     .arg(path));
    if(query.next()) {
        //2
        vch_instype = query.recordValue("vch_instype").toInt();
        vch_printtype = query.recordValue("vch_printtype").toString();
        int_paperwidth = query.recordValue("int_paperwidth").toInt();
        xinye_beep_count = query.recordValue("int_xinye_beep_count").toInt();
    } else {
    }
    //3
    Printer_Normal::resolve(vch_instype,
                            vch_printtype,
                            int_paperwidth,
                            xinye_beep_count,
                            ins);
}

void public_ins_maker::sendError(const QString &err, const QString &title)
{
    QMetaObject::invokeMethod(public_sql::save_login_object(),
                              "showprinterr",
                              Qt::QueuedConnection,
                              Q_ARG(QString, err),
                              Q_ARG(QString, title)
                              );
}
QString public_ins_maker::stringToHex(const QString &str)
{
    return printer_0::trans_UTF82GBK(str).toHex();
}

//https://stackoverflow.com/questions/23577702/printing-qr-codes-through-an-esc-pos-thermal-printer
pieceIns public_ins_maker::draw_qr_code(const QByteArray &hex, const QString &code)
{
    return printer_0::draw_qr_code(hex, code);
}


translatorObj::translatorObj(QObject *parent)
    : QObject(parent)
{
    setObjectName("translatorObj");
}



QTranslator *translatorObj::getPrintTranslate(const QString &languageKey)
{
    if(this->languageKey != languageKey) {
        this->languageKey = languageKey;
        printTranslate.delPointer();
    }

    if( 0 == printTranslate.getPointer() ) {
        printTranslate.setPointer(new QTranslator);
    }

    if(printTranslate.getPointer()->isEmpty()) {
        if(!printTranslate.getPointer()->load("userdata/settings/language/"+languageKey+"_fastfd.qm")) {
            qDebug() << "languageKey:" << languageKey << "load error";
        }
    }

    return printTranslate.getPointer();
}

void translatorObj::setPrintTranslate(const QString &languageKey, QObject *languageObj)
{
    this->languageObj = languageObj;
    if(languageKey.isEmpty()) {
        this->languageKey = lds::get_soft_curent_language_first();//languageKey为空，随软件环境语言
        return ;
    }
    if(languageKey == _CN_) {
        this->languageKey = languageKey;//中文
        return ;
    }

    printTranslate.delPointer();
    getPrintTranslate(languageKey);
}

QString translatorObj::printTr(const char *key)
{
    if(languageKey.isEmpty()) {
        return QApplication::translate(languageObj->metaObject()->className(), key);
    }
    if(languageKey == _CN_) {
        return QString(key);
    }
    QString ret = getPrintTranslate(languageKey)->translate(languageObj->metaObject()->className(), key);
    return ret;
}

bool translatorObj::languageIsCN()
{
    if(languageKey.isEmpty()) {
        return QObject::tr("中文") == "中文";
    }
    if(languageKey == _CN_) {
        return true;
    }
    return false;
}

QString translatorObj::getLanguageKey()
{
    return languageKey;
}

