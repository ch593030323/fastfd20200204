#ifndef PUBLIC_PRINTERR_DIALOG_H
#define PUBLIC_PRINTERR_DIALOG_H

#include <QDialog>
#include "lds_widget_drog.h"
namespace Ui {
class public_printerr_Dialog;
}

class public_printerr_Dialog : public lds_widget_drog
{
    Q_OBJECT

public:
    explicit public_printerr_Dialog(QWidget *parent = 0);
    ~public_printerr_Dialog();

    void setContent(const QString &text, const QString &title);
private slots:
    void toclose();
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::public_printerr_Dialog *ui;
    bool ispressed;
};

#endif // PUBLIC_PRINTERR_DIALOG_H
