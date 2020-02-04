#include "public_printby_ch_billno_over_view.h"
#include <QPainter>
#include <QFontDatabase>
#include "printer_normal.h"
#include <QMouseEvent>
#include "public_printby_ch_billno_over_view_set.h"
#include "transparentcenterdialog.h"
#include "lds_messagebox.h"
#include "public_select_stringlist.h"
#include "lds.h"
#include "backheader.h"
#include <QApplication>
#include"bmp_info.h"

static void convert2Gray(QImage &img) {
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            int gray = qGray(img.pixel(i, j));
            img.setPixel(i, j, QColor(gray, gray, gray).rgb());
        }
    }
}
static void convert2Black(QImage &img) {
    QRgb d = QColor(IMAGE_GRAY_DIV).rgb();
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            if(img.pixel(i, j) >= d) {
                img.setPixel(i, j, QColor("white").rgb());
            } else {
                img.setPixel(i, j, QColor("black").rgb());
            }
        }
    }
}

public_printby_ch_billno_over_view::public_printby_ch_billno_over_view(QWidget *parent) :
    QDialog(parent),
    public_printby_ch_billno_over_view_data()
{
    setFocusPolicy(Qt::StrongFocus);
    setFont(SONGTI_font);
    line_char_count = 48;
}

void public_printby_ch_billno_over_view::setPaperWidth(int paperwidth)
{
    QFontMetrics fm(SONGTI_font);
    public_printby_ch_billno_over_view_data::setPaperWidth(paperwidth);
    this->setFixedWidth(fm.width(QString(line_char_count, '-')));
}
void public_printby_ch_billno_over_view::refresh()
{
    _refresh();
    update();
}

void public_printby_ch_billno_over_view::print()
{
    lds_query_hddpos  query;
    QStringList list;
    query.execSelect(QString(" SELECT vch_printip FROM hddpos.cey_bt_kitchen_print "));
    while(query.next()) {
        list << query.recordValue("vch_printip").toString();
    }
    public_select_stringlist dialog(list, this);
    QString printpath;
    if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
        printpath = dialog.curSelect(Qt::UserRole).toString();
    }
    //
    blockIns ins0 = ins;
    ins0.append(pieceIns("1B6405 1D5601", ""));//走纸5x， 切纸
    backheader::reslove_write(ins0, printpath);
}

void public_printby_ch_billno_over_view::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    paint(&painter);
    painter.fillRect(QRect(0, 0, this->width(), 40), QColor("#ff546b"));
    painter.drawPixmap(QRect(5, 5, 30, 30), QPixmap(":/image/hightcontrast/bill_printer.png"));
    painter.drawPixmap(QRect(40, 5, 30, 30), QPixmap(":/image/hightcontrast/go-up.png"));
    painter.drawPixmap(QRect(75, 5, 30, 30), QPixmap(":/image/hightcontrast/go-down.png"));
    painter.drawPixmap(QRect(110, 5, 30, 30), QPixmap(":/image/hightcontrast/emblem-system.png"));
}

void public_printby_ch_billno_over_view::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
    if(QRect(5, 5, 30, 30).contains(e->pos())) {
        print();
    } else if(QRect(40, 5, 30, 30).contains(e->pos())) {//up
        yscroll_off = qMin(0, yscroll_off + (this->height() - 40));
        update();
    } else if(QRect(75, 5, 30, 30).contains(e->pos())) {//down
        yscroll_off = yscroll_off - (this->height() - 40);
        update();
    } else if(QRect(110, 5, 30, 30).contains(e->pos())) {//down
        public_printby_ch_billno_over_view_set dialog(line_char_count, this);
        dialog.setWindowTitle(QObject::tr("设置"));
        if(QDialog::Accepted == transparentCenterDialog(&dialog).exec()) {
            line_char_count = dialog.paperwidth();
            yscroll_off = 0;
            QFontMetrics fm(SONGTI_font);
            this->setFixedWidth(fm.width(QString(line_char_count, '-')));
            refresh();
            emit signal_resize();
        }
    }
}

