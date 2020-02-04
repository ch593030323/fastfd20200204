#ifndef LDS_LINEEDIT_CLEAR_H
#define LDS_LINEEDIT_CLEAR_H

#include <QLineEdit>

class lds_lineedit_clear : public QLineEdit
{
    Q_OBJECT
public:
    explicit lds_lineedit_clear(QWidget *parent = 0);

protected:
    void focusOutEvent(QFocusEvent *e);
    void focusInEvent(QFocusEvent *e);
signals:
    void getfocus();
    void outfocus();
public:
    /////////////////////////////////////////////////////////////////
    void enableTimeClear();
public slots:
    void setRemain_sub(const QString& text);
public:
    double total;
private slots:
    void totextnotinput(const QString& text);
    /////////////////////////////////////////////////////////////////
};

#endif // LDS_LINEEDIT_CLEAR_H
