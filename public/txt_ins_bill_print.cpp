#include "txt_ins_bill_print.h"
#include "printer_normal.h"
#include "lds.h"
#include "bmp_info.h"
#include "cJSON.h"

const QByteArray txt_ins_bill_print::HEX_FONT_BOLD0 = "1B4500";
const QByteArray txt_ins_bill_print::HEX_FONT_BOLD1 = "1B4501";
const QByteArray txt_ins_bill_print::HEX_FONT_SCALE00 = "1D2100";
const QByteArray txt_ins_bill_print::HEX_FONT_SCALE01 = "1D2101";
const QByteArray txt_ins_bill_print::HEX_FONT_SCALE10 = "1D2110";
const QByteArray txt_ins_bill_print::HEX_FONT_SCALE11 = "1D2111";
const QByteArray txt_ins_bill_print::HEX_ALIGN_CENTER = "1B6101";//1B6101
const QByteArray txt_ins_bill_print::HEX_ALIGN_LEFT = "1B6100";//1B6100

txt_ins_bill_print::txt_ins_bill_print(const lds_model_sqltablemodel_d *model_d, const QString &format_data, QObject *parent) :
    QObject(parent)
{
    this->model_d = model_d;
    this->format_data = format_data;
}

blockIns txt_ins_bill_print::bill_before_reslove()
{
    blockIns ins = billToIns();
    ins.push_front(pieceIns("i", ""));
    return ins;
}

blockIns txt_ins_bill_print::bill_atfter_reslove()
{
    blockIns ins = reslove(format_data);
    ins.push_front(pieceIns("1B40", ""));

    return ins;
}

blockIns txt_ins_bill_print::billToIns()
{
    blockIns ins = reslove(format_data);
    for(int k = 0; k < ins.count(); k ++) {
        if(ins[k].first == HEX_FONT_BOLD0) ins[k].first = "b0";
        if(ins[k].first == HEX_FONT_BOLD1) ins[k].first = "b1";
        if(ins[k].first == HEX_FONT_SCALE00) ins[k].first = "s00";
        if(ins[k].first == HEX_FONT_SCALE01) ins[k].first = "s01";
        if(ins[k].first == HEX_FONT_SCALE10) ins[k].first = "s10";
        if(ins[k].first == HEX_FONT_SCALE11) ins[k].first = "s11";
        if(ins[k].first == HEX_ALIGN_CENTER) ins[k].first = "a00";
        if(ins[k].first == HEX_ALIGN_LEFT) ins[k].first = "a10";
    }
    return ins;
}

blockIns txt_ins_bill_print::reslove(const QString &str)
{
    return reslove(str, true);
}

blockIns txt_ins_bill_print::reslove(const QString &str, bool with_pic_qrc_bar)
{
    map_valid_count = 0;
    blockIns ins_print;
    blockIns ins_block;
    QStringList linelist = str.split("\n");
    QStringList table_content_list;
    QStringList table_perpare_list;
    QList<blockIns> cell_ins_model;
    QString line;
    ins_stack.clear();
    for(int k = 0; k < linelist.count(); reslove_check(linelist, k, ins_stack)) {
        line = linelist[k];
        qDebug() << __LINE__ << "(" << sqr++ << this << line;
        //#
        if(line.startsWith("#")) {
            continue;
        }
        //line
        ins_block = resloveLine(linelist[k], true, with_pic_qrc_bar);

        if(ins_stack.count() > 0) {
            qDebug() << "ins_stack" << ins_stack;
            if(sqr > 35) {
                qDebug() << __LINE__;
            }
        }

        if(ins_stack.value(0) == "[") {
            ///!table
            ins_block.clear();
            qDebug() << __LINE__ << "index_count:" << ins_table.index_count;
            //table_perpare_list
            if(true == ins_table.prepare_begin && false == ins_table.begin) {
                table_perpare_list << line;
            }

            //table_content_list
            if(true == ins_table.prepare_begin && false == ins_table.prepare_end && true == ins_table.begin) {
                table_content_list << line;
            }
            //print perpare content
            if(true == ins_table.prepare_begin && true == ins_table.prepare_end) {
                if(ins_table.index_count > 0) {
                    for(int prepare_k = 0; prepare_k < table_perpare_list.count(); prepare_k ++) {
                        ins_print << resloveLine(table_perpare_list[prepare_k], true, with_pic_qrc_bar, false);
                    }
                    ins_print << txt_ins_bill_print::tableValue(model_d, table_content_list, *this, with_pic_qrc_bar);
                }
                //
                ins_table.init();
                table_content_list.clear();
                table_perpare_list.clear();
            }
            ///~table
        }

        if(ins_stack.value(0) == "(") {
            ///cell
            if(true == ins_cell.prepare_begin && false == ins_cell.prepare_end) {
                cell_ins_model << ins_block;
                ins_block.clear();
            }
            if(true == ins_cell.prepare_begin && true == ins_cell.prepare_end) {
                if(false == ins_cell.begin) {
                    cell_ins_model.clear();
                }
                //
                while(cell_ins_model.count() > 0)
                    ins_print << cell_ins_model.takeFirst();
                //
                ins_cell.init();
            }
            ///~cell
        }
        //
        ins_print<< ins_block;
    }

    return ins_print;
}

