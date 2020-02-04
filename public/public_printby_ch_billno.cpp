#include "public_printby_ch_billno.h"
#include <QtDebug>
#include "n_func.h"
#include "public_mainprinter.h"
#include "lds.h"
#include "lds.h"
#include "w_scr_dish_pay_widget.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "fexpandmain_2.h"
#include "lds_model_sqltablemodel.h"
#include "labelprint_main_dialog.h"
#include <QFile>
#include "public_dialog.h"
#include "public_sql.h"
#include "printer_normal.h"
#include "backheader.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_scr_table.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "printer_pos.h"
#include "txt_ins_bill_print.h"
#include "vch_print_memodata.h"
#include "public_printby_ch_billno_over_view.h"
#include "bmp_info.h"
#include "w_sys_manage_outer_pay_set_locale_wizard.h"
#include "label_img_header.h"
#include "lds.h"

public_printby_ch_billno::public_printby_ch_billno(const QString &ch_billno, const QString &languageKey, QObject *parent) :
    QObject(parent)
{
    model_bp_fmt = new lds_model_sqltablemodel(this);
    model_bp_fmt->setTable("cey_sys_parameter");
    model_bp_fmt->setFilter("vch_parameter like 'bp_fmt_%' ");
    this->ch_billno = ch_billno;
    initData(languageKey);
}

public_printby_ch_billno::public_printby_ch_billno()
    : QObject(0)
{
    model_bp_fmt = 0;
}

public_printby_ch_billno::~public_printby_ch_billno()
{
}

void public_printby_ch_billno::toPayPrint(const QString &bp_path, int bp_count, const QStringList &label_insert_int_flowid_list, payPrintModes m)
{
    if(m & payDrawer) {
        openDrawer();
        lds_thread::msecSleep(lds::sysconf->value("system_setting/drawer_timeout", 1000).toInt());
    }

    if(m & payNormal) {
        toprint(bp_path, bp_count, payNormal);
    }

    if(m & payRefund) {
        toprint(bp_path, bp_count, payRefund);
    }

    if(m & payLabel) {
        public_printby_ch_billno::print_label(ch_billno, label_insert_int_flowid_list);
    }
}

void public_printby_ch_billno::toprint(const QString &path, int manual_bp_count, public_printby_ch_billno::payPrintMode pay_mode)
{
    bp_count = manual_bp_count;
    toMakeFormIns(path, pay_mode);
    print_run();
    //#endif
}

void public_printby_ch_billno::makeIns(int paperwidth)
{
    rowno = 0;
    print_format.paperwidth = paperwidth;
    //init
    print_format.print_ins.clear();
    print_format.drawText("i");
    //prepare
    if(receipt_head_existed("prepare")) {
        print_format.drawText(get_receipt_head("prepare").toByteArray());
    }
    //logo
    if(receipt_head_existed("logo")) {
        print_format.drawText("a00");
        print_format.drawN();
        print_format.drawLogo("", get_receipt_head("logo").toByteArray());
        print_format.drawText("a10");
        print_format.drawText("", "\n");
    }
    //first title
    if(receipt_head_existed("first_title")) {
        print_format.drawN();
        print_format.drawText("a00,s11", get_receipt_head("first_title").toByteArray());
    }
    //second title
    if(receipt_head_existed("second_title")) {
        print_format.drawN();
        print_format.drawText("a00,s11", get_receipt_head("second_title").toByteArray());
    }
    if(receipt_head_existed("reprint")) {
        print_format.drawN();
        print_format.drawText("a00,s11", get_receipt_head("reprint").toByteArray());
    }
    //top desc
    if(receipt_head_existed("top")) {
        print_format.drawN();
        print_format.drawText("a00,s00", get_receipt_head("top").toByteArray());
    }
    //head
    if(model_bp_fmt->tablename_kvmap_count("sale_head") > 0) {
        print_format.drawN();
        print_format.drawText("a10,s00");
        print_format.begin_column_print(model_bp_fmt_value("bp_fmt_begincols", "1").toInt());
        if(sale_head_existed("payno")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_payno_len", "00").toLocal8Bit(), get_sale_head("payno").toString());
        if(sale_head_existed("operid")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_operid_len",  "00").toLocal8Bit(), get_sale_head("operid").toString());
        if(sale_head_existed("table")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_table_len", "00").toLocal8Bit(), get_sale_head("table").toString());
        if(sale_head_existed("div")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_table_len", "00").toLocal8Bit(), get_sale_head("div").toString());

        if(sale_head_existed("person")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_person_len",  "00").toLocal8Bit(), get_sale_head("person").toString());
        if(sale_head_existed("time")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_time_len",  "00").toLocal8Bit(), get_sale_head("time").toString());
        if(sale_head_existed("serial_no")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_serial_no_len",  "00").toLocal8Bit(), get_sale_head("serial_no").toString());

        if(sale_head_existed("waitername")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_waitername_len",  "00").toLocal8Bit(), get_sale_head("waitername").toString());
        if(sale_head_existed("waiter")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_waiter_len",  "00").toLocal8Bit(), get_sale_head("waiter").toString());
        if(sale_head_existed("times")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_times_len",  "00").toLocal8Bit(), get_sale_head("times").toString());
        if(sale_head_existed("memo")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_memo_len",  "00").toLocal8Bit(), get_sale_head("memo").toString());

        if(sale_head_existed("address")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_address_len",  "00").toLocal8Bit(), get_sale_head("address").toString());
        if(sale_head_existed("linkman")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_linkman_len",  "00").toLocal8Bit(), get_sale_head("linkman").toString());
        if(sale_head_existed("tel")) print_format.drawText("s" + model_bp_fmt_value("bp_fmt_tel_len",  "00").toLocal8Bit(), get_sale_head("tel").toString());

        print_format.end_column_print();
    }

    model_bp_fmt->tablename_kvmap_print("sale_detail_title");
    //sale_detail_title
    if(can_print_sale_detail()
            &&model_bp_fmt->tablename_kvmap_count("sale_detail_title") > 0) {
        print_format.drawN();
        print_format.drawText("a10,s00");
        print_format.drawText("-");
        print_format.drawText(bp_content_len);
        //begin - print head
        piecedishprint(0, model_bp_fmt->tablename_kvmap_d("sale_detail_title"), bp_content_len, true);
    }

    //sale_detail
    if(can_print_sale_detail()
            &&model_bp_fmt->tablename_kvmap_count("sale_detail") > 0) {
        print_format.drawN();
        print_format.drawText("a10,s00");
        print_format.drawText("-");
        print_format.drawText("a10");

        QString bp_series_group = model_bp_fmt_value("bp_fmt_series_group");
        QString group_name = "int_flowID";
        if(bp_series_group == "1") {
            //ch_dish_typeno
            group_name = "ch_dish_typeno";
        }
        if(bp_series_group == "2") {
            //ch_sub_typeno
            group_name = "ch_sub_typeno";
        }

        QString ch_dish_typeno;
        QString ch_sub_typeno;

        //        [ch_dish_typeno]	[ch_sub_typeno]	[ch_suitflag]	[int_flowID]	[ch_specialflag]	[ch_presentflag]	[ch_waidai_flag]	[ch_addon_flag]	[num_price_org]	[num_price_add]
        //sale_detail_help_sort
        for(int row = 0; row < model_bp_fmt->tablename_kvmap_count("sale_detail"); row ++) {
            const lds_model_sqltablemodel_d *dish_printd = model_bp_fmt->tablename_kvmap_d("sale_detail");
            QString dish_key = dish_printd->key(row);
            QString dish_comb;
            double dish_total = dish_printd->data(row, "amount").toDouble() + dish_printd->data(row, "num_price_add").toDouble();
            bool ch_dish_group =
                    emptyOrN(dish_printd->data(row, "ch_suitflag").toString()) &&
                    emptyOrN(dish_printd->data(row, "ch_addon_flag").toString());
            ;

            QString group_items =
                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, group_name).toString();
            QString addable_items =
                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "ch_dishno").toString() + "," +
                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "int_discount").toString() + "," +

                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "ch_specialflag").toString() + "," +
                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "ch_presentflag").toString() + "," +
                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "ch_waidai_flag").toString() + "," +
                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "price").toString() + "," +
                    model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "vch_print_memo").toString();


            //ch_suitflag
            if(dish_printd->data(row, "ch_suitflag").toString() == "P") {
                for(row ++; row < model_bp_fmt->tablename_kvmap_count("sale_detail") && dish_printd->data(row, "ch_suitflag").toString() == "Y"; row ++) {
                    dish_comb += dish_printd->key(row) + ",";
                }
                row --;
            }
            //ch_addon_flag
            if(dish_printd->data(row, "ch_addon_flag").toString() == "P") {
                for(row ++; row < model_bp_fmt->tablename_kvmap_count("sale_detail") && dish_printd->data(row, "ch_addon_flag").toString() == "Y"; row ++) {
                    dish_comb += dish_printd->key(row) + ",";
                    dish_total += dish_printd->data(row, "amount").toDouble() + dish_printd->data(row, "num_price_add").toDouble();
                }
                row --;
            }
            //normal
            dish_comb.chop(1);
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "dish_comb", dish_comb);
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "int_flowID", model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "int_flowID"));
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "ch_dish_typeno", model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "ch_dish_typeno"));
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "ch_sub_typeno", model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "ch_sub_typeno"));
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "dish_amount", dish_total);
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "dish_num", model_bp_fmt->tablename_kvmap_field("sale_detail", dish_key, "num"));
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "ch_dish_group", (ch_dish_group ? "Y" : "N") );
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "group_items",  group_items);
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_sort", dish_key, "addable_items",  addable_items);
        }

        //sale_detail_help_sort
        QMap<QString, QString> groupnodishno_comb_map;
        for(int row = 0; row < model_bp_fmt->tablename_kvmap_count("sale_detail_help_sort"); row ++) {
            const lds_model_sqltablemodel_d *dish_printd = model_bp_fmt->tablename_kvmap_d("sale_detail_help_sort");
            QString dish_key = dish_printd->key(row);
            QString group_items = dish_printd->data(row, "group_items").toString();
            groupnodishno_comb_map.insert(group_items, groupnodishno_comb_map.value(group_items) + dish_key + ",");
        }
        for(QMap<QString, QString>::iterator k = groupnodishno_comb_map.begin(); k != groupnodishno_comb_map.end(); k ++) {
            k.value().chop(1);
        }
        for(QMap<QString, QString>::iterator k = groupnodishno_comb_map.begin(); k != groupnodishno_comb_map.end(); k ++) {
            QStringList dish_key_list = k.value().split(",");
            foreach(const QString &dish_key, dish_key_list) {
                QString group_items = model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "group_items").toString();
                QString addable_items = model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "addable_items").toString();
                QString ch_dish_group = model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "ch_dish_group").toString();
                QString dish_key_select;
                bool ok;

                if(false == lds::sysconf->value("system_setting/samedishNotadd_qtprint", false).toBool() && ch_dish_group == "Y") {
                    dish_key_select = model_bp_fmt->tablename_kvmap_indexof("sale_detail_help_group", qrtVariantPairList() << qrtVariantPair("ch_dish_group", "Y") << qrtVariantPair("addable_items", addable_items), &ok);
                    if(ok) {
                        model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_group", dish_key_select, "dish_amount", model_bp_fmt->tablename_kvmap_field("sale_detail_help_group", dish_key_select, "dish_amount").toDouble()
                                                                   + model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "dish_amount").toDouble());
                        model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_group", dish_key_select, "dish_num", model_bp_fmt->tablename_kvmap_field("sale_detail_help_group", dish_key_select, "dish_num").toDouble()
                                                                   + model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "dish_num").toDouble());
                        continue;
                    }
                }

                model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_group", dish_key, "dish_amount", model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "dish_amount"));
                model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_group", dish_key, "dish_num", model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "dish_num"));
                model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_group", dish_key, "ch_dish_group", ch_dish_group);
                model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_group", dish_key, "addable_items", addable_items);
            }
        }
        //sale_detail_help_result
        for(int row = 0; row < model_bp_fmt->tablename_kvmap_count("sale_detail_help_group"); row ++) {
            const lds_model_sqltablemodel_d *dish_printd = model_bp_fmt->tablename_kvmap_d("sale_detail_help_group");
            const lds_model_sqltablemodel_d *sale_detail_printd = model_bp_fmt->tablename_kvmap_d("sale_detail");
            QString dish_key = dish_printd->key(row);

            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_result", dish_key, sale_detail_printd->rowRecord(dish_key));
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_result", dish_key, "num", dish_printd->data(row, "dish_num") );
            model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_result", dish_key, "amount", dish_printd->data(row, "dish_amount") );
            if(dish_printd->data(row, "ch_dish_group").toString() == "N") {
                QStringList dish_comb_key_list = model_bp_fmt->tablename_kvmap_field("sale_detail_help_sort", dish_key, "dish_comb").toString().split(",");
                foreach(const QString &dish_comb_key, dish_comb_key_list) {
                    model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_result", dish_comb_key, sale_detail_printd->rowRecord(dish_comb_key));
                    model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_result", dish_comb_key, "ch_dish_typeno", sale_detail_printd->data(dish_key, "ch_dish_typeno") );
                    model_bp_fmt->tablename_kvmap_insert_field("sale_detail_help_result", dish_comb_key, "ch_sub_typeno", sale_detail_printd->data(dish_key, "ch_sub_typeno") );
                }
            }
            model_bp_fmt->tablename_kvmap_print("sale_detail_help_result");
        }
        model_bp_fmt->tablename_kvmap_print("sale_detail");
        model_bp_fmt->tablename_kvmap_print("sale_detail_help_sort");
        model_bp_fmt->tablename_kvmap_print("sale_detail_help_group");
        model_bp_fmt->tablename_kvmap_print("sale_detail_help_result");


        for(int row = 0; row < model_bp_fmt->tablename_kvmap_count("sale_detail_help_result"); row ++) {
            const lds_model_sqltablemodel_d *dish_printd = model_bp_fmt->tablename_kvmap_d("sale_detail_help_result");
            //1
            if(bp_series_group == "1") {
                if(ch_dish_typeno != dish_printd->data(row, "ch_dish_typeno").toString()) {
                    ch_dish_typeno = dish_printd->data(row, "ch_dish_typeno").toString();
                    print_format.drawText(bp_content_len, litterblank+get_dish_typename(ch_dish_typeno)+":"+get_dish_typeno_sum(dish_printd, ch_dish_typeno) + litterblank, print_format.paperwidth, Qt::AlignCenter);
                    print_format.drawN();
                }
            }
            if(bp_series_group == "2") {
                if(ch_sub_typeno != dish_printd->data(row, "ch_sub_typeno").toString()) {
                    ch_sub_typeno = dish_printd->data(row, "ch_sub_typeno").toString();
                    print_format.drawText(bp_content_len, litterblank+get_sub_typename(ch_sub_typeno)+":"+get_sub_typeno_sum(dish_printd, ch_sub_typeno) + litterblank,  print_format.paperwidth, Qt::AlignCenter);
                    print_format.drawN();
                }
            }
            piecedishprint(row, dish_printd, bp_content_len, false);
        }
    }

    if(model_bp_fmt->tablename_kvmap_count("pay_detail") > 0) {
        print_format.drawN();
        print_format.drawText("a10,s00");
        print_format.drawText("-");
        //pay_detail
        QByteArray bp_tail_len;
        print_format.begin_column_print(model_bp_fmt_value("bp_fmt_offcols", "1").toInt());
        if(pay_detail_existed("num_cost")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_cost_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("消费"))+":",  format_amount( get_pay_detail("num_cost").toFloat(), "num_cost"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_present")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_present_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("赠送"))+":",  format_amount(get_pay_detail("num_present").toFloat(), "num_present"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_discount")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_discount_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("折扣"))+":",  format_amount( get_pay_detail("num_discount").toFloat(), "num_discount"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_service")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_service_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("服务费")) + pay_fixed_service_suffix()+":",  format_amount(get_pay_detail("num_service").toFloat(), "num_service"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_rate")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_tax_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("税率"))+pay_fixed_tax_suffix() + ":",  format_amount(get_pay_detail("num_rate").toFloat(), "num_rate"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_lowcost")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_lowcost_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("低消"))+":",  format_amount(get_pay_detail("num_lowcost").toFloat(), "num_lowcost"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_free")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_free_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("优惠"))+":",  format_amount(get_pay_detail("num_free").toFloat(), "num_free"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_blotout")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_blotout_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("抹零"))+":",  format_amount(get_pay_detail("num_blotout").toFloat(), "num_blotout"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        print_format.drawN();
        //付款方式详细
        if(pay_detail_existed("num_realamount")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_receive_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("应收"))+":",  format_amount(get_pay_detail("num_realamount").toFloat(), "num_realamount"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_amount")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_cash_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("实收"))+":",  format_amount(get_pay_detail("num_amount").toFloat(), "num_amount"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_change")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_change_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("找零"))+":",  format_amount(get_pay_detail("num_change").toFloat(), "num_change"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        if(pay_detail_existed("num_avg")) {
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_avg_len").toLocal8Bit();
            print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("人均"))+":",  format_amount(get_pay_detail("num_avg").toFloat(),  "num_avg"), print_format.column_width(), scale_h_by(bp_tail_len)));
        }
        print_format.drawN();
        //付款方式情况
        if(model_bp_fmt->tablename_kvmap_count("pay_detail_paymode") > 0) {
            print_format.drawText("", print_format.printTr(QT_TR_NOOP("付款方式")));
            print_format.drawN();
            bp_tail_len  = "s" + model_bp_fmt_value("bp_fmt_pay_detail_len").toLocal8Bit();
            for(int row = 0; row < model_bp_fmt->tablename_kvmap_count("pay_detail_paymode"); row ++) {
                const lds_model_sqltablemodel_d *d = model_bp_fmt->tablename_kvmap_d("pay_detail_paymode");
                print_format.drawText(bp_tail_len, key_value_com(d->data(row, "vch_paymodename").toString()+":",  format_amount(d->data(row, "real_payamount").toFloat(), "real_payamount"), print_format.column_width(), scale_h_by(bp_tail_len)));
            }
        }
        //会员情况
        print_format.drawN();
        model_bp_fmt->tablename_kvmap_print("pay_detail_member");
        if(model_bp_fmt->tablename_kvmap_count("pay_detail_member") > 0) {
            print_format.drawText("", print_format.printTr(QT_TR_NOOP("会员详情")));
            print_format.drawN();

            const lds_model_sqltablemodel_d *d = model_bp_fmt->tablename_kvmap_d("pay_detail_member");
            bp_tail_len = "s" + model_bp_fmt_value("bp_fmt_m_no_len").toLocal8Bit();
            if(d->isExistedColumn("vch_voucherno")) print_format.drawText(bp_tail_len, key_value_com((d->data(0, "cash_pay_flag").toString() == "Y" ?print_format.printTr(QT_TR_NOOP("云会员号")):print_format.printTr(QT_TR_NOOP("会员号"))) + ":",  d->data(0, "vch_voucherno").toString(), print_format.column_width(), scale_h_by(bp_tail_len)));
            bp_tail_len = "s" + model_bp_fmt_value("bp_fmt_m_name_len").toLocal8Bit();
            if(d->isExistedColumn("vch_card_name")) print_format.drawText(bp_tail_len, key_value_com((d->data(0, "cash_pay_flag").toString() == "Y" ?print_format.printTr(QT_TR_NOOP("云会员")):print_format.printTr(QT_TR_NOOP("会员"))) + ":",  d->data(0, "vch_card_name").toString(), print_format.column_width(), scale_h_by(bp_tail_len)));
            bp_tail_len = "s" + model_bp_fmt_value("bp_fmt_m_amount_len").toLocal8Bit();
            if(d->isExistedColumn("num_card_remain")) print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("余额"))+":",  format_amount(d->data(0, "num_card_remain").toFloat(), "num_card_remain"), print_format.column_width(), scale_h_by(bp_tail_len)));
            bp_tail_len = "s" + model_bp_fmt_value("bp_fmt_m_point_len").toLocal8Bit();
            if(d->isExistedColumn("num_card_point")) print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("积分"))+":",  format_amount(d->data(0, "num_card_point").toFloat(), "num_card_point"), print_format.column_width(), scale_h_by(bp_tail_len)));
            bp_tail_len = "s" + model_bp_fmt_value("bp_fmt_m_adress_len").toLocal8Bit();
            if(d->isExistedColumn("vch_card_adress")) print_format.drawText(bp_tail_len, key_value_com(print_format.printTr(QT_TR_NOOP("地址"))+":",  d->data(0, "vch_card_adress").toString(), print_format.column_width(), scale_h_by(bp_tail_len)));

        }
        print_format.end_column_print();
    }

    //tip
    if(model_bp_fmt->tablename_kvmap_count("preprint_tip_detail") > 0) {
        print_format.drawN();
        print_format.drawText("a10,s00");
        print_format.drawText("-");
        const lds_model_sqltablemodel_d *d = model_bp_fmt->tablename_kvmap_d("preprint_tip_detail");
        QByteArray bp_tip_len = "s00";

        print_format.drawText("", d->data("tip_desc", 0).toString());
        print_format.drawN();
        print_format.drawText(bp_tip_len, key_value_com(cey_sys_parameterValue(model_bp_fmt, "tip_int_rate_1").toString()+"% Tip = ",  format_amount( d->data("tip_num_value_1", 0).toFloat(), "num_cost"), print_format.column_width(), scale_h_by(bp_tip_len)));
        print_format.drawN();
        print_format.drawText(bp_tip_len, key_value_com(cey_sys_parameterValue(model_bp_fmt, "tip_int_rate_2").toString()+"% Tip = ",  format_amount( d->data("tip_num_value_2", 0).toFloat(), "num_cost"), print_format.column_width(), scale_h_by(bp_tip_len)));
        print_format.drawN();
        print_format.drawText(bp_tip_len, key_value_com(cey_sys_parameterValue(model_bp_fmt, "tip_int_rate_3").toString()+"% Tip = ",  format_amount( d->data("tip_num_value_3", 0).toFloat(), "num_cost"), print_format.column_width(), scale_h_by(bp_tip_len)));
        print_format.drawN();
        print_format.drawText("", "Total:");
    }


    //receipt_tail
    if(model_bp_fmt->tablename_kvmap_count("receipt_tail") > 0) {
        print_format.drawN();
        print_format.drawText("a10,s00");
        print_format.drawText("-");
    }

    //其他
    if(receipt_tail_existed("other")) {
        print_format.drawN();
        print_format.drawText("a10,s00", get_receipt_tail("other").toByteArray());
    }
    //签名
    if(receipt_tail_existed("signature")) {
        print_format.drawN();
        print_format.drawText("a10,s00", get_receipt_tail("signature").toByteArray());
    }
    //票尾语
    if(receipt_tail_existed("bottom")) {
        print_format.drawN();
        print_format.drawText("a00,s00", get_receipt_tail("bottom").toByteArray());
    }
    //一维码
    if(receipt_tail_existed("bar_code")) {
        print_format.drawN();
        print_format.drawoneBarcode("a00,s00", get_receipt_tail("bar_code").toByteArray());
    }
    //二维码
    if(receipt_tail_existed("qr_code")) {
        print_format.drawN();
        print_format.drawQR_Code("a00,s00", get_receipt_tail("qr_code").toByteArray());
    }
}


