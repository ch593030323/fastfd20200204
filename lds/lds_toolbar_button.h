#ifndef LDS_TOOLBAR_BUTTON_H
#define LDS_TOOLBAR_BUTTON_H
#include <QPushButton>
#include "lds_pushbutton_wrap.h"

class lds_toolbar_button:public lds_pushbutton_wrap
{
public:
    lds_toolbar_button(QWidget *parent = 0);
    lds_toolbar_button(const QString &outer_stylesheet, QWidget *parent = 0);
    virtual ~lds_toolbar_button();
    QSize sizeHint()const;

    void updateText(const QString &text);

    void emitClick(bool checked = false);
};

#endif // LDS_TOOLBAR_BUTTON_H