blockIns txt_ins_bill_print::resloveLine(QString line)
{
    return resloveLine(line, true);
}

blockIns txt_ins_bill_print::resloveLine(QString line, bool with_control)
{
    return resloveLine(line, with_control, true);
}

blockIns txt_ins_bill_print::resloveLine(QString line, bool with_control, bool with_pic_qrc_bar)
{
    return resloveLine(line, with_control, with_pic_qrc_bar, true);
}

blockIns txt_ins_bill_print::resloveLine(QString line, bool with_control, bool with_pic_qrc_bar, bool stack_update)
{
    blockIns ins;
    if(line.startsWith("#"))
        return ins;

    resloveData d;
    d.init();

    QString key;
    QList<lineData> line_data_list;
    bool contentKip = false;
    bool mapValueOK = false;
    bool trimedEmptySkip = false;

    for(int k = 0; k < line.count(); k++) {
        key = line.mid(k, 1);
        //d.begin
        if("%" == key ) {//开始
            d.init();
            d.begin = k;

            if("%" == line.mid(k + 1, 1)) {
                d.percentFlag = true;
                k +=2;
                goto resloveEnd;//%%
            }
        } else if(" " == key) {//结束
            goto resloveEnd;
        } else  if(d.begin > -1) {//content
            if( "+" == key || "-" == key ) {//左对齐，文本
                resloveData t = resloveAlignFlag(line, k, d);
                d.txtAlign.flag = true;
                d.txtAlign.str = t.txtAlign.str;
                d.k_off = t.k_off;
                d.begin = t.begin;

                k += d.k_off;
                k--;
                goto resloveEnd;
            } else if( "?" == key) {// question txt
                trimedEmptySkip= true;
                d.txtQuestion.flag = true;
                k += resloveQuestion(line, k, d.txtQuestion.str);
                k--;
                goto resloveEnd;
            } else if(with_control  && k - 1 == d.begin && (key.toInt() > 0 || "0" == key) ) {//加粗、倍高倍宽
                //### 加粗、倍高倍宽
                d.hex.flag = true;
                d.hex.str = "";
                //加粗
                switch(key.toInt() & 0x4) {
                case 0: d.hex.str += HEX_FONT_BOLD0; break;
                case 1: d.hex.str += HEX_FONT_BOLD1; break;
                }
                //倍高倍宽
                switch(key.toInt() & 0x03) {
                case 0: d.hex.str += HEX_FONT_SCALE00; break;
                case 1: d.hex.str += HEX_FONT_SCALE01; break;
                case 2: d.hex.str += HEX_FONT_SCALE10; break;
                case 3: d.hex.str += HEX_FONT_SCALE11; break;
                }
                d.hex.str = hex2_Xstr(d.hex.str);
                goto resloveEnd;
            } else if(with_control && "[" == key) {
                d.tableFlag = true;
                ins_table.prepare_begin = true;
                ins_table.prepare_end = false;
                ins_stack_push(key, stack_update);
                k += 1;
                k += getInt(line, k, ins_table.prepare_begin_column_count);
                ins_table.prepare_begin_column_count = qMax(1, ins_table.prepare_begin_column_count);
                goto resloveEnd;
            } else if(with_control && "]" == key) {
                d.tableFlag = true;
                ins_table.prepare_end = true;
                ins_stack_push(key, stack_update);
                k += 1;
                goto resloveEnd;
            } else if(with_control && "(" == key) {
                d.cellFlag = true;
                ins_cell.prepare_begin = true;
                ins_cell.prepare_end = false;
                ins_stack_push(key, stack_update);
                k += 1;
                ins_cell.param = getWord(line, k);
                k += ins_cell.param.length();
                ins_cell.begin = mapValue(ins_cell.param).length() > 0;

                goto resloveEnd;
            } else if(with_control && ")" == key) {
                d.cellFlag = true;
                ins_cell.prepare_end = true;
                ins_stack_push(key, stack_update);
                k += 1;
                goto resloveEnd;
            } else if(with_control && "#" == key) {
                contentKip = true;
            } else if(with_control && "pic" == line.mid(k, 3)) {
                if(with_pic_qrc_bar) {
                    d.picPath = line.mid(k + 3).trimmed();
                    ins << pieceIns(HEX_ALIGN_CENTER, "");
                    ins << pieceIns(Printer_Normal().pic_code(d.picPath), "\n");
                    ins << pieceIns(HEX_ALIGN_LEFT, "");
                }
                return ins;
            } else if(with_control && "qrc" == line.mid(k, 3)) {
                if(with_pic_qrc_bar) {
                    d.qrcTxt = line.mid(k + 3).trimmed();
                    ins << pieceIns(HEX_ALIGN_CENTER, "");
                    ins << pieceIns(Printer_Normal().qr_code(d.qrcTxt), "\n");
                    ins << pieceIns(HEX_ALIGN_LEFT, "");
                }
                return ins;
            } else if(with_control  && "bar" == line.mid(k, 3)) {
                if(with_pic_qrc_bar) {
                    d.barTxt = line.mid(k + 3).trimmed();
                    ins << pieceIns(HEX_ALIGN_CENTER, "");
                    ins << pieceIns(Printer_Normal().bar_code(d.barTxt), "\n");
                    ins << pieceIns(HEX_ALIGN_LEFT, "");
                }
                return ins;
            } else if("/" == line.mid(k ,1)) {// <=> '' ,  \是转义字符
                d.txt.flag = true;
                d.begin ;
                goto resloveEnd;
            } else {
                QString word = getWord(line, k);
                QString txt =  mapValue(word, mapValueOK);
                if(mapValueOK) {//%m1
                    d.param.flag = true;
                    d.param.str = txt;
                    k += word.length();
                    k--;
                } /*else {//%你好   这种没有意思
                    d.txt.flag = true;
                    d.txt.str = getWord(line, k);
                    k += d.txt.str.length();
                    k--;
                }*/
                goto resloveEnd;
            }
        }
        //end
        if(k >= line.count() - 1) {//结束
            k++;
            goto resloveEnd;
        }
        continue;

resloveEnd:
        //===============TEXT============
        if(d.txtQuestion.flag) {
            line_data_list << lineData(d.begin,
                                       k + 1,
                                       d.txtQuestion.str );
        }
        if(d.txtAlign.flag) {
            line_data_list << lineData(d.begin,
                                       k + 1,
                                       d.txtAlign.str );
        }
        if(d.txt.flag) {
            line_data_list << lineData(d.begin,
                                       k + 1,
                                       d.txt.str);
        }
        if(d.param.flag) {
            line_data_list << lineData(d.begin,
                                       k + 1,
                                       d.param.str );
        }
        //===============CONTROL============
        if(d.hex.flag) {//hex
            line_data_list << lineData(d.begin,
                                       k + 1,
                                       "  " + d.hex.str );
        }
        if(d.percentFlag) {//%
            line_data_list << lineData(d.begin,
                                       k,
                                       "% " );
            k --;
        }
        if(d.tableFlag) {//%[ or %]
            line_data_list << lineData(d.begin,
                                       k,
                                       QString(k - d.begin, ' ') );
        }
        if(d.cellFlag) {//%( or %)
            line_data_list << lineData(d.begin,
                                       k,
                                       QString(k - d.begin, ' ') );
        }

        d.init();
    }

    for(int count = line_data_list.count() - 1; count >= 0; count --) {
        line_data_list[count].replace(line);
    }

    //resloveLineIns
    ins = resloveLineIns(line, contentKip);
    //trimedEmptySkip
    if(trimedEmptySkip) {
        for(int k = 0; k < ins.count(); k++) {
            if(ins[k].second.trimmed().length() > 0) {
                return ins;
            }
        }
        return blockIns();
    }
    //end
    return ins;
}

