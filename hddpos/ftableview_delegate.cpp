#include "ftableview_delegate.h"
#include <QPainter>
#include <QtDebug>
#include "ftableview.h"
#include "lds.h"
#include <QApplication>
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>

const int ftableview_delegate::CELL_BORDER_SPACE = 1;

void ftableview_delegate::setTableViewImageScaled(double imageScaled, QTableView *tableview)
{
    ftableview_delegate *d = qobject_cast<ftableview_delegate *>(tableview->itemDelegate());
    if(d) {
        d->setImageScaled(imageScaled);
        tableview->update();
    }
}

double ftableview_delegate::getTableViewImageScaled(QTableView *tableview)
{
    ftableview_delegate *d = qobject_cast<ftableview_delegate *>(tableview->itemDelegate());
    if(d) {
        return d->getImageScaled();
    }
    return 0;
}

ftableview_delegate::ftableview_delegate(ftableview *tableview)
    :QItemDelegate(tableview)
    , parentView(tableview)
{
    init();
}

ftableview_delegate::ftableview_delegate(ftableview *tableview, bool indexisEmptyDrawGrad)
    :QItemDelegate(tableview)
    , parentView(tableview)
{
    init();
    keepConnect(tableview);
    this->indexisEmptyDrawGrad = indexisEmptyDrawGrad;
}

ftableview_delegate::~ftableview_delegate()
{
    qDebug() << __FUNCTION__;
}

void ftableview_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString textprice = index.data(Qt::UserRole).toString();
    QVariant ch_dishno = index.data(Qt::UserRole + 1);
    QVariant image = index.data(Qt::UserRole + 2);
    int ch_groupno_count = index.data(Qt::UserRole + 3/*组数量*/).toInt();//库存剩余、空：不记库存，不空：计算库存
    QString st_remain = index.data(Qt::UserRole + 4).toString();//库存剩余、空：不记库存，不空：计算库存
    double  dishcount = index.data(Qt::UserRole + 5).toDouble();// "" 0, 1
    QString ischecked = index.data(Qt::UserRole + 6).toString();// "" 0, 1
    QString num_item_warn = index.data(Qt::UserRole + 7).toString();// "" 0, 1
    QStringList item_inventory_list = index.data(Qt::UserRole + 8).toString().split(",", QString::SkipEmptyParts);// "" 0, 1
    QString vch_color = index.data(Qt::UserRole + 9).toString();// "" 0, 1
    QString text2 = index.data(Qt::UserRole + 10).toString();// "" 0, 1
    QString item_inventory;
    int item_inventory_off = 0;
    QRect parent_rect = option.rect;//.adjusted(0, 0, -br, -br);

    QString text = textprice;
    QString price;
    int textprice_split = textprice.lastIndexOf("\n");
    if(textprice_split > 0) {
        text = textprice.mid(0, textprice_split).trimmed();
        price = textprice.mid(textprice_split + 1);
    }
    QFont font = painter->font();
    QFontMetrics fontm(font);
    int textheight = fontm.height();
    QRect rect2(parent_rect.topLeft(), QSize(parent_rect.width(), textheight));
    painter->setRenderHint(QPainter::Antialiasing);

    //点击效果
    /////////////
    bool cellBr =
            bgBorderColor
            && bgBorderColor->isValid()
            && bgBorderColor->alpha() != 0;
    QLinearGradient linearGrad = getLinerBypropety(option.state, vch_color);
    linearGrad.setStart(parent_rect.topLeft());
    linearGrad.setFinalStop(parent_rect.bottomLeft());
    //
    if(b_shwoGrid) {
        drawBorder(painter, drawTableViewLeftTopBorder, index, parent_rect, *bgNormalColor10);
    }
    if(ch_dishno.isValid() == false) {
        if(indexisEmptyDrawGrad) {
            //paintcell
            fillRect(painter, index, parent_rect, linearGrad);
            if(cellBr)
                drawBorder(painter, drawTableViewLeftTopBorder, index, parent_rect, *bgBorderColor);
            //~paintcell
        }
        return;
    }

    //paintcell
//    if(option.state & QStyle::State_Enabled) {
        fillRect(painter, index, parent_rect, linearGrad);
