#include "lds_tableview_virtual.h"
#include <QScrollBar>
#include "lds_tableview_delegate_virtual.h"
#include "lds_tableview_repecify.h"
#include <QEvent>
#include <QStylePainter>
#include <QApplication>
#include <QAbstractButton>
#include <QTime>
#include <QTableWidget>

int lds_tableview_virtual::hheight = -1;
int lds_tableview_virtual::hscrollheight = -1;

lds_tableview_virtual::lds_tableview_virtual(QWidget *parent)
    :QTableView(parent)
{
    corner_button = 0;
    is_show_nest = false;
}

QList<int> lds_tableview_virtual::getSelectedRowList()
{
    QModelIndexList indexlist = selectionModel()->selectedRows();
    QList<int> row_list;
    foreach(const QModelIndex &index, indexlist)
        row_list.append(index.row());
    return row_list;
}

QList<int> lds_tableview_virtual::getSelectedRowListDesc()
{
    QModelIndexList indexlist = selectionModel()->selectedRows();
    QList<int> row_list;
    foreach(const QModelIndex &index, indexlist)
        row_list.append(index.row());
    qSort(row_list.begin(), row_list.end(), qGreater<int>());

    return row_list;
}

QList<int> lds_tableview_virtual::getSelectedRowListAsc()
{
    QModelIndexList indexlist = selectionModel()->selectedRows();
    QList<int> row_list;
    foreach(const QModelIndex &index, indexlist)
        row_list.append(index.row());
    qSort(row_list.begin(), row_list.end(), qLess<int>());

    return row_list;
}

void lds_tableview_virtual::setHeaderResizeMode(Qt::Orientation o, QHeaderView::ResizeMode m)
{
    if(o == Qt::Vertical) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        verticalHeader()->setSectionResizeMode(m);
#else
        verticalHeader()->setResizeMode(m);
#endif
    }
    if(o == Qt::Horizontal) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        horizontalHeader()->setSectionResizeMode(m);
#else
        horizontalHeader()->setResizeMode(m);
#endif
    }
}

QList<int> lds_tableview_virtual::rePecifyHeader(const QStringList &tos)
{
    QList<int> int_tos;
    for(int k = 0; k < tos.count(); k++) {
        int_tos<< -1;
    }
    for(int k = 0; k < this->model()->columnCount(); k++) {
        int index = tos.indexOf(this->model()->headerData(k, Qt::Horizontal).toString());
        if(index >= 0) {
            int_tos[index] = k;
        }
    }
    return rePecifyHeader(int_tos);
}

QList<int> lds_tableview_virtual::rePecifyHeader(const QList<int> &tos)
{
    rePecifyHeaderList = lds_tableview_repecify::rePecifyHeader(this, tos);
    return rePecifyHeaderList;
}

QList<int> lds_tableview_virtual::horizontalShowedHeaderList()
{
    QList<int> columnList = rePecifyHeaderList;
    if(rePecifyHeaderList.count() == 0) {
        for(int k = 0; k < horizontalHeader()->count(); k ++) {
            columnList << k;
        }
    }
    return columnList;
}

void lds_tableview_virtual::toPageUp()
{
    verticalScrollBar()->setValue(verticalScrollBar()->value()-verticalScrollBar()->pageStep());
}

void lds_tableview_virtual::toPageDown()
{
    verticalScrollBar()->setValue(verticalScrollBar()->value()+verticalScrollBar()->pageStep());
}

void lds_tableview_virtual::toSelectAll()
{
    selectAll();
    setFocus();
}

void lds_tableview_virtual::toClearAll()
{
    clearSelection();
    setFocus();
}

void lds_tableview_virtual::tableviewSetpan(QTableView *tableview, int cmp_column, const QList<int> &span_column_list, bool clearSpans)
{
    QAbstractItemModel  *model = tableview->model();
    if(clearSpans)
        tableview->clearSpans();
    //、
    QString dt;
    int rowbegin = 0;
    int rowend = 0;
    for(int row = 0, rowcount = model->rowCount(); row < rowcount; row++) {
        if(dt != model->data(model->index(row, cmp_column ), Qt::EditRole).toString()) {
            if(!dt.isEmpty()) {
                foreach(int c, span_column_list) {
                    tableview->setSpan(rowbegin, c, rowend - rowbegin + 1, 1);
                }
            }
            dt = model->data(model->index(row, cmp_column ), Qt::EditRole).toString();
            rowbegin = row;
        }
        rowend = row;
    }
    //
    if(model->rowCount() > 0) {
        foreach (int c, span_column_list) {
            tableview->setSpan(rowbegin, c, rowend - rowbegin + 1, 1);
        }
    }
}

int lds_tableview_virtual::get_hheight()
{
    if(hheight == -1) {
        QTableWidget t;
        t.setColumnCount(1);
        hheight = t.horizontalHeader()->height();
    }
    return hheight;
}

int lds_tableview_virtual::get_hscrollheight()
{
    if(hscrollheight == -1) {
        QTableWidget t;
        t.setColumnCount(2);
        t.resize(0, 0);
        hscrollheight = t.horizontalHeader()->height();
    }
    return hscrollheight;
}

