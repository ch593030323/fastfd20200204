#ifndef LDS_DOUBLESPINBOX_LINEEDIT_H
#define LDS_DOUBLESPINBOX_LINEEDIT_H

#include <QLineEdit>
#include "lds_bool.h"

class lds_doublespinbox_lineedit : public QLineEdit
{
    Q_OBJECT
public:
    explicit lds_doublespinbox_lineedit(QWidget *parent = 0);
    double value();
    void setValue(double value);

    void enableShowNegativeValue();
protected Q_SLOTS:
    void setText(const QString &text);
protected:
    QString text() const;
    double _value;
    lds_bool_default_false bShowNegativeValue;
};

#endif // LDS_DOUBLESPINBOX_LINEEDIT_H
