#ifndef LDS_LABEL_DOUBLESPINBOX_H
#define LDS_LABEL_DOUBLESPINBOX_H

#include <QWidget>
#include "lds_bool.h"
#include "lds_label_cell.h"

struct lds_label_doublespinbox_data{
    lds_label_doublespinbox_data();
    //
    double value();
    QString text();
    //
    double _value;
    int _decimals;
    //
    QString _edit_value;
    QString _labelText;
    QString _prefix;
    bool _isselectAll;
    bool _enableSelectAll;
};

class lds_label_doublespinbox : public lds_label_cell
{
    Q_OBJECT
public:
    lds_label_doublespinbox(QWidget *parent = 0);

    //virtual
    virtual QString paintLabel();
    virtual QString paintText();
    //~virtual

    void setDecimals(int decimals);
    int decimals();

    void setValue(double value);
    void setValue(const QString &v);
    double value();
    double value_if_prefix(double else_value = 0);

    void setLabelText(const QString &labelText);

    void setBgColorLight(double bgLight);

    void setPrefix(const QString &prefix);
    QString prefix();

    void clear();
    void setBgColor(const QColor &color);

    void enableSelectAll();
    void selectAll();
    void selectCLear();
    bool isselectAll();

    void enableClickable();

    void enableShowNegativeValue();

    QString falseText();

    void setBorder(int left, int top, int right, int bottom);

    void setMessage(const QString &text);
signals:
    void valueChanged(double);
    void signalClicked();
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
private:
    QString text();
private:
    lds_bool_default_false ispressed;
    lds_label_doublespinbox_data p_data;

    lds_bool_default_false isclickable;
    lds_bool_default_false bShowNegativeValue;
    lds_bool_default_false bShowMessage;
    double bgLight;
    QColor bgColor;

    int left, top, right, bottom;
    QString message;
};

#endif // LDS_LABEL_DOUBLESPINBOX_H
