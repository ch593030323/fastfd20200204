#include "fexpandmain_2.h"
#include <QTableView>
#include <QtDebug>
#include <QHeaderView>
#include <QPainter>
#include "public_sql.h"
#include <QDateTime>
#include "n_func.h"
#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "ui_fexpandmain_payover.h"
#include "w_bt_dish.h"
#include <QSqlError>
#include "w_bt_dish_base.h"
#include <QScrollBar>
#include "w_scr_dish_dish_change.h"
#include "w_scr_dish_pay_widget.h"
#include "uo_rw_member.h"
#include "w_m_member_list.h"
#include "w_scr_dish_dish_quit.h"
#include "take_cashier.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "w_scr_dish_num_cook_dialog.h"
#include "w_scr_dish_group_dialog.h"
#include "ui_w_bt_dish_new_dialog_2.h"
#include "w_bt_dish_base.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "ftableview_standmodel_sql_table.h"
#include <QMenu>
#include "w_scr_dish_main_bar_cancel.h"
#include "w_scr_dish_main_pay_info.h"
#include "restauranttableview.h"
#include "w_scr_dish_main_bar_cancel_win.h"
#include "lds_scr_qt.h"
#include "w_m_member_list.h"
#include "w_m_member.h"
#include "w_m_member_list_yun_card_info.h"
#include "w_bt_dish_back.h"


/**
关于套菜数量修改的方案
1.主商品P， 子套菜Y
2.P的数量和Y的数量独立，例如P:0.5, Y:1,2,5
3.换商品时，分：可替换菜【不会改变P的价格】、非可替换商品【改变P的价格】
4.加做法时，做法加价会加到P里
5.P允许修改数量的条件：旗下Y没有做法、旗下Y没有换菜、没有折扣、没有落单、价格等于原价、非特价、非赠送
*/


