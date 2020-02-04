#include "autofontlabel.h"
#include <QResizeEvent>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

AutoFontLabel::AutoFontLabel(QWidget *parent) : QLabel(parent)
{

}

void AutoFontLabel::paintEvent(QPaintEvent *e)
{
    QStyle *style = QWidget::style();
    QPainter painter(this);
    drawFrame(&painter);
    QStyleOption opt;
    opt.initFrom(this);
    //
    QFont font = painter.font();
    font.setPixelSize(this->height() * 0.6);
    painter.setFont(font);
    //
    style->drawItemText(&painter, this->rect(), this->alignment(), opt.palette, isEnabled(), this->text(), foregroundRole());
}
