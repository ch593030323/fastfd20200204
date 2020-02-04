#include "lds_label_text.h"
#include <QPainter>
#include "lds.h"
#include <QMouseEvent>

const int lds_label_text::borderPix = 5;

lds_label_text::lds_label_text(QWidget *parent)
    : lds_label_cell(parent)
{
    const QString skin_name  = lds::sysconf->value("system_setting/skin_name", "obsidian").toString();
    bgColor = skinBgColor(skin_name);
    fontColor = QColor("white");
    b_text_reset = true;
    label_align = Qt::AlignLeft | Qt::AlignVCenter;
    txt_align = Qt::AlignRight | Qt::AlignVCenter;
    txt_font_size = -1;
}

QString lds_label_text::paintLabel()
{
    return labelText;
}

QString lds_label_text::paintText()
{
    return text();
}

void lds_label_text::setLabelText(const QString &labelText)
{
    this->labelText = labelText;
    update();
}

void lds_label_text::setLabelText(const QString &labelText, const QString &text)
{
    setText(text);
    setLabelText(labelText);
}

void lds_label_text::setLabelTextUsr_1(const QString &labelText, const QString &text, const QString &usr_1)
{
    this->usr_1 = usr_1;
    setLabelText(labelText, text);
}

void lds_label_text::setText(const QString &text)
{
    this->txt = text;
    emit textChanged(txt);
    emit paintTextChanged();

    update();
}

void lds_label_text::appendNumber(const QString &text)
{
    if(b_text_reset) {
        setText("");
        b_text_reset = false;
    }
    if(text == "." && txt.contains(".")) return;

    if(this->txt.toDouble() == 0) {
        setText(text);
    } else {
        setText(this->txt + text);
    }
}

void lds_label_text::setValue(double value)
{
    setText(QString::number(value));
}

void lds_label_text::setBgColor(const QColor &bgColor)
{
    this->bgColor = bgColor;
}

QColor lds_label_text::skinBgColor(const QString &skin_name)
{
    if(skin_name == "obsidian")
    {
        return QColor("#2b2b2b");
    }
    if(skin_name == "green")
    {
        return QColor("#3a8200");
    }
    if(skin_name == "skyblue")
    {
        return QColor("#2291a3");
    }
    return QColor();
}

void lds_label_text::setFontColor(const QColor &fontColor)
{
    this->fontColor = fontColor;
}

double lds_label_text::value()
{
    return this->txt.toDouble();
}

QString lds_label_text::text()
{
    return txt;
}

QString lds_label_text::get_usr_1()
{
    return usr_1;
}

void lds_label_text::setLabelAlignment(int align)
{
    label_align = align;
}

void lds_label_text::setTxtAlignment(int align)
{
    txt_align = align;
}

void lds_label_text::setTxtFontSize(int size)
{
    txt_font_size = size;
}

void lds_label_text::paintEvent(QPaintEvent *)
{
    //    if(ispaint()) {
    QPainter painter(this);

    double color_light0 =0;
    double color_light1 =0;
    QColor color =bgColor;
    if(!isEnabled()) {
        color = lds::static_color_light(color, 2);
    }

    painter.fillRect(this->rect(), color);
    QRect rect0 = this->rect().adjusted(borderPix, borderPix, -borderPix, -borderPix);

    QLinearGradient linearGrad;
    linearGrad.setColorAt(0.0, lds::static_color_light(color, color_light0));
    linearGrad.setColorAt(1.0, lds::static_color_light(color, color_light1));
    linearGrad.setStart(rect0.topLeft());
    linearGrad.setFinalStop(rect0.bottomLeft());
    painter.fillRect(rect0, linearGrad);

    painter.setPen(QPen(fontColor));
    painter.drawText(rect0, label_align, paintLabel());

    QFont font = painter.font();
    if(txt_font_size  != -1)
        font.setPointSize(txt_font_size);
    painter.setFont(font);
    painter.drawText(rect0, txt_align | Qt::TextWrapAnywhere, paintText());
    //    }
}

int lds_label_text::labelFlag()
{
    return Qt::AlignLeft | Qt::AlignVCenter;
}

int lds_label_text::txtFlag()
{
    return Qt::AlignRight | Qt::AlignVCenter;
}

bool lds_label_text::ispaint()
{
    return labelText.length() > 0;
}