public_printby_ch_billno_over_view_data::public_printby_ch_billno_over_view_data()
{
    bg_count = 0;
    yscroll_off = 0;
    line_char_count = 48;
    SONGTI_font = qApp->font();
    SONGTI_font.setFamily(lds::FONT_SONGTI_Family);
    SONGTI_font.setPointSize(18);
    //
    QFontMetrics fm(SONGTI_font);
    char_h = fm.height();
    char_w = fm.width("-");
}

void public_printby_ch_billno_over_view_data::initData(lds_model_sqltablemodel *model_cache)
{
    this->model_cache = model_cache;

    model_cache->tablename_kvmap_insert_field("cey_bt_dish_type", "11", "vch_dish_typename", "默认大类`");

    model_cache->tablename_kvmap_insert_field("cey_bt_dish_subtype", "1101", "vch_sub_typename", "主荤");
    model_cache->tablename_kvmap_insert_field("cey_bt_dish_subtype", "1102", "vch_sub_typename", "鱼类海鲜");
    model_cache->tablename_kvmap_insert_field("cey_bt_dish_subtype", "1103", "vch_sub_typename", "热菜");

    //    ================ "sale_detail_title" ================
    //    --------------begin--------------
    model_cache->tablename_kvmap_insert_field("sale_detail_title", "0", createMap(
                                                  "dishname	unit	num	price	amount	disdetail	vch_print_memo	vch_reason	ch_dish_typeno	ch_sub_typeno	ch_suitflag	int_flowID	ch_specialflag	ch_presentflag	ch_waidai_flag	num_price_org	num_price_add	",
                                                  "品名	单位	数量	单价	小计	折扣												"));
    //            --------------end--------------
    //                                                      --------------begin--------------
    model_cache->tablename_kvmap_insert_field("sale_detail", "0", createMap(
                                                  "dishname	unit	num	price	amount	disdetail	vch_print_memo	vch_reason	ch_dish_typeno	ch_sub_typeno	ch_suitflag	int_flowID	ch_specialflag	ch_presentflag	ch_waidai_flag	num_price_org	num_price_add	",
                                                  "剁椒鱼头/牛哇	只	1	20	20	0			11	1101	P	966	N	N	Y	20	0	"));
    model_cache->tablename_kvmap_insert_field("sale_detail", "1", createMap(
                                                  "dishname	unit	num	price	amount	disdetail	vch_print_memo	vch_reason	ch_dish_typeno	ch_sub_typeno	ch_suitflag	int_flowID	ch_specialflag	ch_presentflag	ch_waidai_flag	num_price_org	num_price_add	",
                                                  "肉酿面筋	500克	1	0	0	0	加酱		11	1101	Y	967	N	N		4	1	"));
    model_cache->tablename_kvmap_insert_field("sale_detail", "2", createMap(
                                                  "dishname	unit	num	price	amount	disdetail	vch_print_memo	vch_reason	ch_dish_typeno	ch_sub_typeno	ch_suitflag	int_flowID	ch_specialflag	ch_presentflag	ch_waidai_flag	num_price_org	num_price_add	",
                                                  "糖醋/梅汁小排	份1	1	0	0	0			11	1101	Y	968	N	N		12	0	"));
    model_cache->tablename_kvmap_insert_field("sale_detail", "3", createMap(
                                                  "dishname	unit	num	price	amount	disdetail	vch_print_memo	vch_reason	ch_dish_typeno	ch_sub_typeno	ch_suitflag	int_flowID	ch_specialflag	ch_presentflag	ch_waidai_flag	num_price_org	num_price_add	",
                                                  "酸辣鸡杂	份1	1	8	8	0			11	1102	N	969	Y	N		8	0	"));
    model_cache->tablename_kvmap_insert_field("sale_detail", "4", createMap(
                                                  "dishname	unit	num	price	amount	disdetail	vch_print_memo	vch_reason	ch_dish_typeno	ch_sub_typeno	ch_suitflag	int_flowID	ch_specialflag	ch_presentflag	ch_waidai_flag	num_price_org	num_price_add	",
                                                  "肉沫豆腐/粉皮	份1	1	4	4	0		退货	11	1103	N	970	N	Y	N	5	0	"));
    //                                                      --------------end--------------


    model_cache->tablename_enable_query(false);
    //
    _refresh();
}

