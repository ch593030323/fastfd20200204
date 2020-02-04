#include "lds_toolbar.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QLine>
#include <QDebug>
#include <QFrame>
#include <QPushButton>
#include "lds_toolbar_button.h"
#include <QTimer>
#include <QWidgetAction>
#include "lds_menu.h"
#include "lds_selectgoods.h"
#include "lds_pushbutton_wrap.h"
#include "lds.h"

const QString lds_toolbar::stretch = "stretch";
lds_toolbar::lds_toolbar(QWidget *parent) :
    QFrame(parent)
{
    this->setFrameShape(QFrame::NoFrame);
}

void lds_toolbar::setup(const QStringList &names, Qt::Alignment flag)
{
    if(this->layout()) {
        qDebug() << QObject::tr("layout existed");
        return;
    }
    QStringList names0 = names;
    QBoxLayout *layout=0;
    if(flag==Qt::AlignRight) {
        layout=new QHBoxLayout;
        if(!names.contains(stretch)) names0.prepend(stretch);
    } else if(flag==Qt::AlignLeft) {
        layout=new QHBoxLayout;
        if(!names.contains(stretch)) names0.append(stretch);
    } else if(flag==Qt::AlignTop) {
        layout=new QVBoxLayout;
        if(!names.contains(stretch)) names0.append(stretch);
    } else if(flag==Qt::AlignBottom) {
        layout=new QVBoxLayout;
        if(!names.contains(stretch)) names0.prepend(stretch);
    } else {
        layout=new QHBoxLayout;
        if(!names.contains(stretch)) names0.prepend(stretch);
    }
    layoutaddwidget(layout, names0, flag);

    if(layout) {
        layout->setMargin(0);
        setLayout(layout);
    }
}

lds_toolbar_button *lds_toolbar::index_widget(const QString &name)
{
    return nameTool.value(name);
}

lds_toolbar_button *lds_toolbar::index_widget(int index)
{
    return nameList[index];
}

void lds_toolbar::setDefaultFalseAutoDefaultFalse(const QString &name)
{
    lds_toolbar_button *b = index_widget(name);
    if(b) {
        b->setDefault(false);
        b->setAutoDefault(false);
    }
}

void lds_toolbar::layoutaddwidget(QBoxLayout *layout, const QStringList &names, Qt::Alignment flag)
{
    foreach(QString s, names) {
        if(s == "|") {
            QFrame *line = new QFrame(this);
            line->setObjectName(QString::fromUtf8("line"));
            if(flag==Qt::AlignLeft||flag==Qt::AlignRight) {
                line->setFrameShape(QFrame::VLine);
            } else {
                line->setFrameShape(QFrame::HLine);
            }
            line->setFrameShadow(QFrame::Sunken);
            line->setStyleSheet("background:black;");
            layout->addWidget(line);
        } else if(s == stretch) {
            layout->addStretch(1);
        } else {
            lds_toolbar_button *t=new lds_toolbar_button(this);
            t->setText(s);
            t->setProperty("outer_stylesheet", "pushbutton");
            layout->addWidget(t);
            nameTool.insert(s, t);
            nameList.append(t);
        }
    }
}
