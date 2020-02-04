#include "fexpandmain_tableview.h"
#include <QtDebug>
#include <QHeaderView>
#include <QPainter>
#include <QApplication>
#include "lds_model_sqltablemodel.h"
#include "fexpandmain_2.h"
#include "math.h"
#include "fexpandmain_model_sqltablemodel_data.h"

//QRect fexpandmain_tableview::static_rect0;
//QRect fexpandmain_tableview::static_rect1;

//QRect fexpandmain_tableview::static_rect2;
//QRect fexpandmain_tableview::static_rect3;

//QRect fexpandmain_tableview::static_rect4;
//QRect fexpandmain_tableview::static_rect5;

//QRect fexpandmain_tableview::static_rect6;
//QRect fexpandmain_tableview::static_rect7;

//QRect fexpandmain_tableview::static_rect8;
//QRect fexpandmain_tableview::static_rect9;
//QRect fexpandmain_tableview::static_rect10;
//QRect fexpandmain_tableview::static_rect11;
//QRect fexpandmain_tableview::static_rect12;


int fexpandmain_tableview::text_height;
QString fexpandmain_tableview::show_full_name_cut;
QString fexpandmain_tableview::show_full_cook_cut;


fexpandmain_tableview::fexpandmain_tableview(QWidget *parent): fexpandmain_table(parent)
{
    this->setProperty("outer_stylesheet", "fexpandmain_tableview");
    fexpandmain_delegate *d = new fexpandmain_delegate(this);
    d->keepConnect(this);
    this->setItemDelegate(d);
    text_height = QFontMetrics(this->font()).height() + 5 + 5;
    this->verticalHeader()->setDefaultSectionSize(text_height);

    QTimer::singleShot(1000, this, SLOT(updatecurIndex()));
}

fexpandmain_tableview::~fexpandmain_tableview()
{
}

void fexpandmain_tableview::mousePressEvent(QMouseEvent *event)
{
    lds_tableview_timeout::mousePressEvent(event);
}

void fexpandmain_tableview::mouseReleaseEvent(QMouseEvent *event)
{
    lds_tableview_timeout::mouseReleaseEvent(event);
    int row = pressIndex.row();
    if(pressIndex.isValid()) {
        QPoint p_off = event->pos() - QPoint(columnViewportPosition(pressIndex.column()), rowViewportPosition(row));
        if(nameRect().contains(p_off)) {
            fexpandmain_model_sqltablemodel *model = qobject_cast<fexpandmain_model_sqltablemodel *>(this->model());
            if(model->can_qt_P_dish_fold(row)) {
                emit signal_suit_fold(pressIndex);
            }
        }
    }
}

void fexpandmain_tableview::updatecurIndex()
{
    if(this->isActiveWindow())
        update(this->currentIndex());
    QTimer::singleShot(1000, this, SLOT(updatecurIndex()));
}

void fexpandmain_tableview::setUnderline(QPainter *painter, bool underline)
{
    QFont font = painter->font();
    font.setUnderline(underline);
    painter->setFont(font);
}

void fexpandmain_tableview::drawText(QPainter *painter, const QRect &rect, int flags, const QString &text, bool underline)
{
    if(underline) setUnderline(painter, true);
    painter->drawText(rect, flags, text);
    if(underline) setUnderline(painter, false);
}

void fexpandmain_tableview::to_show_full_zero()
{
    show_full_name_cut = "";
    show_full_cook_cut = "";
}

int fexpandmain_tableview::mutableHeight(int row)
{
    int rowh = text_height;
    const fexpandmain_model_sqltablemodel_data *tablemodel = static_cast<const fexpandmain_model_sqltablemodel_data *>(this->model());
    if(tablemodel == 0)
        return text_height;

    vch_print_memoData vch_print_memo(tablemodel->model_data(row, "vch_print_memo").toString());
    double num_price_add = tablemodel->model_data(row, "num_price_add").toDouble();
    QString dish_desc = tablemodel->get_dish_desc(row);



    if(vch_print_memo.count() > 0 || num_price_add != 0) {
        rowh += text_height;
    }
    if(dish_desc.length() != 0) {
        rowh += text_height;
    }
    return rowh;
}

