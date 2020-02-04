#include "lds_spinbox.h"
#include "lds.h"

lds_spinbox::lds_spinbox(QWidget *parent) :
    lds_doublespinbox(parent)
{
    setDecimals(0);
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(emit_valueChanged(double)));
}

void lds_spinbox::emit_valueChanged(double v)
{
    emit valueChanged(int(v));
}
