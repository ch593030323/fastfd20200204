#include "lds_tableview_delegate_virtual.h"
#include <QApplication>
#include <QtDebug>
#include <QComboBox>
#include <QPainter>
#include <QTableView>
#include <QHeaderView>
#include <QScrollBar>
#include "lds.h"
#include <QDateTime>

void lds_tableview_delegate_virtual::delay_tableview_resize_column_to_content(lds_tableView *tableview, int column)
{
    //tableview->viewport()由于tableview->viewport()的实时性和准确性，这里需要在show之后，显示
    int row =  tableview->verticalScrollBar()->value();
    int count=  //tableview->verticalScrollBar()->pageStep();
            tableview->viewport()->height() / tableview->verticalHeader()->defaultSectionSize();

    QFontMetrics fm(tableview->font());
    int maxcontentw = tableview->horizontalHeader()->sectionSizeHint(column);// model->headerData(column, Qt::Horizontal).toString());
    maxcontentw += 16;
    int textw;
    QString value;
    for(int k = 0; k < count; k++) {
        if(k >= tableview->verticalHeader()->count())
            break;
        if(tableview->columnSpan(row + k, column) != 1)
            break;

        value = tableview->dg_paint_transfer(row+k, column);

        textw = fm.width(value) + 10 + 16;
        lds_tableview_delegate_virtual *d = tableview->cur_item_delegate(column);
        QAbstractItemModel *m = tableview->model();

        if(d != 0) {
            if(1 == d->resize_decimal_mode
                    && 1 == column % 2) {
                textw += tableview->maxDecimalFontWidth(fm, d->resize_decimal_max(column));
            }
        }

        if(maxcontentw < textw) {
            maxcontentw = textw;
        }
    }
    tableview->setColumnWidth(column, maxcontentw);//border--content--border
}


void lds_tableview_delegate_virtual::delay_tableview_resize_column_to_content(lds_tableView *tableview, const QList<int> rePecifyHeaderList, bool scan_all_hhead)
{
    int column =  tableview->horizontalScrollBar()->value();
    int count = 0;
    int count_width = 0;
    //rePecifyHeaderList
    //(0, 46, 1, 5, 6, 18, 11, 51, 36, 7, 9, 10, 30, 8, 16, 27, 19, 35, 37)
    if(rePecifyHeaderList.count() > 0 ) {
        //column\count
        //水平遍历所有的
        if(scan_all_hhead) {
            column = 0;
            count = tableview->horizontalHeader()->count();
        } else {//水平遍历显示的head
            for(int k = column; k < rePecifyHeaderList.count(); k++) {
                count_width += tableview->columnWidth(rePecifyHeaderList.value(k));
                if(count_width > tableview->viewport()->width()) {
                    break;
                }
                count++;
            }
        }

        for(int k = 0; k < count; k ++) {
            if(column + k >= tableview->horizontalHeader()->count()) break;
            lds_tableview_delegate_virtual::delay_tableview_resize_column_to_content(tableview, rePecifyHeaderList.value(column + k));
        }
        return;
    }
    //column\count
    //水平遍历所有的
    if(scan_all_hhead) {
        column = 0;
        count = tableview->horizontalHeader()->count();
    } else {//水平遍历显示的head
        for(int k = column; k < tableview->horizontalHeader()->count(); k ++) {
            if(tableview->isColumnHidden(k)) continue;

            count_width += tableview->columnWidth(k);
            if(count_width > tableview->viewport()->width()) {
                break;
            }
            count++;
        }
    }

    for(int k = 0; k < count; k ++) {
        if(column + k >= tableview->horizontalHeader()->count()) break;

        lds_tableview_delegate_virtual::delay_tableview_resize_column_to_content(tableview, column + k);
    }
}


lds_tableview_delegate_virtual::lds_tableview_delegate_virtual(QObject *parent) :
    QStyledItemDelegate(parent)
{
    resize_decimal_mode = 0;
    bp_fmt_price_dec = lds::func_dec_price();//lds_query_hddpos::selectValue(query, " select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_price_dec' ").toInt();
    bp_fmt_num_dec = lds::func_dec_num();//lds_query_hddpos::selectValue(query, " select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_num_dec' ").toInt();
    bp_fmt_amount_dec = lds::func_dec_amt();//lds_query_hddpos::selectValue(query, " select vch_value from cey_sys_parameter where vch_parameter = 'bp_fmt_amount_dec' ").toInt();

    b_paint_transfer = true;
}

