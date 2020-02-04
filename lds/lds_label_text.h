#ifndef LDS_LABEL_TEXT_H
#define LDS_LABEL_TEXT_H

#include <QWidget>
#include "lds_label_cell.h"

class lds_label_text : public lds_label_cell
{
    Q_OBJECT
public:
    static const int borderPix;
public:
    lds_label_text(QWidget *parent = 0);
    //virtual
    virtual QString paintLabel();
    virtual QString paintText();
    //~virtual

    void setLabelText(const QString &labelText);
    void setLabelText(const QString &labelText, const QString &text);
    void setLabelTextUsr_1(const QString &labelText, const QString &text, const QString &usr_1);
    void setText(const QString &text);
    void appendNumber(const QString &text);
    void setValue(double value);
    void setBgColor(const QColor &bgColor);
    QColor skinBgColor(const QString &skin_name);

    void setFontColor(const QColor &fontColor);
    double value();
    QString text();

    QString get_usr_1();

    void setLabelAlignment(int align);
    void setTxtAlignment(int align);

    void setTxtFontSize(int size);
signals:
    void clicked();
    void textChanged(const QString &);
protected:
    virtual void paintEvent(QPaintEvent *);
private:
    int labelFlag();
    int txtFlag();

    bool ispaint();
private:
    QString txt;
    QString labelText;
    QColor bgColor;
    QColor fontColor;
    QString usr_1;
    bool b_text_reset;
    int label_align;
    int txt_align;
    int txt_font_size;
};

#endif // LDS_LABEL_TEXT_H
