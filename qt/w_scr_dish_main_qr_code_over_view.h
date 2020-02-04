#ifndef W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H
#define W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H

#include <QDialog>
#include "lds_model_sqlstandardmodel.h"
#include "w_scr_dish_main_qr_code.h"
#include <QPushButton>
#include "lds_tableview_delegate_virtual.h"
#include "lds_toolbar_button.h"
#include <QMovie>
#include <QUdpSocket>

namespace Ui {
class w_scr_dish_main_qr_code_over_view;
}

class w_scr_dish_main_qr_code_over_view : public QDialog
{
    Q_OBJECT
public:
    class PushButton : public QPushButton {
    public:
        PushButton(QWidget *parent = 0 );
        int row;
        w_scr_dish_main_qr_code::QrCodeMaster master;
        QrCodeDetailList details;

        //退货
        QString dish_sn;
        lds_tableView *dish_table;
        lds_model_sqlstandardmodel *dish_model;
    };
    class delegate_dish_detail : public lds_tableview_delegate_virtual
    {
    public:
        delegate_dish_detail(QObject *parent = 0);
        virtual void drawText(QPainter *painter,
                              QStyle::State state,
                              const QRect &painter_text_rect, Qt::Alignment displayAlignment, const QString &painter_text) const;

    };
    enum operaType{
        operaDetail,
        operaQuit,
        operaAdd,
        operaDetailYunAdd
    };
public:
    //billType 不为空时，取消自动刷新和接单功能，只显示billType对应的账单类型
    w_scr_dish_main_qr_code_over_view(const QString &billType , QWidget *parent);
    w_scr_dish_main_qr_code_over_view(QWidget *parent);
    ~w_scr_dish_main_qr_code_over_view();

    void selectOrderSn(const QString &orderSn);
    void selectTableNo(const QString &tableNo);

public slots:
    void refresh0();
    void refresh();
    bool refresh(const w_scr_dish_main_qr_code::QrCodeMasterDetails &master_details,
                 int &auto_accept_index,

                 int &billTypeAVailable_count,
                 int &billTypeRCV_count,
                 int &billTypePaid_count,
                 int &billTypeQuickOrder_count,
                 int &billTypeYunAddDish_count
                 );
    void torefresh_check();

    void toset();
    void torefresh_auto();

    void tobillget();
    void tobillpay();
    void tobillcancel();
    void tobilldetail();
    void tobilldetail_yun_add_dish();
    void tobillreprint();
    void tobillquit();
    void tobilldishadd();
    void tobillmoreBtn();
    void tobilltablecancel();//消台
    void tobillcashpay();//现金支付
    void tobiilquickOrder();//先付款订单
    void tobiilYunAddDish();

    void tomin();
    void toexit();
    void todishquit();
    void todishadd();

    void table_record_vscroll_set_max();

    void toAddDishSn();

    void processPendingDatagrams();
private:
    QPushButton * billpayBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billgetBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billcancelBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billdetailBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billdetailBtn_yun_add_dish(int row, const w_scr_dish_main_qr_code::QrCodeMasterDetails &master_details);
    QPushButton * billreprintBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billquitBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billdishaddBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * billTypeRCVMoreBtn(int row, const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * biilquickOrderBtn(int row,  const w_scr_dish_main_qr_code::QrCodeMaster &master);
    QPushButton * biilYunAddDishBtn(int row,  const w_scr_dish_main_qr_code::QrCodeMasterDetails &master_details);

    QPushButton * dishquitBtn(int master_row, const w_scr_dish_main_qr_code::QrCodeMaster &master, const QString &dish_sn, lds_tableView *dish_table, lds_model_sqlstandardmodel *dish_model, bool hasquit);
    QPushButton * dishaddBtn(int master_row,  const w_scr_dish_main_qr_code::QrCodeMaster &master, lds_tableView *dish_table, lds_model_sqlstandardmodel *dish_model);

    bool biilYunAddDishTransaction(const w_scr_dish_main_qr_code::QrCodeMaster &master,
                                   const QrCodeDetailList &details,
                                   QString &errstring);

    void hideButton(int row, int column);

    void showdetail(PushButton *btn, operaType type);
    void showdetailRefresh(int master_row,
                           const w_scr_dish_main_qr_code::QrCodeMaster &master,
                           operaType type,
                           lds_tableView *table,
                           lds_model_sqlstandardmodel *model);
    void showdetailRefresh(
            QrCodeDetailList details,
            lds_tableView *table,
            lds_model_sqlstandardmodel *model);

    bool dishadd(int master_row);

    void textEditAppend(bool success, const QString &text);
    void view_set_app();

    void init();


    void pushButton_bill_hide(bool hide);
    void pushButton_bill_emit_click();
    QVariant pushButton_bill_curusrdata();
    lds_toolbar_button* pushButton_bill_cur();

    bool this_is_topLevel();
private slots:
    void pushButton_bill_check(QObject *obj);
    void pushButton_bill_check(const QString &billType);
public:
    Ui::w_scr_dish_main_qr_code_over_view *ui;
private:
    lds_model_sqlstandardmodel *standmodel;
    QTimer *timer;
    int time_line;
    const bool auto_accept_manual;
    const QString billType;

    //
    class loadingRun{
    public:
        loadingRun(Ui::w_scr_dish_main_qr_code_over_view*ui);
        ~loadingRun();
        void show();
        void setText(const QString &text);
        Ui::w_scr_dish_main_qr_code_over_view *ui;
    };
    QUdpSocket *udpSocket;
};

class w_scr_dish_main_qr_code_over_view_dialog : public w_scr_dish_main_qr_code_over_view {
    Q_OBJECT
public:
    w_scr_dish_main_qr_code_over_view_dialog(const QString &billType , QWidget *parent );
    ~w_scr_dish_main_qr_code_over_view_dialog();
    void init(w_scr_dish_main_qr_code_over_view *w);
};

#endif // W_SCR_DISH_MAIN_QR_CODE_OVER_VIEW_H