void fexpandmain_tableview::updateTableHeight(int row)
{
    if(row < 0)
        return;

    setRowHeight(row, mutableHeight(row));
}

void fexpandmain_tableview::updateTableHeightAll()
{
    for(int row = 0; row < this->verticalHeader()->count(); row ++) {
        this->updateTableHeight(row);
    }
}

int fexpandmain_tableview::intIdWidth()
{
    return 30 + 3;
}

QRect fexpandmain_tableview::intIdRect()
{
    return alignRect(QRect(3, 0, 30, text_height));
}

QRect fexpandmain_tableview::nameRect()
{
    return alignRect(QRect(intIdWidth(), 0, lds::PAY_LEFT_SPACE * 0.55 - intIdWidth(), text_height));
}

QRect fexpandmain_tableview::numRect()
{
    return alignRect(QRect(lds::PAY_LEFT_SPACE * 0.55, 0, lds::PAY_LEFT_SPACE * 0.2, text_height));
}

QRect fexpandmain_tableview::totalRect()
{
    return alignRect(QRect(lds::PAY_LEFT_SPACE * 0.75,  0, lds::PAY_LEFT_SPACE * 0.25, text_height));
}

QRect fexpandmain_tableview::memoNameRect()
{
    return alignRect(QRect(intIdWidth(), 0, lds::PAY_LEFT_SPACE * 0.75 - intIdWidth(), text_height));
}

QRect fexpandmain_tableview::memoTotalRect()
{
    return alignRect(QRect(lds::PAY_LEFT_SPACE * 0.75, 0, lds::PAY_LEFT_SPACE * 0.25, text_height));
}

QRect fexpandmain_tableview::dishDescRect()
{
    return alignRect(QRect(intIdWidth(), 0 * 2, lds::PAY_LEFT_SPACE - intIdWidth(),  text_height));
}

QRect fexpandmain_tableview::alignRect(const QRect &rect)
{
    if(qApp->layoutDirection() == Qt::RightToLeft)
        return QRect(lds::PAY_LEFT_SPACE - rect.x() - rect.width(), rect.y(), rect.width(), rect.height());
    return rect;
}

void fexpandmain_tableview::drawNum(const fexpandmain_model_sqltablemodel_data *tablemodel, QPainter *painter, const QStyleOptionViewItem &option, int row)
{
    int flags = Qt::AlignRight | Qt::AlignVCenter;
    QRect rect = numRect();
    QString text = lds::septNumber(tablemodel->model_data_NIBOLAN(row, "num_num-num_back"), tablemodel->get_cey_sys_parameter_value("bp_fmt_num_dec", 3).toInt()
                                   );
    painter->drawText(rect.translated(0, option.rect.y()), flags, text);
}

void fexpandmain_tableview::drawTotal(const fexpandmain_model_sqltablemodel_data *tablemodel, QPainter *painter, const QStyleOptionViewItem &option, int row)
{
    int flags = Qt::AlignRight | Qt::AlignVCenter;
    QRect rect = totalRect();
    QString text = lds::septNumber(tablemodel->model_data_IF(row, "if(ch_presentflag = 'Y', 0, (num_num-num_back)*num_price*int_discount*0.01)"), tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()
                                   );
    painter->drawText(rect.translated(0, option.rect.y()), flags, text);
}

