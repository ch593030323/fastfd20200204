#include "lds_doublespinbox.h"
#include "lds.h"
#include <QKeyEvent>
#include <QTimer>

lds_doublespinbox::lds_doublespinbox(QWidget *parent) :
    QDoubleSpinBox(parent)
{
    bValuemaxminus = false;
    expandMAXMIN();
    connect(this,SIGNAL(getfocus()),this,SLOT(toSelectAll()), Qt::QueuedConnection);//需要是队列连接方式才可以实现
}

void lds_doublespinbox::expandMAXMIN()
{
    setRange(DBOX_MIN, DBOX_MAX);
}

QSize lds_doublespinbox::sizeHint() const
{
    return QSize(70, QDoubleSpinBox::sizeHint().height());
}

void lds_doublespinbox::setValue(double v)
{
    if(bValuemaxminus) {
        v = this->maximum() - v;
    }
    QDoubleSpinBox::setValue(v);
}

void lds_doublespinbox::setTrueValue(double v)
{
    QDoubleSpinBox::setValue(v);
}

double lds_doublespinbox::value()
{
    if(bValuemaxminus) {
        return this->maximum() - QDoubleSpinBox::value();
    }
    return QDoubleSpinBox::value();
}

void lds_doublespinbox::enableValueMaxMinus()
{
    bValuemaxminus = true;
    this->setStyleSheet("color:#ff546b;");
    setValue(trueValue());
}

bool lds_doublespinbox::isEmpty()
{
    return text().isEmpty();
}

void lds_doublespinbox::focusOutEvent(QFocusEvent *e)
{
    QDoubleSpinBox::focusOutEvent(e);
    emit outfocus();
}

void lds_doublespinbox::focusInEvent(QFocusEvent *e)
{
    QDoubleSpinBox::focusInEvent(e);
    emit getfocus();
}

void lds_doublespinbox::keyPressEvent(QKeyEvent *event)
{
    //先保留，keyPressEvent可能会有ignore的操作
    int ekey = event->key();

    QDoubleSpinBox::keyPressEvent(event);

    switch(ekey) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        QTimer::singleShot(10, this, SIGNAL(returnPressedDelay()));
        break;
    default:
        break;
    }
}

double lds_doublespinbox::valueFromText(const QString &text) const
{
    QString r = text;
    r = r.remove(0, prefix().length());
    r.chop(suffix().length());
    return lds::groupSeparatorStr2v(r);
}

QString lds_doublespinbox::textFromValue(double value1) const
{
    return  lds::septNumber(value1, this->decimals());
}

QString lds_doublespinbox::text() const
{
    return QDoubleSpinBox::text();
}

void lds_doublespinbox::toSelectAll()
{
    if(this->isEnabled()) {
        this->selectAll();
    }
}

double lds_doublespinbox::trueValue()
{
    return QDoubleSpinBox::value();
}
