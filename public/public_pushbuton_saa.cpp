#include "public_pushbuton_saa.h"
#include "lds.h"
#include <QPainter>
#include "saas_pay_detail.h"

public_pushbuton_SAA::public_pushbuton_SAA(QWidget *parent) :
    QPushButton(parent)
{
}

void public_pushbuton_SAA::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
    QPainter p(this);

    saas_pay_detail::paintLeftTopAA(this->rect(), &p);
}