QString txt_ins_bill_print::resloveText(QString line)
{
    QString str1;
    blockIns block = resloveLine(line, false);
    foreach(const pieceIns& p, block) {
        str1 += p.second;
    }
    str1.chop(1);
    return str1;
}


//\x1B\x45\x00\x1D\x21\x01
blockIns txt_ins_bill_print::resloveResult(const QString &str)
{
    blockIns ins;
    QStringList linelist = str.split("\n");
    for(int k = 0; k < linelist.count(); k++) {
        QStringList plist = linelist[k].split("\\x");
        pieceIns p;
        QString content;
        for(int m = 0; m < plist.count(); m ++) {
            content = plist[m];
            if(m == 0) {
                p.second = /*trans_UTF82GBK*/(content.toLocal8Bit());
                ins << p;
                p.first = "";
                p.second = "";
            }
            if(m > 0) {
                if(content.length() == 2) {
                    p.first += content.toLocal8Bit();
                }

                if(content.length() > 2) {
                    p.first += content.left(2).toLocal8Bit();
                    p.second = /*trans_UTF82GBK*/(content.mid(2).toLocal8Bit());
                    ins << p;
                    p.first = "";
                    p.second = "";
                }
                if(content.length() < 2) {
                    p.second = /*trans_UTF82GBK*/(content.toLocal8Bit());
                    ins << p;
                    p.first = "";
                    p.second = "";
                }
            }
        }
        if(p.first.length() > 0 || p.second.length() > 0) {
            ins << p;
        }
        ins << pieceIns("", "\n");
    }

    //qDebug() << ins;
    return ins;
}

