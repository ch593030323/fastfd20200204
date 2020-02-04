#include "restraurantdelegate_list_table.h"
#include <QPainter>
#include <QPixmap>
#include "restauranttableview.h"
#include "public_sql.h"
#include <QtDebug>

restraurantDelegate_list_table::restraurantDelegate_list_table(QObject *parent)
    : lds_tableview_delegate_virtual(parent)
{
}

void restraurantDelegate_list_table::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, QColor("#ff546b"));
        painter->setPen(option.palette.brush(QPalette::HighlightedText).color());
    } else {
        painter->setPen(option.palette.brush(QPalette::Text).color());
    }

    QString key = index.data(Qt::UserRole + 1).toString();
    if(key.length() == 0) return;
    if("-1" == key) {
        painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignHCenter,  "---" + QObject::tr("上一页") + "---");
        return;
    }
    if("-2" == key) {
        painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignHCenter,  "---" + QObject::tr("下一页") + "---");
        return;
    }
    if("-3" == key) {
        painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignHCenter,  "---" + QObject::tr("新增") + "---");
        return;
    }

    QRect prect = option.rect;
    drawTable(painter, QRect(prect.x(), prect.y(), prect.height(), prect.height()), index, 0.0);

    prect.adjust(prect.height() + 5, 0, 0, 0);
    painter->drawText(prect, Qt::AlignVCenter | Qt::AlignLeft,  key);
    painter->drawText(prect.adjusted(prect.width()/2, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft,  index.data(Qt::UserRole).toString());
}

void restraurantDelegate_list_table::drawTable(QPainter *painter, const QRect &rect, const QModelIndex &index, qreal xyRadius) const
{
    cell_content cell[3][3];
    //预打单
    QMap<QString/*ch_tableno*/, bool> *preprinttablemap = &(public_sql::preprinttablemap);
    //开台状态
    QString ch_billnos = index.data(Qt::UserRole+2).toString();
    QMap<int, QString> map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    QString billno;
    //预订状态
    QString order_tableno = index.data(Qt::UserRole+5).toString();//order_tableno
    //酒吧状态
    QList<int> div_bar;
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
        QStringList div_bar_str = index.data(Qt::UserRole+9/*div_bar*/).toString().split(",", QString::SkipEmptyParts);
        foreach(const QString &div, div_bar_str) {
            div_bar << div.toInt();
        }
    }
    ///上色 初始
    //默认 空闲
    tableSTATE tstate= restraurantDelegate::gettableState( tableState_KONGXIAN);
    //预订
    if(!order_tableno.isEmpty()) {
        tstate= restraurantDelegate::gettableState(tableState_YUDING);
    }
    restraurantDelegate::cell_init_showed_color(cell, tstate.colorname1);
    //酒吧
    for(int k = 0; k < 9; k ++) {
        int sqr_2 = restraurantDelegate::chbillno2index(k);
        //
        if(div_bar.contains(k)) {
            cell[sqr_2 % 3][sqr_2 / 3].isDensePattern = true;
        }
        //
        billno = map.value(k);
        if(billno.isEmpty()) continue;
        cell[sqr_2 % 3][sqr_2 / 3].showed_color = restraurantDelegate::gettableState(tableState_KAITAI).colorname1;
        if(preprinttablemap->value(billno, false)) {//预打单
            cell[sqr_2 % 3][sqr_2 / 3].showed_color = restraurantDelegate::gettableState( tableState_YUDA).colorname1;
        }
    }
    //未分单
    bool f1  = div_bar.count() == 0 || (div_bar.count() == 1 && div_bar.first() == 0);
    //分单号未清台
    bool f2  = map.count() == 0 || (map.count() == 1 && map.value(0).length() > 0);
//    qDebug() << index.row() << f1 << f2 << div_bar << map << cell[1][1].showed_color;
    //12 true false () QMap((0, "B1801040200002")(3, "B1801040200003")(8, "B1801040200004")) "#50c600"
    //单一桌台
    if(f1 && f2) {
        for(int k = 0; k < 9; k++) {
            if(k == 4) continue;
            cell[k / 3][k % 3] = cell[1][1];
        }
    }
    //pie
    bool painter_linear = f1 && f2;
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            restraurantDelegate::painterCell(painter, cell, x, y,  rect, !painter_linear, false, xyRadius, 1);
        }
    }
}


