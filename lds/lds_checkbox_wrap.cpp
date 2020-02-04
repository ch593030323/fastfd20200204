#include "lds_checkbox_wrap.h"
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMouseEvent>
#include <QtDebug>

lds_checkbox_wrap::lds_checkbox_wrap(QWidget *parent) :
    QCheckBox(parent)
{
    textflag = int(Qt::TextWordWrap|Qt::AlignLeft|Qt::AlignVCenter);
}

void lds_checkbox_wrap::paintEvent(QPaintEvent *e)
{
    return QCheckBox::paintEvent(e);
//    //1
//    QStylePainter p(this);
//    QStyleOptionButton opt;
//    initStyleOption(&opt);
//    //2
//    QString text = opt.text;
//    opt.text.clear();
//    QBrush brush = p.brush();
//    //3
//    p.drawControl(QStyle::CE_CheckBox, opt);

//    //4
//    p.setBrush(brush);
//    p.drawText(textrect(), textflag,
//               text);
}

bool lds_checkbox_wrap::hitButton(const QPoint &p) const
{
    return rect().contains(p);
}

void lds_checkbox_wrap::resizeEvent(QResizeEvent *e)
{
    QSize msize =  this->style()->itemTextRect(this->fontMetrics(), QRect(0,0,textrect().width(),0), int(Qt::TextWordWrap), false,this->text()).size();
    this->setMinimumHeight(msize.expandedTo(this->sizeHint()).height());//保证显示文本的min高度
    QCheckBox::resizeEvent(e);
}

QSize lds_checkbox_wrap::sizeHint() const
{
    //    return QCheckBox::sizeHint();
    int maxfont_h = this->fontMetrics().boundingRect(QRect(), textflag, this->text()).width();
    if(this->text().count() >= 3) {
        return QSize(textleftspacing()+qMax(maxfont_h, QCheckBox::sizeHint().width()/2),
                     QCheckBox::sizeHint().height()/*保证复选框能够显示完全*/
                     );
    } else {
        return QSize(textleftspacing()+qMax(maxfont_h, QCheckBox::sizeHint().width()),
                     QCheckBox::sizeHint().height()/*保证复选框能够显示完全*/
                     );
    }
}

void lds_checkbox_wrap::mousePressEvent(QMouseEvent *e)
{
    QCheckBox::mousePressEvent(e);

}

QRect lds_checkbox_wrap::textrect() const
{
    return this->rect().adjusted(textleftspacing(), 0, 0, 0);

}

int lds_checkbox_wrap::textleftspacing() const
{
    return 2*style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing)+this->iconSize().width();
}