void fexpandmain_2::showPayinfo(QWidget *tableview, w_scr_dish_main_pay_info *pay_info_widget, const QString &ch_payno, int pay_type, bool ispromotion, const QString &discount_desc)
{
    //2
    lds_query_hddpos  query;
    QString title;
    switch(pay_type) {
    case w_scr_dish_pay_widget::OPERAT_PAY:
        title = QObject::tr("结算成功");
        break;
    case w_scr_dish_pay_widget::OPERAT_QUITBILL:
        title = QObject::tr("退货");
        break;
    case w_scr_dish_pay_widget::OPERAT_WAIMAI:
        title = QObject::tr("外卖结算");
        break;
    }

    lds::DOUBLE_DATA ddata;
    ///跳过优惠、自动抹零
    query.execSelect(QString("select ifnull(sum(num_realamount), 0), ifnull(sum(num_payamount), 0), ifnull(sum(num_change), 0), count(0) as count  from cey_u_checkout_detail "
                             " where ch_payno = '%1' and ch_paymodeno <> 'ZZ'  and ch_paymodeno <> 'YY' ")
                     .arg(ch_payno));
    query.next();
    if(query.recordValue("count").toInt() > 0) {
        ddata.udpKey = lds::udp_pay_request;
        ddata.p_desc = title;
        ddata.p_pay_desc = pay_info_widget->ui->label->text();
        ddata.p_pay = lds::septNumber(query.recordValue(0).toDouble());
        ddata.p_receive_desc = pay_info_widget->ui->label_2->text();
        ddata.p_receive = lds::septNumber(query.recordValue(1).toDouble());
        ddata.p_change_desc = pay_info_widget->ui->label_3->text();
        ddata.p_change = lds::septNumber(query.recordValue(2).toDouble());
        public_sql::doublescreensenddataByconf(ddata);
    }
    //w_scr_dish_main_pay_info
    pay_info_widget->ui->label_desc->setText(ddata.p_desc);
    pay_info_widget->ui->label_num_payamount->setText(ddata.p_pay);
    pay_info_widget->ui->label_num_realamount->setText(ddata.p_receive);
    pay_info_widget->ui->label_num_change->setText(ddata.p_change);
    //
    if(ispromotion) {
        pay_info_widget->ui->label_promotion->setText(QObject::tr("促销对象为会员,请先读卡"));
        lds::showWidget(pay_info_widget->ui->label_promotion);
    } else  {
        pay_info_widget->ui->label_promotion->setText("");
        lds::hideWidget(pay_info_widget->ui->label_promotion);
    }
    //
    if(discount_desc.length() > 0) {
        pay_info_widget->ui->label_discount->setText(discount_desc);
        lds::showWidget(pay_info_widget->ui->label_discount);
    } else {
        pay_info_widget->ui->label_discount->setText("");
        lds::hideWidget(pay_info_widget->ui->label_discount);
    }
    //
    lds::showWidget(pay_info_widget->ui->label_desc, ddata.p_desc.length() > 0);
    lds::showWidget(pay_info_widget->ui->label_num_payamount, ddata.p_pay.length() > 0);lds::showWidget(pay_info_widget->ui->label, lds::widgetIsShow(pay_info_widget->ui->label_num_payamount));
    lds::showWidget(pay_info_widget->ui->label_num_realamount, ddata.p_pay.length() > 0);lds::showWidget(pay_info_widget->ui->label_2, lds::widgetIsShow(pay_info_widget->ui->label_num_realamount));
    lds::showWidget(pay_info_widget->ui->label_num_change, ddata.p_pay.length() > 0);lds::showWidget(pay_info_widget->ui->label_3, lds::widgetIsShow(pay_info_widget->ui->label_num_change));
    lds::hideWidget(pay_info_widget->ui->frame, lds::widgetIsHide(pay_info_widget->ui->label_num_payamount) && lds::widgetIsHide(pay_info_widget->ui->label_num_realamount) && lds::widgetIsHide(pay_info_widget->ui->label_num_change) );
    lds::hideWidget(pay_info_widget,
                    lds::widgetIsHide(pay_info_widget->ui->label_desc)
                    && lds::widgetIsHide(pay_info_widget->ui->label_promotion)
                    && lds::widgetIsHide(pay_info_widget->ui->label_discount)
                    );

    int width = qMin(tableview->height(), tableview->width())*5/6;
    pay_info_widget->resize(width, pay_info_widget->sizeHint().height());
    pay_info_widget->move(lds::getParentCenterPopup(tableview, pay_info_widget));
    pay_info_widget->raise();
}

fexpandmain_2::fexpandmain_2(QObject *parent) :
    frontViewDialog_virtual(parent)
{
}

fexpandmain_2::~fexpandmain_2()
{
}



void fexpandmain_2::updateDishImage(QWidget *parent, const QModelIndex &index, ftableview_standmodel_virtual *model_dish, const QString &ch_sub_typeno, const QString &ch_groupno)
{
    lds_query_hddpos  query;
    if(index.isValid()) {//点击非商品出
        if(index.data(Qt::UserRole + 1).toString() == "-3") goto tonew;

        if((model_dish->rowCount() - 1) == index.row() && (model_dish->columnCount() - 1) == index.column()) {
            switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("商品操作"),  QObject::tr("新增"), QObject::tr("修改"), QObject::tr("取消"))) {
            case 0:
                goto tonew;
                return;
            case 1:
                goto tochange;
                return;
            case 2:
                return;
            }
        }
        goto tochange;
    }
    goto tonew;
    return;
tochange:
    {
        QString ch_dishno = index.data(Qt::UserRole+1).toString();
        w_bt_dish_base_change_by_ch_dishno dialog(parent, ch_dishno);
        if(ch_groupno.length() > 0)
            dialog.ui->lineEdit_ch_groupno->setEnabled(false);
        transparentCenterDialog(&dialog, false).exec();
        query.execSelect(QString("SELECT ch_dishno, CONCAT(vch_dishname,'\n',num_price) from cey_bt_dish where ch_stopflag <> 'Y' and ch_tempflag <> 'Y' and ch_dishno = '%1' ")
                         .arg(ch_dishno));
        if(query.next()) {
            model_dish->setData(index, query.recordValue(1), Qt::UserRole);
            QPixmap pixmap(model_dish->imagedishdir+"/"+ch_dishno);
            if(pixmap.isNull()) {
                model_dish->setData(index, ch_dishno, Qt::UserRole+2);
            } else {
                model_dish->setData(index, pixmap, Qt::UserRole+2);
            }
        } else {
            qDebug("not ch_dishno match filter!");
        }
        model_dish->refreshcur();
        return;
    }