QImage public_printby_ch_billno_over_view_data::getImage(const QString &logo_file_path)
{
    //    480 - 30
    //    int width = line_char_count * PRINTER_CHAR_WIDTH_PIXELS;
    int height = paint(0).height();
    int width = 0;
    QRect paint_rect = QRect(0, 0, 600, height);

    QImage image(paint_rect.size(), QImage::Format_ARGB32);
    {
        image.fill(QColor("white"));
        QPainter painter(&image);
        width = paint(&painter).width();
    }
    if(image.width() > width)
        image = image.copy(0, 0, width, height);
    image = image.scaledToWidth(line_char_count * PRINTER_CHAR_WIDTH_PIXELS, Qt::SmoothTransformation);
    convert2Black(image);
    //
    QImage logo(logo_file_path);
    if(false == logo.isNull()) {
        QImage image2(image.width(), image.height() + logo.height(), QImage::Format_RGB16);
        image2.fill(QColor("white"));
        QPainter painter(&image2);
        painter.drawImage((image2.width() - logo.width()) / 2, 0, logo);
        painter.drawImage(0, logo.height(), image);
        image = image2;
    }

    return image;
}

void public_printby_ch_billno_over_view_data::setPaperWidth(int paperwidth)
{
    line_char_count = paperwidth;
}

int public_printby_ch_billno_over_view_data::curPaperWidth()
{
    return line_char_count;
}

const blockIns &public_printby_ch_billno_over_view_data::get_ins()
{
    return ins;
}

