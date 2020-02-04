#ifndef LDS_TIMEEDIT_H
#define LDS_TIMEEDIT_H

#include "lds_datetimeedit.h"

class lds_timeEdit : public lds_datetimeEdit
{
    Q_OBJECT

public:
    explicit lds_timeEdit(QWidget *parent = 0);
};

#endif // LDS_TIMEEDIT_H