blockIns txt_ins_bill_print::resloveLineIns(const QString &line, bool contentKip)
{
    blockIns ins;
    //
    QStringList plist = line.split("\\x");
    pieceIns p;
    QString content;
    for(int m = 0; m < plist.count(); m ++) {
        content = plist[m];
        if(m == 0) {
            p.second = /*trans_UTF82GBK*/(content.toLocal8Bit());
            ins << p;
            p.first = "";
            p.second = "";
        }
        if(m > 0) {
            if(content.length() == 2) {
                p.first += content.toLocal8Bit();
            }

            if(content.length() > 2) {
                p.first += content.left(2).toLocal8Bit();
                p.second = /*trans_UTF82GBK*/(content.mid(2).toLocal8Bit());
                ins << p;
                p.first = "";
                p.second = "";
            }
            if(content.length() < 2) {
                p.second = /*trans_UTF82GBK*/(content.toLocal8Bit());
                ins << p;
                p.first = "";
                p.second = "";
            }
        }
    }
    if(p.first.length() > 0 || p.second.length() > 0) {
        ins << p;
    }
    //
    if(ins_table.index >= 0) {
        if((ins_table.index + 1) % ins_table.prepare_begin_column_count == 0) {
            ins << pieceIns("", "\n");
        }
        if((ins_table.index + 1) >= ins_table.index_count) {
            if(false == ins.last().second.endsWith("\n")) {
                ins << pieceIns("", "\n");
            }
        }
    } else {
        ins << pieceIns("", "\n");
    }
    //
    if(contentKip) {
        for(int k = 0; k < ins.count(); k++) {
            ins[k].second = "";
        }
    }
    return ins;
}

QString txt_ins_bill_print::mapValue(const QString &key, bool &ok)
{
    ok = false;
    for(int row = 0; row < model_d->rowCount(); row ++) {
        if(model_d->headerString(row, Qt::Vertical).mid(1) == key) {// remove first char %
            QString d = model_d->data(row, 0).toString();
            if(false == d.trimmed().isEmpty())
                map_valid_count ++;

            if(ins_table.prepare_begin)
            {
                QSharedPointer<cJSON> array = QSharedPointer<cJSON>(cJSON_Parse(d.toLocal8Bit()), cJSON_Delete);
                if(false == array.isNull()) {
                    int array_count = cJSON_GetArraySize(array.data());
                    if(ins_table.index_count == -1) {
                        ins_table.begin = true;
                        ins_table.index_count = cJSON_GetArraySize(array.data());
                    }
                    if(ins_table.index >= 0 && ins_table.index < ins_table.index_count) {
                        ok = true;
                        return cJSON_GetArrayItem(array.data(), ins_table.index)->valuestring;
                    }
                    return "";
                }
            }
            ok = true;
            return d;
        }
    }
    return "";
}