tonew:
    {
        updateDishImage_add(parent, model_dish, ch_sub_typeno, ch_groupno);
        model_dish->refreshcur();
    }
}

void fexpandmain_2::updateDishImage_add(QWidget *parent, ftableview_standmodel_virtual *model_dish, const QString &ch_sub_typeno, const QString &ch_groupno)
{
    lds_query_hddpos  query;
    QString ch_dish_typeno;
    query.execSelect(QString("select ch_dish_typeno FROM hddpos.cey_bt_dish_subtype where ch_sub_typeno = '%1' ")
                     .arg(ch_sub_typeno));
    query.next();
    ch_dish_typeno = query.recordValue("ch_dish_typeno").toString();

    w_bt_dish_base_new dialog(parent, ch_dish_typeno, ch_sub_typeno, ch_groupno, "");
    dialog.setWindowTitle(QObject::tr("新增"));
    transparentCenterDialog(&dialog, false).exec();
    model_dish->refreshcur();
}

//-1nothing happen 0clear 1read
fexpandmain_2::ReadMemType fexpandmain_2::m_read_exec(QWidget *parent, fexpandmain_model_sqltablemodel_data *tablemodel, const QString &localMemberNo)
{
    bool reReadCard = false;
    QString mtitle;
    QString errstring;
    w_m_member_list_yun_auto_close d;

    if(tablemodel->vch_membernod.vch_memberno.length() > 0) {
        switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("已读会员卡") + "\n" + tablemodel->vch_membernod.desc(), QObject::tr("清除读卡信息"),  QObject::tr("重新读卡"), QObject::tr("取消"))) {
        case 0://清除读卡信息
            tablemodel->vch_membernod.init();
            tablemodel->vch_membernod_m_update(errstring);
            return cleared;
        case 1://重新读卡
            tablemodel->vch_membernod.init();
            tablemodel->vch_membernod_m_update(errstring);
            reReadCard = true;
            break;
        default://取消
            return rejected;
        }
    }
    errstring = "";
    //读卡
    uo_rw_member uo_member;
    QString cardno;
    n_func::READCARDINFO cardinfo;
    if(localMemberNo.isEmpty()) {
        if(!w_m_member_list::static_to_memeber_readcard(parent,cardno, uo_member, true, true)) {//读卡未完成
            goto end;
        }
    } else {
        cardno = localMemberNo;
    }
    //判断卡是否存在
    if(cardno.isEmpty()) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("无可查询的卡"));
        goto end;
    }
    //读卡信息
    if(!n_func::qtcardinfo_readcard(cardinfo, cardno)) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID,cardinfo.errstring);
        //新增卡
        w_m_member_list_yun_card_info dialog(parent);
        if(false == dialog.initData(cardno, errstring)) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
            goto end;
        }
        if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
            goto end;
        }
        //重新读卡
        if(false == n_func::qtcardinfo_readcard(cardinfo, cardno)) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, cardinfo.errstring);
            goto end;
        }
    }
    //判断密码
    if(!cardinfo.vch_password.isEmpty()) {
        lds_dialog_input inputdialog(parent);
        inputdialog.hideKey();
        inputdialog.ui->label->setText(QObject::tr("密码"));
        inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
        inputdialog.setWindowTitle(QObject::tr("请输入密钥"));
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            goto end;//取消
        }
        if(cardinfo.vch_password != inputdialog.ui->lineEdit->text()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            goto end;
        }
    }
    //判断卡的状态
    if(cardinfo.vch_memberno.isEmpty()) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("此卡未发卡或不存在"));
        goto end;
    }
    if(cardinfo.ch_state == "4") {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("此卡已经作废"));
        goto end;
    }
    if(cardinfo.ch_state == "3") {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("此卡已经挂失"));
        goto end;
    }
    if(cardinfo.ch_state == "2") {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("此卡已经止付"));
        goto end;
    }
    if(cardinfo.ch_state.isEmpty()) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("非本公司卡或无效卡"));
        goto end;
    }
    if(cardinfo.dt_limit <= n_func::f_get_sysdatetime()) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("此卡已经超过有效期"));
        goto end;
    }
    if(cardinfo.ch_cardflag != "Y") {
        if(1 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("会员卡")+cardinfo.vch_memberno+QObject::tr("还未发卡"), QObject::tr("发卡"), QObject::tr("取消"))) {
            goto end;//取消发卡
        }

        w_m_member_list_yun_card_info dialog(parent);
        if(false == dialog.initData(cardno, errstring)) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, errstring);
            goto end;
        }
        if(QDialog::Accepted != transparentCenterDialog(&dialog).exec()) {
            goto end;
        }
        //重新读卡
        n_func::qtcardinfo_readcard(cardinfo, cardno);
    }
    //
    if(w_m_member_list::is_yun_member)
        mtitle = QObject::tr("云会员") + cardinfo.vch_memberno;
    else
        mtitle = QObject::tr("会员卡") + cardinfo.vch_memberno;

    //根据会员类别信息，更新信息--begin
    switch(cardinfo.ch_favourable.toInt()) {
    case 0://无优惠
        break;
    case 1://会员价
        if(cardinfo.ch_pricekind.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+ QObject::tr("没有设置会员价"));
            goto end;
        }
        if(1 == lds_messagebox::information(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+  QObject::tr("当前读卡信息") +":" + QObject::tr("会员特价"), QObject::tr("确定"), QObject::tr("取消"))) {
            goto end;
        }
        break;
    case 2://固定折扣
        if(cardinfo.int_discount.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+ QObject::tr("没有设置会员折扣"));
            goto end;
        }
        if(1 == lds_messagebox::information(parent, MESSAGE_TITLE_VOID, mtitle + "\n"+  QObject::tr("当前读卡信息") + ":" + QObject::tr("折扣(%1)").arg(cardinfo.int_discount), QObject::tr("确定"), QObject::tr("取消"))) {
            goto end;
        }
        break;
    default://没有找到该会员优惠信息
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("没有找到该会员优惠信息"));
        goto end;
        break;
    }

    tablemodel->vch_membernod.init0(cardinfo.vch_memberno,
                                    cardinfo.vch_member,
                                    cardinfo.ch_member_belong_state,
                                    cardinfo.ch_favourable,
                                    cardinfo.int_discount.toInt(),
                                    cardinfo.ch_pricekind,
                                    cardinfo.ch_attribute,
                                    cardinfo.vch_typename
                                    );
    tablemodel->vch_membernod_m_update(errstring);
    return readed;
