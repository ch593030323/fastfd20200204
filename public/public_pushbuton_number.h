#ifndef PUBLIC_PUSHBUTON_NUMBER_H
#define PUBLIC_PUSHBUTON_NUMBER_H

#include <QPushButton>

class public_pushbuton_number : public QPushButton
{
    Q_OBJECT
public:
    public_pushbuton_number(QWidget *parent);
    void setNumber(double num);
    double number();

    void setNumber(const QString &desc);
   QString numberStr();

    void setText(const QString &text);
    QString text() const;

    void setSuffix(const QString &suffix);
protected:
    void paintEvent(QPaintEvent *e);
private:
    QString _numberStr;
    QString _text;
    QString _suffix;
};

#endif // PUBLIC_PUSHBUTON_NUMBER_H