QPainterPath fexpandmain_tableview::pathEllipse(const QRectF &rect)
{
    QPainterPath path;
    path.addEllipse(rect);
    return path;
}
//QPainterPath fexpandmain_tableview::pathStar()
//{
//    qreal off = 1.4;
//    QPainterPath path;
//    path.moveTo( QPointF(6.9, 13.7) );
//    path.lineTo( QPointF(12.78, 13.82) );
//    path.lineTo( QPointF(9.46, 8.76) );
//    path.lineTo( QPointF(11.38, 8.16) );
//    path.lineTo( QPointF(14.7, 13.96) );
//    path.lineTo( QPointF(18.1, 8.16) );
//    path.lineTo( QPointF(20.02, 8.8) );
//    path.lineTo( QPointF(16.7, 13.82) );
//    path.lineTo( QPointF(22.5, 13.82) );
//    path.lineTo( QPointF(22.5, 15.62) );
//    path.lineTo( QPointF(16.7, 15.62) );
//    path.lineTo( QPointF(19.98, 21.72) );
//    path.lineTo( QPointF(18.1, 22.24) );
//    path.lineTo( QPointF(14.78, 16.56) );
//    path.lineTo( QPointF(11.38, 22.24) );
//    path.lineTo( QPointF(9.5, 21.68) );
//    path.lineTo( QPointF(12.86, 15.62) );
//    path.lineTo( QPointF(6.9, 15.62) );
//    return path;
//}

//QPainterPath fexpandmain_tableview::pathAdd()
//{
//    QPainterPath path;
//    path.moveTo( QPointF(6.9, 13.5) );
//    path.lineTo( QPointF(13.5, 13.5) );
//    path.lineTo( QPointF(13.5, 6.9) );
//    path.lineTo( QPointF(15.9, 6.9) );
//    path.lineTo( QPointF(15.9, 13.5) );
//    path.lineTo( QPointF(22.5, 13.5) );
//    path.lineTo( QPointF(22.5, 15.9) );
//    path.lineTo( QPointF(15.9, 15.9) );
//    path.lineTo( QPointF(15.9, 22.5) );
//    path.lineTo( QPointF(13.5, 22.5) );
//    path.lineTo( QPointF(13.5, 15.9) );
//    path.lineTo( QPointF(6.9, 15.9) );


//    return path;
//}

//QPainterPath fexpandmain_tableview::pathSub()
//{
//    QPainterPath path;
//    path.moveTo( QPointF(6.9, 13.5) );
//    path.lineTo( QPointF(22.5, 13.5) );
//    path.lineTo( QPointF(22.5, 15.9) );
//    path.lineTo( QPointF(6.9, 15.9) );
//    return path;
//}

//QPainterPath fexpandmain_tableview::pathDot()
//{
//    QPainterPath path;
//    path.addRect(6.5, 13.5, 2.8, 2.8);
//    path.addRect(13.5, 13.5, 2.8, 2.8);
//    path.addRect(20.5, 13.5, 2.8, 2.8);
//    return path;
//}

//QPainterPath fexpandmain_tableview::pathDel()
//{
//    QPainterPath path;

//    QRect boundingRect(-9, 0, 30, 20);
//    qreal startAngle = 261;
//    qreal sweepLength = 110;
//    path.moveTo(QPointF(3.6, 20));
//    path.arcTo(boundingRect, startAngle, sweepLength);
//    QRect boundingRect2(-19, -10, 40, 30);
//    qreal startAngle2 = 350;
//    qreal sweepLength2 = -72;
//    path.arcTo(boundingRect2, startAngle2, sweepLength2);
//    return path;
//}

//QPainterPath fexpandmain_tableview::pathDel2()
//{
//    QPainterPath path;
//    path.moveTo( QPointF(8.66, 10.13) );
//    path.lineTo( QPointF(10.33, 8.46) );
//    path.lineTo( QPointF(23, 21) );
//    path.lineTo( QPointF(21.4, 22.6) );

//    return path;
//}

