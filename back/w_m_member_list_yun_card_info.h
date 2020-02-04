#ifndef W_M_MEMBER_LIST_YUN_CARD_INFO_H
#define W_M_MEMBER_LIST_YUN_CARD_INFO_H

#include <QWidget>
#include "w_sys_manage_cloudsync.h"
class Ui_w_m_member_sendcard_Dialog;

class w_m_member_list_yun_card_info : public QDialog
{
    Q_OBJECT
public:
    enum memberState{
        memberNew,
        memberChange
    };

public:
    explicit w_m_member_list_yun_card_info(QWidget *parent = 0);
    ~w_m_member_list_yun_card_info();

    bool initData(const QString &vch_memberno, QString &errstring);

private slots:
    void tosendCard();
    void tocancel();

private:
    bool initDataY(const QString &vch_memberno, QString &errstring);
    bool initDataL(const QString &vch_memberno, QString &errstring);
    bool sendcardY();
    bool sendcardL();

    bool writeIC(const QString &vch_memberno, QString &errstring);
private:
    Ui_w_m_member_sendcard_Dialog *ui;
    QString is_cardtype;
    QString vip_ic_password;
    QDateTime sqldt;

};

#endif // W_M_MEMBER_LIST_YUN_CARD_INFO_H
