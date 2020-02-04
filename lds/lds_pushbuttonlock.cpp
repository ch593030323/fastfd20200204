#include "lds_pushbuttonlock.h"
#include <QApplication>

lds_pushbuttonlock::lds_pushbuttonlock(QWidget *lockwidget, QObject *parent) :
    QObject(parent)
{
    l = lockwidget;
}

void lds_pushbuttonlock::toLock()
{
    if(l) {
        l->setEnabled(false);
        QApplication::processEvents();
    }
}

lds_pushbuttonlock::~lds_pushbuttonlock()
{
    if(l) {
        l->setEnabled(true);
    }
}
