#ifndef W_M_MEMBER_CARDIC_H
#define W_M_MEMBER_CARDIC_H

#include <QDialog>
#include "uo_rw_member.h"
#include "n_func.h"
class Ui_w_m_member_cardic_Dialog;
class w_m_member_cardic : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_cardic(QWidget *parent = 0);
private slots:
    void to_read_card();
    void to_clear();
    void to_exit();
    void to_dynamic_clear();
private:
    Ui_w_m_member_cardic_Dialog *ui;
    uo_rw_member::read_card_type lst_card;
    uo_rw_member uo_member;
    n_func::st_member_type  lst_member;

};

#endif // W_M_MEMBER_CARDIC_H
