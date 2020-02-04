#ifndef DOUBLEWIDGET_NORMAL_PAYING_H
#define DOUBLEWIDGET_NORMAL_PAYING_H

#include "doublewidget.h"
class lds_label_text;
class fexpandmain_paying;
class QPushButton;
class doublewidget_normal_paying : public doublewidget
{
public:
    doublewidget_normal_paying(QSize dsize ,QWidget *parent = 0);

    void doublewidget_layout();
protected slots:
    void adc_timer_start();
    void adc_timer_out();
    void standmodel_setdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void standmodel_insetdata(int row, const QString &p0, const QString &p1, const QString &p2, const QString &p3, const QString &p4);
    void qr_code_adv_hold();
    void total_settext(const QString &text);
    void num_settext(const QString &text);

    void update_pay_request(const lds::DOUBLE_DATA &data);
    void update_skin_request(const lds::DOUBLE_DATA &data);
    void update_paying_request(const lds::DOUBLE_DATA &data);
private:
    QString blank;
    int leftspace;
    int rightspace;
    fexpandmain_paying *paying_view;
    QPushButton *pushbutton_bottom;
    bool ispaying;
};

class doublewidget_normal_paying_delegate : public QStyledItemDelegate {
public:
    doublewidget_normal_paying_delegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
    void keepConnect(fexpandmain_table *tableview);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;


    int *border;
    QColor *divColor;
    QColor *nameColor;
    QColor *nameSubColor;
    QColor *defColor;
    QColor *itemBgColor;

};

#endif // DOUBLEWIDGET_NORMAL_PAYING_H