bool txt_ins_bill_print::isInt(const QString &str)
{
    bool ok;
    str.toInt(&ok);
    return ok;
}

int txt_ins_bill_print::getInt(const QString &str, int begin, int &intValue)
{
    QString s;
    for(int k = begin; k < str.count(); k++) {
        if(false == isInt(str.mid(k, 1))) {
            break;
        }
        s += str.mid(k, 1);
    }
    intValue = 0;
    if(s.count() > 0)
        intValue = s.toInt();
    return s.count();
}

int txt_ins_bill_print::txtWidth(const QString &txt)
{
    return lds::size_g_of_u(txt);
}

QString txt_ins_bill_print::hex2_Xstr(const QString &hex)
{
    int k = 0;
    QString h = hex;
    while(k < h.count()) {
        h.insert(k, "\\x");
        k += 4;
    }
    return h;
}

QImage txt_ins_bill_print::billToImage()
{
    return billToImage(billToIns());
}

void txt_ins_bill_print::setResloveTable(const txt_ins_bill_print::resloveTable &r)
{
    ins_table = r;
}

void txt_ins_bill_print::setResloveCell(const txt_ins_bill_print::resloveCell &r)
{
    ins_cell = r;
}

QString txt_ins_bill_print::getFormat_data()
{
    return format_data;
}

const txt_ins_bill_print::resloveTable &txt_ins_bill_print::get_ins_table() const
{
    return ins_table;
}

const txt_ins_bill_print::resloveCell &txt_ins_bill_print::get_ins_cell() const
{
    return ins_cell;
}

void txt_ins_bill_print::ins_table_index_equal0()
{
    ins_table.index = 0;
}

void txt_ins_bill_print::ins_table_next()
{
    ins_table.index ++;
}

QImage txt_ins_bill_print::billToImage(const blockIns &ins)
{
    QFont font = qApp->font();
    font.setFamily(lds::FONT_SONGTI_Family);
    font.setPointSize(18);
    QFontMetrics fm(font);
    double scle = PRINTER_80MM_WIDTH_PIXELS / fm.width(QString(PRINTER_80MM_WIDTH_CHARS, '-'));
    scle = 1.0;

    QImage image(paint_image(0, font, ins) * scle, QImage::Format_ARGB32);
    image.fill(QColor("white"));
    QPainter painter(&image);
    painter.scale(scle, scle);
    paint_image(&painter, font, ins);

#ifdef QT_DEBUG
    image.save("txt_ins_bill_print.jpg");
#endif
    return image;
}

blockIns txt_ins_bill_print::tableValue(const lds_model_sqltablemodel_d *model_d, const QStringList &content_list, const txt_ins_bill_print &org, bool with_pic_qrc_bar)
{
    blockIns k;
    txt_ins_bill_print d(model_d, content_list.join("\n"));
    d.setResloveTable(org.get_ins_table());
    d.setResloveCell(org.get_ins_cell());
    for(d.ins_table_index_equal0(); d.get_ins_table().index < d.get_ins_table().index_count; d.ins_table_next()) {
        //%pm[1]%(%ss%)
        k += d.reslove(d.getFormat_data(), with_pic_qrc_bar);
    }
    return k;
}

blockIns txt_ins_bill_print::cellValue(const lds_model_sqltablemodel_d *model_d, const QStringList &content_list, const txt_ins_bill_print &org, bool with_pic_qrc_bar)
{
    blockIns k;
    txt_ins_bill_print d(model_d, content_list.join("\n"));
    d.setResloveTable(org.get_ins_table());
    d.setResloveCell(org.get_ins_cell());
    k += d.reslove(d.getFormat_data(), with_pic_qrc_bar);
    return k;
    //    for(;d.get_ins_table().index < d.get_ins_table().index_count; d.ins_table_next()) {
    //        //%pm[1]%(%ss%)
    //        k += d.reslove(d.getFormat_data(), with_pic_qrc_bar);
    //    }
    //    return k;
}

