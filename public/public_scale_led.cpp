#include "public_scale_led.h"
#include <QPainter>

public_scale_led::public_scale_led(QWidget *parent) :
    QWidget(parent)
{
}

void public_scale_led::appendCell(const QString &key, const QString &title, const QString &suffix)
{
    cellData k;
    k.key = key;
    k.title = title;
    k.suffix = suffix;
    k.number = "000.000";

    cell_list << k;
}

void public_scale_led::updateCellNumber(const QString &key, const QString &number)
{
    for(int k = 0; k < cell_list.count(); k++) {
        if(cell_list[k].key == key) {
            cell_list[k].number = number;
        }
    }
    update();
}

void public_scale_led::updateCellNumber(int index, const QString &number)
{
    if(index >=0 && index < cell_list.count()) {
        cell_list[index].number = number;
        update();
    }
}

QString public_scale_led::cellNumber(const QString &key)
{
    for(int k = 0; k < cell_list.count(); k++) {
        if(cell_list[k].key == key) {
            return cell_list[k].number;
        }
    }
    return "";
}

void public_scale_led::updateCellTitle(const QString &key, const QString &title)
{
    for(int k = 0; k < cell_list.count(); k++) {
        if(cell_list[k].key == key) {
            cell_list[k].title = title;
        }
    }
    update();
}

QColor public_scale_led::greenColor()
{
    return QColor(9, 187, 7);
}

void public_scale_led::setFONT_DIGIT_Family(const QString &FONT_DIGIT_Family)
{
    this->FONT_DIGIT_Family = FONT_DIGIT_Family;
}

const QList<public_scale_led::cellData> &public_scale_led::get_cell_list_ref()
{
    return cell_list;
}

void public_scale_led::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    QFont font = this->font();
    QFont digitfont = font;

    digitfont.setFamily(FONT_DIGIT_Family);

    QRect rect = this->rect();
    int height_num = rect.height() * 0.72;
    int height_label = rect.height()  - height_num;
    font.setPixelSize(height_label - 10);
    digitfont.setPixelSize(height_num - 18);
    QFontMetrics digitfm(digitfont);
    int cell_width = digitfm.width("0000.000");
    int cell_width_right = digitfm.width("0");
    cell_width = cell_width + cell_width_right;

    for(int k = 0; k < cell_list.count(); k++)
    {
        QRect r = QRect(rect.x() + cell_width * k, rect.y(), cell_width, height_num);
        //draw border
        r.adjust(1, 1, -1, -1);
        painter.fillRect(r.x(), r.y() - 1, r.width() + 1, 1, QColor(206, 206, 206, 120));
        painter.fillRect(r.x() + r.width(), r.y(), 1, r.height() + 1, QColor(206, 206, 206, 120));
        painter.fillRect(r.x() - 1, r.y() + r.height(), r.width() + 1, 1, QColor(206, 206, 206, 120));
        painter.fillRect(r.x() - 1, r.y() - 1, 1, r.height() + 1, QColor(206, 206, 206, 120));

        //draw number
        painter.setPen(QPen(greenColor(), 2));
        painter.setFont(digitfont);
        painter.drawText(r.adjusted(0, 0, -cell_width_right, 0), int(Qt::AlignRight | Qt::AlignVCenter), cell_list[k].number);

        //draw suffix
        painter.setFont(font);
        painter.setPen(QColor(141, 141, 141));
        painter.drawText(QRect(r.x(), r.y(), r.width(), digitfm.height() / 2 + height_num / 2- digitfm.descent()), int(Qt::AlignRight | Qt::AlignBottom), cell_list[k].suffix);

        r = QRect(rect.x() + cell_width * k, rect.y() + height_num, cell_width, height_label);
        //draw border
        r.adjust(1, 1, -1, -1);
        painter.fillRect(r.x(), r.y() - 1, r.width() + 1, 1, QColor(206, 206, 206, 120));
        painter.fillRect(r.x() + r.width(), r.y(), 1, r.height() + 1, QColor(206, 206, 206, 120));
        painter.fillRect(r.x() - 1, r.y() + r.height(), r.width() + 1, 1, QColor(206, 206, 206, 120));
        painter.fillRect(r.x() - 1, r.y() - 1, 1, r.height() + 1, QColor(206, 206, 206, 120));
        painter.fillRect(r, QColor(165, 186, 144));

        //draw title
        painter.setPen(QPen(QColor("white"), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawText(r, int(Qt::AlignCenter), cell_list[k].title);
    }
}
