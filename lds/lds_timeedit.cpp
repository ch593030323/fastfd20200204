#include "lds_timeedit.h"
#include "lds.h"

lds_timeEdit::lds_timeEdit(QWidget *parent) :
    lds_datetimeEdit(parent)
{
    setDisplayFormat("hh:mm:ss");
}
