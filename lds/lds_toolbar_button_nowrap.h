#ifndef LDS_TOOLBAR_BUTTON_NOWRAP_H
#define LDS_TOOLBAR_BUTTON_NOWRAP_H

#include <QPushButton>

class lds_toolbar_button_nowrap : public QPushButton
{
    Q_OBJECT
public:
    explicit lds_toolbar_button_nowrap(QWidget *parent = 0);

    QSize sizeHint()const;

    void updateText(const QString &text);

};

#endif // LDS_TOOLBAR_BUTTON_NOWRAP_H