void public_printby_ch_billno::toprint_preprint(const QString &path, const QString &title, const QString &flowid_sql_filter)
{
    const QString country =  n_func::f_get_sysparm("locale_country_name");
    const QString preprint_text =  n_func::f_get_sysparm(LOCALE_RECEIPT_REPRINT_SQL_PATH(country));
    if(preprint_text.count() > 0) {
        QString and_sql_filter;
        if(!flowid_sql_filter.isEmpty()) {
            and_sql_filter = " and " + flowid_sql_filter;
        }
        lds_model_sqltablemodel model;
        translatorObj trObj(&model);
        trObj.setPrintTranslate(print_format.getLanguageKey(), this);

        public_printby_ch_billno::txt_print_bill_create_table(&model);
        public_printby_ch_billno::txt_print_fill_sale_head(&model, ch_billno);
        public_printby_ch_billno::txt_print_fill_sale_detail(&model, ch_billno, and_sql_filter);
        public_printby_ch_billno::txt_print_fill_pay_detail_preprint(&model, ch_billno, flowid_sql_filter);
        public_printby_ch_billno::txt_print_fill_company_info(&model, ch_billno);

        model.tablename_kvmap_print();
        txt_ins_bill_print d(model.tablename_kvmap_d("bill_ins_table"), preprint_text);
        print_format.print_ins = d.bill_before_reslove();
    } else {
        rowno = 0;
        //printpath
        print_format_init_path(path);

        //time_diff
        QString ch_tableno = model_bp_fmt->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_tableno").toString();
        model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "div", lds_scr_table::cey_bt_table_div_sqr(ch_tableno, ch_billno));
        model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "dt_operdate", n_func::f_get_sysdatetime());

        //receipt_head
        if(model_bp_fmt_value("bp_fmt_print_prepare") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "prepare", model_bp_fmt_value("bp_fmt_print_prepare_ins"));
        if(model_bp_fmt_value("bp_fmt_title_logo_en") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "logo", model_bp_fmt_value("bp_fmt_title_logo"));
        /*必定打印表头 if(model_bp_fmt_value("bp_fmt_title_enable") == "1")*/ model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "first_title", title);
        if(model_bp_fmt_value("bp_fmt_title_sc_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "second_title", model_bp_fmt_value("bp_fmt_title_second"));
        if(model_bp_fmt_value("bp_fmt_top_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "top", model_bp_fmt_value("bp_fmt_top_desc"));

        //sale_head
        if(model_bp_fmt_value("bp_fmt_operid") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "operid", print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());
        if(model_bp_fmt_value("bp_fmt_table") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "table", print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());
        if(model_bp_fmt_value("bp_fmt_table") == "1" && 0 != get_receipt_head("div").toInt()) model_bp_fmt->tablename_kvmap_insert_value("sale_head", "div", print_format.printTr(QT_TR_NOOP("分单号"))+":"+get_receipt_head("div").toString());
        if(model_bp_fmt_value("bp_fmt_person") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "person", print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());
        if(model_bp_fmt_value("bp_fmt_waiter") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waiter", print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
        if(model_bp_fmt_value("bp_fmt_waitername") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waitername", print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));
        if(model_bp_fmt_value("bp_fmt_time") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "time",  print_format.printTr(QT_TR_NOOP("时间"))+":"+ get_receipt_head("dt_operdate").toDateTime().toString(yyyyMMddhhmmss));// cey_u_checkout_master_dt);

        //sale_detail
        model_bp_fmt_fill_orderdish(false, flowid_sql_filter);

        //pay_detail
        if(ch_billno.startsWith("R")) {//二维码点餐
            model_bp_fmt_fill_pay_detail_preprint_qr_code();
        } else {
            model_bp_fmt_fill_pay_detail_preprint(flowid_sql_filter);
        }
        //receipt_tail
        model_bp_fmt_fill_receipt_tail();
        //make ins
        model_bp_fmt->tablename_kvmap_print();
        makeIns(print_format.paperwidth);
    }
    //print
    print_run();
}

void public_printby_ch_billno::toprint_dish_return(const QString &path, const QStringList &int_quit_flow_list)
{
    rowno = 0;
    //printpath
    print_format_init_path(path);

    //time_diff
    model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "dt_operdate", n_func::f_get_sysdatetime());

    //receipt_head
    if(model_bp_fmt_value("bp_fmt_print_prepare") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "prepare", model_bp_fmt_value("bp_fmt_print_prepare_ins"));
    /*必定打印表头 if(model_bp_fmt_value("bp_fmt_title_enable") == "1")*/ model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "first_title", print_format.printTr(QT_TR_NOOP("退单")));
    if(model_bp_fmt_value("bp_fmt_title_sc_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "second_title", model_bp_fmt_value("bp_fmt_title_second"));

    //sale_head
    if(model_bp_fmt_value("bp_fmt_operid") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "operid", print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());
    if(model_bp_fmt_value("bp_fmt_table") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "table", print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());
    if(model_bp_fmt_value("bp_fmt_person") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "person", print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());
    if(model_bp_fmt_value("bp_fmt_waiter") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waiter", print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
    if(model_bp_fmt_value("bp_fmt_waitername") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waitername", print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));
    if(model_bp_fmt_value("bp_fmt_time") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "time",  print_format.printTr(QT_TR_NOOP("时间"))+":"+ get_receipt_head("dt_operdate").toDateTime().toString(yyyyMMddhhmmss));// cey_u_checkout_master_dt);

    //sale_detail
    model_bp_fmt_fill_orderdish_return(int_quit_flow_list);
    //receipt_tail
    model_bp_fmt_fill_receipt_tail();
    //make ins
    makeIns(print_format.paperwidth);
    //print
    print_run();
}

void public_printby_ch_billno::toprint_dish_return(const QString &ch_billno, const QString &path, const QStringList &int_quit_flow_list)
{
    public_printby_ch_billno printer(ch_billno);
    printer.toprint_dish_return(path, int_quit_flow_list);
}

void public_printby_ch_billno::toMakeFormIns(const QString &path, public_printby_ch_billno::payPrintMode pay_mode)
{
    const QString country =  n_func::f_get_sysparm("locale_country_name");
    const QString pay_text =  n_func::f_get_sysparm(LOCALE_RECEIPT_PAY_SQL_PATH(country));
    if(pay_text.count() > 0) {
        lds_model_sqltablemodel model;
        translatorObj trObj(&model);
        trObj.setPrintTranslate(print_format.getLanguageKey(), this);

        public_printby_ch_billno::txt_print_bill_create_table(&model);
        public_printby_ch_billno::txt_print_fill_sale_head(&model, ch_billno);
        public_printby_ch_billno::txt_print_fill_sale_detail(&model, ch_billno);
        public_printby_ch_billno::txt_print_fill_pay_detail(&model, ch_billno);
        public_printby_ch_billno::txt_print_fill_company_info(&model, ch_billno);

        model.tablename_kvmap_print();

        txt_ins_bill_print d(model.tablename_kvmap_d("bill_ins_table"), pay_text);
        print_format.print_ins = d.billToIns();
    } else {
        lds_query_hddpos  query;
        rowno = 0;
        //printpath
        print_format_init_path(path);

        //serial_no_cur
        QString serial_no_cur = model_bp_fmt->tablename_kvmap_value("cey_u_master", ch_billno, "ch_billno", "vch_memo").toString();
        //time_diff
        query.execSelect(QString("select timediff(b.dt_operdate, a.dt_operdate) as time_diff, b.dt_operdate  from cey_u_table  a, cey_u_checkout_master b where a.ch_billno = b.ch_billno and b.ch_state = 'Y' and b.ch_billno = '%1' ")
                         .arg(ch_billno));
        query.next();
        model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "time_diff", query.recordValue("time_diff"));
        model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "dt_operdate", query.recordValue("dt_operdate"));
        model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "serial_no_cur", serial_no_cur);

        //receipt_head
        QString first_title;
        if(ch_billno.startsWith("R")) {
            first_title = print_format.printTr(QT_TR_NOOP("二维码付款"));
        } else {
            first_title = print_format.printTr(QT_TR_NOOP("付款单"));
        }


        if(model_bp_fmt_value("bp_fmt_print_prepare") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "prepare", model_bp_fmt_value("bp_fmt_print_prepare_ins"));
        if(model_bp_fmt_value("bp_fmt_title_logo_en") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "logo", model_bp_fmt_value("bp_fmt_title_logo"));
        if(model_bp_fmt_value("bp_fmt_title_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "first_title", first_title);
        if(model_bp_fmt_value("bp_fmt_title_sc_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "second_title", model_bp_fmt_value("bp_fmt_title_second"));
        if(payReprint & pay_mode)           model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "reprint", print_format.printTr(QT_TR_NOOP("补打")));
        if(model_bp_fmt_value("bp_fmt_top_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "top", model_bp_fmt_value("bp_fmt_top_desc"));

        //sale_head
        if(model_bp_fmt_value("bp_fmt_payno") == "1")  model_bp_fmt->tablename_kvmap_insert_value("sale_head", "payno", print_format.printTr(QT_TR_NOOP("单号"))+":"+get_cey_u_table("ch_payno").toString());
        if(model_bp_fmt_value("bp_fmt_operid") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "operid", print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());
        if(model_bp_fmt_value("bp_fmt_table") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "table", print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());
        if(model_bp_fmt_value("bp_fmt_person") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "person", print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());
        if(model_bp_fmt_value("bp_fmt_waiter") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waiter", print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
        if(model_bp_fmt_value("bp_fmt_waitername") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waitername", print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));
        if(model_bp_fmt_value("bp_fmt_serial_no") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "serial_no",  bp_fmt_serial_no_desc+":"+get_receipt_head("serial_no_cur").toString());
        if(model_bp_fmt_value("bp_fmt_time") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "time",  print_format.printTr(QT_TR_NOOP("时间"))+":"+ get_receipt_head("dt_operdate").toDateTime().toString(yyyyMMddhhmmss));// cey_u_checkout_master_dt);
        if(model_bp_fmt_value("bp_fmt_times") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "times",  print_format.printTr(QT_TR_NOOP("时长"))+":"+get_receipt_head("time_diff").toString());//time_diff);
        if(model_bp_fmt_value("bp_fmt_memo") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "memo",  print_format.printTr(QT_TR_NOOP("开台备注"))+":"+get_cey_u_table("vch_memo").toString());

        //sale_detail
        model_bp_fmt_fill_orderdish(payRefund & pay_mode);
        //pay_detail
        model_bp_fmt_fill_pay_detail();
        //receipt_tail
        model_bp_fmt_fill_receipt_tail();
        //make ins
        makeIns(print_format.paperwidth);
        //print
    }
}

QImage public_printby_ch_billno::toMakeFormImg()
{
    QImage img;
    const QString country =  n_func::f_get_sysparm("locale_country_name");
    const QString pay_text =  n_func::f_get_sysparm(LOCALE_RECEIPT_PAY_SQL_PATH(country));
    if(pay_text.count() > 0) {
        img = txt_ins_bill_print::billToImage(print_format.print_ins);
    } else {
        public_printby_ch_billno_over_view_data d;
        d.setPaperWidth(print_format.paperwidth);
        d.ins = print_format.print_ins;
        //所有正常的字体都倍高
        for(int k = 0; k < d.ins.count(); k ++) {
            d.ins[k].first.replace("s00", "s01");
            d.ins[k].first.replace("s01", "s01");
            d.ins[k].first.replace("s11", "s11");
            d.ins[k].first.replace("s11", "s11");
        }

        QString logo_path;
        if(receipt_head_existed("logo")) {
            logo_path = get_receipt_head("logo").toByteArray();
        }

        img = d.getImage(logo_path);// /home/cz/图片/Lebanon.jpg
        //#ifdef QT_DEBUG
        img.save("print.png");
        //#endif
    }
    return img;
}

QImage public_printby_ch_billno::getPayPrintImg(const QString &bp_path, payPrintModes m)
{
    if(m & payNormal) {
        toMakeFormIns(bp_path, payNormal);
    }

    if(m & payRefund) {
        toMakeFormIns(bp_path, payRefund);
    }

    return toMakeFormImg();
}

void public_printby_ch_billno::toprintwaimai(const QString &path)
{
    rowno = 0;
    print_format_init_path(path);

    //time_diff
    model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "dt_operdate", n_func::f_get_sysdatetime());

    //receipt_head
    if(model_bp_fmt_value("bp_fmt_print_prepare") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "prepare", model_bp_fmt_value("bp_fmt_print_prepare_ins"));
    /*必定打印表头 if(model_bp_fmt_value("bp_fmt_title_enable") == "1")*/ model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "first_title", print_format.printTr(QT_TR_NOOP("外卖单")));

    //sale_head
    if(model_bp_fmt_value("bp_fmt_operid") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "operid", print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());
    if(model_bp_fmt_value("bp_fmt_table") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "table", print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());
    if(model_bp_fmt_value("bp_fmt_person") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "person", print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());
    if(model_bp_fmt_value("bp_fmt_time") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "time",  print_format.printTr(QT_TR_NOOP("时间"))+":"+ get_receipt_head("dt_operdate").toDateTime().toString(yyyyMMddhhmmss));// cey_u_checkout_master_dt);
    if(model_bp_fmt_value("bp_fmt_address") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "address", print_format.printTr(QT_TR_NOOP("外卖地址"))+":"+ get_cey_u_togo("vch_address").toString());
    if(model_bp_fmt_value("bp_fmt_linkman") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "linkman", print_format.printTr(QT_TR_NOOP("外卖人"))+":"+get_cey_u_togo("vch_linkman").toString());
    if(model_bp_fmt_value("bp_fmt_tel") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "tel", print_format.printTr(QT_TR_NOOP("外卖电话"))+":"+get_cey_u_togo("vch_tel").toString());

    model_bp_fmt->tablename_kvmap_print("sale_head");
    //sale_detail
    model_bp_fmt_fill_orderdish();

    //pay_detail
    if(model_bp_fmt_value("bp_fmt_cost") == "1") {
        if(model_bp_fmt->tablename_kvmap_count("sale_detail")) {
            const lds_model_sqltablemodel_d *dish_printd = model_bp_fmt->tablename_kvmap_d("sale_detail");
            model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_cost", get_sum(dish_printd));// query.recordValue("num_cost"));
        }
    }
    //receipt_tail
    model_bp_fmt_fill_receipt_tail();
    //make ins
    makeIns(print_format.paperwidth);
    //print
    print_run();
}

void public_printby_ch_billno::toprintwaimai_pay(const QString &path, int manual_bp_count)
{
    rowno = 0;
    bp_count = manual_bp_count;
    //printpath
    print_format_init_path(path);

    //serial_no_cur
    QString serial_no_cur = model_bp_fmt->tablename_kvmap_value("cey_u_master", ch_billno, "ch_billno", "vch_memo").toString();
    model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "serial_no_cur", serial_no_cur);
    model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "dt_operdate", model_bp_fmt->tablename_kvmap_value("cey_u_checkout_master", ch_billno, "ch_billno", "dt_operdate", " ch_state = 'Y'  "));

    //receipt_head
    if(model_bp_fmt_value("bp_fmt_print_prepare") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "prepare", model_bp_fmt_value("bp_fmt_print_prepare_ins"));
    if(model_bp_fmt_value("bp_fmt_title_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "first_title", print_format.printTr(QT_TR_NOOP("付款单")));
    if(model_bp_fmt_value("bp_fmt_title_sc_enable") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "second_title", model_bp_fmt_value("bp_fmt_title_second"));

    //sale_head
    if(model_bp_fmt_value("bp_fmt_payno") == "1")  model_bp_fmt->tablename_kvmap_insert_value("sale_head", "payno", print_format.printTr(QT_TR_NOOP("单号"))+":"+get_cey_u_table("ch_payno").toString());
    if(model_bp_fmt_value("bp_fmt_operid") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "operid", print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());
    if(model_bp_fmt_value("bp_fmt_table") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "table", print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());
    if(model_bp_fmt_value("bp_fmt_person") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "person", print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());
    if(model_bp_fmt_value("bp_fmt_waiter") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waiter", print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
    if(model_bp_fmt_value("bp_fmt_waitername") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waitername", print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));
    if(model_bp_fmt_value("bp_fmt_serial_no") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "serial_no",  bp_fmt_serial_no_desc+":"+get_receipt_head("serial_no_cur").toString());

    if(model_bp_fmt_value("bp_fmt_time") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "time",  print_format.printTr(QT_TR_NOOP("时间"))+":"+ get_receipt_head("dt_operdate").toDateTime().toString(yyyyMMddhhmmss));// cey_u_checkout_master_dt);
    if(model_bp_fmt_value("bp_fmt_address") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "address", print_format.printTr(QT_TR_NOOP("外卖地址"))+":"+ get_cey_u_togo("vch_address").toString());
    if(model_bp_fmt_value("bp_fmt_linkman") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "linkman", print_format.printTr(QT_TR_NOOP("外卖人"))+":"+get_cey_u_togo("vch_linkman").toString());
    if(model_bp_fmt_value("bp_fmt_tel") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "tel", print_format.printTr(QT_TR_NOOP("外卖电话"))+":"+get_cey_u_togo("vch_tel").toString());

    //sale_detail
    model_bp_fmt_fill_orderdish();
    //pay_detail
    model_bp_fmt_fill_pay_detail();
    //receipt_tail
    model_bp_fmt_fill_receipt_tail();
    //make ins
    makeIns(print_format.paperwidth);
    //print
    print_run();
}

blockIns public_printby_ch_billno::toprint_preprint_div(const QString &path, double &ret_total, int sqr)
{
    rowno = 0;
    //printpath
    print_format_init_path(path);
    //
    if(sqr != -1) {
        model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "top", print_format.printTr(QT_TR_NOOP("分单单号"))+":"+QString::number(sqr));
    }
    //sale_detail
    model_bp_fmt_fill_orderdish();
    //pay_detail
    double subtotal  = 0;   ///消费情况
    if(ch_billno.startsWith("R")) {//二维码点餐
        subtotal = model_bp_fmt_fill_pay_detail_preprint_qr_code();
    } else {
        subtotal = model_bp_fmt_fill_pay_detail_preprint();
    }
    //makeIns
    makeIns(print_format.paperwidth);
    //print
    ret_total = subtotal;
    return print_format.print_ins;
    //    print_format.synchPrint();
    //    return subtotal;
}

blockIns public_printby_ch_billno::toprint_preprint_div_title(const QString &path, const QString &title)
{
    rowno = 0;
    //printpath
    print_format_init_path(path);
    //time_diff
    model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "dt_operdate", n_func::f_get_sysdatetime());

    //receipt_head
    if(model_bp_fmt_value("bp_fmt_print_prepare") == "1") model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "prepare", model_bp_fmt_value("bp_fmt_print_prepare_ins"));
    /*必定打印表头 if(model_bp_fmt_value("bp_fmt_title_enable") == "1")*/ model_bp_fmt->tablename_kvmap_insert_value("receipt_head", "first_title", title);

    //sale_head
    if(model_bp_fmt_value("bp_fmt_operid") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "operid", print_format.printTr(QT_TR_NOOP("工号"))+":"+get_bp_fmt_operid());
    if(model_bp_fmt_value("bp_fmt_table") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "table", print_format.printTr(QT_TR_NOOP("餐桌"))+":"+get_bp_fmt_table());
    if(model_bp_fmt_value("bp_fmt_person") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "person", print_format.printTr(QT_TR_NOOP("人数"))+":"+get_cey_u_table("int_person").toString());
    if(model_bp_fmt_value("bp_fmt_waiter") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waiter", print_format.printTr(QT_TR_NOOP("服务员"))+":"+get_cey_u_table("vch_waiter").toString());
    if(model_bp_fmt_value("bp_fmt_waitername") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "waitername", print_format.printTr(QT_TR_NOOP("服务员名称"))+":"+name_of_operid(get_cey_u_table("vch_waiter").toString()));
    if(model_bp_fmt_value("bp_fmt_time") == "1") model_bp_fmt->tablename_kvmap_insert_value("sale_head", "time",  print_format.printTr(QT_TR_NOOP("时间"))+":"+ get_receipt_head("dt_operdate").toDateTime().toString(yyyyMMddhhmmss));// cey_u_checkout_master_dt);

    model_bp_fmt->tablename_kvmap_print();
    //make ins
    makeIns(print_format.paperwidth);

    return print_format.print_ins;
    //    print_format.synchPrint();
}

blockIns public_printby_ch_billno::toprint_preprint_div_tail(const QString &path, double subtotal, int idvcount)
{
    rowno = 0;
    print_format_init_path(path);
    if(model_bp_fmt_value("bp_fmt_cost") == "1" && idvcount > 1) {
        model_bp_fmt->tablename_kvmap_insert_value("receipt_tail", "other",
                                                   print_format.printTr(QT_TR_NOOP("单数")) + ":" + QString::number(idvcount) + "  "+
                                                   print_format.printTr(QT_TR_NOOP("总计")) + ":" + format_amount(subtotal, "subtotal"));
    }
    //receipt_tail
    model_bp_fmt_fill_receipt_tail();
    //make ins
    makeIns(print_format.paperwidth);

    //表尾走纸
    int count = model_bp_fmt_value("bp_fmt_tail_block").toInt();
    print_format.drawText("", QString(count, '\n'));

    //打印最后
    print_format.drawTextEnd("1B6405 1D5601");//走纸5x， 切纸
    //print
    return print_format.print_ins;
    //    print_run();
}

void public_printby_ch_billno::clear_ins()
{
    print_format.print_ins.clear();
}

void public_printby_ch_billno::clear_model_bp_fmt()
{
    model_bp_fmt->tablename_kvmap_delete("receipt_head");
    model_bp_fmt->tablename_kvmap_delete("sale_head");
    model_bp_fmt->tablename_kvmap_delete("pay_detail");
    model_bp_fmt->tablename_kvmap_delete("receipt_tail");
}

QString public_printby_ch_billno::defpath()
{
    return lds::sysconf->value("system_setting/printerpath").toString();
}

void public_printby_ch_billno::print_label(const QString &ch_billno)
{
    print_label(ch_billno, QString());
}

//void public_printby_ch_billno::print_label(const QString &ch_billno, int print_int_flowid_max)
//{
//    QString and_where_sql;
//    if(print_int_flowid_max != -1) {
//        and_where_sql = QString(" and a.int_flowid > %1 ").arg(print_int_flowid_max);
//    }
//    print_label(ch_billno, and_where_sql);
//}

void public_printby_ch_billno::print_label(const QString &ch_billno, const QString &and_where_sql)
{
    lds_model_sqltablemodel m_cache;
    lds_query_hddpos  query;
    query.execSelect("select count(0) from cey_bt_dish_subtype where ch_labelprint_flag = 'Y' ");
    query.next();
    if(0 == query.record().value(0).toInt()) {
        return;
    }
    //手动输入手牌号
    QString serial_no_cur = m_cache.tablename_kvmap_value("cey_u_master", ch_billno, "ch_billno", "vch_memo").toString();

    QString vch_company = m_cache.tablename_kvmap_value("cey_sys_company", "1", "1", "vch_company").toString();

    QStandardItemModel printmodel;
    printmodel.setHorizontalHeaderLabels(QStringList()
                                         << QObject::tr("打印次数")<< QObject::tr("单号")<< QObject::tr("价格")
                                         << QObject::tr("品名") <<  QObject::tr("品码") <<QObject::tr("时间") << QObject::tr("做法") << QObject::tr("店名")
                                         << QObject::tr("数量")
                                         );
    /// 金额：赠送0， 非赠送((点菜-退货)*价格)*0.01*折扣+加价
    query.execSelect(QString(
                         "select  if(a.ch_presentflag = 'Y', 0,   (a.num_num - a.num_back)*a.num_price*0.01*a.int_discount)  + a.num_price_add  as num_cost,  b.vch_dishname, a.ch_dishno, a.dt_operdate, vch_print_memo, "
                         " b.ch_weight, num_num-num_back as dish_num, a.ch_addon_flag, a.int_flowid, a.num_price  from cey_u_orderdish a, cey_bt_dish b, cey_bt_dish_subtype c "
                         " where a.ch_dishno = b.ch_dishno and b.ch_sub_typeno = c.ch_sub_typeno and c.ch_labelprint_flag = 'Y' and ch_billno = '%1' and (num_num-num_back) > 0 "
                         )
                     .arg(ch_billno)
                     + and_where_sql);
    while(query.next()) {
        m_cache.tablename_kvmap_insert_field("label_print_sale_detail", query.recordValue("int_flowid").toString(), query.record());
    }
    m_cache.tablename_kvmap_print("label_print_sale_detail");

    const lds_model_sqltablemodel_d *d = m_cache.tablename_kvmap_d("label_print_sale_detail");
    for(int row = 0, off; d != 0 && row < d->rowCount(); row += off + 1) {
        double dish_num = qMax(d->data(row, "dish_num").toDouble(), 1.0);
        double num_cost_per = 0;
        vch_print_memoData ch_addon_flag_d;
        for(int k = row + 1; k < d->rowCount() && d->data(k, "ch_addon_flag").toString() == "Y"; k ++) {
            ch_addon_flag_d.append("", d->data(k, "vch_dishname").toString(), d->data(row, "dish_num").toDouble(), d->data(k, "num_cost").toDouble());
        }
        off = ch_addon_flag_d.count();

        if(d->data(row, "ch_weight").toString() == "Y") {//称重商品
            dish_num = d->data(row, "dish_num").toDouble();
            num_cost_per = d->data(row, "num_cost").toDouble();
            num_cost_per += ch_addon_flag_d.total();
            vch_print_memoData vch_print_memo_d(d->data(row, "vch_print_memo").toString());
            vch_print_memo_d.append(ch_addon_flag_d);

            printmodel.appendRow(QList<QStandardItem*>()
                                 <<new QStandardItem("1")
                                 <<new QStandardItem((serial_no_cur))
                                 <<new QStandardItem(QString::number(num_cost_per))
                                 <<new QStandardItem(d->data(row, "vch_dishname").toString())
                                 <<new QStandardItem(d->data(row, "ch_dishno").toString())
                                 <<new QStandardItem(d->data(row, "dt_operdate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                 <<new QStandardItem(vch_print_memo_d.jsonToString())
                                 <<new QStandardItem(vch_company)
                                 <<new QStandardItem(QString().sprintf("%.02f", dish_num))
                                 );
        } else {
            //sql dish_num > 0
            num_cost_per = d->data(row, "num_cost").toDouble() / dish_num;
            num_cost_per += ch_addon_flag_d.total() / dish_num;
            vch_print_memoData vch_print_memo_d(d->data(row, "vch_print_memo").toString());
            vch_print_memo_d.append(ch_addon_flag_d);
            QString vch_print_memo_per = vch_print_memo_d.jsonToString(1 / dish_num);

            for(int k = 0; k < dish_num; k ++) {//非承重商品
                float k_num = 1;
                if(dish_num - k < 1) {
                    k_num = dish_num - k;
                }
                printmodel.appendRow(QList<QStandardItem*>()
                                     <<new QStandardItem("1")
                                     <<new QStandardItem((serial_no_cur))
                                     <<new QStandardItem(QString::number(num_cost_per))
                                     <<new QStandardItem(d->data(row, "vch_dishname").toString())
                                     <<new QStandardItem(d->data(row, "ch_dishno").toString())
                                     <<new QStandardItem(d->data(row, "dt_operdate").toDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                     <<new QStandardItem(vch_print_memo_per)
                                     <<new QStandardItem(vch_company)
                                     <<new QStandardItem(QString::number(k_num))
                                     );
            }
        }
    }

#if defined(Q_OS_WIN) || defined(QT_DEBUG)
    QString text = lds::fileOpenReadAll(lds::sysconf->value("w_sys_manage_outer_settings_label_win/label_format").toString());
    label_img_header::LabelTable table = label_img_header::resolveLabel(text);
    table.clearContent();
    for(int row = 0; row < printmodel.rowCount(); row++) {
        table.appendData("serial_no", printmodel.item(row, 1)->text());
        table.appendData("item_price", printmodel.item(row, 2)->text());
        table.appendData("item_name", printmodel.item(row, 3)->text());
        table.appendData("item_code", printmodel.item(row, 4)->text());
        table.appendData("bill_time", printmodel.item(row, 5)->text());
        table.appendData("item_costumize", printmodel.item(row, 6)->text());
        table.appendData("shop_name", printmodel.item(row, 7)->text());
        table.appendData("item_num", printmodel.item(row, 8)->text());
    }
    label_img_header::print(table);
    return;
    #endif
    for(int row = 0; row < printmodel.rowCount(); row++) {
        printmodel.item(row, 1)->setText(printmodel.item(row, 1)->text() + QString("(%1_%2)").arg(printmodel.rowCount()).arg(row + 1));
    }

    QString errstring;
    labelprint_main_dialog_object print_help;
    if(!print_help.printLabel(&printmodel, false, &errstring)) {
        QMetaObject::invokeMethod(public_sql::save_login_object(),
                                  "showprinterr",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, errstring),
                                  Q_ARG(QString, QObject::tr("标签打印机"))
                                  );
    }
}

void public_printby_ch_billno::print_label(const QString &ch_billno, const QStringList &insert_int_flowid_list)
{
    QString and_where_sql;
    if(insert_int_flowid_list.count() > 0) {
        and_where_sql = " and a.int_flowid in  (" + insert_int_flowid_list.join(",") + ") ";//QString(" and a.int_flowid in  ").arg(print_int_flowid_max);
    }
    print_label(ch_billno, and_where_sql);
}

void public_printby_ch_billno::print_label_test(QWidget *parent)
{
    labelprint_main_dialog dialog(parent);
    dialog.printmodel->setHorizontalHeaderLabels(QStringList()
                                                 << QObject::tr("打印次数")
                                                 //                                                 << QObject::tr("抬头")
                                                 << QObject::tr("单号")

                                                 << QObject::tr("价格")
                                                 //                                                 << QObject::tr("电话")
                                                 //                                                 << QObject::tr("地址")

                                                 << QObject::tr("品名")
                                                 << QObject::tr("品码")
                                                 << QObject::tr("时间")
                                                 << QObject::tr("做法")
                                                 << QObject::tr("店名")
                                                 << QObject::tr("数量")
                                                 );
    dialog.printmodel->appendRow(QList<QStandardItem*>()
                                 <<new QStandardItem("1")
                                 //                                 <<new QStandardItem(QObject::tr("抬头")
                                 <<new QStandardItem(QObject::tr("单号"))
                                 <<new QStandardItem(QObject::tr("价格"))
                                 //                                 <<new QStandardItem(QObject::tr("电话")
                                 //                                 <<new QStandardItem(QObject::tr("地址")

                                 <<new QStandardItem(QObject::tr("品名"))
                                 <<new QStandardItem(QObject::tr("品码"))
                                 <<new QStandardItem(QObject::tr("时间"))
                                 <<new QStandardItem(QObject::tr("做法"))
                                 <<new QStandardItem(QObject::tr("店名"))
                                 <<new QStandardItem(QObject::tr("数量"))
                                 );
    dialog.set_label_enabled();
    dialog.inputfinished(true);
    dialog.toprint(parent);
}

const blockIns &public_printby_ch_billno::getPintIns() const
{
    return print_format.print_ins;
}

void public_printby_ch_billno::model_bp_fmt_fill_orderdish(bool pay_exit, const QString & flowid_sql_filter)
{
    lds_query_hddpos  query;
    //打印明细
    //打印本单序号、赠送标识、特价标识、超出长度截断、大类分组
    //品名、单位、数量、价格、金额、折扣、做法
    //特价商品打印原价：特价/原价 ： 12.00/15.00
    //折扣：1元
    //大类分组时，套菜不参与分组

    QString order_sql = "order by a.int_flowid asc  ";
    //    QString bp_series_group = model_bp_fmt_value("bp_fmt_series_group");
    //    if(bp_series_group == "1") order_sql = " order by b.ch_dish_typeno asc";
    //    if(bp_series_group == "2") order_sql = " order by b.ch_sub_typeno asc";


    QString and_sql_filter;
    if(!flowid_sql_filter.isEmpty()) {
        and_sql_filter = " and " + flowid_sql_filter;
    }
    QString intersql = interSelectSql(model_bp_fmt, pay_exit, and_sql_filter);
    //header
    query.execSelect(" select "// 序号默认3个长度
                     +QString(" '%1' as dishname"
                              ", '%2' as unit"
                              ", '%3' as num"
                              ", '%4' as price"
                              ", '%5' as amount"
                              ", '%6' as disdetail"
                              ", '' as vch_print_memo"
                              ", '' as vch_reason "
                              ", '' as ch_dish_typeno"
                              ", '' as ch_sub_typeno"
                              ", '' as ch_suitflag"
                              ", '' as int_flowID"
                              ", '' as ch_specialflag"
                              ", '' as ch_presentflag "
                              ", '' as ch_waidai_flag "
                              ", '' as ch_addon_flag "
                              ", '' as ch_dishno "
                              ", '' as int_discount "

                              ", '' as num_price_org"
                              ", '' as num_price_add"
                              ", '' as dishname_2" )
                     .arg(print_format.printTr(QT_TR_NOOP("品名")))
                     .arg(print_format.printTr(QT_TR_NOOP("单位")))
                     .arg(print_format.printTr(QT_TR_NOOP("数量")))
                     .arg(print_format.printTr(QT_TR_NOOP("单价")))
                     .arg(print_format.printTr(QT_TR_NOOP("小计")))
                     .arg(print_format.printTr(QT_TR_NOOP("折扣")))
                     );
    query.next();
    model_bp_fmt_append_query("sale_detail_title", query);
    //content
    query.execSelect(intersql.arg(ch_billno)
                     +order_sql);
    while(query.next()) {
        model_bp_fmt_append_sale_detail(query);
    }

    model_bp_fmt->tablename_kvmap_print("sale_detail");
    ///
}

void public_printby_ch_billno::model_bp_fmt_fill_orderdish_return(const QStringList &int_quit_flow_list)
{
    lds_query_hddpos  query;
    //打印明细
    //打印本单序号、赠送标识、特价标识、超出长度截断、大类分组
    //品名、单位、数量、价格、金额、折扣、做法
    //特价商品打印原价：特价/原价 ： 12.00/15.00
    //折扣：1元
    //大类分组时，套菜不参与分组

    QString appsql = "order by a.int_flowid asc ";

    QString bp_vch_dishname_No  = model_bp_fmt_value("bp_vch_dishname_No", "vch_dishname");
    QString int_quit_flows = int_quit_flow_list.join(",");
    QString dishname;
    QString dishname_2;
    dishname_split(bp_vch_dishname_No, dishname, dishname_2, "b");//cey_bt_dish b
    //
    QString intersql =
            QString() +
            "   SELECT   "
            "   (select " + dishname + " from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as dishname, "

            "   (select c.vch_unitname from cey_bt_unit c, cey_bt_dish b where c.ch_unitno = b.ch_unitno and a.ch_dishno = b.ch_dishno) as unit, "
            "   -1*d.num_back_cur as num, "
            "   a.num_price as price, "
            "    ((0 - d.num_back_cur)*a.num_price*0.01*a.int_discount  )  as amount,  "
            "   a.num_price*-1*d.num_back_cur*(1-a.int_discount*0.01)  as disdetail,  "
            "   d.vch_print_memo,  "
            "   d.vch_reason, "

            "   (select b.ch_dish_typeno from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as ch_dish_typeno, "
            "   (select b.ch_sub_typeno from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as ch_sub_typeno, "

            "   a.ch_suitflag,  "
            "   a.int_flowID,  "
            "   a.ch_specialflag,  "
            "   a.ch_presentflag,   "
            "   a.ch_waidai_flag,   "
            "   a.ch_addon_flag,  "
            "   a.ch_dishno,  "
            "   a.int_discount,  "

            "   a.num_price_org, "
            "   -d.num_price_add as num_price_add, "
            "   (select " + dishname_2 + " from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as dishname_2 "

            "   FROM cey_u_orderdish a "
            ", cey_u_orderdish_log d where d.ch_type = 1 "
            "  and a.ch_billno = '%1' and  a.int_flowID = d.int_orderflow "
            "  and d.int_flowID in (" + int_quit_flows + ")"
            ;

    //header
    query.execSelect(" select "// 序号默认3个长度
                     +QString(" '%1' as dishname"
                              ", '%2' as unit"
                              ", '%3' as num"
                              ", '%4' as price"
                              ", '%5' as amount"
                              ", '%6' as disdetail"
                              ", '' as vch_print_memo"
                              ", '' as vch_reason "
                              ", '' as ch_dish_typeno"
                              ", '' as ch_sub_typeno"
                              ", '' as ch_suitflag"
                              ", '' as int_flowID"
                              ", '' as ch_specialflag"
                              ", '' as ch_presentflag "
                              ", '' as ch_waidai_flag "
                              ", '' as ch_addon_flag"
                              ", '' as ch_dishno"
                              ", '' as int_discount"

                              ", '' as num_price_org"
                              ", '' as num_price_add"
                              ", '' as dishname_2" )
                     .arg(print_format.printTr(QT_TR_NOOP("品名")))
                     .arg(print_format.printTr(QT_TR_NOOP("单位")))
                     .arg(print_format.printTr(QT_TR_NOOP("数量")))
                     .arg(print_format.printTr(QT_TR_NOOP("单价")))
                     .arg(print_format.printTr(QT_TR_NOOP("小计")))
                     .arg(print_format.printTr(QT_TR_NOOP("折扣")))
                     );

    query.next();
    model_bp_fmt_append_query("sale_detail_title", query);
    //content
    qDebug() << intersql.arg(ch_billno)  +appsql;
    query.execSelect(intersql.arg(ch_billno)  +appsql);
    while(query.next()) {
        model_bp_fmt_append_sale_detail(query);
    }
    //~content
}

void public_printby_ch_billno::print_run()
{
    int count = model_bp_fmt_value("bp_fmt_tail_block").toInt();
    const QString country = cey_sys_parameterValue(model_bp_fmt, "locale_country_name").toString();
    if(w_sys_manage_outer_pay_set_locale_wizard::bp_country_print_img(country)) {
        QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(print_format.print_path);
        if(printer.isNull()) {
            backheader::sendError("PRINT_IS_NULL", "");
            return;
        }
        printer->tryOpen(print_format.print_path);
        QByteArray pHex;
        QImage img = toMakeFormImg();
        Bmp_info::pic2Ins(img, pHex);
        printer->writedirect(pHex);
        printer->writedirect(QString(count, '\n').toLocal8Bit());
        printer->writeHex("1B6405 1D5601");//走纸5x， 切纸
        lds_thread::msecSleep(1000);
        return;
    }
    print_format.drawText("", QString(count, '\n'));
    print_format.drawTextEnd("1B6405 1D5601");//走纸5x， 切纸
    print_format.resolve();
    for(int k = 0; k < bp_count; k++) {
        print_format.write_bylanguage();
        lds_thread::msecSleep(1000);
    }
}

void public_printby_ch_billno::model_bp_fmt_fill_pay_detail()
{
    lds_query_hddpos  query;
    ///消费情况
    query.execSelect(QString("select  *  from cey_u_checkout_master where ch_billno = '%1'  and ch_state = 'Y'  ")//cey_u_checkout_master 存储的会员的优惠信息
                     .arg(ch_billno));
    query.next();
    QString vch_memberno = query.recordValue("vch_memberno").toString();
    QString ch_member_belong_state = query.recordValue("ch_member_belong_state").toString();
    QMap<QString/*会员号*/, QString/*state*/> m_state_map;
    if(!vch_memberno.isEmpty()) {
        m_state_map.insert(vch_memberno, ch_member_belong_state);
    }
    ///---begin--////////////////////////////////////////
    if(model_bp_fmt_value("bp_fmt_cost") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_cost", query.recordValue("num_cost"));
    }
    if(query.recordValue("num_present").toDouble() != 0) {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_present", query.recordValue("num_present"));
    }
    if(query.recordValue("num_discount").toDouble() != 0) {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_discount", query.recordValue("num_discount"));
    }
    if(query.recordValue("num_service").toDouble() != 0) {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_service", query.recordValue("num_service"));
    }
    if(model_bp_fmt_value("bp_fmt_tax") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_rate", query.recordValue("num_rate"));
    }
    if(query.recordValue("num_lowcost").toDouble() != 0) {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_lowcost", query.recordValue("num_lowcost"));
    }

    if(query.recordValue("num_free").toDouble() != 0) {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_free", query.recordValue("num_free"));
    }
    if(query.recordValue("num_blotout").toDouble() != 0) {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_blotout", query.recordValue("num_blotout"));
    }
    ///
    ///付款方式详细
    //    print_format.drawN();
    ///跳过 ：优惠、自动抹零
    query.execSelect(QString("select ifnull(sum(num_realamount), 0) as num_realamount, ifnull(sum(num_payamount), 0) as num_amount, ifnull(sum(num_change), 0)  as num_change from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno <> 'ZZ'  and ch_paymodeno <> 'YY'   ")//
                     .arg(get_cey_u_table("ch_payno").toString()));
    query.next();
    if(model_bp_fmt_value("bp_fmt_receive") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_realamount", query.recordValue("num_realamount"));
    }
    if(model_bp_fmt_value("bp_fmt_cash") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_amount", query.recordValue("num_amount"));
    }

    if(model_bp_fmt_value("bp_fmt_change") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_change", query.recordValue("num_change"));
    }
    if(get_cey_u_table("int_person").toInt() != 0) {
        if(model_bp_fmt_value("bp_fmt_avg") == "1") {
            model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_avg", query.recordValue("num_realamount").toFloat() / get_cey_u_table("int_person").toInt());
        }
    }
    ///
    ///付款方式情况
    ///
    if(model_bp_fmt_value("bp_fmt_pay_detail") == "1") {
        query.execSelect(QString(" SELECT b.ch_paymodeno, b.vch_paymodename, a.num_payamount-a.num_change as real_payamount, a.vch_voucherno FROM cey_u_checkout_detail a, cey_bt_paymode b where a.ch_paymodeno = b.ch_paymodeno and a.ch_payno = '%1' ")
                         .arg(get_cey_u_table("ch_payno").toString()));
        while(query.next()) {
            if(query.recordValue("ch_paymodeno").toString() == "ZZ"//优惠跳过
                    || query.recordValue("ch_paymodeno").toString() == "YY"//自动抹零
                    ) {
                continue;
            }
            model_bp_fmt->tablename_kvmap_insert_field("pay_detail_paymode", query.recordValue("ch_paymodeno").toString(), "vch_paymodename", query.recordValue("vch_paymodename"));
            model_bp_fmt->tablename_kvmap_insert_field("pay_detail_paymode", query.recordValue("ch_paymodeno").toString(), "real_payamount", query.recordValue("real_payamount"));
        }
    }

    ///会员情况
    query.execSelect(QString("select a.vch_voucherno, if(a.ch_paymodeno='10', 'Y', 'N') as cash_pay_flag, a.vch_card_name, a.num_card_point, a.num_card_remain, a.vch_card_adress  from  cey_u_checkout_detail a where  ( a.ch_paymodeno = '02' or a.ch_paymodeno = '10' ) and a.ch_payno = '%1' ")
                     .arg(get_cey_u_table("ch_payno").toString()));
    if(query.next()) {
        QString cardno = query.recordValue("vch_voucherno").toString();

        if(model_bp_fmt_value("bp_fmt_m_no") == "1") model_bp_fmt->tablename_kvmap_insert_field("pay_detail_member", cardno, "vch_voucherno", query.recordValue("vch_voucherno"));
        if(model_bp_fmt_value("bp_fmt_m_name") == "1") model_bp_fmt->tablename_kvmap_insert_field("pay_detail_member", cardno, "vch_card_name", query.recordValue("vch_card_name"));
        if(model_bp_fmt_value("bp_fmt_m_amount") == "1") model_bp_fmt->tablename_kvmap_insert_field("pay_detail_member", cardno, "num_card_remain", query.recordValue("num_card_remain"));
        if(model_bp_fmt_value("bp_fmt_m_point") == "1") model_bp_fmt->tablename_kvmap_insert_field("pay_detail_member", cardno, "num_card_point", query.recordValue("num_card_point"));
        if(model_bp_fmt_value("bp_fmt_m_adress") == "1") model_bp_fmt->tablename_kvmap_insert_field("pay_detail_member", cardno, "vch_card_adress", query.recordValue("vch_card_adress"));

        if(model_bp_fmt->tablename_kvmap_count("pay_detail_member") > 0) model_bp_fmt->tablename_kvmap_insert_field("pay_detail_member", cardno, "cash_pay_flag", query.recordValue("cash_pay_flag"));
    }
    ///---end--////////////////////////////////////////
}

QSharedPointer<lds_model_sqltablemodel> public_printby_ch_billno::make_pay_detail_preprint(const lds_model_sqltablemodel *m, const QString &ch_billno, const QString &flowid_sql_filter)
{
    QString and_sql_filter;
    if(!flowid_sql_filter.isEmpty()) {
        and_sql_filter = " and " + flowid_sql_filter;
    }
    QSharedPointer<lds_model_sqltablemodel> ret_model(new lds_model_sqltablemodel);
    ///消费情况
    fexpandmain_model_sqltablemodel_data tablemodel;
    tablemodel.setTable("cey_u_orderdish");
    tablemodel.select(ch_billno, " 1 " + and_sql_filter);

    m->tablename_kvmap_print();
    int ch_tax_mode = m->tablename_kvmap_value("cey_u_taxsale_master", ch_billno, "ch_billno", "ch_tax_mode").toInt();
    //pay_receive
    double pay_receive = tablemodel.getKeepdata().dish_total();
    double subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
    double num_blotout = pay_receive - subtotal;
    ///
    //    if(tablemodel.getKeepdata().num_service != 0 ||  tablemodel.getKeepdata().num_rate() != 0) {//服务费 != 0 或 税率 != 0
    //        QString tax_desc;
    switch(ch_tax_mode) {
    case 1://含税,不含服务费
        //pay_receive
        //            tax_desc = "("+print_format.printTr(QT_TR_NOOP("不计")) + ")";
        pay_receive += tablemodel.getKeepdata().num_service;
        subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
        num_blotout = pay_receive - subtotal;
        break;
    case 2://不含税,含服务费
        //pay_receive
        pay_receive += tablemodel.getKeepdata().num_service + tablemodel.getKeepdata().num_rate();
        subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
        num_blotout = pay_receive - subtotal;
        break;
    case 3://含税,含服务费
        //pay_receive
        //            tax_desc = "("+print_format.printTr(QT_TR_NOOP("不计")) + ")";
        pay_receive += tablemodel.getKeepdata().num_service;
        subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
        num_blotout = pay_receive - subtotal;
        break;
    case 0://不含税,不含服务费
        //pay_receive
        pay_receive += tablemodel.getKeepdata().num_service + tablemodel.getKeepdata().num_rate();
        subtotal = w_scr_dish_pay_widget::valuerountbysys(pay_receive);//四舍五入
        num_blotout = pay_receive - subtotal;
        break;
    }
    ret_model->tablename_kvmap_insert_value("pay_detail", "num_cost", tablemodel.getKeepdata().consum);
    ret_model->tablename_kvmap_insert_value("pay_detail", "num_discount", tablemodel.getKeepdata().discount);
    ret_model->tablename_kvmap_insert_value("pay_detail", "num_present", tablemodel.getKeepdata().present);
    ret_model->tablename_kvmap_insert_value("pay_detail", "num_service", tablemodel.getKeepdata().num_service);
    ret_model->tablename_kvmap_insert_value("pay_detail", "num_rate", tablemodel.getKeepdata().num_rate());
    ret_model->tablename_kvmap_insert_value("pay_detail", "num_blotout", num_blotout);
    //    }
    ret_model->tablename_kvmap_insert_value("pay_detail", "num_realamount", subtotal);

    ret_model->tablename_kvmap_insert_value("preprint_tip_detail", "tip_num_value_1",  tablemodel.getKeepdata().dish_total() * cey_sys_parameterValue(m, "tip_int_rate_1").toDouble() / 100.0 );
    ret_model->tablename_kvmap_insert_value("preprint_tip_detail", "tip_num_value_2",  tablemodel.getKeepdata().dish_total() * cey_sys_parameterValue(m, "tip_int_rate_2").toDouble() / 100.0 );
    ret_model->tablename_kvmap_insert_value("preprint_tip_detail", "tip_num_value_3",  tablemodel.getKeepdata().dish_total() * cey_sys_parameterValue(m, "tip_int_rate_3").toDouble() / 100.0 );

    return ret_model;
}

double public_printby_ch_billno::model_bp_fmt_fill_pay_detail_preprint(const QString &flowid_sql_filter)
{
    QSharedPointer<lds_model_sqltablemodel> paydetail_m = make_pay_detail_preprint(model_bp_fmt, ch_billno, flowid_sql_filter);

    double num_cost = paydetail_m->tablename_kvmap_value("pay_detail", "num_cost").toDouble();
    double num_discount = paydetail_m->tablename_kvmap_value("pay_detail", "num_discount").toDouble();
    double num_present = paydetail_m->tablename_kvmap_value("pay_detail", "num_present").toDouble();
    double num_service = paydetail_m->tablename_kvmap_value("pay_detail", "num_service").toDouble();
    double num_rate = paydetail_m->tablename_kvmap_value("pay_detail", "num_rate").toDouble();
    double num_blotout = paydetail_m->tablename_kvmap_value("pay_detail", "num_blotout").toDouble();

    double num_amount = paydetail_m->tablename_kvmap_value("pay_detail", "num_amount").toDouble();

    double tip_num_value_1 = paydetail_m->tablename_kvmap_value("preprint_tip_detail", "tip_num_value_1").toDouble();
    double tip_num_value_2 = paydetail_m->tablename_kvmap_value("preprint_tip_detail", "tip_num_value_2").toDouble();
    double tip_num_value_3 = paydetail_m->tablename_kvmap_value("preprint_tip_detail", "tip_num_value_3").toDouble();
    //
    if(num_service != 0
            ||  num_rate != 0) {//服务费 != 0 或 税率 != 0
        model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_cost", num_cost);
        if(num_discount != 0) {
            model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_discount",  num_discount);
        }
        if(num_present != 0) {
            model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_present",  num_present);
        }
        if(num_service != 0)//服务费 != 0
            model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_service",  num_service);
        if(num_rate != 0)//税率 != 0
            model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_rate",  num_rate);
        if(num_blotout != 0) //抹零 != 0
            model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_blotout",  num_blotout);
    }
    model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_amount",  num_amount);

    if(cey_sys_parameterValue(model_bp_fmt, "tip_enable").toString() == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("preprint_tip_detail", "tip_desc",  "Tip Guide");
        model_bp_fmt->tablename_kvmap_insert_value("preprint_tip_detail", "tip_num_value_1",  tip_num_value_1);
        model_bp_fmt->tablename_kvmap_insert_value("preprint_tip_detail", "tip_num_value_2",  tip_num_value_2);
        model_bp_fmt->tablename_kvmap_insert_value("preprint_tip_detail", "tip_num_value_3",  tip_num_value_3);
    }

    return num_amount;
}

double public_printby_ch_billno::model_bp_fmt_fill_pay_detail_preprint_qr_code(int int_flowid_greater_than)
{
    double sub_total = get_cey_u_table("num_cost").toDouble();
    model_bp_fmt->tablename_kvmap_insert_value("pay_detail", "num_amount",  sub_total);
    return sub_total;
}

bool public_printby_ch_billno::can_print_sale_detail()
{
    if(0 == bp_dishname.toInt() + bp_num.toInt() + bp_unit.toInt() + bp_price.toInt() + bp_total.toInt() + bp_disdetail.toInt() + bp_amount.toInt()) {
        return false;
    }
    return true;
}

void public_printby_ch_billno::initData(const QString &languageKey)
{
    print_format.setPrintTranslate(languageKey, this);
    rowno = 0;
    //打印分数
    bp_count = 1;

    paperwidth = 48;

    bp_content_len = "s" + model_bp_fmt_value("bp_fmt_content_len").toLocal8Bit();
    int bp_content_len_scale_h = 1;
    int s0 = bp_content_len.mid(1,1).toInt();//s00
    if(s0 != 0) {
        bp_content_len_scale_h = s0 + 1;
    }
    //品名
    bp_dishname = model_bp_fmt_value("bp_fmt_dishname");
    bp_dishname_len = model_bp_fmt_value("bp_fmt_dishname_len").toInt() / bp_content_len_scale_h;
    bp_dishname_row = model_bp_fmt_value("bp_fmt_dishname_row", "1").toInt();
    bp_dishname_left = model_bp_fmt_value("bp_fmt_dishname_left").toInt();
    //单位
    bp_unit = model_bp_fmt_value("bp_fmt_unit");
    bp_unit_len = model_bp_fmt_value("bp_fmt_unit_len").toInt() / bp_content_len_scale_h;
    bp_unit_row = model_bp_fmt_value("bp_fmt_unit_row", "1").toInt();
    bp_unit_left = model_bp_fmt_value("bp_fmt_unit_left").toInt();
    //数量
    bp_num = model_bp_fmt_value("bp_fmt_num");
    bp_num_len = model_bp_fmt_value("bp_fmt_num_len").toInt() / bp_content_len_scale_h;
    bp_num_row = model_bp_fmt_value("bp_fmt_num_row", "1").toInt();
    bp_num_left = model_bp_fmt_value("bp_fmt_num_left").toInt();

    bp_num_dec = model_bp_fmt_value("bp_fmt_num_dec").toInt();
    //价格
    bp_price = model_bp_fmt_value("bp_fmt_price");
    bp_price_len = model_bp_fmt_value("bp_fmt_price_len").toInt() / bp_content_len_scale_h;
    bp_price_row = model_bp_fmt_value("bp_fmt_price_row", "1").toInt();
    bp_price_left = model_bp_fmt_value("bp_fmt_price_left").toInt();
    bp_fmt_price_with_org = model_bp_fmt_value("bp_fmt_price_with_org");

    bp_price_dec = model_bp_fmt_value("bp_fmt_price_dec").toInt();
    //金额
    bp_amount = model_bp_fmt_value("bp_fmt_amount");
    bp_amount_len = model_bp_fmt_value("bp_fmt_amount_len").toInt() / bp_content_len_scale_h;
    bp_amount_row = model_bp_fmt_value("bp_fmt_amount_row", "1").toInt();
    bp_amount_left = model_bp_fmt_value("bp_fmt_amount_left").toInt();

    bp_amount_dec = model_bp_fmt_value("bp_fmt_amount_dec").toInt();
    //折扣
    bp_disdetail = model_bp_fmt_value("bp_fmt_disdetail");
    bp_disdetail_len = model_bp_fmt_value("bp_fmt_disdetail_len").toInt() / bp_content_len_scale_h;
    bp_disdetail_row = model_bp_fmt_value("bp_fmt_disdetail_row", "1").toInt();
    bp_fmt_disdetail_left = model_bp_fmt_value("bp_fmt_disdetail_left").toInt();
    //合计
    bp_total = model_bp_fmt_value("bp_fmt_total");
    bp_total_len = model_bp_fmt_value("bp_fmt_total_len").toInt() / bp_content_len_scale_h;
    bp_total_row = model_bp_fmt_value("bp_fmt_total_row", "1").toInt();
    bp_fmt_total_left = model_bp_fmt_value("bp_fmt_total_left").toInt();

    //做法
    bp_fmt_table_fmts =  model_bp_fmt_value("bp_fmt_table_fmts");
    bp_fmt_serial_no_desc =  model_bp_fmt_value("bp_fmt_serial_no_desc", print_format.printTr(QT_TR_NOOP("流水号")));
    bp_fmt_operid_fmts =  model_bp_fmt_value("bp_fmt_operid_fmts", "no");

    litterblank ="--";
}

QString public_printby_ch_billno::get_dish_typename(const QString &ch_dish_typeno)
{
    return model_bp_fmt->tablename_kvmap_value("cey_bt_dish_type", ch_dish_typeno, "ch_dish_typeno", "vch_dish_typename").toString();
}

QString public_printby_ch_billno::get_sub_typename(const QString &ch_sub_typeno)
{
    return model_bp_fmt->tablename_kvmap_value("cey_bt_dish_subtype", ch_sub_typeno, "ch_sub_typeno", "vch_sub_typename").toString();
}

QString public_printby_ch_billno::get_dish_typeno_sum(const lds_model_sqltablemodel_d *dish_printd, const QString &ch_dish_typeno)
{
    float subtotal = 0;
    for(int row = 0; row < dish_printd->rowCount(); row ++) {
        if(dish_printd->data(row, "ch_dish_typeno").toString() == ch_dish_typeno) {
            subtotal += dish_printd->data(row, "amount").toDouble();
            subtotal += dish_printd->data(row, "num_price_add").toDouble();
        }
    }
    dish_printd->print();
    return format_amount(subtotal, "subtotal");
}

QString public_printby_ch_billno::get_sub_typeno_sum(const lds_model_sqltablemodel_d *dish_printd, const QString &ch_sub_typeno)
{
    float subtotal = 0;
    for(int row = 0; row < dish_printd->rowCount(); row ++) {
        if(dish_printd->data(row, "ch_sub_typeno").toString() == ch_sub_typeno) {
            subtotal += dish_printd->data(row, "amount").toDouble();
            subtotal += dish_printd->data(row, "num_price_add").toDouble();
        }
    }
    return format_amount(subtotal, "subtotal");
}

QString public_printby_ch_billno::get_sum(const lds_model_sqltablemodel_d *dish_printd)
{
    float subtotal = 0;
    for(int row = 0; row < dish_printd->rowCount(); row ++) {
        subtotal += dish_printd->data(row, "amount").toDouble();
    }
    return format_amount(subtotal, "subtotal");
}

void public_printby_ch_billno::piecedishprint(int row, const lds_model_sqltablemodel_d *dish_printd, const QByteArray &pre, bool isheader)
{
    print_format.drawText("a10");
    //2
    QByteArray vch_print_memo = dish_printd->data(row, "vch_print_memo").toByteArray();
    QString vch_reason = dish_printd->data(row, "vch_reason").toString();
    QString ch_suitflag = dish_printd->data(row, "ch_suitflag").toString();
    QString ch_addon_flag = dish_printd->data(row, "ch_addon_flag").toString();
    QString suit_left_spacing;
    QString price;
    QString amount;
    QString dish_num;
    QString disdetail;
    QString total;
    int amount_left = -1;
    int dishname_left = -1;
    float num_price_add = 0;
    QString num_price_add_str;
    QString preapp;
    //    //
    if(ch_suitflag == "Y" || ch_addon_flag == "Y") {
        suit_left_spacing += "  ";
    }
    if(model_bp_fmt_value("bp_fmt_presentflag") == "1") {
        if(dish_printd->data(row, "ch_presentflag").toString() == "Y") {
            preapp += print_format.printTr(QT_TR_NOOP("赠"))+")";
        }
    }
    if(model_bp_fmt_value("bp_fmt_specialflag") == "1") {
        if(dish_printd->data(row, "ch_specialflag").toString() == "Y") {
            preapp += print_format.printTr(QT_TR_NOOP("特"))+")";
        }
    }
    if(dish_printd->data(row, "ch_waidai_flag").toString() == "Y") {
        preapp += print_format.printTr(QT_TR_NOOP("外带"))+")";
    }
    //
    if(!isheader && model_bp_fmt_value("bp_fmt_rowid") == "1") {
        if(ch_suitflag == "Y" || ch_addon_flag == "Y") {
            suit_left_spacing += "  ";
        } else {
            rowno++;
            preapp = QString("%1.").arg(rowno) +preapp;
        }
    }
    if(isheader) {
        price = dish_printd->data(row, "price").toString();
        amount = dish_printd->data(row, "amount").toString();
        dish_num = dish_printd->data(row, "num").toString();
        disdetail = dish_printd->data(row, "disdetail").toString();
        total = print_format.printTr(QT_TR_NOOP("合计"));
    } else {
        price = format_price(dish_printd->data(row, "price").toFloat(), "price");
        if(bp_fmt_price_with_org == "1"
                &&ch_suitflag != "Y") {//非子套菜
            if(dish_printd->data(row, "price").toFloat() != dish_printd->data(row, "num_price_org").toFloat()) {
                price = price+"/" + format_price(dish_printd->data(row, "num_price_org").toFloat(), "num_price_org");
            }
        }
        amount = format_amount(dish_printd->data(row, "amount").toFloat(), "amount");
        dish_num = format_num(dish_printd->data(row, "num").toFloat(), "num");
        if(!print_format.languageIsCN()) {
            dish_num = "x"+dish_num;
        }
        disdetail ="";
        if(dish_printd->data(row, "disdetail").toFloat() != 0) {
            disdetail = format_amount(dish_printd->data(row, "disdetail").toFloat(), "disdetail");
        }
        total = format_amount(dish_printd->data(row, "price").toFloat() * dish_printd->data(row, "num").toFloat(), "total");
        num_price_add = dish_printd->data(row, "num_price_add").toFloat();
        num_price_add_str = format_amount(num_price_add, "num_price_add");
    }
    QStringList  bp_fmt_hheader_order = model_bp_fmt_value("bp_fmt_hheader_order", "dishname,num,unit,price,total,disdetail,amount").split(",");

    QString dishname = dish_printd->data(row, "dishname").toString();
    QString dishname_2 = dish_printd->data(row, "dishname_2").toString();
    //第一行
    foreach(const QString &bp_fmt_hheader, bp_fmt_hheader_order) {
        if(bp_fmt_hheader == "dishname" && bp_dishname_row == 1 && bp_dishname == "1") {dishname_left = print_format.left(); print_format.drawText(pre, QString(bp_dishname_left, ' ') + getfixedlen(suit_left_spacing + preapp + dishname, bp_dishname_len), bp_dishname_len, Qt::AlignLeft);}
        if(bp_fmt_hheader == "num" && bp_num_row == 1 && bp_num == "1")print_format.drawText(pre, QString(bp_num_left, ' ')+dish_num, bp_num_len, Qt::AlignRight);
        if(bp_fmt_hheader == "unit" && bp_unit_row == 1 && bp_unit == "1")print_format.drawText(pre, QString(bp_unit_left, ' ')+dish_printd->data(row, "unit").toString(), bp_unit_len, Qt::AlignLeft);
        if(bp_fmt_hheader == "price" && bp_price_row == 1 && bp_price == "1")print_format.drawText(pre, QString(bp_price_left, ' ')+price, bp_price_len, Qt::AlignRight);
        if(bp_fmt_hheader == "total" && bp_total_row == 1 && bp_total == "1")print_format.drawText(pre, QString(bp_fmt_total_left, ' ')+total, bp_total_len, Qt::AlignRight);
        if(bp_fmt_hheader == "disdetail" && bp_disdetail_row == 1 && bp_disdetail == "1")print_format.drawText(pre, QString(bp_fmt_disdetail_left, ' ')+disdetail, bp_disdetail_len, Qt::AlignRight);
        if(bp_fmt_hheader == "amount" && bp_amount_row == 1 && bp_amount == "1") {amount_left = print_format.left(); print_format.drawText(pre, QString(bp_amount_left, ' ')+amount, bp_amount_len, Qt::AlignRight);}
    }
    print_format.drawN();

    //第二行
    foreach(const QString &bp_fmt_hheader, bp_fmt_hheader_order) {
        if(bp_fmt_hheader == "dishname" && bp_dishname_row == 2 && bp_dishname == "1") {dishname_left = print_format.left(); print_format.drawText(pre, QString(bp_dishname_left, ' ') + getfixedlen(suit_left_spacing + preapp + dishname, bp_dishname_len), bp_dishname_len, Qt::AlignLeft);}
        if(bp_fmt_hheader == "num" && bp_num_row == 2 && bp_num == "1")print_format.drawText(pre, QString(bp_num_left, ' ')+dish_num, bp_num_len, Qt::AlignRight);
        if(bp_fmt_hheader == "unit" && bp_unit_row == 2 && bp_unit == "1")print_format.drawText(pre, QString(bp_unit_left, ' ')+dish_printd->data(row, "unit").toString(), bp_unit_len, Qt::AlignLeft);
        if(bp_fmt_hheader == "price" && bp_price_row == 2 && bp_price == "1")print_format.drawText(pre, QString(bp_price_left, ' ')+price, bp_price_len, Qt::AlignRight);
        if(bp_fmt_hheader == "total" && bp_total_row == 2 && bp_total == "1")print_format.drawText(pre, QString(bp_fmt_total_left, ' ')+total, bp_total_len, Qt::AlignRight);
        if(bp_fmt_hheader == "disdetail" && bp_disdetail_row == 2 && bp_disdetail == "1")print_format.drawText(pre, QString(bp_fmt_disdetail_left, ' ')+disdetail, bp_disdetail_len, Qt::AlignRight);
        if(bp_fmt_hheader == "amount" && bp_amount_row == 2 && bp_amount == "1") {   amount_left = print_format.left(); print_format.drawText(pre, QString(bp_amount_left, ' ')+amount, bp_amount_len, Qt::AlignRight);}
    }
    //第三行
    if(dishname_2.count() > 0) {
        foreach(const QString &bp_fmt_hheader, bp_fmt_hheader_order) {
            if(bp_fmt_hheader == "dishname" && /*bp_dishname_row == 2 && */bp_dishname == "1") {dishname_left = print_format.left(); print_format.drawText(pre, QString(bp_dishname_left, ' ') + getfixedlen(suit_left_spacing + QString(lds::size_g_of_u(preapp), ' ') + dishname_2, bp_dishname_len), bp_dishname_len, Qt::AlignLeft);}
        }
        print_format.drawN();
    }

    if(dishname_left >= 0
            && (vch_print_memo.length() > 0 || num_price_add != 0)
            ) {
        print_format.drawN();
        QString vch_print_memo_desc = suit_left_spacing + print_format.printTr(QT_TR_NOOP("做法")) + ":" + vch_print_memo;
        print_format.drawText("", vch_print_memo_desc);

        ///打印总包含金额、则打印做法加价
        if(amount_left >= 0) {
            int amount_left0 = amount_left - lds::size_g_of_u(vch_print_memo_desc);
            if( amount_left0 < 0) {
                print_format.drawN();
            }  else {
                amount_left = amount_left0;
            }

            print_format.drawText(pre, QString(amount_left, ' '));
            print_format.drawText(pre, QString(bp_amount_left, ' ')+num_price_add_str, bp_amount_len, Qt::AlignRight);
        }
        ///～打印总包含金额、则打印做法加价
    }
    if(dishname_left >= 0
            && vch_reason.length() > 0
            ) {
        print_format.drawN();
        print_format.drawText("", QString(dishname_left, ' '));
        print_format.drawText("", print_format.printTr(QT_TR_NOOP("原因")) + ":"+vch_reason);
        print_format.drawN();
    }

    print_format.drawN();//最后添加
}

int public_printby_ch_billno::format_Negative(const QString &field_name)
{
    //num_cost num_prensent num_discount num_rate num_blutout num_service num_lowcost num_cash
    if(field_name == "disdetail"
            || field_name == "num_blotout"
            || field_name == "num_present"
            || field_name == "num_discount"
            || field_name == "num_free"
            ) {
        return -1;
    }
    return 1;
}

QString public_printby_ch_billno::format_amount(float value, const QString &field_name)
{
    if(field_name == "num_cost") {
        int ch_tax_mode = model_bp_fmt->tablename_kvmap_value("cey_u_taxsale_master", ch_billno, "ch_billno", "ch_tax_mode").toInt();
        if(ch_tax_mode == 1//含税
                || ch_tax_mode == 3//含税
                ) {
            value -= get_pay_detail("num_rate").toFloat();
        }
    }
    QString ret =  model_bp_fmt_value("pay_unit_prefix") + lds:: septNumber(format_Negative(field_name) * value, bp_amount_dec);
    return ret;
}

QString public_printby_ch_billno::format_num(float value, const QString &field_name)
{
    return lds:: septNumber(format_Negative(field_name) * value, bp_num_dec);
}

QString public_printby_ch_billno::format_price(float value, const QString &field_name)
{
    return model_bp_fmt_value("pay_unit_prefix") + lds:: septNumber(format_Negative(field_name) * value, bp_price_dec);
}

QString public_printby_ch_billno::name_of_operid(const QString &operid)
{
    if(operid == "0000")
        return "system";
    return model_bp_fmt->tablename_kvmap_value("cey_sys_operator", operid, "vch_operID", "vch_operator").toString();
}

QString public_printby_ch_billno::getfixedlen(const QString &value, int len, int isCut)
{
    QString retstr;
    len = len;
    int slen = lds::size_g_of_u(value);
    if(isCut != 1 || slen < len) {
        retstr = value;
    } else {
        for(int k = 0; k < value.count(); k++) {
            retstr += value[k];
            if(lds::size_g_of_u(retstr) > len) {
                retstr.chop(1);
                break;
            }
        }
    }
    return retstr;
}

QString public_printby_ch_billno::getfixedlen(const QString &value, int len)
{
    return getfixedlen(value, len, model_bp_fmt_value("bp_fmt_overstep_cut").toInt());
}

QString public_printby_ch_billno::key_value_com(const QString &k, const QString &v, int paperwidth, int xscale)
{
    int len = paperwidth / xscale;
    int leftspace = 2 / xscale;
    int klen = lds::size_g_of_u(k);
    int vlen = lds::size_g_of_u(v);
    return k + QString(len - klen - vlen - leftspace, ' ') + v + QString(leftspace, ' ');
}

int public_printby_ch_billno::scale_h_by(const QByteArray &s00)
{
    int scale = 1;
    int s0 = s00.mid(1,1).toInt();//s00
    if(s0 != 0) {
        scale = s0 + 1;
    }
    return scale;
}

QString public_printby_ch_billno::get_bp_fmt_table(const QString &vch_tablename, const QString &ch_tableno)
{
    if(bp_fmt_table_fmts == "name(no)") {
        return vch_tablename+"("+ch_tableno+")";
    }
    if(bp_fmt_table_fmts == "name") {
        return vch_tablename;
    }
    if(bp_fmt_table_fmts == "no") {
        return ch_tableno;
    }
    return vch_tablename+"("+ch_tableno+")";
}

QString public_printby_ch_billno::get_bp_fmt_table()
{
    QString ch_tableno = model_bp_fmt->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_tableno").toString();
    QString vch_tablename;
    if(ch_tableno == lds_scr_table::Q000)
        vch_tablename  = print_format.printTr(QT_TR_NOOP("快餐"));
    else
        vch_tablename = model_bp_fmt->tablename_kvmap_value("cey_bt_table", ch_tableno, "ch_tableno", "vch_tablename").toString();
    return get_bp_fmt_table(vch_tablename, ch_tableno);
}

QString public_printby_ch_billno::get_bp_fmt_operid(const QString &vch_operator, const QString &vch_operid)
{
    if(bp_fmt_operid_fmts == "name(no)") {
        return vch_operator+"("+vch_operid+")";
    }
    if(bp_fmt_operid_fmts == "name") {
        return vch_operator;
    }
    if(bp_fmt_operid_fmts == "no") {
        return vch_operid;
    }
    return vch_operator+"("+vch_operid+")";
}

QString public_printby_ch_billno::get_bp_fmt_operid()
{
    QString vch_operid = model_bp_fmt->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_operid").toString();
    QString vch_operator = name_of_operid(vch_operid);
    return get_bp_fmt_operid(vch_operator, vch_operid);
}

QVariant public_printby_ch_billno::get_cey_u_table(const QString &sql_value_name)
{
    return model_bp_fmt->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", sql_value_name);
}

QVariant public_printby_ch_billno::get_cey_u_togo(const QString &sql_value_name)
{
    return model_bp_fmt->tablename_kvmap_value("cey_u_togo", ch_billno, "ch_billno", sql_value_name);
}

bool public_printby_ch_billno::receipt_head_existed(const QString &key)
{
    return model_bp_fmt->tablename_key_existed("receipt_head", key);
}

bool public_printby_ch_billno::sale_head_existed(const QString &key)
{
    return model_bp_fmt->tablename_key_existed("sale_head", key);
}

QVariant public_printby_ch_billno::get_sale_head(const QString &key)
{
    return model_bp_fmt->tablename_kvmap_value("sale_head", key);
}

QVariant public_printby_ch_billno::get_receipt_head(const QString &key)
{
    return model_bp_fmt->tablename_kvmap_value("receipt_head", key);
}

bool public_printby_ch_billno::pay_detail_existed(const QString &key)
{
    return model_bp_fmt->tablename_key_existed("pay_detail", key);
}

QVariant public_printby_ch_billno::get_pay_detail(const QString &key)
{
    return model_bp_fmt->tablename_kvmap_value("pay_detail", key);
}

bool public_printby_ch_billno::receipt_tail_existed(const QString &key)
{
    return model_bp_fmt->tablename_key_existed("receipt_tail", key);
}

QVariant public_printby_ch_billno::get_receipt_tail(const QString &key)
{
    return model_bp_fmt->tablename_kvmap_value("receipt_tail", key);
}

QString public_printby_ch_billno::model_bp_fmt_value(const QString &as_parameter, const QVariant &as_default)
{
    return model_bp_fmt_value(model_bp_fmt, as_parameter, as_default);
}

QString public_printby_ch_billno::model_bp_fmt_value(lds_model_sqltablemodel *model_bp_fmt, const QString &as_parameter, const QVariant &as_default)
{
    QString ret;
    int row = -1;

    //
    if(as_parameter.startsWith("bp_fmt")) {
        if(model_bp_fmt->tablename_key_existed("vch_parameter_row", as_parameter)) {
            row = model_bp_fmt->tablename_kvmap_value("vch_parameter_row", as_parameter).toInt();
        } else {
            for(int k = 0; k < model_bp_fmt->rowCount(); k++) {
                if(model_bp_fmt->model_data(k, "vch_parameter").toString() == as_parameter) {
                    row = k;
                    break;
                }
            }
            model_bp_fmt->tablename_kvmap_insert_value("vch_parameter_row", as_parameter, row);
        }
    }
    //
    if(row >= 0) {
        ret = model_bp_fmt->model_data(row, "vch_value").toString();
    } else {
        ret = model_bp_fmt->tablename_kvmap_value("cey_sys_parameter", as_parameter, "vch_parameter", "vch_value").toString();
    }
    //
    if(ret.trimmed().isEmpty()) {
        ret = as_default.toString();
    }
    return ret;
}


void public_printby_ch_billno::txt_print_fill_sale_head(lds_model_sqltablemodel *m, const QString &ch_billno)
{
    lds_query_hddpos  query;
    //serial_no_cur
    QString serial_no_cur = m->tablename_kvmap_value("cey_u_master", ch_billno, "ch_billno", "vch_memo").toString();
    //time_diff
    QString time_diff;
    QDateTime dt_operdate;
    query.execSelect(QString("select timediff(b.dt_operdate, a.dt_operdate) as time_diff, b.dt_operdate  from cey_u_table  a, cey_u_checkout_master b where a.ch_billno = b.ch_billno and b.ch_state = 'Y' and b.ch_billno = '%1' ")
                     .arg(ch_billno));
    query.next();
    time_diff = query.recordValue("time_diff").toString();
    dt_operdate = query.recordValue("dt_operdate").toDateTime();

    txt_print_bill_set_data(m, "sale_head", "dt_opendate", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "dt_operdate").toDateTime().toString(yyyyMMddhhmmss));
    txt_print_bill_set_data(m, "sale_head", "ch_billno", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_billno"));

    txt_print_bill_set_data(m, "sale_head", "ch_payno", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_payno"));
    txt_print_bill_set_data(m, "sale_head", "vch_operid", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_operid"));
    txt_print_bill_set_data(m, "sale_head", "vch_operator", model_operator(m, m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_operid").toString()));
    txt_print_bill_set_data(m, "sale_head", "ch_tableno", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_tableno"));
    txt_print_bill_set_data(m, "sale_head", "vch_tablename", model_tablename(m, m->tablename_kvmap_field("bill_ins_table", "%cz3", "value").toString()));

    txt_print_bill_set_data(m, "sale_head", "int_person", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "int_person"));
    txt_print_bill_set_data(m, "sale_head", "vch_waiter", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_waiter"));
    txt_print_bill_set_data(m, "sale_head", "vch_waitername", model_operator(m, m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_waiter").toString()));
    txt_print_bill_set_data(m, "sale_head", "serial_no", serial_no_cur);
    txt_print_bill_set_data(m, "sale_head", "dt_paydate", dt_operdate.toString(yyyyMMddhhmmss));

    txt_print_bill_set_data(m, "sale_head", "times", time_diff);
    txt_print_bill_set_data(m, "sale_head", "vch_memo", m->tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_memo"));

    //waimai
    txt_print_bill_set_data(m, "sale_head", "vch_address", m->tablename_kvmap_value("cey_u_togo", ch_billno, "ch_billno", "vch_address"));
    txt_print_bill_set_data(m, "sale_head", "vch_linkman", m->tablename_kvmap_value("cey_u_togo", ch_billno, "ch_billno", "vch_linkman"));
    txt_print_bill_set_data(m, "sale_head", "vch_tel", m->tablename_kvmap_value("cey_u_togo", ch_billno, "ch_billno", "vch_tel"));
}

void public_printby_ch_billno::txt_print_fill_sale_detail(lds_model_sqltablemodel *m, const QString &ch_billno, const QString &and_sql_filter)
{
    QStringList m_rowid;
    QStringList m_dishname;
    QStringList m_dishname_2;
    QStringList m_num;
    QStringList m_unit;
    QStringList m_price;

    QStringList m_total;
    QStringList m_disdetail;
    QStringList m_amount;
    /*
%bd	["1","2","3"] 	rowid	本单序号
%pm	["牛生筋生筋","大肥腰","宫后筋"] 	dishname	品名
%sl	["12.36","1.22","0.23"] 	num	数量
%dw	["kg","斤","杯"] 	unit	单位
%dj	["1.20","0.53","99.66"] 	price	单价
%hj	["2.33","3.33","55.33"] 	total	合计
%zk	["0.2","0.2","0.2"] 	disdetail	折扣
%xj	["2.33","3.33","55.33"] 	amount	小计
*/
    lds_query_hddpos  query;
    int rowid_sqr = 0;
    vch_print_memoData vch_print_memo;
    QString ch_suitflag;
    double num_price_add;

    QString ch_specialflag;
    QString ch_presentflag;
    QString ch_waidai_flag;
    translatorObj *print_format = m->findChild<translatorObj *>("translatorObj");

    query.execSelect(interSelectSql(m, false, and_sql_filter).arg(ch_billno) + " order by a.int_flowid asc ");
    while(query.next()) {
        vch_print_memo.select(query.recordValue("vch_print_memo").toByteArray());
        ch_suitflag = query.recordValue("ch_suitflag").toString();
        num_price_add = query.recordValue("num_price_add").toDouble();

        ch_specialflag = query.recordValue("ch_specialflag").toString();
        ch_presentflag = query.recordValue("ch_presentflag").toString();
        ch_waidai_flag = query.recordValue("ch_waidai_flag").toString();

        m_rowid << QString::number(++rowid_sqr).rightJustified(2, ' ') + ".";
        m_dishname << query.recordValue("dishname").toString();
        m_dishname_2 << query.recordValue("dishname_2").toString();
        m_num <<  septNumberCurrency(m, query.recordValue("num").toDouble(), cey_sys_parameterValue(m, "bp_fmt_num_dec").toInt(), false);
        m_unit << query.recordValue("unit").toString();
        m_price << septNumberCurrency(m, query.recordValue("price").toDouble(), cey_sys_parameterValue(m, "bp_fmt_price_dec").toInt());
        m_total << septNumberCurrency(m, query.recordValue("price").toDouble() * query.recordValue("num").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt());

        m_disdetail << septNumberCurrency(m, query.recordValue("disdetail").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt());
        m_amount << septNumberCurrency(m, query.recordValue("amount").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt());

        //
        if(print_format != 0) {
            if(ch_presentflag == "Y") {
                m_dishname.last() = print_format->printTr(QT_TR_NOOP("赠"))+")" + m_dishname.last();
            }
            if(ch_specialflag == "Y") {
                m_dishname.last() = print_format->printTr(QT_TR_NOOP("特"))+")" + m_dishname.last();
            }
            if(ch_waidai_flag == "Y") {
                m_dishname.last() = print_format->printTr(QT_TR_NOOP("外带"))+")" + m_dishname.last();
            }
        }

        //
        if(ch_suitflag == "Y") {
            rowid_sqr --;
            m_rowid.last() = QString(3, ' ');
            m_price.last() = "";
            m_total.last() = "";
            m_disdetail.last() = "";
            m_amount.last() = "";
        }

        ///
        if(vch_print_memo.count() > 0 || num_price_add > 0) {
            m_rowid << QString(3, ' ');
            m_dishname << vch_print_memo.jsonToString();
            m_dishname_2 << "";
            m_num << "";
            m_unit << "";
            m_price << "";
            m_total << septNumberCurrency(m, num_price_add, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt());

            m_disdetail << "";
            m_amount << septNumberCurrency(m, num_price_add, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt());

            if(ch_suitflag == "Y") {
                m_price.last() = "";
                m_total.last() = "";
                m_disdetail.last() = "";
                m_amount.last() = "";
            }
            if(ch_suitflag == "P") {
                m_dishname.last() = QObject::tr("做法");
            }
        }///~
    }



    txt_print_bill_set_data(m, "sale_detail", "rowid", stringListToJsonArray(m_rowid));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "sale_detail", "dishname", stringListToJsonArray(m_dishname));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "sale_detail", "dishname_2", stringListToJsonArray(m_dishname_2));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "sale_detail", "num",  stringListToJsonArray(m_num));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "sale_detail", "unit", stringListToJsonArray(m_unit));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "sale_detail", "price", stringListToJsonArray(m_price));//.join("\",\"").prepend("[\"").append("\"]"));

    txt_print_bill_set_data(m, "sale_detail", "total", stringListToJsonArray(m_total));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "sale_detail", "disdetail", stringListToJsonArray(m_disdetail));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "sale_detail", "amount", stringListToJsonArray(m_amount));//.join("\",\"").prepend("[\"").append("\"]"));
}

/*
%xf	12.66	cost	消费
%zs	58.10	present	赠送
%zk2	0.25	discount	折扣
%fw3	9.66	service	服务费
%ss	5.33	tax	税收
%dx	0.01	lowcost	低消差
%yh	0.02	free	优惠
%ml	0.00	blotout	抹零
%ys	2.36	cash	应收
%ss2	78.36	receive	实收
%zl	55.12	change	找零
%rj	12.36	avg	人均
*/
void public_printby_ch_billno::txt_print_fill_pay_detail(lds_model_sqltablemodel *m, const QString &ch_billno)
{
    lds_query_hddpos  query;
    ///消费情况
    query.execSelect(QString("select  *  from cey_u_checkout_master where ch_billno = '%1'  and ch_state = 'Y'  ")//cey_u_checkout_master 存储的会员的优惠信息
                     .arg(ch_billno));
    query.next();
    QString ch_payno = query.recordValue("ch_payno").toString();
    double num_cost = query.recordValue("num_cost").toDouble();
    double num_present = query.recordValue("num_present").toDouble();
    double num_discount = query.recordValue("num_discount").toDouble();
    int int_discount_off = 0;
    if(num_cost != num_present) {
        int_discount_off = num_discount * 100 / (num_cost - num_present);
    }


    txt_print_bill_set_data(m, "pay_detail", "num_cost", zeroEmpty(m, num_cost, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_present", zeroEmpty(m, -1 * num_present, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_discount", zeroEmpty(m, -1 * num_discount, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "int_discount_off", int_discount_off);
    txt_print_bill_set_data(m, "pay_detail", "num_service", zeroEmpty(m, query.recordValue("num_service").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_rate", zeroEmpty(m, query.recordValue("num_rate").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

    txt_print_bill_set_data(m, "pay_detail", "num_lowcost", zeroEmpty(m, query.recordValue("num_lowcost").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_free", zeroEmpty(m, -1 * query.recordValue("num_free").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_blotout", zeroEmpty(m, -1 * query.recordValue("num_blotout").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

    QString tax_name1 = lds_scr_table::int_rate_name(m, 1);
    QString tax_name2 = lds_scr_table::int_rate_name(m, 2);
    QString tax_name3 = lds_scr_table::int_rate_name(m, 3);
    txt_print_bill_set_data(m, "pay_detail", "tax_name1", tax_name1);
    txt_print_bill_set_data(m, "pay_detail", "tax_name2", tax_name2);
    txt_print_bill_set_data(m, "pay_detail", "tax_name2", tax_name3);

    txt_print_bill_set_data(m, "pay_detail", "pay_fixed_service_value", "");
    txt_print_bill_set_data(m, "pay_detail", "pay_fixed_tax_value", "");
    txt_print_bill_set_data(m, "pay_detail", "tax_before", "");
    txt_print_bill_set_data(m, "pay_detail", "tax_after", "");
    if("1" == cey_sys_parameterValue(m, "pay_fixed_service")) {
        if(cey_sys_parameterValue(m, "pay_fixed_service_value").toDouble() != 0)
            txt_print_bill_set_data(m, "pay_detail", "pay_fixed_service_value", cey_sys_parameterValue(m, "pay_fixed_service_value"));
    }
    if("Y" == m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "ch_fix_flag").toString()) {
        double num_before = m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "num_before").toDouble();

        double num_cur = m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "num_cur").toDouble();
        double num_cur_2 = m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "num_cur_2").toDouble();
        double num_cur_3 = m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "num_cur_3").toDouble();

        double num_fix_tax_1 = m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "int_fix_tax").toDouble();
        double num_fix_tax_2 = m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "int_fix_tax_2").toDouble();
        double num_fix_tax_3 = m->tablename_kvmap_value("cey_u_taxsale_master", ch_payno, "ch_payno", "int_fix_tax_3").toDouble();
        double num_fix_tax = num_fix_tax_1 + num_fix_tax_2 + num_fix_tax_3;

        txt_print_bill_set_data(m, "pay_detail", "pay_fixed_tax_value", num_fix_tax);
        txt_print_bill_set_data(m, "pay_detail", "tax_before", septNumberCurrency(m, num_before, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
        txt_print_bill_set_data(m, "pay_detail", "tax_after", septNumberCurrency(m, num_before - num_cur, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

        txt_print_bill_set_data(m, "pay_detail", "num_fix_tax_total", num_fix_tax);
        txt_print_bill_set_data(m, "pay_detail", "num_cur", septNumberCurrency(m, num_cur, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
        txt_print_bill_set_data(m, "pay_detail", "num_cur_2", septNumberCurrency(m, num_cur_2, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
        txt_print_bill_set_data(m, "pay_detail", "num_cur_total", septNumberCurrency(m, num_cur + num_cur_2 + num_cur_3, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    }

    ///付款方式详细
    /// 跳过：优惠、自动抹零
    double int_person = m->tablename_kvmap_value("sale_head", "int_person").toDouble();
    query.execSelect(QString("select ifnull(sum(num_realamount), 0) as num_realamount, ifnull(sum(num_payamount), 0) as num_amount, ifnull(sum(num_change), 0)  as num_change from cey_u_checkout_detail where ch_payno = '%1' and ch_paymodeno <> 'ZZ'  and ch_paymodeno <> 'YY'  ")
                     .arg(ch_payno));
    query.next();

    txt_print_bill_set_data(m, "pay_detail", "num_realamount",  septNumberCurrency(m, query.recordValue("num_realamount").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_amount",  septNumberCurrency(m, query.recordValue("num_amount").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_change", zeroEmpty(m, query.recordValue("num_change").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_avg", "");
    if(int_person != 0)
        txt_print_bill_set_data(m, "pay_detail", "num_avg", zeroEmpty(m, m->tablename_kvmap_value("pay_detail", "num_realamount").toDouble() / qMax(1.0, int_person), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

    ///付款方式情况
    QStringList m_pay_name;
    QStringList m_pay_value;
    query.execSelect(QString(" SELECT b.ch_paymodeno, b.vch_paymodename, a.num_payamount-a.num_change as real_payamount, a.vch_voucherno FROM cey_u_checkout_detail a, cey_bt_paymode b where a.ch_paymodeno = b.ch_paymodeno and a.ch_payno = '%1' ")
                     .arg(ch_payno));
    while(query.next()) {
        if(query.recordValue("ch_paymodeno").toString() == "ZZ"//优惠跳过
                || query.recordValue("ch_paymodeno").toString() == "YY"//自动抹零
                ) {
            continue;
        }
        m_pay_name << query.recordValue("vch_paymodename").toString();
        m_pay_value <<  septNumberCurrency(m, query.recordValue("real_payamount").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt());
    }
    txt_print_bill_set_data(m, "pay_detail", "pay_name", stringListToJsonArray(m_pay_name));//));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_set_data(m, "pay_detail", "pay_value", stringListToJsonArray(m_pay_value));//.join("\",\"").prepend("[\"").append("\"]"));

    ///会员情况
    query.execSelect(QString("select a.vch_voucherno, if(a.ch_paymodeno='10', 'Y', 'N') as cash_pay_flag, a.vch_card_name, a.num_card_point, a.num_card_remain, a.vch_card_adress  from  cey_u_checkout_detail a where  ( a.ch_paymodeno = '02' or a.ch_paymodeno = '10' ) and a.ch_payno = '%1' ")
                     .arg(ch_payno));
    if(query.next()) {
        txt_print_bill_set_data(m, "pay_detail_member", "vch_voucherno", query.recordValue("vch_voucherno"));
        txt_print_bill_set_data(m, "pay_detail_member", "vch_card_name", query.recordValue("vch_card_name"));
        txt_print_bill_set_data(m, "pay_detail_member", "num_card_remain", zeroEmpty(m, query.recordValue("num_card_remain").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
        txt_print_bill_set_data(m, "pay_detail_member", "num_card_point", zeroEmpty(m, query.recordValue("num_card_point").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
        txt_print_bill_set_data(m, "pay_detail_member", "vch_card_adress", query.recordValue("vch_card_adress"));
        txt_print_bill_set_data(m, "pay_detail_member", "cash_pay_flag", query.recordValue("cash_pay_flag"));

    }
    ///---end--////////////////////////////////////////
}

void public_printby_ch_billno::txt_print_fill_company_info(lds_model_sqltablemodel *m, const QString &ch_billno)
{
    lds_query_hddpos  query;
    query.execSelect(" SELECT vch_company, vch_tel, vch_address FROM cey_sys_company;  ");
    query.next();

    txt_print_bill_set_data(m, "company_info", "vch_address", query.recordValue("vch_address"));
    txt_print_bill_set_data(m, "company_info", "vch_tel", query.recordValue("vch_tel"));
    txt_print_bill_set_data(m, "company_info", "vch_company", query.recordValue("vch_company"));
}

void public_printby_ch_billno::txt_print_fill_pay_detail_preprint(lds_model_sqltablemodel *m, const QString &ch_billno, const QString &flowid_sql_filter)
{
    QSharedPointer<lds_model_sqltablemodel> paydetail_m =   make_pay_detail_preprint(m, ch_billno, flowid_sql_filter);
    paydetail_m->tablename_kvmap_print();
    double num_cost = paydetail_m->tablename_kvmap_value("pay_detail", "num_cost").toDouble();
    double num_discount = paydetail_m->tablename_kvmap_value("pay_detail", "num_discount").toDouble();
    double num_present = paydetail_m->tablename_kvmap_value("pay_detail", "num_present").toDouble();
    double num_service = paydetail_m->tablename_kvmap_value("pay_detail", "num_service").toDouble();
    double num_rate = paydetail_m->tablename_kvmap_value("pay_detail", "num_rate").toDouble();
    double num_blotout = paydetail_m->tablename_kvmap_value("pay_detail", "num_blotout").toDouble();

    double num_amount = paydetail_m->tablename_kvmap_value("pay_detail", "num_amount").toDouble();
    double num_realamount = paydetail_m->tablename_kvmap_value("pay_detail", "num_realamount").toDouble();

    double tip_num_value_1 = paydetail_m->tablename_kvmap_value("preprint_tip_detail", "tip_num_value_1").toDouble();
    double tip_num_value_2 = paydetail_m->tablename_kvmap_value("preprint_tip_detail", "tip_num_value_2").toDouble();
    double tip_num_value_3 = paydetail_m->tablename_kvmap_value("preprint_tip_detail", "tip_num_value_3").toDouble();


    ///

    txt_print_bill_set_data(m, "pay_detail", "num_cost", "");
    txt_print_bill_set_data(m, "pay_detail", "num_present", "");
    txt_print_bill_set_data(m, "pay_detail", "num_discount", "");
    txt_print_bill_set_data(m, "pay_detail", "num_service", "");
    txt_print_bill_set_data(m, "pay_detail", "num_rate", "");

    txt_print_bill_set_data(m, "pay_detail", "num_lowcost", "");
    txt_print_bill_set_data(m, "pay_detail", "num_free",  "");
    txt_print_bill_set_data(m, "pay_detail", "num_blotout", "");
    txt_print_bill_set_data(m, "pay_detail", "num_amount", "");
    txt_print_bill_set_data(m, "pay_detail", "num_realamount",  zeroEmpty(m,  num_realamount, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

    txt_print_bill_set_data(m, "preprint_tip_detail", "tip_num_value_1", "");
    txt_print_bill_set_data(m, "preprint_tip_detail", "tip_num_value_2", "");
    txt_print_bill_set_data(m, "preprint_tip_detail", "tip_num_value_3", "");
    ///
    //    if(num_service != 0 ||  num_rate != 0) {//服务费 != 0 或 税率 != 0
    txt_print_bill_set_data(m, "pay_detail", "num_cost", zeroEmpty(m, num_cost, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_present", zeroEmpty(m, -1 * num_present, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_discount", zeroEmpty(m, -1 * num_discount, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_service", zeroEmpty(m, num_service, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_rate", zeroEmpty(m, num_rate, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

    txt_print_bill_set_data(m, "pay_detail", "num_lowcost", "");
    txt_print_bill_set_data(m, "pay_detail", "num_free",  "");
    txt_print_bill_set_data(m, "pay_detail", "num_blotout", zeroEmpty(m, -1 * num_blotout, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_set_data(m, "pay_detail", "num_amount",  zeroEmpty(m,  num_amount, cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

    if(cey_sys_parameterValue(m, "tip_enable").toString() == "1") {
        txt_print_bill_set_data(m, "preprint_tip_detail", "tip_num_value_1", tip_num_value_1);
        txt_print_bill_set_data(m, "preprint_tip_detail", "tip_num_value_2", tip_num_value_2);
        txt_print_bill_set_data(m, "preprint_tip_detail", "tip_num_value_3", tip_num_value_3);
    }
    //    }
}

void public_printby_ch_billno::txt_print_bill_set_data(lds_model_sqltablemodel *m, const QString &table_name, const QString &key_name, const QVariant &value)
{
    const lds_model_sqltablemodel_d *d =  m->tablename_kvmap_d("bill_ins_table");
    qrtVariantPairList selects;
    selects << qrtVariantPair("table_name", table_name) << qrtVariantPair("key_name", key_name);
#ifdef QT_DEBUG
    if(d->findValueCount(selects) != 1) {//"pay_detail", "num_lowcost"
        qDebug() << __FILE__ << __LINE__ << selects;
        QObject *o = 0;
        o->children();
    }
#endif
    QString key = d->headerString(d->findValue(selects), Qt::Vertical);

    m->tablename_kvmap_insert_field("bill_ins_table", key, "value", value);
    m->tablename_kvmap_insert_field("bill_ins_table", key, "table_name", table_name);
    m->tablename_kvmap_insert_field("bill_ins_table", key, "key_name", key_name);
}

void public_printby_ch_billno::txt_print_bill_create_field(lds_model_sqltablemodel *m, const QString &key, const QString &table_name, const QString &key_name, const QString &explain, const QVariant &value)
{
    m->tablename_kvmap_insert_field("bill_ins_table", key, "value", value);
    m->tablename_kvmap_insert_field("bill_ins_table", key, "table_name", table_name);
    m->tablename_kvmap_insert_field("bill_ins_table", key, "key_name", key_name);
    m->tablename_kvmap_insert_field("bill_ins_table", key, "explain", explain);

#ifdef QT_DEBUG
    const lds_model_sqltablemodel_d *d =  m->tablename_kvmap_d("bill_ins_table");
    qrtVariantPairList selects;
    selects << qrtVariantPair("table_name", table_name) << qrtVariantPair("key_name", key_name);
    if(d->findValueCount(selects) != 1) {
        qDebug() << __FILE__ << __LINE__ << key_name << d->findValueCount(selects) << selects;
        QObject *o = 0;
        o->children();
    }
#endif
}

void public_printby_ch_billno::txt_print_bill_create_table(lds_model_sqltablemodel *m)
{
    txt_print_bill_create_field(m, "%kts", "sale_head", "dt_opendate", "开台时间", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "dt_operdate").toDateTime().toString(yyyyMMddhhmmss));
    txt_print_bill_create_field(m, "%zdh", "sale_head", "ch_billno", "账单号", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_billno"));

    txt_print_bill_create_field(m, "%fk", "sale_head", "ch_payno", "付款单号", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_payno"));
    txt_print_bill_create_field(m, "%cz", "sale_head", "vch_operid", "操作员ID", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_operid"));
    txt_print_bill_create_field(m, "%cz2", "sale_head", "vch_operator", "操作员", "");//model_operator(m, "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_operid").toString()));
    txt_print_bill_create_field(m, "%cz3", "sale_head", "ch_tableno", "餐桌编码", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "ch_tableno"));
    txt_print_bill_create_field(m, "%cz4", "sale_head", "vch_tablename", "餐桌", "");//model_tablename(m, "");//tablename_kvmap_field("bill_ins_table", "%cz3", "value").toString()));

    txt_print_bill_create_field(m, "%rs", "sale_head", "int_person", "人数", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "int_person"));
    txt_print_bill_create_field(m, "%fw2", "sale_head", "vch_waiter", "服务员ID", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_waiter"));
    txt_print_bill_create_field(m, "%fw", "sale_head", "vch_waitername", "服务员", "");//model_operator(m, "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_waiter").toString()));
    txt_print_bill_create_field(m, "%ls", "sale_head", "serial_no", "流水号", "");//serial_no_cur);
    txt_print_bill_create_field(m, "%sj", "sale_head", "dt_paydate", "支付时间", "");//dt_operdate.toString(yyyyMMddhhmmss));

    txt_print_bill_create_field(m, "%sc", "sale_head", "times", "时长", "");//time_diff);
    txt_print_bill_create_field(m, "%bz", "sale_head", "vch_memo", "备注", "");//tablename_kvmap_value("cey_u_table", ch_billno, "ch_billno", "vch_memo"));

    //waimai
    txt_print_bill_create_field(m, "%wm", "sale_head", "vch_address", "外卖地址", "");//tablename_kvmap_value("cey_u_togo", ch_billno, "ch_billno", "vch_address"));
    txt_print_bill_create_field(m, "%wm2", "sale_head", "vch_linkman", "外卖人", "");//tablename_kvmap_value("cey_u_togo", ch_billno, "ch_billno", "vch_linkman"));
    txt_print_bill_create_field(m, "%wm4", "sale_head", "vch_tel", "外卖电话", "");//tablename_kvmap_value("cey_u_togo", ch_billno, "ch_billno", "vch_tel"));

    txt_print_bill_create_field(m, "%bd", "sale_detail", "rowid", "本单序号", "");//_rowid));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%pm", "sale_detail", "dishname", "品名", "");//_dishname));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%pm2", "sale_detail", "dishname_2", "品名", "");//_dishname));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%sl", "sale_detail", "num","数量",  "");//m_num));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%dw", "sale_detail", "unit", "单位", "");//_unit));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%dj", "sale_detail", "price", "单价", "");//_price));//.join("\",\"").prepend("[\"").append("\"]"));

    txt_print_bill_create_field(m, "%hj", "sale_detail", "total", "合计", "");//_total));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%zk", "sale_detail", "disdetail", "折扣", "");//_disdetail));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%xj", "sale_detail", "amount", "小计", "");//_amount));//.join("\",\"").prepend("[\"").append("\"]"));

    txt_print_bill_create_field(m, "%xf", "pay_detail", "num_cost", "消费", "");
    txt_print_bill_create_field(m, "%zs", "pay_detail", "num_present", "赠送", "");
    txt_print_bill_create_field(m, "%zk2", "pay_detail", "num_discount", "折扣", "");
    txt_print_bill_create_field(m, "%zk3", "pay_detail", "int_discount_off", "折让百分比", "");
    txt_print_bill_create_field(m, "%fw3", "pay_detail", "num_service", "服务费", "");
    txt_print_bill_create_field(m, "%ss", "pay_detail", "num_rate", "税收", "");

    txt_print_bill_create_field(m, "%dx", "pay_detail", "num_lowcost", "低消差", "");//(m, query.recordValue("num_lowcost").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_create_field(m, "%yh", "pay_detail", "num_free", "优惠", "");//(m, -1 * query.recordValue("num_free").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_create_field(m, "%ml", "pay_detail", "num_blotout", "抹零", "");//(m, -1 * query.recordValue("num_blotout").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));

    txt_print_bill_create_field(m, "%sn1", "pay_detail", "tax_name1", "税率1名称", "");
    txt_print_bill_create_field(m, "%sn2", "pay_detail", "tax_name2", "税率2名称", "");
    txt_print_bill_create_field(m, "%sn3", "pay_detail", "tax_name3", "税率3名称", "");

    txt_print_bill_create_field(m, "%fw4", "pay_detail", "pay_fixed_service_value", "固定服务费率", "");
    txt_print_bill_create_field(m, "%ss3", "pay_detail", "pay_fixed_tax_value", "固定税率", "");
    txt_print_bill_create_field(m, "%ss4", "pay_detail", "tax_before", "税收前", "");
    txt_print_bill_create_field(m, "%ss5", "pay_detail", "tax_after", "税收后", "");

    txt_print_bill_create_field(m, "%sr", "pay_detail", "num_fix_tax_total", "总税率", "");//
    txt_print_bill_create_field(m, "%sj1", "pay_detail", "num_cur", "税1", "");
    txt_print_bill_create_field(m, "%sj2", "pay_detail", "num_cur_2", "税2", "");
    txt_print_bill_create_field(m, "%sjt", "pay_detail", "num_cur_total", "总税", "");

    txt_print_bill_create_field(m, "%ys", "pay_detail", "num_realamount", "应收",  "");
    txt_print_bill_create_field(m, "%ss2", "pay_detail", "num_amount", "实收",  "");
    txt_print_bill_create_field(m, "%zl", "pay_detail", "num_change", "找零", "");
    txt_print_bill_create_field(m, "%rj", "pay_detail", "num_avg", "人均", "");

    txt_print_bill_create_field(m, "%fk2", "pay_detail", "pay_name", "付款名称", "");//m_pay_name));//.join("\",\"").prepend("[\"").append("\"]"));
    txt_print_bill_create_field(m, "%fk3", "pay_detail", "pay_value", "付款金额", "");//m_pay_value));//.join("\",\"").prepend("[\"").append("\"]"));

    txt_print_bill_create_field(m, "%bm", "pay_detail_member", "vch_voucherno", "编码", "");//query.recordValue("vch_voucherno"));
    txt_print_bill_create_field(m, "%mc", "pay_detail_member", "vch_card_name", "名称", "");//query.recordValue("vch_card_name"));
    txt_print_bill_create_field(m, "%ys2", "pay_detail_member", "num_card_remain", "余额","");// zeroEmpty(m, query.recordValue("num_card_remain").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_create_field(m, "%jf", "pay_detail_member", "num_card_point", "积分", "");//zeroEmpty(m, query.recordValue("num_card_point").toDouble(), cey_sys_parameterValue(m, "bp_fmt_amount_dec").toInt()));
    txt_print_bill_create_field(m, "%dz2", "pay_detail_member", "vch_card_adress", "会员地址", "");
    txt_print_bill_create_field(m, "%yh2", "pay_detail_member", "cash_pay_flag", "是否云会员", "");

    txt_print_bill_create_field(m, "%dz", "company_info", "vch_address", "公司地址", "");//query.recordValue("vch_address"));
    txt_print_bill_create_field(m, "%dh", "company_info", "vch_tel", "公司电话", "");//query.recordValue("vch_tel"));
    txt_print_bill_create_field(m, "%gs", "company_info", "vch_company", "公司名称", "");//query.recordValue("vch_company"));

    txt_print_bill_create_field(m, "%xf1", "preprint_tip_detail", "tip_num_value_1", "指导小费", "");
    txt_print_bill_create_field(m, "%xf2", "preprint_tip_detail", "tip_num_value_2", "指导小费", "");
    txt_print_bill_create_field(m, "%xf3", "preprint_tip_detail", "tip_num_value_3", "指导小费", "");
}

QString public_printby_ch_billno::model_operator(lds_model_sqltablemodel *m, const QString &operid)
{
    if("0000" == operid) {
        return "system";
    }
    return m->tablename_kvmap_value("cey_sys_operator", operid, "vch_operID", "vch_operator").toString();
}

QString public_printby_ch_billno::model_tablename(lds_model_sqltablemodel *m, const QString &ch_tableno)
{
    if("Q000" == ch_tableno)
        return ch_tableno;
    return m->tablename_kvmap_value("cey_bt_table", ch_tableno, "ch_tableno", "vch_tablename").toString();
}

QString public_printby_ch_billno::septNumberCurrency(lds_model_sqltablemodel *m, double value, int decimal, bool fillCurrency)
{
    return (fillCurrency ? cey_sys_parameterValue(m, "pay_unit_prefix").toString() : "" ) + lds:: septNumber(value, decimal);
}

QVariant public_printby_ch_billno::cey_sys_parameterValue(const lds_model_sqltablemodel *m, const QString &parameter, const QVariant &def)
{
    return m->tablename_kvmap_value("cey_sys_parameter", parameter, "vch_parameter", "vch_value", "", def);
}

QString public_printby_ch_billno::zeroEmpty(lds_model_sqltablemodel *m, double value, int decimal, bool fillCurrency)
{
    if(value == 0)
        return "";
    return  septNumberCurrency(m, value, decimal, fillCurrency);
}

QString public_printby_ch_billno::interSelectSql(lds_model_sqltablemodel *m, bool pay_exit, const QString &and_sql_filter)
{
    QString dishname;
    QString dishname_2;
    dishname_split(cey_sys_parameterValue(m, "bp_vch_dishname_No", "vch_dishname").toString(), dishname, dishname_2, "b");//cey_bt_dish b
    QString num_num = pay_exit ? "0" : "a.num_num";// " + num_num + "
    QString and_dish_num_filter = pay_exit ? " and a.num_back > 0 " : " and (a.num_num - a.num_back) <> 0 ";

    return QString() +
            " SELECT "
            "   (select " + dishname + " from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as dishname "
            ",  (select c.vch_unitname from cey_bt_unit c, cey_bt_dish b where c.ch_unitno = b.ch_unitno and a.ch_dishno = b.ch_dishno) as unit "
            ",  (" + num_num + " - a.num_back) as num"//sum
            ",  a.num_price as price"
            ",  ( if(ch_presentflag = 'Y' ,   0 ,   (" + num_num + " - a.num_back)*a.num_price*0.01*a.int_discount )   ) as amount"//sum
            ",  (a.num_price*(" + num_num + " - a.num_back)*(1-a.int_discount*0.01) ) as disdetail"//sum
            ",  vch_print_memo"
            ",  (select vch_reason from cey_u_orderdish_log aa where aa.int_orderflow = a.int_flowID limit 0, 1) as vch_reason"

            ",  (select b.ch_dish_typeno from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as ch_dish_typeno "
            ",  (select b.ch_sub_typeno from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as ch_sub_typeno "

            ",  a.ch_suitflag"
            ",  a.int_flowID"
            ",  a.ch_specialflag"
            ",  a.ch_presentflag"
            ",  a.ch_waidai_flag"
            ",  a.ch_addon_flag"
            ",  a.ch_dishno"
            ",  a.int_discount"

            " , a.num_price_org"
            " , a.num_price_add"
            "  , (select " + dishname_2 + " from cey_bt_dish b where a.ch_dishno = b.ch_dishno) as dishname_2 "
            "   FROM cey_u_orderdish a"
            "  where a.ch_billno = '%1' "+
            and_dish_num_filter +
            and_sql_filter
            ;
}

void public_printby_ch_billno::dishname_split(const QString &dishname_show, QString &dishname, QString &dishname_2)
{
    const QStringList sql_field = QStringList() << "vch_dishname" << "vch_dishname_2";
    dishname = dishname_show.split(",").value(0);
    dishname_2 = dishname_show.split(",").value(1);

    if(sql_field.contains(dishname))
        dishname = dishname;
    else
        dishname = "";

    if(sql_field.contains(dishname_2))
        dishname_2 = dishname_2;
    else
        dishname_2 = "";
}

void public_printby_ch_billno::dishname_split(const QString &dishname_show, QString &dishname, QString &dishname_2, const QString &tablename)
{
    dishname_split(dishname_show, dishname, dishname_2);

    if(dishname != "")
        dishname = tablename + "." + dishname;
    else
        dishname = " '' ";

    if(dishname_2 != "")
        dishname_2 = tablename + "." + dishname_2;
    else
        dishname_2 = " '' ";
}

void public_printby_ch_billno::payPrint(const QString &bp_ch_billno, const QString &bp_languageKey, const QStringList &label_insert_int_flowid_list, int pay_print_mode, const QImage &image)
{
    QString bp_path = lds::sysconf->value("system_setting/printerpath").toString();
    int bp_count = n_func::f_get_sysparm("bp_count", "1").toInt();
    public_printby_ch_billno::payPrintModes bp_m = public_printby_ch_billno::payPrintModes(pay_print_mode);
    int bp_fmt_tail_block = n_func::f_get_sysparm("bp_fmt_tail_block", "0").toInt();
    QString errstring;
    //
    if(bp_m & public_printby_ch_billno::payDrawer) {
        while(1) {
            if(false == public_printby_ch_billno::openDrawer(&errstring)) {
                backheader::sendError(errstring, "PRINT_ERROR");
                break;
            }
            //防止钱箱太快，影响小票
            lds_thread::msecSleep(lds::sysconf->value("system_setting/drawer_timeout", 1000).toInt());
            break;
        }//while
    }

    if( (bp_m & public_printby_ch_billno::payNormal)
            || (bp_m & public_printby_ch_billno::payRefund) ) {
        QSharedPointer<Printer_POS> printer = Printer_POS::get_pos_printer_by_path(bp_path);
        forever {
            //begin
            if(printer.isNull()) {
                backheader::sendError("PRINT_IS_NULL", "PRINT_ERROR");
                break;
            }
            if(false == printer->tryOpen(bp_path)) {
                backheader::sendError(printer->lastError(), "PRINT_ERROR");
                break;
            }
            QByteArray pHex;
            Bmp_info::pic2Ins(image, pHex);//83058
            int print_wait_time = pHex.count() / 40000;
            print_wait_time = qMax(print_wait_time, 1);
            print_wait_time = qMin(print_wait_time, 3);
            for(int k = 0; k < bp_count; k ++) {
                printer->writedirect(pHex);
                printer->writedirect(QString(bp_fmt_tail_block, '\n').toLocal8Bit());
                printer->writeHex("1B6405 1D5601");//走纸5x， 切纸
                if(printer->isNetPrinter())
                    printer->waitForReadyReadv(print_wait_time * 1000);
                else
                    lds_thread::msecSleep(print_wait_time * 1000);
            }
            //~begin
            break;
        }//forever
    }
    if(bp_m & public_printby_ch_billno::payLabel) {
        public_printby_ch_billno::print_label(bp_ch_billno, label_insert_int_flowid_list);
    }
}

bool public_printby_ch_billno::openDrawer(QString *errstring)
{
    blockIns ins;
    ins += pieceIns(lds::sysconf->value("system_setting/qianxiangzhiling").toByteArray(), "");
    QString printerpath = lds::sysconf->value("system_setting/printerpath").toString();
    return Printer_Normal::toprint(printerpath, ins, 1, errstring);
}

QString public_printby_ch_billno::stringListToJsonArray(const QStringList &list)
{
    if(list.count() == 0)
        return "[]";
    QString value;
    foreach(const QString &str, list) {
        value += "\"" + str + "\",";
    }
    value.chop(1);
    return "[" + value + "]";
}

QString public_printby_ch_billno::pay_fixed_service_suffix()
{
    QString pay_fixed_service_value = model_bp_fmt->tablename_kvmap_value("cey_sys_parameter", "pay_fixed_service_value", "vch_parameter", "vch_value").toString();
    QString pay_fixed_service = model_bp_fmt->tablename_kvmap_value("cey_sys_parameter", "pay_fixed_service", "vch_parameter", "vch_value").toString();

    return (pay_fixed_service == "1" ? QString("(%1%)").arg(pay_fixed_service_value) : "");
}

QString public_printby_ch_billno::pay_fixed_tax_suffix()
{
    QString pay_fixed_tax = model_bp_fmt->tablename_kvmap_value("cey_sys_parameter", "pay_fixed_tax", "vch_parameter", "vch_value").toString();
    QString pay_fixed_tax_value = model_bp_fmt->tablename_kvmap_value("cey_sys_parameter", "pay_fixed_tax_value", "vch_parameter", "vch_value").toString();

    return (pay_fixed_tax == "1" ? QString("(%1%)").arg(pay_fixed_tax_value) : "");
}

void public_printby_ch_billno::model_bp_fmt_append_query(const QString &tablename, lds_query_hddpos  &query)
{
    int count = query.recordCount();
    QString key = QString::number(model_bp_fmt->tablename_kvmap_count(tablename));
    for(int k = 0; k < count; k++)
        model_bp_fmt->tablename_kvmap_insert_field(tablename, key, query.record().fieldName(k), query.recordValue(k));
}

void public_printby_ch_billno::model_bp_fmt_append_sale_detail(lds_query_hddpos  &query)
{
    QString tablename = "sale_detail";
    int count = query.recordCount();
    QString key = QString::number(model_bp_fmt->tablename_kvmap_count(tablename));
    QString field_name;
    QVariant field_value;
    for(int k = 0; k < count; k++) {
        field_name = query.record().fieldName(k);
        field_value = query.recordValue(k);
        if(field_name == "vch_print_memo") {
            field_value = vch_print_memoData(field_value.toString()).jsonToString();
        }
        model_bp_fmt->tablename_kvmap_insert_field(tablename, key, field_name, field_value);
    }
}

void public_printby_ch_billno::model_bp_fmt_fill_receipt_tail()
{
    if(model_bp_fmt_value("bp_fmt_underwrite") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("receipt_tail", "signature",  print_format.printTr(QT_TR_NOOP("签名")) + ":");
    }
    //票尾语
    QString bp_fmt_end_desc = model_bp_fmt_value("bp_fmt_end_desc");
    if(bp_fmt_end_desc.length() > 0) {
        model_bp_fmt->tablename_kvmap_insert_value("receipt_tail", "bottom",  bp_fmt_end_desc);
    }
    //一维码
    if(model_bp_fmt_value("bp_fmt_end_bc_en") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("receipt_tail", "bar_code",  model_bp_fmt_value("bp_fmt_end_bc"));
    }
    //二维码
    if(model_bp_fmt_value("bp_fmt_end_rc_en") == "1") {
        model_bp_fmt->tablename_kvmap_insert_value("receipt_tail", "qr_code",  model_bp_fmt_value("bp_fmt_end_rc"));
    }
}

void public_printby_ch_billno::print_format_init_path(const QString &path)
{
    print_format.print_path = path;
    paperwidth = model_bp_fmt->tablename_kvmap_value("cey_bt_kitchen_print", print_format.print_path, "vch_printip", "int_paperwidth").toInt();
    if(paperwidth == 0) {
        qDebug("cey_bt_kitchen_print not exist int_paperwidth, please check !");
        paperwidth = 32;
    }
    print_format.paperwidth = paperwidth;
}

bool public_printby_ch_billno::emptyOrN(const QString &d)
{
    return d.count() == 0 || d == "N";
}