void lds_tableview_virtual::setCornerText(const QString &text)
{
    QAbstractButton* btn = findChild<QAbstractButton*>();
    if (btn && QString("QTableCornerButton") == btn->metaObject()->className())
    {
        btn->setText(text);

        // adjust the width of the vertical header to match the preferred corner button width

        // (unfortunately QAbstractButton doesn't implement any size hinting functionality)

        QStyleOptionHeader opt;
        opt.text = btn->text();
        QSize s = (btn->style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), btn).
                   expandedTo(QApplication::globalStrut()));
        if (s.isValid())
            verticalHeader()->setMinimumWidth(s.width());

        if(0 == corner_button) {
            corner_button = btn;
            btn->installEventFilter(this);
        }
    }
}

QAbstractButton *lds_tableview_virtual::cornerButton()
{
    return corner_button;
}

void lds_tableview_virtual::justShowColumns(const QList<int> &column_list)
{
    for(int column = 0, columncount = this->horizontalHeader()->count(); column < columncount; column ++) {
        this->setColumnHidden(column, !column_list.contains(column));
    }
}

void lds_tableview_virtual::justShowColumn(int column)
{
    justShowColumns(QList<int>() << column);
}

void lds_tableview_virtual::setShowNest(bool flag)
{
    is_show_nest =flag;
}

QModelIndex lds_tableview_virtual::model_index(int row, int column)
{
    QAbstractItemModel *m = this->model();
    if(m) {
        return m->index(row, column);
    }
    return QModelIndex();
}

QVariant lds_tableview_virtual::model_data(int row, int column)
{
    QAbstractItemModel *m = this->model();
    if(m) {
        return m->data(m->index(row, column));
    }
    return QVariant();
}

bool lds_tableview_virtual::eventFilter(QObject* o, QEvent* e)
{
    //    if (e->type() == QEvent::Paint)
    //    {
    //        QAbstractButton* btn = qobject_cast<QAbstractButton*>(o);
    //        if (btn && QString("QTableCornerButton") == btn->metaObject()->className())
    //        {
    //            // paint by hand (borrowed from QTableCornerButton)

    //            QStyleOptionHeader opt;
    //            opt.init(btn);
    //            QStyle::State state = QStyle::State_None;
    //            if (btn->isEnabled())
    //                state |= QStyle::State_Enabled;
    //            if (btn->isActiveWindow())
    //                state |= QStyle::State_Active;
    //            if (btn->isDown())
    //                state |= QStyle::State_Sunken;
    //            opt.state = state;
    //            opt.rect = btn->rect();
    //            opt.text = btn->text(); // this line is the only difference to QTableCornerButton
    //            opt.textAlignment = Qt::AlignCenter;

    //            opt.position = QStyleOptionHeader::OnlyOneSection;
    //            QStylePainter painter(btn);
    //            painter.drawControl(QStyle::CE_Header, opt);
    //            return true; // eat event

    //        }
    //    }
    return QTableView::eventFilter(o, e);
}

void lds_tableview_virtual::paintEvent(QPaintEvent *e)
{
    if(is_show_nest && this->horizontalHeader()->count() != 0 && this->showGrid()) {
        QHeaderView *hheader = this->horizontalHeader();
        QHeaderView *vheader = this->verticalHeader();
        QPainter p(viewport());
//        int w =  hheader->defaultSectionSize();
//        int h = vheader->defaultSectionSize();
        int xoff = 0;
        int yoff = 0;
        for(int k = 0; k < hheader->count(); k++) {
            xoff += hheader->sectionSize(k);
        }
        for(int k = 0; k < vheader->count(); k++) {
            yoff += vheader->sectionSize(k);
        }

        p.translate(-this->horizontalOffset(), -this->verticalOffset());

        QStyleOptionViewItemV4 option = viewOptions();
        int gridHint = style()->styleHint(QStyle::SH_Table_GridLineColor, &option, this);
        QColor gridColor = static_cast<QRgb>(gridHint);
        p.setPen(QPen(gridColor, 0));

        //        //右边
        //        for(int row = 0, row_begin = 0; row <= vheader->count(); row ++) {
        //            //水平
        //            if(row_begin != 0)
        //                p.drawLine(QPointF(xoff, row_begin - 1), QPointF(this->width(), row_begin - 1));
        //            row_begin += vheader->sectionSize(row);
        //        }
        //下边
        for(int column = 0, column_begin = 0; column <=  hheader->count(); column ++) {
            //垂直
            if(column_begin != 0)
                p.drawLine(QPointF(column_begin - 1, yoff), QPointF(column_begin - 1, this->height()));
            column_begin += hheader->sectionSize(column);
        }
        //        //右下边
        //        p.translate(0, yoff);
        //        for(int row = 1, rowcount = this->height() / h; row < rowcount ; row ++) {
        //            for(int column = 1, columncount = this->width() / w; column < columncount; column ++) {
        //                //水平
        //                p.drawLine(QPointF(0, h * row - 1), QPointF(this->width(), h * row - 1));
        //            }
        //        }
        //        p.translate(xoff, -yoff);
        //        for(int row = 1, rowcount = this->height() / h; row < rowcount ; row ++) {
        //            for(int column = 1, columncount = this->width() / w; column < columncount; column ++) {
        //                //垂直
        //                p.drawLine(QPointF(w * column - 1, 0), QPointF(w * column - 1, this->height()));
        //            }
        //        }
    }

    QTableView::paintEvent(e);
}
