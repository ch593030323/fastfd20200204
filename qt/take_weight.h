#ifndef TAKE_WEIGHT_H
#define TAKE_WEIGHT_H

#include <QDialog>
#include "electroniccountprice_resolve.h"

class QTimeLine;
class Ui_take_weight;

class take_weight : public QDialog
{
    Q_OBJECT
    
public:
    explicit take_weight(const QString &ch_dishno,  float dishprice, float dishnum, int int_discount, QWidget *parent);
    ~take_weight();
    bool check(QWidget *parent, const QString &ch_suitflag, const QString &ch_presentflag);
    void retranslateView();
    float _weight;
    Ui_take_weight *ui;
    void setAppendbtn(bool visiabled);


    void refresh(const QString &ch_dishno,  float dishprice, float dishnum, int int_discount);
private slots:
    void toappend();
    void took();
    void toexit();

    void toqupi();
    void tozhiling();

    void quipistate(bool f);
    void lingweistate(bool f);

    void updategetweight(const QString &value);
    void updategetweight(double value);
    void updateweighttype(int key);
    void updatetotal(const QString &value);

    void updateAutook(bool f);
    void toAutoOk(int step);

private:
    void initData();
private:

    float _curdishnum;
    float kg2otherrate;
    QTimeLine *tline;
    QString ch_dishno;
    QString ch_count;
    double num_count_weight;
     QString weight_old;
     ElectronicCountPrice_resolve countprice;

};

#endif // TAKE_WEIGHT_H
