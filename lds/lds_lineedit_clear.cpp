#include "lds_lineedit_clear.h"
#include <QTimer>

lds_lineedit_clear::lds_lineedit_clear(QWidget *parent) :
    QLineEdit(parent)
{
    total = 0;
}

void lds_lineedit_clear::enableTimeClear()
{
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(totextnotinput(QString)));
}

void lds_lineedit_clear::totextnotinput(const QString &text)
{
    if(text.length() ==1)
        QTimer::singleShot(100, this,SLOT(clear()));
}

void lds_lineedit_clear::setRemain_sub(const QString &text)
{
    if(total > 0) {
        setText(QString().sprintf("%0.2f", total - text.toDouble()));
    }
}

void lds_lineedit_clear::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit outfocus();
}

void lds_lineedit_clear::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit getfocus();
}