void lds_tableview_delegate_virtual::paintText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index,  const QVariant &dd) const
{
    QStyleOptionViewItemV4 opt;
    opt = option;
    initStyleOption(&opt, index);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    const QWidget *widget = option.widget;
#else
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
    const QWidget *widget = v3? v3->widget : 0;
#endif
    QStyle *style = widget? widget->style() : QApplication::style();
    //
    QFontMetrics fm(painter->font());
    QString field_name = fieldName(index);
    QString painter_text = paint_transfer(dd, field_name);
    QRect painter_text_rect = opt.rect;
    int text_spacing = 5;

    if(false == opt.icon.isNull()) {
        QSize text_size = QSize(painter_text_rect.width() - opt.decorationSize.width() - text_spacing, painter_text_rect.height());
        painter_text_rect = QStyle::alignedRect(option.direction, Qt::AlignRight | Qt::AlignVCenter, text_size, painter_text_rect);
    }
    if(lds::isNumber(dd)) {
        opt.displayAlignment = Qt::AlignRight| Qt::AlignVCenter;
    }
    opt.state = option.state;
    opt.text = "";
    //2
    const QAbstractItemView *tableview = qobject_cast<const QAbstractItemView*>(widget);
    if((tableview == 0 ? true : tableview->editTriggers() != QTableView::NoEditTriggers)
            &&(index.flags() & Qt::ItemIsEditable)
            && (opt.state & QStyle::State_Enabled)) {
        opt.palette.setBrush(QPalette::HighlightedText, QColor("green"));
        opt.palette.setBrush(QPalette::Text, QColor("green"));
    }

    if(opt.state & QStyle::State_Selected)
        painter->setPen(opt.palette.brush(QPalette::HighlightedText).color());
    else
        painter->setPen(opt.palette.brush(QPalette::Text).color());
    //
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
    //
    if(1 == resize_decimal_mode) {//奇数是num_dish, 偶数是double
        if(lds::isNumber(dd)
                && resize_decimal_max(index.column()) > 0) {
            int dot_pos = painter_text.indexOf(".");
            LayoutDirectionSaver d(painter, Qt::LeftToRight);
            drawText(painter, opt.state, painter_text_rect.adjusted(text_spacing, 0, -fm.width(".000") -text_spacing, 0), Qt::AlignVCenter | Qt::AlignRight, painter_text.mid(0, dot_pos));
            if(dot_pos > 0) {
                drawText(painter, opt.state, painter_text_rect.adjusted(painter_text_rect.width() - fm.width(".000") -text_spacing, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, painter_text.mid(dot_pos));
            }
            return;
        }
    }
    //drawText

    drawText(painter, opt.state, painter_text_rect.adjusted(text_spacing, 0, -text_spacing, 0), opt.displayAlignment, painter_text);
}

void lds_tableview_delegate_virtual::drawText(QPainter *painter,  QStyle::State state, const QRect &painter_text_rect, Qt::Alignment displayAlignment, const QString &painter_text) const
{
    Q_UNUSED(state)

    painter->drawText(painter_text_rect, displayAlignment, painter_text);
}

void lds_tableview_delegate_virtual::paintProgressBar(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QString *text) const
{
    Q_UNUSED(text)
    const QAbstractItemModel *itemModel = index.model();
    //获得索引对应Model中的数据
    int finishedSubTaskNum = itemModel->data(index, Qt::DisplayRole).toInt();
    int subTaskNum = itemModel->data(itemModel->index(index.row(),8), Qt::DisplayRole).toInt();

    //进度条的风格选项
    QStyleOptionProgressBarV2 *editor = new QStyleOptionProgressBarV2();
    editor->rect = option.rect;
    editor->minimum = 0;
    editor->maximum = subTaskNum;
    editor->progress = finishedSubTaskNum;
    int t = finishedSubTaskNum/subTaskNum;
    editor->text = QString::number(t) + "%";
    editor->textVisible = true;
    //绘制进度条
    QApplication::style()->drawControl(QStyle::CE_ProgressBar, editor, painter);
}

void lds_tableview_delegate_virtual::paintBtn(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const QVariant &dd, int text_color) const
{
    QStyleOptionButton *editor = new QStyleOptionButton();
    editor->rect = option.rect;
    editor->state = option.state;

    if(0 == text_color) {//text
        editor->text = paint_transfer(dd, fieldName(index));
    }
    if(1 == text_color) {//color
        editor->iconSize = option.rect.size()- QSize(15, 15);
        QPixmap p(editor->iconSize);
        p.fill(dd.toString());
        editor->icon = p;
    }
    QApplication::style()->drawControl(QStyle::CE_PushButton, editor, painter);
}

void lds_tableview_delegate_virtual::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    paintText(painter, option, index, index.data());
}

