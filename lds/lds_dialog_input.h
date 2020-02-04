#ifndef Lds_DIALOG_INPUT_H
#define Lds_DIALOG_INPUT_H

#include <QDialog>
#include "lds_messagebox.h"
class Ui_lds_dialog_input_Dialog;

class lds_dialog_input : public QDialog
{
    Q_OBJECT
public:
    enum viewFlag{
        viewShow1 = 1,
        viewShow2 = 2,
        viewShow3 = 4,
        viewShowCom = 8,
        viewShowKey = 16,
        viewShowLink = 32,
    };
    Q_DECLARE_FLAGS(viewFlags, viewFlag)

public:
    explicit lds_dialog_input(QWidget *parent = 0);
    ~lds_dialog_input();

public:
    Ui_lds_dialog_input_Dialog *ui;
    void setDotEnabled(bool f);
    void delay_set_focus();
    /////////////////////////////////////////////////////////
    void showSecondLineEdt();
    void hideSecondLineEdt();

    void hideKey();

    void closeDetail();
    void openDetail();

    void disableESC();
    void hideCancel();

    void resetButtons(const QStringList &names);

    void showView(int flags);
public slots:
    void toLineEditSetNum(double num);
public:
    static bool checkdiscount(const QString &value, QWidget *parent);//核对折扣
    static bool checknum(const QString &value, QWidget *parent);//核对数量
    static bool checknumWith0(const QString &value, QWidget *parent);//核对数量
signals:
    void signal_detailshow(bool show);
protected slots:
    virtual void took();
    virtual void toexit();
    void toself();

    void update_state();
    void update_state2();

    void lineselectall();
    void todo();
private:
    int getlineheight();
    int curFixedHeight();
protected:
    QString took_cardno;


protected:
    bool eventFilter(QObject *o, QEvent *e);
};

#endif // Lds_DIALOG_INPUT_H
