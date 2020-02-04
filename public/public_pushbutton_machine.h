#ifndef PUBLIC_PUSHBUTTON_MACHINE_H
#define PUBLIC_PUSHBUTTON_MACHINE_H

#include <QPushButton>

class public_pushbutton_machine : public QPushButton
{
    Q_OBJECT
public:
    explicit public_pushbutton_machine(QWidget *parent = 0);

    void emit_signal_transactioned();
    void setCheckable1(bool ischeckable1);
    void setChecked1(bool ischecked1);
    bool isChecked1();
signals:
    void signal_transactioned();

protected:
    virtual  void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
private:
    bool ischecked1;
    bool ischeckable1;

};

#endif // PUBLIC_PUSHBUTTON_MACHINE_H