fexpandmain_tableview::fexpandmain_delegate::fexpandmain_delegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void fexpandmain_tableview::fexpandmain_delegate::keepConnect(fexpandmain_tableview *tableview)
{
    divColor = &tableview->divColor;
    nameColor = &tableview->nameColor;
    nameSubColor = &tableview->nameSubColor;
    border = &tableview->border;
    defColor = &tableview->defColor;
    itemBgColor = &tableview->itemBgColor;
}

void fexpandmain_tableview::fexpandmain_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const fexpandmain_model_sqltablemodel_data *tablemodel = static_cast<const fexpandmain_model_sqltablemodel_data *>(index.model());
    int row = index.row();
    vch_print_memoData vch_print_memo(tablemodel->model_data(row, "vch_print_memo").toString());
    double num_price_add = tablemodel->model_data(row, "num_price_add").toDouble();
    QString dish_desc = tablemodel->get_dish_desc(row);

    if(option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor("#ff546b"));
        painter->setPen("white");
    } else {
        painter->fillRect(option.rect, *itemBgColor);
        painter->fillRect(QRect(option.rect.left(), option.rect.bottom(), option.rect.width(), 1), *divColor);
        painter->setPen(*defColor);
    }

    QString int_id_str = tablemodel->tablename_kvmap_value("fexpandmain_delegate_row", QString::number(row)).toString();
    QRect name_rect = nameRect();
    QFontMetrics fm(painter->font());
    int xoff = 0;
    int yoff = option.rect.y();

    if(tablemodel->opera(row) == lds_model_sqltablemodel::OpInsert) {
        int_id_str += ".";
    }
    if(tablemodel->can_qt_Y_dish_fold(row)) {
    } else {
        painter->drawText(intIdRect().translated(0, yoff),  (Qt::AlignLeft | Qt::AlignVCenter), int_id_str);
    }
    QString vch_dishname = tablemodel->get_cey_bt_dish_value(tablemodel->model_data(row, "ch_dishno").toString(), "vch_dishname").toString();
    drawText(painter, nameRect().translated(0, yoff),
             (Qt::AlignLeft | Qt::AlignVCenter),
             show_full_going_name(fm, vch_dishname, name_rect, show_full_name_cut, option.state),
             tablemodel->can_qt_P_dish_fold(row)
             );

    drawNum(tablemodel, painter, option, row);
    if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {
    } else {
        drawTotal(tablemodel, painter, option, row);
    }

    if(vch_print_memo.count() > 0 || num_price_add != 0) {
        yoff += text_height;
        QRect memo_rect = memoNameRect().translated(xoff, yoff).adjusted(0, 0, -xoff, 0);
        painter->drawText(memo_rect, (Qt::AlignLeft | Qt::AlignVCenter),
                          show_full_going_name(fm, QObject::tr("做法") + ":" + vch_print_memo.jsonToString(), memo_rect, show_full_cook_cut, option.state));
        if(tablemodel->model_data(row, "ch_suitflag").toString() == "Y") {
        } else {
            painter->drawText(memoTotalRect().translated(0, yoff), (Qt::AlignRight | Qt::AlignVCenter), QString("%2")
                              .arg(lds::septNumber(num_price_add, tablemodel->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt()))
                              );
        }
    }
    if(dish_desc.length() > 0) {
        yoff += text_height;
        painter->drawText(dishDescRect().translated(xoff, yoff).adjusted(0, 0, -xoff, 0), (Qt::AlignLeft | Qt::AlignVCenter), dish_desc);
    }
}

QString fexpandmain_tableview::fexpandmain_delegate::show_full_going_name(QFontMetrics fm,  QString text, const QRect &rect, QString &cut, const QStyle::State &state) const
{
    if(state & QStyle::State_Selected) {
        int name_cut = cut.toInt();
        if(fm.width(text.mid(name_cut)) > rect.width()) {
            name_cut = cut.toInt() + 1;
        } else {
            name_cut = -2;
        }
        cut = QString::number(name_cut);
        text.remove(0, name_cut);
    }
    return text;
}