QSize txt_ins_bill_print::paint_image(QPainter *painter, QFont font, const blockIns &ins)
{
    QFontMetrics fontm(font);
    if(painter) painter->setFont(font);

    QSize img_size = QSize(0, 0);
    int char_h = fontm.height();
    double xscale = 1;
    double yscale = 1;
    int xoff = 0;
    int yoff = 0;
    int text_width;
    QRect text_rect;
    for(int k = 0; k < ins.count(); k ++) {
        if(ins[k].first.count() == 3) {
            //ins[k].first[0] == "s";
            if(xscale == 1 && ins[k].first[1] == '0') xscale = 1;
            else if(xscale == 2 && ins[k].first[1] == '0') xscale = 0.5;
            else if(xscale == 1 && ins[k].first[1] == '1') xscale = 2;
            else if(xscale == 2 && ins[k].first[1] == '1') xscale = 1;

            if(yscale == 1 && ins[k].first[2] == '0') yscale = 1;
            else if(yscale == 2 && ins[k].first[2] == '0') yscale = 0.5;
            else if(yscale == 1 && ins[k].first[2] == '1') yscale = 2;
            else if(yscale == 2 && ins[k].first[2] == '1') yscale = 1;
            if(painter) painter->scale(xscale, yscale);
            xscale = qMax(1.0, xscale);
            yscale = qMax(1.0, yscale);
        }

        text_width = fontm.width(ins[k].second);
        text_rect = QRect(xoff / xscale, yoff / yscale, text_width, char_h);
        text_rect.adjust(0, -text_rect.height() / 4, 0, text_rect.height() / 4);
        //        if(painter) painter->fillRect(text_rect, QColor("red"));
        if(painter) painter->drawText(text_rect, Qt::AlignLeft | Qt::AlignVCenter, ins[k].second);
        if(ins[k].second.endsWith("\n") || k == ins.count() - 1) {
            img_size.setWidth(qMax(img_size.width(), xoff));
            img_size.setHeight(qMax(img_size.height(), yoff));

            xoff = 0;
            yoff += char_h * yscale;
        } else {
            xoff += text_width;
        }
    }

    return img_size;
}

//#%?+str%-np
//?-你好:%-8m5
//?+服务费：%-fw3

//?(*%p*)
int txt_ins_bill_print::resloveQuestion(const QString &line, int k, QString &ret)
{
    QString m;
    QString p1, p2;
    //    int percentCount = 0;
    int n1 = 0;
    int n2 = 0;
    QString param;
    QString param_str;
    QString f1, f2, str1, str2;
    bool ok;
    const int begin = k;

    k += 1;

    //?(*%p*)
    if(line.mid(k, 1) == "(") {
        for(; k < line.length(); k++) {
            if(line.mid(k, 2) == ")") {
                break;
            }
        }
        k += 1;
        m = mid(line, begin + 1, k);//+str%+np

        int map_valid_count_begin = map_valid_count;
        str1 = resloveText(m);
        int map_valid_count_end = map_valid_count;
        ret = str1;
        if(map_valid_count_begin == map_valid_count_end)
            ret = "";

        return k - begin;
    }
    //?+服务费：%-fw3
    if(line.mid(k, 1) == "+" || line.mid(k, 1) == "-") {
        for(; k < line.length(); k++) {
            if(line.mid(k, 2) == "%+"
                    || line.mid(k, 2) == "%-"
                    ) {
                p1 = mid(line, begin + 1, k);
                break;
            }
        }
        k += 1;
        p2 = getWord(line, k);//+np
        k+= p2.length();
        m = mid(line, begin + 1, k);//+str%+np
        ///
        f1 = p1.left(1);
        p1 = p1.mid(1);
        str1 = p1;
        while(str1.count() > 0 && str1.mid(str1.length() - 1, 1) == " ")
            str1.chop(1);
        str1 = resloveText(str1);

        f2 = p2.left(1);
        param = p2.mid(1 + getInt(p2, 1, n2));//p
        param_str = mapValue(param, ok);
        str2 = param_str;

        if(n2 == 0) {
            if(f2 == "-")
                n2 = txtWidth(str2);
            else
                n2 = 2 +  txtWidth(m) - txtWidth(str1);
        }
        //
        n1 = 2 + txtWidth(m) - n2;
        //    }
        if(param_str.length() == 0)
            ret = QString(2 + txtWidth(m), ' ');//2:%?
        else
            ret = flagFill(f1, n1, str1) + flagFill(f2, n2, str2);
    }
    return m.length() + 1;
}

