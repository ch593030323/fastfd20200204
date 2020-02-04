#include "lds_toolbar_button_nowrap.h"
#include "lds.h"

lds_toolbar_button_nowrap::lds_toolbar_button_nowrap(QWidget *parent) :
    QPushButton(parent)
{
    this->setMinimumHeight(40);
}
QSize lds_toolbar_button_nowrap::sizeHint() const
{
    return lds::pushButtonSizeHint(this, QPushButton::sizeHint());
}

void lds_toolbar_button_nowrap::updateText(const QString &text)
{
    setText(text);
}
