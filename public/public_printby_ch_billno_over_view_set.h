#ifndef PUBLIC_PRINTBY_CH_BILLNO_OVER_VIEW_SET_H
#define PUBLIC_PRINTBY_CH_BILLNO_OVER_VIEW_SET_H

#include <QDialog>

namespace Ui {
class public_printby_ch_billno_over_view_set;
}

class public_printby_ch_billno_over_view_set : public QDialog
{
    Q_OBJECT

public:
    explicit public_printby_ch_billno_over_view_set(int paperwidth, QWidget *parent = 0);
    ~public_printby_ch_billno_over_view_set();

    int paperwidth();
private:
    Ui::public_printby_ch_billno_over_view_set *ui;
};

#endif // PUBLIC_PRINTBY_CH_BILLNO_OVER_VIEW_SET_H