end:
    if(reReadCard)
        return cleared;//重新读卡但是没有读到卡
    //nothing happen
    return rejected;
}

QString fexpandmain_2::get_sub_vch_print_memo(const QString &org, const QString &sub)
{
    QString org0 = org;
    org0.remove(org0.indexOf(sub), sub.length());
    return org0;
}

bool fexpandmain_2::guqing_check(QString &msg, const QString &ch_dishno, double off)
{
    double num_guqing_num = 0;

    off = qRound(100*off)/100.0;
    lds_query_hddpos  query;

    //商品估清清除
    guqing_try_clear();

    query.execSelect(QString("SELECT num_guqing_num, vch_dishname from cey_bt_dish where ch_dishno = '%1' and ifnull(ch_guqing_flag, '') = 'Y'  ")
                     .arg(ch_dishno));
    if(query.next()) {
        num_guqing_num = query.recordValue("num_guqing_num").toDouble();
        bool num_guqing_flag = (off) > num_guqing_num;
        if(num_guqing_flag) {
            msg +=  QString("%1\t").arg(QObject::tr("估清不足")).arg(num_guqing_num-off);
        } else {
            msg +=  QString("%1:%2\t").arg(QObject::tr("估清剩余")).arg(num_guqing_num-off);
        }
        msg += query.recordValue("vch_dishname").toString() + "\n";

        if(num_guqing_flag) return false;
    }
    return  true;
}

