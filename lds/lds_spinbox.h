#ifndef LDS_SPINBOX_H
#define LDS_SPINBOX_H

#include <QSpinBox>
#include "lds_doublespinbox.h"

class lds_spinbox : public lds_doublespinbox
{
    Q_OBJECT
public:
    explicit lds_spinbox(QWidget *parent = 0);

signals:
    void valueChanged(int);
private slots:
    void emit_valueChanged(double v);
};

#endif // LDS_SPINBOX_H
