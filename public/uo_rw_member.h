#ifndef UO_RW_MEMBER_H
#define UO_RW_MEMBER_H

#include <QObject>

/*
    lst_card.amount = ldec_amount; 有效
    lst_card.point = ldec_point; 有效
    lst_card.cardid = as_memberno; 有效
    lst_card.flag = true; 有效

    is_cardtype 有效
    errtext 有效
*/
class uo_rw_member : public QObject
{
    Q_OBJECT
public:
    explicit uo_rw_member(QObject *parent = 0);
    typedef struct read_card{
        QString cardid;
        double amount;
        double point;
        bool flag;
        QString s_type;
        QString icbaud;
        QString iccom;
        QString ictype;
        QString cardname;
        QString card_type;
        QString password;
        QString branchno;
        QString payno;
        double amount_base;
        double amount_base_cost;
        QString errtext;
        QString parm;
        QString cardno;
    } read_card_type;


    void of_init(QWidget *parent, const QString &as_action, bool *ok = 0);

    read_card_type of_read_card(QWidget *parent, const QString &as_memberno);
    QString of_send_card(QWidget *parent, const QString &as_memberno);//return errtext


    QString is_action;
    QString is_cardtype;
    QString text;
    bool visible;
};

#endif // UO_RW_MEMBER_H