void fexpandmain_2::guqing_try_clear()
{
    lds_query_hddpos  query;
    QString sql_dt_yyyyMMdd = n_func::f_get_sysdatetime().toString("yyyy-MM-dd");
    QString guqing_clear_day = n_func::f_get_sysparm("guqing_clear_day", "");

    //第一次进入
    if(guqing_clear_day.isEmpty()) {
        guqing_clear_day =  sql_dt_yyyyMMdd;
        n_func::f_set_sysparm("guqing_clear_day", sql_dt_yyyyMMdd);
    }
    if(guqing_clear_day != sql_dt_yyyyMMdd) {
        query.execUpdate("cey_bt_dish", "ch_guqing_flag", "N", "num_guqing_num", 0,  "");//初始化商品估清
        n_func::f_set_sysparm("guqing_clear_day", sql_dt_yyyyMMdd);
    }
}

bool fexpandmain_2::guqing_update(const QString &ch_dishno, double off)
{
    return lds_scr_qt::guqing_update(ch_dishno, off);
}

//若rowP是主商品，则依次替换子套菜
//若rowp是子套菜，则子套菜本来不能修改数量
bool fexpandmain_2::guqing_try_check_P(lds_model_sqltablemodel *tablemodel, QString &msg, int rowP, double off)
{
    if(off == 0) return true;//0表示没有改变

    double dish_num = tablemodel->model_data_NIBOLAN(rowP, "num_num-num_back");
    QString ch_dishno = tablemodel->model_data(rowP, "ch_dishno").toString();

    if(dish_num == 0) {
        qDebug() << __FILE__<<__LINE__ << "dish_num is 0";
    }

    bool f = true;

    double off_rate_P_new_old = off / dish_num;
    if(false == guqing_check(msg, ch_dishno, off)) {
        f = false;
    }

    for(int row = rowP+1; row < tablemodel->rowCount(); row++) {
        if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {
            ch_dishno = tablemodel->model_data(row, "ch_dishno").toString();
            dish_num = tablemodel->model_data_NIBOLAN(row, "num_num-num_back");
            if(!guqing_check(msg, ch_dishno, dish_num * off_rate_P_new_old)) {
                f = false;
            }
            continue;
        }
        break;
    }
    return f;
}

bool fexpandmain_2::material_insert(lds_query_hddpos  &query, const QString &ch_billno, int int_id)
{
    return lds_scr_qt::material_insert(query, ch_billno, int_id);
}

bool fexpandmain_2::cancelTable_transaction(const QString &ch_tableno, int sqr, QString &errstring)
{
    if(false == cancelTableCheck(ch_tableno, sqr, errstring)) {
        return false;
    }
    //
    lds_query_hddpos  query;
    lds_query_hddpos::tran_saction();
    if(cancelTable(ch_tableno, sqr, errstring)) {
        lds_query_hddpos::com_mit();
        return true;
    }
    lds_query_hddpos::roll_back();
    errstring = query.recordError();
    return false;
}

