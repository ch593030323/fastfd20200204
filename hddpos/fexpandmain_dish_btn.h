#ifndef FEXPANDMAIN_DISH_BTN_H
#define FEXPANDMAIN_DISH_BTN_H

#include <QFrame>
#include <QHBoxLayout>
#include "lds_pushbutton_oblique.h"
class QScrollBar;


class fexpandmain_dish_btn : public QFrame
{
    Q_OBJECT
public:
    explicit fexpandmain_dish_btn(QWidget *parent = 0);
    ~fexpandmain_dish_btn();

    void setup(const QString &ch_tableno, QScrollBar *vbar);
    lds_pushbutton_oblique *up ;
    lds_pushbutton_oblique *down ;
    lds_pushbutton_oblique *waimai ;
    lds_pushbutton_oblique *desk ;
    lds_pushbutton_oblique *waidai ;

    void addMenuInitAndShow();
signals:
   void signal_waimai();
   void signal_waimaipay();

private slots:
    void toUpdaeEnabledFromScrollBar();

    void toDown();
    void toUp();
    void toInitWaimaiMenuAndShow();
private:
    void setup(const QStringList &list);
    void setUpBtn(bool disabled);
    void setDownBtn(bool disabled);

    QHBoxLayout *hlayout;
    QScrollBar *vbar;
};

#endif // FEXPANDMAIN_DISH_BTN_H
