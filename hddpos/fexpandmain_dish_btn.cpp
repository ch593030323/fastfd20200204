#include "fexpandmain_dish_btn.h"
#include <QtDebug>
#include <QScrollBar>
#include <QWidgetAction>
#include "lds_menu.h"
#include "ui_waimai_menu.h"
#include "w_scr_dish_main.h"
#include "lds_scr_table.h"


fexpandmain_dish_btn::fexpandmain_dish_btn(QWidget *parent) :
    QFrame(parent)
{
    hlayout = new QHBoxLayout;
    this->setLayout(hlayout);
    vbar = 0;

    up = new lds_pushbutton_oblique(this);
    up->setFocusPolicy(Qt::NoFocus);
    up->setProperty("outer_stylesheet", "pushbutton_bottom");
    up->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    up->setIcon(QPixmap(":/image/btn_icon/28.png"));
    up->setIconSize(QSize(40, 40));
    up->enablePaintMenuPopup(false);

    down = new lds_pushbutton_oblique(this);
    down->setFocusPolicy(Qt::NoFocus);
    down->setProperty("outer_stylesheet", "pushbutton_bottom");
    down->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    down->setIcon(QPixmap(":/image/btn_icon/27.png"));
    down->setIconSize(QSize(40, 40));
    down->enablePaintMenuPopup(false);

    waidai = new lds_pushbutton_oblique(this);
    waidai->setFocusPolicy(Qt::NoFocus);
    waidai->setProperty("outer_stylesheet", "pushbutton_bottom");
    waidai->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    waidai->setIcon(QPixmap(":/image/btn_icon/25.png"));
    waidai->setIconSize(QSize(40, 40));
    waidai->enablePaintMenuPopup(false);

    waimai = new lds_pushbutton_oblique(this);
    waimai->setFocusPolicy(Qt::NoFocus);
    waimai->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    waimai->setIcon(QPixmap(":/image/btn_icon/24.png"));
    waimai->setIconSize(QSize(40, 40));
    waimai->enablePaintMenuPopup(false);

    desk = new lds_pushbutton_oblique(this);
    desk->setFocusPolicy(Qt::NoFocus);
    desk->setProperty("outer_stylesheet", "pushbutton_bottom");
    desk->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    desk->setIcon(QPixmap(":/image/btn_icon/26.png"));
    desk->setIconSize(QSize(40, 40));
    desk->enablePaintMenuPopup(false);

    hlayout->addWidget(desk);
    hlayout->addWidget(waidai);
    hlayout->addWidget(waimai);
    hlayout->addWidget(up);
    hlayout->addWidget(down);
}

fexpandmain_dish_btn::~fexpandmain_dish_btn()
{
    qDebug() << __FUNCTION__;
}

void fexpandmain_dish_btn::setup(const QStringList &list)
{
    foreach(const QString &str, list) {
        lds_pushbutton_oblique *p = new lds_pushbutton_oblique;
        p->setText(str);
        p->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        hlayout->addWidget(p);
    }
}

void fexpandmain_dish_btn::setup(const QString &ch_tableno, QScrollBar *vbar)
{
    this->vbar = vbar;
    setDownBtn(false);
    setUpBtn(false);

    connect(up, SIGNAL(clicked()), this,SLOT(toUp()));
    connect(down, SIGNAL(clicked()), this,SLOT(toDown()));
    connect(vbar, SIGNAL(rangeChanged(int,int)), this,SLOT(toUpdaeEnabledFromScrollBar()));

    waimai->setProperty("outer_stylesheet", "pushbutton_bottom");
    if(ch_tableno == lds_scr_table::Q000) {
        waimai->enablePaintMenuPopup(true);
    }
}

void fexpandmain_dish_btn::addMenuInitAndShow()
{
    connect(waimai, SIGNAL(clicked(bool)), this, SLOT(toInitWaimaiMenuAndShow()));
}

void fexpandmain_dish_btn::toInitWaimaiMenuAndShow()
{
    if(waimai->menu() == 0) {
        QWidget *w = new QWidget;
        Ui_waimai_menu *ui = new Ui_waimai_menu;
        ui->setupUi(w);
        w->setFixedSize(w->size());
        connect(ui->pushButton_waimai, SIGNAL(clicked()),this, SIGNAL(signal_waimai()));
        connect(ui->pushButton_waimaipay, SIGNAL(clicked()),this, SIGNAL(signal_waimaipay()));

        QWidgetAction *widgetaction=new QWidgetAction(waimai);
        widgetaction->setDefaultWidget(w);

        lds_menu* m=new lds_menu(this);
        m->addAction(widgetaction);

        waimai->setMenu(m);
        waimai->showMenu();
    }
}

void fexpandmain_dish_btn::setUpBtn(bool disabled)
{
    up->setDisabled(disabled);
}

void fexpandmain_dish_btn::setDownBtn(bool disabled)
{
    down->setDisabled(disabled);
}

void fexpandmain_dish_btn::toUpdaeEnabledFromScrollBar()
{
    /*down*/setDownBtn(vbar->value() == vbar->maximum());
    /*up*/setUpBtn(vbar->value() == vbar->minimum());
}

void fexpandmain_dish_btn::toDown()
{
    vbar->setValue(vbar->value()+vbar->pageStep());
    toUpdaeEnabledFromScrollBar();
}

void fexpandmain_dish_btn::toUp()
{
    vbar->setValue(vbar->value()-vbar->pageStep());
    toUpdaeEnabledFromScrollBar();
}
