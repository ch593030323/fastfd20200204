#include "lds_toolbar_button.h"
#include <QVariant>
#include <QtDebug>
#include "lds.h"

lds_toolbar_button::lds_toolbar_button(QWidget *parent) :lds_pushbutton_wrap(parent)
{
}

lds_toolbar_button::lds_toolbar_button(const QString &outer_stylesheet, QWidget *parent)
    :lds_pushbutton_wrap(parent)
{
    this->setProperty("outer_stylesheet", outer_stylesheet);
}

lds_toolbar_button::~lds_toolbar_button()
{
}

QSize lds_toolbar_button::sizeHint() const
{
    return lds::pushButtonSizeHint(this, QPushButton::sizeHint());
}

void lds_toolbar_button::updateText(const QString &text)
{
    setText(text);
}

void lds_toolbar_button::emitClick(bool checked)
{
    emit clicked(checked);
}