//-15m7
int txt_ins_bill_print::resloveAlign(const QString &line, int k, QString &ret, bool &hasParam)
{
    const int k_org = k;
    int width;
    QString m = getWord(line, k);
    int ret_skip = m.length();
    QString f = line.mid(k, 1);
    k += 1;

    k += getInt(line, k, width);

    //
    m = m.mid(k - k_org);

    //%%
    QString str = m.replace("%%", "%") + QString(m.count("%%"), ' ');
    str = mapValue(str, hasParam);
    if(str.trimmed().isEmpty())
        str = m;

    ret = flagFill(f, width, str);

    return ret_skip;
}

txt_ins_bill_print::resloveData txt_ins_bill_print::resloveAlignFlag(const QString &line, int k, txt_ins_bill_print::resloveData d)
{
    bool hasParam = false;
    d.k_off += resloveAlign(line, k, d.txtAlign.str, hasParam);
    //左对齐，看一格
    int off;
    if(hasParam)
        off = d.rpLength(k) - txtWidth(d.txtAlign.str);
    else
        off = d.rpLength(k) - d.txtAlign.str.length();

    //
    if("-" == line.mid(k, 1)) {
        if(0 <= off) {
            d.txtAlign.str = QString(off, ' ') + d.txtAlign.str;
        } else {
            while(0 > off
                  && " " == line.mid(d.begin - 1, 1)) {
                d.begin -=1;

                off ++;
            }
        }
    }

    //右对齐，看两格
    if("+" == line.mid(k, 1)) {
        if(0 <= off) {
            d.txtAlign.str = d.txtAlign.str + QString(off, ' ');
        } else {
            while(0 > off
                  && " " == line.mid(k + d.k_off, 1)) {
                d.k_off += 1;

                off ++;
            }
        }
    }
    return d;
}

QString txt_ins_bill_print::mapValue(const QString &key)
{
    bool ok;
    return mapValue(key, ok);
}

QString txt_ins_bill_print::flagFill(const QString &flag, int width, const QString &str)
{
    char fill = ' ';

    if(flag == "+") {
        return str + QString(width - txtWidth(str), fill);
    }
    return QString(width - txtWidth(str), fill) + str;
}

QString txt_ins_bill_print::getWord(const QString &text, int k, QStringList splitValues)
{
    if(splitValues.isEmpty())
        splitValues << "%" << " ";

    QString ret;
    for(; k < text.length(); k++) {
        //        if(text.mid(k, 2) == "%%") {
        //            ret += text.mid(k, 1);
        //            k ++;
        /*  } else*/ if(splitValues.contains(text.mid(k, 1))) {
            break;
        }
        ret += text.mid(k, 1);
    }
    return ret;
}

QString txt_ins_bill_print::mid(const QString &line, int begin, int end)
{
    return line.mid(begin, end - begin);
}

void txt_ins_bill_print::reslove_check(const QStringList &list, int &k, QStack<QString> &ins_stack)
{
    qDebug() << __LINE__ << "(" << sqr++ << this << list[k];
    k ++;
    qDebug() << __LINE__ << "(" << sqr++ << this << ins_stack;
    if(ins_stack.count() > 0 && ins_stack.top() == "]") {
        while(ins_stack.count() > 0 && ins_stack.pop() != "[");
    }
    if(ins_stack.count() > 0 && ins_stack.top() == ")") {
        while(ins_stack.count() > 0 && ins_stack.pop() != "(");
    }
    qDebug() << __LINE__ << "(" << sqr++ << this << ins_stack;
}

void txt_ins_bill_print::ins_stack_push(const QString &key, bool enable)
{
    if(enable)
        ins_stack.push(key);
}

void txt_ins_bill_print::lineData::replace(QString &str)
{
    str.replace(begin, end - begin, replaceValue);
}

int txt_ins_bill_print::resloveData::rpLength(int k)
{
    return k + k_off - begin;
}