//    }
    if(cellBr)
        drawBorder(painter, drawTableViewLeftTopBorder, index, parent_rect, *bgBorderColor);
    //~paintcell

    //add
    if(ch_dishno == "-3") {
        painter->scale(2.0, 2.0);
        painter->drawText(QRect(parent_rect.x() / 2.0, parent_rect.y() / 2.0, parent_rect.width() / 2.0, parent_rect.height() / 2.0), Qt::AlignCenter, "+");
        painter->scale(1/2.0, 1/2.0);
        return;
    }
    //text image
    QString text0 = text;
    QRect rect0 = parent_rect;
    int align0 = int(Qt::AlignCenter|Qt::TextWordWrap);
    if(textColor)
        painter->setPen(*textColor);
    if(text2.count() > 0)
        text0 = text0 + "\n" + text2;

    //////////////////////
    if(ch_dishno.toInt() >= 0 && icon_style == AutoIcon) {
        QPixmap pixmap = qvariant_cast<QPixmap>(image);
        if(pixmap.isNull()) {
            if(price.length() > 0) {
                text0 = text0 + "\n" + pay_unit_prefix + price;
            }
        } else {
            align0 = int(Qt::AlignCenter|Qt::TextWordWrap);

            int textheight = parent_rect.height()*1.0/5;
            QRect rectpixmap = parent_rect.adjusted(imageMargin,imageMargin + textheight,-imageMargin,-imageMargin-textheight);
            pixmap = pixmap.scaled(rectpixmap.size() * imageScaled, Qt::KeepAspectRatio);

            painter->drawPixmap(QStyle::alignedRect(option.direction, Qt::AlignCenter, pixmap.size(), rectpixmap), pixmap);
            rect0 = parent_rect.adjusted(0,  parent_rect.height() - fontm.height() * (text0.count("\n") + 1),  0, 0);
            //draw price
            painter->drawText(parent_rect.adjusted(0,  0,  0, -(parent_rect.height() - textheight) ), align0,  pay_unit_prefix + price);
        }
    }
    //////////////////////////
    painter->drawText(rect0, align0,  text0);

    if(ch_groupno_count > 1) {
        painter->drawText(rect2, Qt::AlignVCenter|Qt::AlignLeft, QString::number(ch_groupno_count));
    }

    if("1" == ischecked) {
        QString pg_path = ":/image/widgets_pic/check_on.png";
        if(temp_save_pixmap_map.find(pg_path) == temp_save_pixmap_map.end()) {
            temp_save_pixmap_map.insert(pg_path, QPixmap(pg_path).scaledToWidth(30, Qt::SmoothTransformation));
        }
        painter->drawPixmap(QStyle::alignedRect(option.direction, Qt::AlignRight | Qt::AlignTop, temp_save_pixmap_map.value(pg_path).size(), parent_rect), temp_save_pixmap_map.value(pg_path));
    }


    if(ch_dishno == "-3") {//商品类别，新增
        painter->setPen(QPen(*textColor, 3, Qt::DashDotDotLine));
        font.setPixelSize(parent_rect.height()*2/3);
        painter->setFont(font);
        painter->drawText(parent_rect, Qt::AlignCenter, "+");
    }

    if(lds::sysconf->value("system_setting/inventory_qt_show_info", false).toBool()
            && !st_remain.isEmpty()) {
        QString table_desc;
        QString table_color;
        if(st_remain.toFloat() > 0) {
            if(st_remain.toFloat() <= num_item_warn.toFloat()) {
                if("green" == lds::sysconf->value("system_setting/skin_name", "obsidian").toString()) {
                    table_color = "#ff8b00";
                } else {
                    table_color = "yellow";
                }
                table_desc = QObject::tr("库存") + ":" +QString::number(st_remain.toDouble())+QObject::tr("请补仓");
            } else {
                if("obsidian" == lds::sysconf->value("system_setting/skin_name", "obsidian").toString()) {
                    table_color = "cyan";
                } else {
                    table_color = "green";
                }
                table_desc = QObject::tr("库存") + ":" +QString::number(st_remain.toDouble());
            }
        } else {
            table_color = "red";
            table_desc = QObject::tr("售完");
        }

        painter->setPen(table_color);
        painter->drawText(rect2, Qt::AlignVCenter|Qt::AlignRight, table_desc);
    }
    /////////////



    /// 原料库存预警
    //13012131231213312
    if(lds::sysconf->value("system_setting/inventory_qt_show_info", false).toBool()) {
        foreach(const QString &k, item_inventory_list) {
            QString k0 = k.split("/").value(1);
            QString k1 = k.split("/").value(2);
            if(k0.isEmpty()) {
                item_inventory += "3";
                continue;
            }
            if(k0.toFloat() < 0) {
                item_inventory += "0";
                continue;
            }
            if(k0.toFloat() < k1.toFloat()) {
                item_inventory += "1";
                continue;
            }
            if(k0.toFloat() >= k1.toFloat()) {
                item_inventory += "2";
                continue;
            }
        }
        qSort(item_inventory.begin(), item_inventory.end(), qLess<QChar>());
        foreach(const QChar &c, item_inventory) {
            QColor color;
            //若是y小于top
            int item_inventory_y = parent_rect.bottom() - item_inventory_off * 5 - 5;
            if(item_inventory_y < parent_rect.top()) {
                break;
            }
            if(c == '0') {
                color = QColor("red");
            }
            if(c == '1') {
                color = QColor("yellow");
            }
            if(c == '2') {
                color = QColor("green");
            }
            if(c == '3') {
                color = QColor("gray");
            }
            painter->fillRect(QRect(parent_rect.right() - 5, item_inventory_y, 5, 5), color);
            item_inventory_off++;
        }
        ///~原料库存预警
    }
}

