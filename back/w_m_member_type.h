#ifndef W_M_MEMBER_TYPE_H
#define W_M_MEMBER_TYPE_H

#include <QDialog>
#include "uo_rw_member.h"
class Ui_w_m_member_type_dialog;
class w_m_member_type : public QDialog
{
    Q_OBJECT
public:
    explicit w_m_member_type(QWidget *parent = 0);
    
private slots:
    void toreadcard();
    void took();
    void toexit();
private:
    Ui_w_m_member_type_dialog *ui;
    uo_rw_member::read_card_type lst_card;
};

#endif // W_M_MEMBER_TYPE_H
