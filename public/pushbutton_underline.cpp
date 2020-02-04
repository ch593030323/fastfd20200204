#include "pushbutton_underline.h"
#include <QtDebug>

pushbutton_underline::pushbutton_underline(QWidget *parent) : QPushButton(parent)
{
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QString text_align = "text-align:left;";
    if(this->layoutDirection() == Qt::RightToLeft) {
        text_align = "text-align:right;";
    }
    this->setStyleSheet(
                "QPushButton{color:#ff546b;text-decoration: underline;" + text_align +"}"
                "QPushButton:disabled{color:gray;}"
                );
    this->setFlat(true);
    this->setFocusPolicy(Qt::NoFocus);

    //    xp1 = 0;
    //    yp1 = 3;
    //    xp2 = -3;
    //    yp2 = -3;
    //    textflags = int(Qt::AlignLeft|Qt::AlignVCenter|Qt::TextWordWrap);
    //    b_enable_frame = false;
}

void pushbutton_underline::setText(const QString &text)
{
    QPushButton::setText(text);
    //    qDebug() << __FILE__ << __LINE__ << QFontMetrics(this->font()).width(text) << text;
    //    this->setFixedWidth(QFontMetrics(this->font()).width(text));//
}

