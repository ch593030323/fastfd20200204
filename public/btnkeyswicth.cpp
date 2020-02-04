#include "btnkeyswicth.h"
#include <QPainter>
#include <QPaintEvent>
#include "lds.h"


btnkeyswicth::btnkeyswicth(QWidget *parent)
    : lds_pushbutton_wrap(parent) {

    enabledSwitchKey(true);
    enableLongPress(true);
}

void btnkeyswicth::init(const QString &k1, const QString &v1, const QString &k2, const QString &v2, const QString &k_cur)
{
    cur_index = 1;
    if(k1 == k_cur)  cur_index = 1;
    if(k2 == k_cur)  cur_index = 2;
    if("" == k_cur)  cur_index = 1;
    key1 = k1;
    value1 = v1;
    key2 = k2;
    value2 = v2;
}

void btnkeyswicth::init(const QString &k1, const QString &k2, const QString &k_cur)
{
    cur_index = 1;
    if(k1 == k_cur)  cur_index = 1;
    if(k2 == k_cur)  cur_index = 2;
    if("" == k_cur)  cur_index = 1;

    key1 = k1;
    value1 = k1;
    key2 = k2;
    value2 = k2;
}

void btnkeyswicth::setValues(const QString &v1, const QString &v2)
{
    value1 = v1;
    value2 = v2;
}

void btnkeyswicth::enabledSwitchKey(bool enabled)
{
    disconnect(this, SIGNAL(clicked()),this, SLOT(switchKey()));
    if(enabled) {
        connect(this, SIGNAL(clicked()),this, SLOT(switchKey()));
    }
}

void btnkeyswicth::switchKey()
{
    if(false == switchData())
        return;
    update();
    emitKeyChanged();
}

bool btnkeyswicth::switchData()
{
    if(cur_index == 1) {
        cur_index = 2;
    } else if(cur_index == 2) {
        cur_index = 1;
    }
    return true;
}

void btnkeyswicth::paintEvent(QPaintEvent *e)
{
    QPushButton::paintEvent(e);
    QPainter painter(this);
    QString firstV;

    firstV = firstValue();
    if(firstV.isEmpty()) {
        firstV = "["+QObject::tr("主")+"]";
    }
    QFontMetrics fm(painter.font());
    if(fm.width(firstV) >= width()) {
        painter.drawText(rect(), Qt::AlignLeft | Qt::AlignVCenter, firstV);
    } else {
        painter.drawText(rect(), Qt::AlignCenter, firstV);
    }

    QString secondV = secondValue();
    if(secondV.isEmpty()) {
        secondV = "["+QObject::tr("次")+"]";
    }
    painter.drawText(rect(), Qt::AlignRight | Qt::AlignBottom, secondV);
}

QString btnkeyswicth::firstValue()
{
    if(cur_index == 1) {
        return value1;
    }
    if(cur_index == 2) {
        return value2;
    }
    return value1;
}

QString btnkeyswicth::secondValue()
{
    if(cur_index == 1) {
        return value2;
    }
    if(cur_index == 2) {
        return value1;
    }
    return value2;
}

QString btnkeyswicth::firstKey()
{
    if(cur_index == 1) {
        return key1;
    }
    if(cur_index == 2) {
        return key2;
    }
    return key1;
}

QString btnkeyswicth::secondKey()
{
    if(cur_index == 1) {
        return key2;
    }
    if(cur_index == 2) {
        return key1;
    }
    return key2;
}

void btnkeyswicth::emitKeyChanged()
{
    emit keyChanged();
    emit keyChanged(firstKey());
}