//void drawTable(const QString &tableno, const QStringList &ch_billno_list, const QRect &rect) {
//    cell_content cell[3][3];
//    tableSTATE tstate ;
//    ///头
//    //空闲
//    tstate= restraurantDelegate::gettableState( tableState_KONGXIAN);
//    //预订
//    if(!order_tableno.isEmpty()) {
//        tstate= restraurantDelegate::gettableState(tableState_YUDING);
//    }
//    /////////未分单
//    if(false == rect_is_div) {
//        //开台
//        if(ch_billno.length() > 0) {
//            tstate= restraurantDelegate::gettableState(tableState_KAITAI);
//        }
//        //预打单
//        if(ch_billno.length() > 0) {
//            if(preprinttablemap->value(ch_billno, false)) {
//                tstate= restraurantDelegate::gettableState( tableState_YUDA);
//            }
//        }
//        //酒吧 未清台
//        if(div_bar.count() > 0) {
//            for(int k = 0; k < 9; k ++) {
//                int sqr_2 = static_chbillno2index(k);
//                cell[sqr_2 % 3][sqr_2 / 3].isDensePattern = true;
//            }
//        }
//    }
//    ///中间
//    cell_init_showed_color(cell, tstate.colorname1);
//    /////////分单
//    if(true == rect_is_div) {
//        //开台
//        //预打单
//        QString billno;
//        for(int k = 0; k < 9; k ++) {
//            int sqr_2 = static_chbillno2index(k);
//            billno = map.value(k);
//            if(billno.isEmpty()) continue;
//            cell[sqr_2 % 3][sqr_2 / 3].showed_color = restraurantDelegate::gettableState(tableState_KAITAI).colorname1;
//            if(preprinttablemap->value(billno, false)) {//预打单
//                cell[sqr_2 % 3][sqr_2 / 3].showed_color = restraurantDelegate::gettableState( tableState_YUDA).colorname1;
//            }
//        }
//        //酒吧 未清台
//        for(int k = 0; k < 9; k ++) {
//            int sqr_2 = static_chbillno2index(k);
//            if(div_bar.contains(QString::number(k))) {
//                cell[sqr_2 % 3][sqr_2 / 3].isDensePattern = true;
//            }
//        }
//    }

//    ///尾
//    //选中
//    if(option.state & QStyle::State_Selected) {//选中
//        QStringList checklist = State_Selected_checked.split(",", QString::SkipEmptyParts);
//        foreach(const QString &check, checklist) {//勾选
//            int sqr_2 = static_chbillno2index(check.toInt());
//            cell[sqr_2 % 3][sqr_2 / 3].ischecked = true;
//        }
//    }
//    //置灰
//    foreach(const QString &sqr, disable_sqr_list) {
//        int sqr_2 = static_chbillno2index(sqr.toInt());
//        cell[sqr_2 % 3][sqr_2 / 3].isdisabled = true;
//    }
//    //////////绘制
//    if(rect_is_div) {
//        parent_rect.adjust(1, 1, -1, -1);//增加分割线
//    }

//    //border
//    painter->setPen(QPen(QColor("dimgrey"), 4));
//    painter->drawPath(getRoundRectPath(parent_rect.adjusted(2, 2, 0, 0), xyRadius, xyRadius, xyRadius, xyRadius));
//    //pie
//    for(int x = 0; x < 3; x++) {
//        for(int y = 0; y < 3; y++) {
//            printcell(painter, cell, x, y,  parent_rect);
//        }
//    }
//}


restraurantDelegate_list_dish::restraurantDelegate_list_dish(QObject *parent)
    : lds_tableview_delegate_virtual(parent)
{

}

void restraurantDelegate_list_dish::drawText(QPainter *painter, QStyle::State state, const QRect &painter_text_rect, Qt::Alignment displayAlignment, const QString &painter_text) const
{
    if(painter_text.contains(":")) {
        int split_index = painter_text.indexOf(":");
        QString p0 = painter_text.mid(0, split_index + 1);
        QString p1 = painter_text.mid(split_index + 1);

        QFont font = painter->font();
        QFontMetrics fm(font);
        QRect rect = QStyle::alignedRect(painter->layoutDirection(), displayAlignment, QSize(fm.width(painter_text), fm.height()), painter_text_rect);

        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, p0);

        if(state & QStyle::State_Selected) {

        } else {
            painter->setPen(QColor("#ff546b"));
        }
        painter->drawText(rect.adjusted(fm.width(p0), 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, p1);
        return;
    }
    lds_tableview_delegate_virtual::drawText(painter, state, painter_text_rect, displayAlignment, painter_text);
}