QWidget *lds_tableview_delegate_virtual::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(aParent, option, index);
}

void lds_tableview_delegate_virtual::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
    emit signal_edit_commit(index);
}

void lds_tableview_delegate_virtual::setModelData(const QVariant &d, QAbstractItemModel *model, const QModelIndex &index) const
{
    model->setData(index, d);
    emit signal_edit_commit(index);
}

void lds_tableview_delegate_virtual::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

QString lds_tableview_delegate_virtual::paint_transfer(const QVariant &dd, const QString &field_name) const
{
    if(field_name == "int_deduct") {
        qDebug() << __LINE__;
    }
    if(false == b_paint_transfer)
        return dd.toString();

    if(dd.type() == QVariant::DateTime) {
        return dd.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    if(dd.type() == QVariant::Date) {
        return dd.toDateTime().toString("yyyy-MM-dd");
    }
    if(dd.type() == QVariant::Time) {
        return dd.toDateTime().toString("hh:mm:ss");
    }
    if(dd.type() == QVariant::Double) {
        return lds::septNumber(dd.toDouble(), resize_decimal_field(field_name));
    }
    switch(resize_decimal_mode) {
    case 0://正常
        break;
    case 1://奇数是num_dish, 偶数是double
        return lds::reportModelMapValue(dd, dd.toString());//
    }

    return dd.toString();
}

QString lds_tableview_delegate_virtual::fieldName(const QModelIndex &index) const
{
    QString field_name = index.model()->headerData(index.column(), Qt::Horizontal).toString();
    QString fname;
    switch(resize_decimal_mode) {
    case 0://正常
        fname = field_name;
        break;
    case 1://奇数是num_dish, 偶数是double
        if(index.column() % 2 == 1) {
            fname = index.model()->data(index.sibling(index.row(), index.column() - 1)).toString();
        } else {
            fname = field_name;
        }
        break;
    }

    return fname;
}

int lds_tableview_delegate_virtual::resize_decimal_field(const QString &field_name) const
{
    //数量
    if(field_name == "num_dish"
            || field_name == "num_num"
            || field_name == "num_back"
            || field_name == "num_material"
            ||field_name == "num_store"
            ) {
        return bp_fmt_num_dec;
    }
    //单价
    if(field_name == "num_price"
            || field_name == "num_sale_price"
            || field_name == "num_add_price"
            //            || field_name == "num_price_add"
            ||field_name == "num_store_price"
            ) {
        return bp_fmt_price_dec;
    }
    //金额
    return bp_fmt_amount_dec;
}

void lds_tableview_delegate_virtual::resize_decimal_enable()
{
    resize_decimal_mode = 1;
}

void lds_tableview_delegate_virtual::resize_decimal(int column, int maxdecimals)
{
    resize_decimal_map.insert(column, maxdecimals);
}

int lds_tableview_delegate_virtual::resize_decimal_max(int column) const
{
    return resize_decimal_map.value(column, 0);
}

void lds_tableview_delegate_virtual::disable_paint_transfer()
{
    b_paint_transfer = false;
}


//LayoutDirectionSaver
lds_tableview_delegate_virtual::LayoutDirectionSaver::LayoutDirectionSaver(QPainter *painter, Qt::LayoutDirection will_dir)
{
    keep(painter, will_dir);
}

lds_tableview_delegate_virtual::LayoutDirectionSaver::LayoutDirectionSaver()
{
    painter = 0;
}

lds_tableview_delegate_virtual::LayoutDirectionSaver::~LayoutDirectionSaver()
{
    if(painter != 0)
        painter->setLayoutDirection(old_dir);
}

void lds_tableview_delegate_virtual::LayoutDirectionSaver::keep(QPainter *painter, Qt::LayoutDirection will_dir)
{
    this->painter = painter;
    old_dir = painter->layoutDirection();
    painter->setLayoutDirection(will_dir);
}
//~LayoutDirectionSaver