void public_printby_ch_billno_over_view_data::_refresh()
{

    //            ================ "pay_detail" ================
    //            --------------begin--------------
    model_cache->tablename_kvmap_drop("pay_detail");
    if(f_get_sysparm("bp_fmt_cost") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_cost", "0.00");
    if(f_get_sysparm("bp_fmt_discount") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_discount", "0.00");
    if(f_get_sysparm("bp_fmt_present") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_present", "0.00");
    if(f_get_sysparm("bp_fmt_service") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_service", "0.00");
    if(f_get_sysparm("bp_fmt_tax") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_rate", "0.00");
    if(f_get_sysparm("bp_fmt_lowcost") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_lowcost", "0.00");
    if(f_get_sysparm("bp_fmt_free") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_free", "0.00");
    if(f_get_sysparm("bp_fmt_blotout") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_blotout", "0.00");

    if(f_get_sysparm("bp_fmt_receive") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_realamount", "0.00");
    if(f_get_sysparm("bp_fmt_cash") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_amount", "0.00");
    if(f_get_sysparm("bp_fmt_change") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_change", "0.00");

    if(f_get_sysparm("bp_fmt_avg") == "1")model_cache->tablename_kvmap_insert_value("pay_detail", "num_realamount_per_person", "0.00");
    //            --------------end--------------

    model_cache->tablename_kvmap_print("pay_detail");


    //            ================ "pay_detail_paymode" ================
    //            --------------begin--------------
    //            "Key	vch_paymodename	real_payamount	"
    model_cache->tablename_kvmap_drop("pay_detail_paymode");
    if(f_get_sysparm("bp_fmt_pay_detail") == "1") {
        model_cache->tablename_kvmap_insert_field("pay_detail_paymode", "00", "vch_paymodename", "现金");
        model_cache->tablename_kvmap_insert_field("pay_detail_paymode",  "00", "real_payamount", "0.00");
        model_cache->tablename_kvmap_insert_field("pay_detail_paymode",  "02", "vch_paymodename", "会员");
        model_cache->tablename_kvmap_insert_field("pay_detail_paymode",  "02", "real_payamount", "0.00");
    }
    model_cache->tablename_kvmap_drop("pay_detail_member");
    if(f_get_sysparm("bp_fmt_m_no") == "1") model_cache->tablename_kvmap_insert_field("pay_detail_member", "1001", "vch_voucherno", "1001");
    if(f_get_sysparm("bp_fmt_m_name") == "1") model_cache->tablename_kvmap_insert_field("pay_detail_member", "1001", "vch_card_name", "1001");
    if(f_get_sysparm("bp_fmt_m_amount") == "1") model_cache->tablename_kvmap_insert_field("pay_detail_member", "1001", "num_card_remain", "0.00");
    if(f_get_sysparm("bp_fmt_m_point") == "1") model_cache->tablename_kvmap_insert_field("pay_detail_member", "1001", "num_card_point", "0.00");
    if(f_get_sysparm("bp_fmt_m_adress") == "1") model_cache->tablename_kvmap_insert_field("pay_detail_member", "1001", "vch_card_adress", "NAN JING QIXIA STREET No.:25.66");

    if(model_cache->tablename_kvmap_count("pay_detail_member") > 0) model_cache->tablename_kvmap_insert_field("pay_detail_member", "1001", "cash_pay_flag", "N");

    //            --------------end--------------



    //            ================ "receipt_head" ================
    //            --------------begin--------------
    //            "Key		"
    model_cache->tablename_kvmap_drop("receipt_head");
    //    if(f_get_sysparm("bp_fmt_print_prepare") == "1") model_cache->tablename_kvmap_insert_value("receipt_head", "prepare", "");
    //    if(f_get_sysparm("bp_fmt_title_logo_en") == "1") model_cache->tablename_kvmap_insert_value("receipt_head", "logo",  "/home/cz/视频/0fastfdupdate/fastfd_localdata/logo.png");
    if(f_get_sysparm("bp_fmt_title_enable") == "1") model_cache->tablename_kvmap_insert_value("receipt_head", "first_title", "付款单");
    if(f_get_sysparm("bp_fmt_title_sc_enable") == "1") model_cache->tablename_kvmap_insert_value("receipt_head", "second_title", f_get_sysparm("bp_fmt_title_second"));
    if(f_get_sysparm("bp_fmt_top_enable") == "1") model_cache->tablename_kvmap_insert_value("receipt_head", "top", f_get_sysparm("bp_fmt_top_desc"));
    //    if(payReprint == pay_mode)           model_cache->tablename_kvmap_insert_value("receipt_head", "reprint", print_format.printTr(QT_TR_NOOP("补打")));

    //                                                      --------------end--------------



    //                                                      ================ "sale_head" ================
    //                                                      --------------begin--------------
    //                                                      "Key		"
    model_cache->tablename_kvmap_drop("sale_head");
    if(f_get_sysparm("bp_fmt_payno") == "1")  model_cache->tablename_kvmap_insert_value("sale_head", "payno", "单号:P1805240500005");
    if(f_get_sysparm("bp_fmt_operid") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "operid", "工号:0000");
    if(f_get_sysparm("bp_fmt_table") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "table", "餐桌:Table1009(1009)");
    if(f_get_sysparm("bp_fmt_person") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "person",  "人数:1");
    if(f_get_sysparm("bp_fmt_waiter") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "waiter", "服务员:system");
    if(f_get_sysparm("bp_fmt_waitername") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "waitername", "服务员名称:system");
    if(f_get_sysparm("bp_fmt_serial_no") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "serial_no",  "流水号:010");
    if(f_get_sysparm("bp_fmt_time") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "time",  "时间:2018-05-24 14:53:20");
    if(f_get_sysparm("bp_fmt_times") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "times",  "时长:12:01");
    if(f_get_sysparm("bp_fmt_memo") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "memo",  "开台备注:welcome");

    if(f_get_sysparm("bp_fmt_address") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "address",  "地址:A");
    if(f_get_sysparm("bp_fmt_linkman") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "linkman",  "外卖人:1111");
    if(f_get_sysparm("bp_fmt_tel") == "1") model_cache->tablename_kvmap_insert_value("sale_head", "tel",  "电话:00000000000");

    //
    model_cache->tablename_kvmap_drop("receipt_tail");
    if(f_get_sysparm("bp_fmt_underwrite") == "1")  model_cache->tablename_kvmap_insert_value("receipt_tail", "signature",  "签名" ":");
    if(f_get_sysparm("bp_fmt_end_desc").length() > 0)  model_cache->tablename_kvmap_insert_value("receipt_tail", "bottom",  f_get_sysparm("bp_fmt_end_desc"));

    public_printby_ch_billno printer;
    printer.model_bp_fmt = model_cache;
    printer.initData("");
    printer.makeIns(line_char_count);
    ins = printer.getPintIns();
}

QSize public_printby_ch_billno_over_view_data::paint(QPainter *painter)
{
    int max_witdh = 0;
    int x = 0;
    int y = 30 + 10 + yscroll_off;
    int charaters = 0;
    if(painter) painter->setFont(SONGTI_font);
    //    if(painter) setPainterFontPixel(painter, PRINTER_CHAR_WIDTH_PIXELS);

    if(painter) painter->setPen(Qt::black);
    if(painter) painter->setRenderHint(QPainter::SmoothPixmapTransform);
    blockIns ins= this->ins;

    Printer_Normal::reSort(ins);
    char blank = ' ';

    //remove blank
    for(int k = 0, count = ins.count(); k < count; k++) {
        ins[k].first = ins[k].first.replace(" ", "");
    }

    int align = Qt::AlignLeft | Qt::AlignTop;
    double xscale = 1;
    double yscale_o = 1;
    double yscale_n = 1;//换行时
    QList<QByteArray> cmds;
    for(int k = 0, count = ins.count(); k < count; k++) {
        cmds.clear();

        //make cmds by order
        foreach(const QByteArray &cmd, ins[k].first.split(',')) {
            if(cmd.length() == 0
                    || cmds.contains(cmd))
                continue;
            if(cmd.startsWith("i")
                    || cmd.startsWith("-")
                    ) {
                cmds.push_front(cmd);
            } else if(cmd.startsWith("s")) {
                cmds.insert(cmds.indexOf("i") + 1, cmd);
            } else {
                cmds.push_back(cmd);
            }
        }
        //conduct cmds
        foreach(const QByteArray &cmd, cmds) {
            if(cmd.startsWith("i")
                    || cmd.startsWith("-")
                    ) {
                if(painter) setPainterFont(painter, QFont::Bold);
                align = Qt::AlignLeft | Qt::AlignTop;
                if(painter) scale(painter, 1.0 / xscale, 1.0 / yscale_o);
                xscale = 1;
                yscale_o = 1;
                if(cmd.startsWith("-")) {
                    ins[k].second = QByteArray(line_char_count, '-');
                }
                //                cmds.clear();
                //                break;
            } else  if(cmd.startsWith("s")) {
                if(painter) scale(painter, 1.0 / xscale, 1.0 / yscale_o);

                xscale = cmd.mid(1, 1).toInt() + 1;
                yscale_o = cmd.mid(2, 1).toInt() + 1;
                if(painter) scale(painter, xscale, yscale_o);
            } else if(cmd.startsWith("a")) {
                if(cmd.mid(1) == "10") {
                    align = Qt::AlignLeft | Qt::AlignTop;
                } else if(cmd.mid(1) == "00") {
                    align = Qt::AlignHCenter | Qt::AlignTop;
                } else if(cmd.mid(1) == "01") {
                    align = Qt::AlignRight | Qt::AlignTop;
                }
            } else if(cmd.startsWith("b")) {
                if(cmd.mid(1) == "1") {
                    if(painter) setPainterFont(painter, QFont::Black);
                } else {
                    if(painter) setPainterFont(painter, QFont::Bold);
                }
            }
        }
        //check cmd content
        if(ins[k].second.length() == 0) {
            continue;
        }
        if(ins[k].second == "\n") {
            if(charaters == 0)//已经是换行了
                continue;
            y +=  char_h  * yscale_n;x = 0; yscale_n = 1;charaters = 0;
            continue;
        }
        //
        if(ins[k].second.contains("P191224")){
            qDebug() << ins[k].second ;
        }
        QList<QByteArray> text_list = textSplit_n_detect_width(charaters, xscale, ins[k].second);
        qDebug() << text_list;
        for(int k = 0, count = text_list.count(); k < count; k++ ) { //QString text, text_list) {
            QByteArray &text = text_list[k];
            if(align & Qt::AlignLeft) {
                //            text += QByteArray(qMax(0.0, line_char_count - textCharaters(text) * xscale) / xscale, blank);
            } else if(align & Qt::AlignHCenter) {
                text += QByteArray(qMax(0.0, line_char_count - textCharaters(text) * xscale) / xscale /2, blank);
                text = QByteArray(qMax(0.0, line_char_count - textCharaters(text) * xscale) / xscale, blank) + text;
            } else if(align & Qt::AlignRight) {
                //            text = QByteArray(qMax(0.0, line_char_count - textCharaters(text) * xscale) / xscale, blank) + text;
            }
        }

        //draw content
        //yscale_n 只支持换行是的高度的增加
        yscale_n = qMax(yscale_n, yscale_o);
        for(int k = 0, count = text_list.count(); k < count; k++ ) {
            QString text= text_list[k];
            if(painter) drawText(painter, QRect(x / xscale, y / yscale_o, fontm_width(painter, text),  char_h * yscale_n / yscale_o ), align, text);
            if(painter) x += fontm_width(painter, text) * xscale;
            if(painter) max_witdh = qMax(max_witdh, x);
            if(line_char_count != 0
                    && k == count - 1) {//最后一个
                charaters += textCharaters(text) * xscale;
                if(charaters > 0 && charaters % line_char_count == 0) {
                    y +=  char_h  * yscale_n;x = 0; yscale_n = 1;charaters = 0;//最后一次退出时，对yscale_n的初始化
                }
            } else {
                y +=  char_h  * yscale_n;x = 0; charaters = 0;//多个换行文本进入时，是不会改变yscale_n的值的
            }
        }
    }

    if(painter) scale(painter, 1.0 / xscale, 1.0 / yscale_o);
    return QSize(max_witdh, y + 30);
}

QString public_printby_ch_billno_over_view_data::f_get_sysparm(const QString &as_parameter, const QVariant &as_default)
{
    return public_printby_ch_billno::model_bp_fmt_value(model_cache, as_parameter, as_default);
}

QRect public_printby_ch_billno_over_view_data::scaleRect(const QRect &rect, double scale)
{
    return QRect(rect.x() * scale, rect.y() * scale, rect.width() * scale, rect.height() *scale);
}

int public_printby_ch_billno_over_view_data::textWidth(const QString &text)
{
    return lds::size_g_of_u(text) *  char_w;
}

int public_printby_ch_billno_over_view_data::textCharaters(const QString &text)
{
    return lds::size_g_of_u(text);
}

QString public_printby_ch_billno_over_view_data::textMid(const QString &text, int pos, int length)
{
    int c = 0;
    int begin = -1;
    int  len = -1;
    for(int k = 0; k < text.count(); k ++) {
        //
        if(begin == -1 && c >= pos) {
            begin = k;
        }
        if(len == -1 && c >= pos + length) {
            len = k - begin;
        }
        //
        c += lds::size_g_of_u(text.mid(k, 1));
    }
    return text.mid(begin, len);
}

QString public_printby_ch_billno_over_view_data::textAppendBlank(const QString &text, int ideal_width)
{
    return text + QString(ideal_width - textWidth(text), ' ');
}

void public_printby_ch_billno_over_view_data::drawText(QPainter *painter, QRect rect, int align, const QString &text)
{
    if(align & Qt::AlignLeft) {
        int all = lds::size_g_of_u(text);
        QRectF rectf(rect);
        qreal x = rectf.x();
        qreal y = rectf.y();
        qreal h = rectf.height();
        qreal f = rectf.width()  / all;
        foreach(const QString &k, text_detect_blank(text)) {
            qreal w = lds::size_g_of_u(k)  * f;
            public_sql::drawTextForArabic(painter, QRectF(x, y, w, h), align, k);
            x += w;
        }
        return;
    }

    public_sql::drawTextForArabic(painter, rect, align, text);
}

void public_printby_ch_billno_over_view_data::scale(QPainter *painter, qreal sx, qreal sy)
{
    painter->scale(sx, sy);
}

QList<QByteArray> public_printby_ch_billno_over_view_data::textSplit_n_detect_width(int xoff, int xscale, const QByteArray &text)
{

    //一行有5个char
    //\n\n\n            = 三行
    //hell\n\n\n      = 三行
    //hello\n\n\n    = 三行
    //hello1\n\n\n  = 四行

    QList<QByteArray> text_list = text.split('\n');
    int text_index;
    for(int k = 0; k < text_list.count() && line_char_count > 0; k ++) {

        QString text_str = text_list[k];
        QList<QByteArray> text_list_insert;
        while(( (xoff % line_char_count) + textCharaters(text_str) * xscale) > line_char_count) {//49 48
            text_index = (line_char_count - xoff % line_char_count) / xscale;
            QString mid_text = textMid(text_str, 0, text_index);
            text_list_insert << mid_text.toLocal8Bit();

            text_str.remove(0, mid_text.length());//可能会有中文
            xoff = 0;//第一次后必定是为0
        }
        xoff = 0;//第一次后必定是为0
        if(text_str.length() > 0)
            text_list_insert << text_str.toLocal8Bit();
        //
        if(text_list_insert.count() > 1) {
            text_list.removeAt(k);
            foreach(const QByteArray &byte, text_list_insert) {
                text_list.insert(k, byte);
                k ++;
            }
            k --;
        }
    }
    return text_list;
}

/*
 * ---12-23---:---,12-,23---
*/
QList<QString> public_printby_ch_billno_over_view_data::text_detect_blank(const QString &text)
{
    QList<QString> list;
    QChar pre = 0;
    foreach(QChar c, text) {
        if(pre == 0) {
            list.append("");
            list.last().append(c);
        } else if(pre == ' ' && c == ' ') {
            list.last().append(c);
        } else if(pre != ' ' && c != ' ') {
            list.last().append(c);
        } else if(pre == ' ' && c != ' ') {
            list.append("");
            list.last().append(c);
        } else if(pre != ' ' && c == ' ') {
            list.append("");
            list.last().append(c);
        }
        pre = c;
    }
    //
    for(int k = 1; k < list.count(); k ++) {
        if(list[k].trimmed() == "") {
            list[k - 1] += list[k];
            list[k] = "";
        }
    }
    //
    for(int k = 0; k < list.count(); ) {
        if(list[k].count() == 0) {
            list.removeAt(k);
        } else {
            k ++;
        }
    }
    return list;
}

QVariantMap public_printby_ch_billno_over_view_data::createMap(const QString &table_key, const QString &table_value)
{
    QVariantMap m;
    QStringList k = table_key.split("\t", QString::SkipEmptyParts);
    QStringList v = table_value.split("\t");
    for(int q = 0; q < k.count(); q++) {
        m.insert(k.value(q), v.value(q));
    }
    return m;
}

void public_printby_ch_billno_over_view_data::setPainterFont(QPainter *p, int weight)
{
    QFont f = p->font();
    f.setWeight(QFont::Light);
    f.setWeight(weight);
    p->setFont(f);
}

void public_printby_ch_billno_over_view_data::setPainterFontPixel(QPainter *p, int pixel)
{
    //        QFont f = p->font();
    //        QFontMetrics m(f);
    //        f.setPixelSize(m.height() / m.width("a") * pixel);
    //        m = QFontMetrics(f);
    //        p->setFont(f);
}

int public_printby_ch_billno_over_view_data::fontm_width(QPainter *p, const QString &text)
{
    QFont f = p->font();
    QFontMetrics m(f);
    return m.width(QString(textCharaters(text), '-'));
}
