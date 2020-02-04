#ifndef AUTOFONTLABEL_H
#define AUTOFONTLABEL_H

#include <QLabel>

class AutoFontLabel : public QLabel
{
    Q_OBJECT
public:
    AutoFontLabel(QWidget *parent);
protected:
    void paintEvent(QPaintEvent *e);
};

#endif // AUTOFONTLABEL_H