void fexpandmain_2::wineTake(QWidget *parent)
{
    lds_query_hddpos  query;
    QString vch_guest;
    QString vch_handle;
    int ret;
    switch(lds_messagebox::question(parent, MESSAGE_TITLE_VOID, QObject::tr("存取酒"), QObject::tr("存酒"), QObject::tr("取酒"), QObject::tr("取消"))) {
    case 0:
    {
        //vch_handle
        lds_dialog_input inputdialog(parent);
        inputdialog.setWindowTitle(QObject::tr("录入手机号"));
        inputdialog.ui->label->setText(QObject::tr("手机号"));
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            return;
        }
        vch_handle = inputdialog.ui->lineEdit->text().trimmed();
        if(vch_handle.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("手机号不能为空"));
            return;
        }
        //vch_guest
        query.execSelect(QString("SELECT  vch_guest from cey_bt_table_bar_wine  where vch_handle = '%1' limit 1")
                         .arg(vch_handle));
        if(query.next()) {
            vch_guest = query.recordValue("vch_guest").toString();
        } else {
            if(1 == lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("没找到该手机记录"), QObject::tr("新增"), QObject::tr("取消"))) {
                return;
            }
            inputdialog.setWindowTitle(QObject::tr("录入姓名"));
            inputdialog.ui->label->setText(QObject::tr("姓名"));
            inputdialog.ui->lineEdit->clear();
            inputdialog.delay_set_focus();

            while(QDialog::Accepted == (ret = transparentCenterDialog(&inputdialog).exec())) {
                vch_guest = inputdialog.ui->lineEdit->text().trimmed();
                if(vch_guest.isEmpty()) {
                    lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("姓名为空"));
                    inputdialog.delay_set_focus();
                    continue;
                }
                break;
            }
            if(QDialog::Accepted != ret) {
                return;
            }
        }
        //w_scr_dish_main_bar_cancel_win_pend
        w_scr_dish_main_bar_cancel_win_pend dialog(vch_handle, vch_guest, parent);
        transparentCenterDialog(&dialog).exec();
    }
        break;
    case 1:
    {
        //vch_handle
        lds_dialog_input inputdialog(parent);
        inputdialog.setWindowTitle(QObject::tr("录入手机号"));
        inputdialog.ui->label->setText(QObject::tr("手机号"));
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            return;
        }
        vch_handle = inputdialog.ui->lineEdit->text().trimmed();
        if(vch_handle.isEmpty()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("手机号不能为空"));
            inputdialog.delay_set_focus();
            return;
        }
        query.execSelect(QString("SELECT  vch_guest from cey_bt_table_bar_wine  where vch_handle = '%1' limit 1")
                         .arg(vch_handle));
        if(!query.next()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("手机号不存在"));
            inputdialog.delay_set_focus();
            return;
        }

        //w_scr_dish_main_bar_cancel_win_take
        w_scr_dish_main_bar_cancel_win_take dialog(vch_handle, parent);
        transparentCenterDialog(&dialog).exec();

    }
        break;
    }
}

bool fexpandmain_2::cancelTable(const QString &ch_tableno, int sqr, QString &errstring)
{
    //
    QString ch_billno;
    QString div = QString::number(sqr);
    if(sqr == 0) {
        div = "";
    } else {
        div = "_"+div;
    }
    lds_query_hddpos  query;
    if(query.execUpdate("cey_bt_table", "ch_billno"+div, "", qrtEqual("ch_tableno", ch_tableno))) {
        return true;
    }

    //    query.execSelect(QString("SELECT ch_billno%1 from cey_bt_table where ch_tableno = '%2' ")
    //                     .arg(div)
    //                     .arg(ch_tableno));
    //    query.next();
    //    ch_billno = query.recordValue(0).toString();
    //
    //    if(query.execUpdate("cey_u_table", "ch_order_state", 3,  QString("ch_billno = '%1' ").arg(ch_billno))) {
    //        return true;
    //    }
    errstring = query.recordError();
    return false;
}

bool fexpandmain_2::cancelTableCheck(const QString &ch_tableno, int sqr, QString &errstring)
{
    if(lds_scr_table::cey_bt_table_has_locked(ch_tableno, sqr)) {
        errstring = QObject::tr("该餐桌已经被锁定");
        return false;
    }
    //
    QString ch_billno = lds_scr_table::cey_bt_table_ch_billno_get(ch_tableno, sqr);
    if(lds_scr_table::cey_u_orderdish_dish_existed(ch_billno)) {
        errstring = QObject::tr("当前桌位存在商品消费");
        return false;
    }
    return true;
}