void ftableview_delegate::setIconStyle(ftableview_delegate::IconStyle style)
{
    icon_style = style;
}

void ftableview_delegate::setImageScaled(double imageScaled)
{
    this->imageScaled = imageScaled;
}

double ftableview_delegate::getImageScaled()
{
    return imageScaled;
}

QLinearGradient ftableview_delegate::getLinerBypropety(QStyle::State state, const QString &vch_color) const
{
    QLinearGradient retv;
    QColor color;
    double color_light1;
    double color_light0;

    //默认
    color = QColor(vch_color);
    if(!color.isValid()) {
        color = *bgNormalColor10;
    }
    color_light1 = colorLight;
    color_light0 = 0;
    if(true == *bgNormalPlain)
        color_light1 = 0;

    //点击
    if(state &QStyle::State_Selected) {
        color = QColor("#ff546b");
        color_light1 = 0;
        color_light0 = 0;
        goto end;
    }
//    if(state & QStyle::State_Enabled) {
//    } else {
//        color_light1 = colorLight;
//        color_light0 = colorLight;
//        goto end;
//    }

end:
    retv.setColorAt(0.0, lds::static_color_light(color, color_light1));
    retv.setColorAt(1.0, lds::static_color_light(color, color_light0));

    return retv;
}

void ftableview_delegate::keepConnect(ftableview *tableview)
{
    bgNormalColor10 = &tableview->bgNormalColor10;
    bgNormalPlain = &tableview->bgNormalPlain;
    textColor = &tableview->textColor;
    bgBorderColor = &tableview->bgBorderColor;
    tableview_property = tableview->property("outer_stylesheet").toString();
}

void ftableview_delegate::enableRoundRected()
{
}

void ftableview_delegate::disableDrawBorder()
{
}

void ftableview_delegate::setColorLight(double light)
{
    colorLight = light;
}

void ftableview_delegate::setImageMargin(int m)
{
    imageMargin = m;
}

void ftableview_delegate::set_pay_unit_prefix(const QString &pay_unit_prefix)
{
    //    set_pay_unit_prefix(n_func::f_get_sysparm("pay_unit_prefix", " "));
    this->pay_unit_prefix = pay_unit_prefix;
}

QPainterPath ftableview_delegate::roundedRect(const QRect &rect, qreal xRadius, qreal yRadius) const
{
    QPainterPath path;
    path.addRoundedRect(rect, xRadius, yRadius);
    return path;
}

void ftableview_delegate::init()
{
    imageScaled = 1.0;

    textColor= 0;
    bgNormalColor10 = 0;
    bgNormalPlain = 0;
    bgBorderColor = 0;
    icon_style = AutoIcon;
    colorLight = 2;
    imageMargin = 5;
}

void ftableview_delegate::drawBorder(QPainter *p, bool drawTableViewLeftTopBorder, const QModelIndex &index, const QRect &rect, QColor color)
{
    const int br = CELL_BORDER_SPACE;
    if(drawTableViewLeftTopBorder) {
        if(index.column() == 0) {
            p->fillRect(QRect(rect.left(), rect.top(), br, rect.height()), color);
        }
        if(index.row() == 0) {
            p->fillRect(QRect(rect.left(), rect.top(), rect.width(), br), color);
        }
    }

    p->fillRect(QRect(rect.left(), rect.bottom(), rect.width(), br), color);
    p->fillRect(QRect(rect.right(), rect.top(), br, rect.height()), color);
}

void ftableview_delegate::fillRect(QPainter *p, const QModelIndex &index, const QRect &rect, QBrush color)
{
    const int br = CELL_BORDER_SPACE;
    QRect r = rect;
    if(index.column() == 0) {
        r.adjust(br, 0, 0, 0);
    }
    if(index.row() == 0) {
        r.adjust(0, br, 0, 0);
    }

    p->fillRect(QRect(r.x(), r.y(), r.width() - br, r.height() - br), color);
}

void ftableview_delegate::fillRect(QPainter *p, const QModelIndex &index, const QRect &rect, QColor color)
{
    const int br = CELL_BORDER_SPACE;
    QRect r = rect;
    if(index.column() == 0) {
        r.adjust(br, 0, 0, 0);
    }
    if(index.row() == 0) {
        r.adjust(0, br, 0, 0);
    }

    p->fillRect(QRect(r.x(), r.y(), r.width() - br, r.height() - br), color);
}
