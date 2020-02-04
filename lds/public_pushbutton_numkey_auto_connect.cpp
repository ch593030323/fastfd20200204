#include "public_pushbutton_numkey_auto_connect.h"
#include <QPointer>
#include <QApplication>
#include <QKeyEvent>

public_pushbutton_numkey_auto_connect::public_pushbutton_numkey_auto_connect(QWidget *parent)
    : public_pushbutton_numkey(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(setNum()));
}

void public_pushbutton_numkey_auto_connect::setNum()
{
    QPointer<QWidget> w = qApp->focusWidget();
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    //键盘数字
    QKeyEvent keyPress(QEvent::KeyPress, b->text().data()->unicode(), Qt::NoModifier, b->text());
    qApp->sendEvent(w, &keyPress);
}
