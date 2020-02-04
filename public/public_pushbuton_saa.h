#ifndef PUBLIC_PUSHBUTON_SAA_H
#define PUBLIC_PUSHBUTON_SAA_H

#include <QPushButton>

class public_pushbuton_SAA : public QPushButton
{
    Q_OBJECT
public:
    explicit public_pushbuton_SAA(QWidget *parent = 0);

    void paintEvent(QPaintEvent *e);

};

#endif // PUBLIC_PUSHBUTON_SAA_H
