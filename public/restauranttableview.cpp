#include "restauranttableview.h"
#include <QPainter>
#include "public_sql.h"
#include <QtDebug>
#include <QTime>
#include "n_func.h"
#include "lds.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include <QHeaderView>
#include <QKeyEvent>
//1    2   3
//4    0   5
//6    7   8



//A
//0 1 2
//3 4 5
//6 7 8

//B
//1 2 3
//4 0 5
//6 7 8

//B ****i>4?:i(i==0)?4:(i-1);*****A
//A ****i>4?:i(i==4)?0:(i+1);*****B
//1
static int static_chbillno2index(int v) {
    return v>4?v:(v==0)?4:(v-1);
}

static int static_index2chbillno(int v) {
    return v>4?v:(v==4)?0:(v+1);
}

//3
static QColor static_color_light(const QString &c, double light_0_3, int a = 255) {//0 1 2 3 原 亮 很亮 最亮
    return lds::static_color_light(c, light_0_3, a);
}

void restraurantDelegate::cell_init_showed_color(cell_content cell[][3], const QString &showed)  {
    cell[0][0].showed_color = showed;
    cell[0][1].showed_color = showed;
    cell[0][2].showed_color = showed;

    cell[1][0].showed_color = showed;
    cell[1][1].showed_color = showed;
    cell[1][2].showed_color = showed;

    cell[2][0].showed_color = showed;
    cell[2][1].showed_color = showed;
    cell[2][2].showed_color = showed;
}

restraurantDelegate::restraurantDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    textColor = 0;
    this->model_cache = new fexpandmain_model_sqltablemodel(this);
    this->preprinttablemap = &(public_sql::preprinttablemap);
}


static qreal xyRadius = 10;
static QPainterPath getRoundRectPath(const QRectF &rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightBottomRadius, qreal rightTopRadius) {

    QPainterPath path;
    path.moveTo(rect.topLeft() + QPoint(leftTopRadius, 0));
    path.arcTo(QRectF(rect.left(), rect.top(),  2.0 * leftTopRadius, 2.0 * leftTopRadius), 90, 90);

    path.lineTo(rect.bottomLeft() - QPoint(0, leftBottomRadius));
    path.arcTo(QRectF(rect.left(), rect.bottom()  - 2.0 * leftBottomRadius,  2.0 * leftBottomRadius, 2.0 * leftBottomRadius), 180, 90);

    path.lineTo(rect.bottomRight() - QPoint(rightBottomRadius, 0));
    path.arcTo(QRectF(rect.right() - 2.0 * rightBottomRadius, rect.bottom()  - 2.0 * rightBottomRadius, 2.0 * rightBottomRadius, 2.0 * rightBottomRadius), 270, 90);

    path.lineTo(rect.topRight() + QPoint(0, rightTopRadius));
    path.arcTo(QRectF(rect.right() - 2.0 * rightTopRadius, rect.top(),  2.0 * rightTopRadius, 2.0 * rightTopRadius), 0, 90);

    path.closeSubpath();

    return path;
}
static QPainterPath getRectLinePath(const QRectF &xyRect, int x, int y) {

    //    xyRect = xyRect.adjusted(1, 1, -1, -1);
    int wh =  2 * xyRadius;
    QPainterPath path;
    if(x == 0 && y == 0) {
        path.moveTo(xyRect.topRight());
        path.arcTo(QRectF(xyRect.left(), xyRect.top(),  wh, wh), 90, 90);
        path.lineTo(xyRect.bottomLeft());
    }
    if(x == 0 && y == 1) {
        path.moveTo(xyRect.topLeft());
        path.lineTo(xyRect.bottomLeft());
    }
    if(x == 0 && y == 2) {
        path.moveTo(xyRect.topLeft());
        path.arcTo(QRectF(xyRect.left(), xyRect.bottom() - 2 * xyRadius,  wh, wh), 180, 90);
        path.lineTo(xyRect.bottomRight());
    }
    if(x == 1 && y == 2) {
        path.moveTo(xyRect.bottomLeft());
        path.lineTo(xyRect.bottomRight());
    }

    if(x == 2 && y == 2) {
        path.moveTo(xyRect.bottomLeft());
        path.arcTo(QRectF(xyRect.right() - 2 * xyRadius, xyRect.bottom() - 2 * xyRadius,  wh, wh), 270, 90);
        path.lineTo(xyRect.topRight());
    }
    if(x == 2 && y == 1) {
        path.moveTo(xyRect.bottomRight());
        path.lineTo(xyRect.topRight());
    }
    if(x == 2 && y == 0) {
        path.moveTo(xyRect.bottomRight());
        path.arcTo(QRectF(xyRect.right() - 2 * xyRadius, xyRect.top(),  wh, wh), 0, 90);
        path.lineTo(xyRect.topLeft());
    }
    if(x == 1 && y == 0) {
        path.moveTo(xyRect.topRight());
        path.lineTo(xyRect.topLeft());
    }
    return path;
}

void restraurantDelegate::painterCell(QPainter *painter, cell_content cell[][3], int x, int y, const QRect &rect, bool  painter_linear, bool gray_mask, qreal xyRadius, qreal  pattern_scale) {
    const cell_content &celldata = cell[x][y];
    QRectF rect1 = QRectF(rect.x() * 1.0 + rect.width() * x / 3.0, rect.y() * 1.0 + rect.height()*y/3.0, rect.width() * 1.0 / 3.0, rect.height()*1.0 / 3.0);

    QLinearGradient linearGrad1(rect1.topLeft(),       rect1.bottomLeft());
    QColor linearGrad1_start;
    QColor linearGrad1_end;

    QPainterPath linear_path;
    QPixmap pg;
    //isdisabled
    if(celldata.isdisabled) {//置灰
        pg = QPixmap(":/image/widgets_pic/disable_check_on.png");
        QString disablecolor = restraurantDelegate::gettableState(tableState_DISABLE).colorname1;
        linearGrad1_start = static_color_light(disablecolor, 3 - y);// QColor::fromHsl(h, s, l + (2 - y + 1)* 22));
        linearGrad1_end = static_color_light(disablecolor, 3 - y - 1);//QColor::fromHsl(h, s, l + (2 - y)* 22));
    } else {
        pg = QPixmap(":/image/widgets_pic/check_on.png");
        linearGrad1_start = static_color_light(celldata.showed_color, 3 - y);// QColor::fromHsl(h, s, l + (2 - y + 1)* 22));
        linearGrad1_end = static_color_light(celldata.showed_color, 3 - y - 1);//QColor::fromHsl(h, s, l + (2 - y)* 22));
    }
    if(false == painter_linear) {
        linearGrad1_start = linearGrad1_end = static_color_light(celldata.showed_color, 3);//QColor(celldata.showed_color);
    }
    if(true == gray_mask) {
        linearGrad1_start = linearGrad1_start.dark(200);
        linearGrad1_end = linearGrad1_end.dark(200);
    }
    linearGrad1.setColorAt(0, linearGrad1_start);
    linearGrad1.setColorAt(1, linearGrad1_end);

    if(0 == x && 0 == y) {
        linear_path = getRoundRectPath(rect1, xyRadius, 0, 0, 0);
    } else if(0 == x && 2 == y) {
        linear_path = getRoundRectPath(rect1, 0, xyRadius, 0, 0);
    } else if(2 == x && 2 == y) {
        linear_path = getRoundRectPath(rect1, 0, 0, xyRadius, 0);
    } else if(2 == x && 0 == y) {
        linear_path = getRoundRectPath(rect1, 0, 0, 0, xyRadius);
    } else {
        linear_path = getRoundRectPath(rect1, 0, 0, 0, 0);
    }
    painter->fillPath(linear_path, linearGrad1);
    //isDensePattern
    if(celldata.isDensePattern) {
        restraurantDelegate::paintDensePattern(painter,
                                               linear_path,
                                               QColor(restraurantDelegate::gettableState(tableState_BAR).colorname1),
                                               Qt::DiagCrossPattern,
                                               pattern_scale,
                                               false
                                               );
    }

    //check
    if(celldata.ischecked) {
        pg = pg.scaledToWidth(30, Qt::SmoothTransformation);
        painter->drawPixmap(QStyle::alignedRect(painter->layoutDirection(), Qt::AlignCenter, pg.size(), rect1.toRect()), pg);
    }
}

const QString restraurantDelegate::sql_ch_billnos =
        " concat("
        " concat( ifnull(ch_billno, ''), ','),   concat( ifnull(ch_billno_1, ''), ','), concat( ifnull(ch_billno_2, ''), ','), "
        " concat( ifnull(ch_billno_3, ''), ','), concat( ifnull(ch_billno_4, ''), ','), concat( ifnull(ch_billno_5, ''), ','), "
        " concat( ifnull(ch_billno_6, ''), ','), concat( ifnull(ch_billno_7, ''), ','), concat( ifnull(ch_billno_8, ''), ',')  "
        " )"
        ;

restaurantTableView::restaurantTableView(QWidget *parent)
    :ftableview(parent), d(0)
{
    this->setProperty("outer_stylesheet", "restaurantTableView");
}

void restaurantTableView::setCurrentIndex(const QString &ch_tableno)
{
    this->setCurrentIndex(QModelIndex());
    for(int row = 0; row < m->rowCount(); row++) {
        for(int column = 0; column < m->columnCount(); column++) {
            if(m->data(m->index(row, column), Qt::UserRole + 1).toString() == ch_tableno) {
                this->setCurrentIndex(m->index(row, column));
                return;
            }
        }
    }
}

void restaurantTableView::setCurrentIndex(const QModelIndex &index)
{
    ftableview::setCurrentIndex(index);
}

bool restaurantTableView::find_selected_ch_tableno(const QString &ch_tableno)
{
    QModelIndexList indexlist = selectionModel()->selectedIndexes();
    foreach(const QModelIndex &index, indexlist) {
        if(m->data(index, Qt::UserRole + 1).toString() == ch_tableno) {
            return true;
        }
    }
    return false;
}

QModelIndex restaurantTableView::getIndexByChtableno(const QString &ch_tableno)
{
    for(int row = 0; row < m->rowCount(); row++) {
        for(int column = 0; column < m->columnCount(); column++) {
            if(m->data(m->index(row, column), Qt::UserRole + 1/*ch_tableno*/).toString() == ch_tableno) {
                return m->index(row, column);
            }
        }
    }
    return QModelIndex();
}

void restaurantTableView::setItemDelegate(restraurantDelegate *delegate)
{
    d = delegate;
    d->keepConnect(this);
    QTableView::setItemDelegate(delegate);
}

restraurantDelegate *restaurantTableView::get_delegate()
{
    return d;
}

QStringList restaurantTableView::getCheckList(const QModelIndex &index)
{
    QStringList sqr_list = index.data(Qt::UserRole + 7/*indexs*/).toString().split(",",  QString::SkipEmptyParts);
    QStringList disable_list = index.data(Qt::UserRole + 8/*disable*/).toString().split(",",  QString::SkipEmptyParts);

    foreach(const QString &disable, disable_list) {
        sqr_list.removeAll(disable);
    }

    return sqr_list;
}

QString restaurantTableView::getChbillnoBySqr(const QModelIndex &index, int sqr)
{
    QString ch_billnos = m->data(index, Qt::UserRole+2 /*ch_billnos*/).toString();
    return getChbillnoBySqr(ch_billnos, sqr);
}

QString restaurantTableView::getChbillnoBySqr(const QString &ch_billnos, int sqr)
{
    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(ch_billnos);
    return ch_billno_map.value(sqr);
}

void restaurantTableView::enableMutiSelected(bool f)
{
    if(f) {
        this->setSelectionMode(QTableView::MultiSelection);
        this->setSelectionBehavior(QTableView::SelectItems);
    } else {
        this->setSelectionMode(QTableView::SingleSelection);
        this->setSelectionBehavior(QTableView::SelectItems);
    }
}

void restaurantTableView::loadTableList(const QStringList &tableList)
{
    tablenolist = tableList.toSet();
    toloadTableNo();
}

void restaurantTableView::tocurindex_checklist(const QString &checklist)
{
    m->setData(currentIndex(), checklist, Qt::UserRole+7/*check*/);
    emit siganlmodelcheck(currentIndex());
}

void restaurantTableView::toSaveCurTable(const QModelIndex &index)
{
    if(this->selectionModel()->selectedIndexes().contains(index)) {
        tablenolist.insert(index.data(Qt::UserRole+1).toString());//ch_tableno
    } else {
        tablenolist.remove(index.data(Qt::UserRole+1).toString());//ch_tableno
    }
    emit signalSaveTableFinished();
}

void restaurantTableView::toloadTableNo()
{
    if(tablenolist.isEmpty()) return;
    QString ch_tableno;
    for(int row = 0, rowcount = m->rowCount(); row < rowcount; row++) {
        for(int column = 0, columncount = m->columnCount(); column<columncount; column++) {
            ch_tableno = m->data(m->index(row, column), Qt::UserRole+1).toString();
            if(tablenolist.contains(ch_tableno)) {
                this->setCurrentIndex(ch_tableno);
            }
        }
    }
}

/*
单选，绿色
单选，橙色

多选-绿色
多选-橙色*/
void restaurantTableView::toshowpop(const QModelIndex&index)
{
    //分单：选分单号、确认、取消
    //未分单：选单号，取消
    QMap<int, QString> ch_billno_map = restraurantDelegate::get_ch_billno_div_map(index.data(Qt::UserRole + 2).toString()/*ch_billnos*/);

    if(restraurantDelegate::ch_billno_is_div(ch_billno_map)) {
    } else {
        return;//非分单不在参与pop
    }
    //第二次点击直接进入
    //pop
    w_scr_dish_restaurant_div_pop_only_table *_9Box = new w_scr_dish_restaurant_div_pop_only_table;

    _9Box->update_by_ch_billnos(ch_billno_map);
    _9Box->setWindowFlags(Qt::Popup);
    _9Box->setAttribute(Qt::WA_DeleteOnClose);//指针自动删除
    _9Box->resize(150, 150);
    _9Box->enableMutilSelected();
    _9Box->load_div_check_respone(this);
    if(_type & pop_single) _9Box->enableSinglSelectedWhenMutil();
    if(_type & pop_select_KONGXIAN) _9Box->enableItemFlagSelectedWith_nochbillno();

    //同步选中
    QStringList checklist = getCheckList(index);//需要过滤disable的获取check
    foreach(const QString &check, checklist) {
        _9Box->setcurindex_by_sqr(check.toInt());
    }

    //弹出位置
    QPoint pos_bottom;
    QPoint pos_top;

    int voff =  verticalHeader()->sectionPosition(index.row());
    int hoff =  horizontalHeader()->sectionPosition(index.column());


    pos_bottom = QPoint(hoff, voff+rowHeight(index.row()));
    pos_top = QPoint(hoff, voff);

    pos_bottom = viewport()->mapToGlobal(pos_bottom);
    pos_top = viewport()->mapToGlobal(pos_top);

    QSize size = _9Box->size();

    _9Box->move(lds::getPopPoint(pos_bottom, pos_top, size, this->layoutDirection()));
    _9Box->raise();
    _9Box->show();
}

void restaurantTableView::keyPressEvent(QKeyEvent *e)
{
    ftableview::keyPressEvent(e);
}

tableSTATE restraurantDelegate::gettableState(tableState state)
{
    return lds_scr_table::gettableState(state);
}

QPixmap restraurantDelegate::getPixmapByStateList(QList<tableState> statelist)//目前支持两种
{
    QPixmap p(15, 15);
    QPainter painter;
    painter.begin(&p);
    QRect rect = QRect(0, 0, 15, 15);

    switch(statelist.count()) {
    case 1:{
        tableState state = statelist[0];
        if(state == tableState_BAR) {
            QLinearGradient linearGrad1(rect.x(), rect.y(), rect.width(), rect.height());
            linearGrad1.setColorAt(0, color_light(gettableState(tableState_KONGXIAN).colorname1, 3));
            linearGrad1.setColorAt(1, color_light(gettableState(tableState_KONGXIAN).colorname1, 0));
            painter.fillRect(rect, linearGrad1);
            restraurantDelegate::paintDensePattern(&painter,
                                                   rect,
                                                   QColor(restraurantDelegate::gettableState(state).colorname1),
                                                   Qt::DiagCrossPattern,
                                                   1.3,
                                                   false
                                                   );
        } else {
            QLinearGradient linearGrad1(rect.x(), rect.y(), rect.width(), rect.height());
            linearGrad1.setColorAt(0, color_light(gettableState(state).colorname1, 3));
            linearGrad1.setColorAt(1, color_light(gettableState(state).colorname1, 0));
            painter.fillRect(rect, linearGrad1);
        }
        break;
    }
    case 2:{
        painter.fillRect(rect, gettableState(statelist[0]).colorname1);
        QPolygonF polygon(QVector<QPointF>() << QPointF(15, 0) << QPointF(15, 15) << QPointF(0, 15));
        QPainterPath path;
        path.addPolygon(polygon);
        painter.fillPath(path, color_light(gettableState( tableState_YUDA).colorname1, 2));
        break;
    }
    }

    painter.end();

    return p;
}

QPixmap restraurantDelegate::getPixmapByStateList(tableState state)
{
    return getPixmapByStateList(QList<tableState>() << state);
}

QList<tableSTATE> &restraurantDelegate::getTablestatelist()
{
    return lds_scr_table::getTablestatelist();
}

void restraurantDelegate::clearCache()
{
    lds_scr_table::tablestatelist.clear();
}

QColor restraurantDelegate::color_light(const QString &c, double light, int a) {//0 1 2 3 原 亮 很亮 最亮
    return static_color_light(c, light, a);
}

QColor restraurantDelegate::color_light(const QColor &c, double light, int a)
{
    return static_color_light(c.name(), light, a);
}

int restraurantDelegate::chbillno2index(int v)
{
    return static_chbillno2index(v);
}

int restraurantDelegate::index2chbillno(int v)
{
    return static_index2chbillno(v);
}

const int restraurantDelegate::table_border = 3;//三的倍数
void restraurantDelegate::paintDensePattern(QPainter *painter, const QPainterPath &path, const QColor &color, Qt::BrushStyle pattern, qreal pattern_scale, bool drawPen)
{
    //save
    QBrush b = painter->brush();
    QPen p = painter->pen();
    //scale
    QBrush pattern_brush(QColor(color.red(), color.green(), color.blue(), 80), pattern);
    QMatrix m = pattern_brush.matrix();
    m.scale(pattern_scale, pattern_scale);
    pattern_brush.setMatrix(m);
    painter->setBrush(pattern_brush);
    //draw
    if(false == drawPen)
        painter->setPen(Qt::NoPen);
    painter->drawPath(path);
    //restore
    painter->setPen(p);
    painter->setBrush(b);
}

void restraurantDelegate::paintDensePattern(QPainter *painter, const QRectF &rect, const QColor &color, Qt::BrushStyle pattern, qreal pattern_scale, bool drawPen)
{
    QPainterPath path;
    path.addRect(rect);
    paintDensePattern(painter, path, color, pattern, pattern_scale, drawPen);
}

restraurantDelegate::~restraurantDelegate()
{
    delete model_cache;
}

void restraurantDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    paintCellData p_dta;
    paintCellFill(p_dta, index);

    p_dta.rect = option.rect;
    p_dta.option_state = option.state;

    paintTable(painter, p_dta);
}

//void restraurantDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    //pay_unit_prefix
//    QString pay_unit_prefix = model_cache->tablename_kvmap_value("cey_sys_parameter", "pay_unit_prefix", "vch_parameter", "vch_value").toString();
//    //1
//    QRect rect = option.rect;
//    QRect parent_rect=rect.adjusted(table_border, table_border, -table_border - rect.width() % 3, -table_border - rect.height() % 3);
//    parent_rect.adjust(0, 0, - parent_rect.width() % 3, - parent_rect.height() % 3);//确保cell的长宽是3的倍数
//    QFont f = painter->font();
//    QString vch_tablename = index.data(Qt::UserRole).toString();
//    QString ch_tableno = index.data(Qt::UserRole+1).toString();
//    QString ch_billnos = index.data(Qt::UserRole+2).toString();

//    QString int_person = lds::septNumber(index.data(Qt::UserRole+3/*int_person*/).toInt());
//    QString num_cost = lds::septNumber(index.data(Qt::UserRole+4/*num_cost*/).toDouble(), model_cache->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
//    QString order_tableno = index.data(Qt::UserRole+5).toString();//order_tableno
//    QTime order_time = index.data(Qt::UserRole+6).toTime();//order_time
//    QString State_Selected_checked = index.data(Qt::UserRole+7/*check*/).toString();//1,2,3,0
//    QStringList disable_sqr_list = index.data(Qt::UserRole+8/*disable*/).toString().split(",", QString::SkipEmptyParts);//1,2,3,0

//    QStringList div_bar;
//    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
//        div_bar = index.data(Qt::UserRole+9/*div_bar*/).toString().split(",", QString::SkipEmptyParts);
//    }
//    QMap<int, QString> map = get_ch_billno_div_map(ch_billnos);
//    const QString show_table_code_name = lds::sysconf->value("w_scr_dish_restaurant_parameter/map_show", "show_table_name").toString();
//    QString tableno_n_name;
//    if(show_table_code_name == "show_table_code") {
//        tableno_n_name = ch_tableno;
//    } else if(show_table_code_name == "show_table_name") {
//        tableno_n_name = vch_tablename;
//    } else if(show_table_code_name == "show_table_code_name") {
//        tableno_n_name = ch_tableno + "\n" + vch_tablename;
//    } else {
//        tableno_n_name = ch_tableno + "\n" + vch_tablename;
//    }

//    //2
//    if(ch_tableno.isEmpty()) return;
//    painter->setPen(Qt::NoPen);
//    painter->setRenderHint(QPainter::Antialiasing);
//    f.setPointSize(10);
//    painter->setFont(f);

//    //
//    if(ch_tableno == "-3") {//新增
//        painter->setPen(QPen(QColor("#555555"), 3, Qt::DashDotDotLine));

//        parent_rect.adjust(1,1,-1,-1);
//        double woff = parent_rect.width()/10.0;
//        double hoff = parent_rect.height()/10.0;
//        painter->drawRoundedRect(parent_rect, woff, hoff);

//        painter->setPen(Qt::NoPen);
//        painter->setBrush(QColor("#555555"));
//        painter->drawRoundedRect(parent_rect.adjusted(woff*1.5, hoff*4, -woff*1.5, -hoff*4), hoff/1.2, hoff/1.2);
//        painter->drawRoundedRect(parent_rect.adjusted(woff*4, hoff*1.5, -woff*4, -hoff*1.5), woff/1.2, woff/1.2);
//        return;
//    }
//    ///绘制准备
//    //ch_billno
//    QString ch_billno =  map.value(0);
//    //rect_is_div
//    bool rect_is_div = ch_billno_is_div(map)
//            || bar_is_div(div_bar);

//    QString show_info;
//    cell_content cell[3][3];
//    tableSTATE tstate ;
//    ///头
//    //空闲
//    tstate= lds_scr_table::gettableState( tableState_KONGXIAN);
//    //预订
//    if(!order_tableno.isEmpty()) {
//        tstate= lds_scr_table::gettableState(tableState_YUDING);
//    }
//    /////////未分单
//    if(false == rect_is_div) {
//        //开台
//        if(ch_billno.length() > 0) {
//            tstate= lds_scr_table::gettableState(tableState_KAITAI);
//        }
//        if(ch_billno.startsWith("R")) {//二维码点餐
//            tstate = lds_scr_table::gettableState(tableState_QR_CODE);
//        }
//        //预打单
//        if(ch_billno.length() > 0) {
//            if(preprinttablemap->value(ch_billno, false)) {
//                tstate= lds_scr_table::gettableState( tableState_YUDA);
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
//            cell[sqr_2 % 3][sqr_2 / 3].showed_color = lds_scr_table::gettableState(tableState_KAITAI).colorname1;
//            if(preprinttablemap->value(billno, false)) {//预打单
//                cell[sqr_2 % 3][sqr_2 / 3].showed_color = lds_scr_table::gettableState( tableState_YUDA).colorname1;
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

//    //border
//    painter->setPen(QPen(QColor("dimgrey"), 4));
//    painter->drawPath(getRoundRectPath(parent_rect.adjusted(2, 2, 0, 0), xyRadius, xyRadius, xyRadius, xyRadius));
//    //pie
//    bool gray_mask = false;
//    if(option.state & QStyle::State_Selected) {
//        gray_mask = true;
//    }
//    for(int x = 0; x < 3; x++) {
//        for(int y = 0; y < 3; y++) {
//            painterCell(painter, cell, x, y,
//                        parent_rect,
//                        true,//painter_linear
//                        gray_mask,
//                        xyRadius,
//                        2//pattern_scale
//                        );
//        }
//    }
//    if(rect_is_div) {
//        //增加分割线
//        painter->setPen(QPen(QColor("white"), 0.5));
//        painter->drawLine(QPointF(parent_rect.x() + parent_rect.width() * 1.0/ 3.0, parent_rect.y()), QPointF(parent_rect.x() + parent_rect.width() *1.0 / 3.0, parent_rect.y() + parent_rect.height()) );
//        painter->drawLine(QPointF(parent_rect.x() + parent_rect.width() * 2.0/ 3.0, parent_rect.y()), QPointF(parent_rect.x() + parent_rect.width() * 2.0/ 3.0, parent_rect.y() + parent_rect.height()) );

//        painter->drawLine(QPointF(parent_rect.x(), parent_rect.y() + parent_rect.height() * 1.0/ 3.0), QPointF(parent_rect.x() + parent_rect.width(), parent_rect.y() + parent_rect.height() *1.0 / 3.0) );
//        painter->drawLine(QPointF(parent_rect.x(), parent_rect.y() + parent_rect.height() * 2.0/ 3.0), QPointF(parent_rect.x() + parent_rect.width(), parent_rect.y() + parent_rect.height() *2.0 / 3.0) );
//    }

//    ///开始绘制文本信息
//    parent_rect.adjust(table_border,table_border,-table_border,-table_border);
//    QFontMetrics fm(painter->font());
//    int x = parent_rect.x();
//    int y = parent_rect.y();
//    int w = parent_rect.width();
//    int h = parent_rect.height();
//    int h1 = fm.height();
//    int_person = int_person;
//    int w1 = fm.width(int_person)+5;
//    QPixmap pixmap_man = QPixmap(":/image/man.png").scaledToHeight(h1);
//    int w2 = pixmap_man.width();
//    painter->setPen(QColor(*textColor));

//    ///上一页、下一页
//    if(ch_tableno == "-1" || ch_tableno == "-2") {
//        painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), vch_tablename);
//        return;
//    }

//    //已经开台
//    if(rect_is_div) {//分单
//        if(!num_cost.isEmpty()) {//左上角，右下角
//            show_info = tableno_n_name;
//            painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), show_info);
//            return;
//        }
//    }
//    //未分单,开台
//    if(!rect_is_div && !ch_billno.isEmpty()) {
//        show_info = tableno_n_name;
//        painter->drawText(parent_rect, int(Qt::AlignTop | Qt::AlignLeft), show_info);

//        //画人
//        painter->drawPixmap(QPoint(x+w-w1-w2, y+h-2*h1),  pixmap_man);
//        //            painter->fillRect(QRect(x+w-w1, y+h-2*h1, w1, h1), "green");
//        painter->drawText(QRect(x+w-w1, y+h-2*h1, w1, h1), Qt::AlignCenter, int_person);

//        show_info = "\n"+ pay_unit_prefix + num_cost;
//        painter->drawText(parent_rect, int(Qt::AlignBottom | Qt::AlignRight), show_info);
//        return;
//    }
//    //预订
//    if(order_time.isValid()) {
//        show_info += tableno_n_name;
//        show_info += "\n" + order_time.toString("hh:mm:ss");
//        painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), show_info);
//        return;
//    }
//    //未使用
//    show_info += tableno_n_name +"\n";
//    int w3 = fm.width(int_person)+5;
//    //画人
//    painter->drawPixmap(QPoint(x+(w-w2-w3)/2,  y+(h+h1)/2),  pixmap_man);
//    painter->drawText(QRect(x+(w+w2-w3)/2,  y+(h+h1)/2, w3, h1), Qt::AlignCenter, int_person);
//    painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), show_info);
//}

void restraurantDelegate::keepConnect(restaurantTableView *tableview)
{
    textColor = &tableview->textColor;
}


QMap<int, QString> restraurantDelegate::get_ch_billno_div_map(const QString &ch_billnos)
{
    QStringList ch_billno_list = ch_billnos.split(",");
    QMap<int, QString> map;
    QString ch_billno    = ch_billno_list.value(0);if(!ch_billno  .isEmpty()) map.insert(0, ch_billno  );
    QString ch_billno_1 = ch_billno_list.value(1);if(!ch_billno_1.isEmpty()) map.insert(1, ch_billno_1);
    QString ch_billno_2 = ch_billno_list.value(2);if(!ch_billno_2.isEmpty()) map.insert(2, ch_billno_2);
    QString ch_billno_3 = ch_billno_list.value(3);if(!ch_billno_3.isEmpty()) map.insert(3, ch_billno_3);
    QString ch_billno_4 = ch_billno_list.value(4);if(!ch_billno_4.isEmpty()) map.insert(4, ch_billno_4);

    QString ch_billno_5 = ch_billno_list.value(5);if(!ch_billno_5.isEmpty()) map.insert(5, ch_billno_5);
    QString ch_billno_6 = ch_billno_list.value(6);if(!ch_billno_6.isEmpty()) map.insert(6, ch_billno_6);
    QString ch_billno_7 = ch_billno_list.value(7);if(!ch_billno_7.isEmpty()) map.insert(7, ch_billno_7);
    QString ch_billno_8 = ch_billno_list.value(8);if(!ch_billno_8.isEmpty()) map.insert(8, ch_billno_8);

    return map;
}

bool restraurantDelegate::ch_billno_is_div(const QMap<int, QString> &map)
{
    return
            map.contains(1) || map.contains(2) || map.contains(3)
            || map.contains(4) || map.contains(5) || map.contains(6)
            || map.contains(7) || map.contains(8);
}

bool restraurantDelegate::ch_billno_is_div(const QString &ch_billnos)
{
    return ch_billno_is_div(get_ch_billno_div_map(ch_billnos));
}

bool restraurantDelegate::bar_is_div(const QStringList &int_div_id_list)
{
    bool rect_is_div = int_div_id_list.contains("1") || int_div_id_list.contains("2") || int_div_id_list.contains("3")
            || int_div_id_list.contains("4") || int_div_id_list.contains("5") || int_div_id_list.contains("6")
            || int_div_id_list.contains("7") || int_div_id_list.contains("8");
    return rect_is_div;
}

void restraurantDelegate::paintCellFill(restraurantDelegate::paintCellData &p_dta, const QSqlRecord &index) const
{
    p_dta.vch_tablename = index.value(0).toString();
    p_dta.ch_tableno = index.value(0 +1).toString();
    p_dta.ch_billnos = index.value(0 +2).toString();

    p_dta.int_person = lds::septNumber(index.value(0 +3/*int_person*/).toInt());
    p_dta.num_cost = lds::septNumber(index.value(0 +4/*num_cost*/).toDouble(), model_cache->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    p_dta.order_tableno = index.value(0 +5).toString();//order_tableno
    p_dta.order_time = index.value(0 +6).toTime();//order_time
    p_dta.State_Selected_checked = index.value(0 +7/*check*/).toString();//1,2,3,0
    p_dta.disable_sqr_list = index.value(0 +8/*disable*/).toString().split(",", QString::SkipEmptyParts);//1,2,3,0
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
        p_dta.div_bar = index.value(0 +9/*div_bar*/).toString().split(",", QString::SkipEmptyParts);
    }

    //other
    p_dta.pay_unit_prefix = model_cache->tablename_kvmap_value("cey_sys_parameter", "pay_unit_prefix", "vch_parameter", "vch_value").toString();
    p_dta.preprinttablemap = preprinttablemap;
    //painter
    //    p_dta.rect = option.rect;
    //    p_dta.option_state = option.state;
}

void restraurantDelegate::paintCellFill(restraurantDelegate::paintCellData &p_dta, const QModelIndex &index) const
{
    p_dta.vch_tablename = index.data(Qt::UserRole).toString();
    p_dta.ch_tableno = index.data(Qt::UserRole+1).toString();
    p_dta.ch_billnos = index.data(Qt::UserRole+2).toString();

    p_dta.int_person = lds::septNumber(index.data(Qt::UserRole+3/*int_person*/).toInt());
    p_dta.num_cost = lds::septNumber(index.data(Qt::UserRole+4/*num_cost*/).toDouble(), model_cache->get_cey_sys_parameter_value("bp_fmt_amount_dec", 2).toInt());
    p_dta.order_tableno = index.data(Qt::UserRole+5).toString();//order_tableno
    p_dta.order_time = index.data(Qt::UserRole+6).toTime();//order_time
    p_dta.State_Selected_checked = index.data(Qt::UserRole+7/*check*/).toString();//1,2,3,0
    p_dta.disable_sqr_list = index.data(Qt::UserRole+8/*disable*/).toString().split(",", QString::SkipEmptyParts);//1,2,3,0
    if(2 == lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
        p_dta.div_bar = index.data(Qt::UserRole+9/*div_bar*/).toString().split(",", QString::SkipEmptyParts);
    }

    //other
    p_dta.pay_unit_prefix = model_cache->tablename_kvmap_value("cey_sys_parameter", "pay_unit_prefix", "vch_parameter", "vch_value").toString();
    p_dta.preprinttablemap = preprinttablemap;
    //painter
    //    p_dta.rect = option.rect;
    //    p_dta.option_state = option.state;
}

void restraurantDelegate::paintTable(QPainter *painter, const restraurantDelegate::paintCellData &p_dta)
{
    const QString &vch_tablename = p_dta.vch_tablename;
    const QString &ch_tableno = p_dta.ch_tableno;
    const QString &ch_billnos = p_dta.ch_billnos;

    const QString &int_person = p_dta.int_person;
    const QString &num_cost = p_dta.num_cost;
    const QString &order_tableno = p_dta.order_tableno;
    const QTime &order_time = p_dta.order_time;
    const QString &State_Selected_checked = p_dta.State_Selected_checked;
    const QStringList &disable_sqr_list = p_dta.disable_sqr_list;

    const QStringList &div_bar = p_dta.div_bar;

    const QString &pay_unit_prefix = p_dta.pay_unit_prefix;
    const QRect &rect = p_dta.rect;
    const QStyle::State &option_state = p_dta.option_state;
    const QMap<QString/*ch_tableno*/, bool> *preprinttablemap = p_dta.preprinttablemap;
    //
    const int table_border = 3;
    QRect parent_rect=rect.adjusted(table_border, table_border, -table_border - rect.width() % 3, -table_border - rect.height() % 3);
    parent_rect.adjust(0, 0, - parent_rect.width() % 3, - parent_rect.height() % 3);//确保cell的长宽是3的倍数
    QFont f = painter->font();

    QMap<int, QString> map = get_ch_billno_div_map(ch_billnos);
    const QString show_table_code_name = lds::sysconf->value("w_scr_dish_restaurant_parameter/map_show", "show_table_name").toString();
    QString tableno_n_name;
    if(show_table_code_name == "show_table_code") {
        tableno_n_name = ch_tableno;
    } else if(show_table_code_name == "show_table_name") {
        tableno_n_name = vch_tablename;
    } else if(show_table_code_name == "show_table_code_name") {
        tableno_n_name = ch_tableno + "\n" + vch_tablename;
    } else {
        tableno_n_name = ch_tableno + "\n" + vch_tablename;
    }

    //2
    if(ch_tableno.isEmpty()) return;
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    f.setPointSize(10);
    painter->setFont(f);

    //
    if(ch_tableno == "-3") {//新增
        painter->setPen(QPen(QColor("#555555"), 3, Qt::DashDotDotLine));

        parent_rect.adjust(1,1,-1,-1);
        double woff = parent_rect.width()/10.0;
        double hoff = parent_rect.height()/10.0;
        painter->drawRoundedRect(parent_rect, woff, hoff);

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor("#555555"));
        painter->drawRoundedRect(parent_rect.adjusted(woff*1.5, hoff*4, -woff*1.5, -hoff*4), hoff/1.2, hoff/1.2);
        painter->drawRoundedRect(parent_rect.adjusted(woff*4, hoff*1.5, -woff*4, -hoff*1.5), woff/1.2, woff/1.2);
        return;
    }
    ///绘制准备
    //ch_billno
    QString ch_billno =  map.value(0);
    //rect_is_div
    bool rect_is_div = ch_billno_is_div(map)
            || bar_is_div(div_bar);

    QString show_info;
    cell_content cell[3][3];
    tableSTATE tstate ;
    ///头
    //空闲
    tstate= lds_scr_table::gettableState( tableState_KONGXIAN);
    //预订
    if(!order_tableno.isEmpty()) {
        tstate= lds_scr_table::gettableState(tableState_YUDING);
    }
    /////////未分单
    if(false == rect_is_div) {
        //开台
        if(ch_billno.length() > 0) {
            tstate= lds_scr_table::gettableState(tableState_KAITAI);
        }
        if(ch_billno.startsWith("R")) {//二维码点餐
            tstate = lds_scr_table::gettableState(tableState_QR_CODE);
        }
        //预打单
        if(ch_billno.length() > 0) {
            if(preprinttablemap->value(ch_billno, false)) {
                tstate= lds_scr_table::gettableState( tableState_YUDA);
            }
        }
        //酒吧 未清台
        if(div_bar.count() > 0) {
            for(int k = 0; k < 9; k ++) {
                int sqr_2 = static_chbillno2index(k);
                cell[sqr_2 % 3][sqr_2 / 3].isDensePattern = true;
            }
        }
    }
    ///中间
    cell_init_showed_color(cell, tstate.colorname1);
    /////////分单
    if(true == rect_is_div) {
        //开台
        //预打单
        QString billno;
        for(int k = 0; k < 9; k ++) {
            int sqr_2 = static_chbillno2index(k);
            billno = map.value(k);
            if(billno.isEmpty()) continue;
            cell[sqr_2 % 3][sqr_2 / 3].showed_color = lds_scr_table::gettableState(tableState_KAITAI).colorname1;
            if(preprinttablemap->value(billno, false)) {//预打单
                cell[sqr_2 % 3][sqr_2 / 3].showed_color = lds_scr_table::gettableState( tableState_YUDA).colorname1;
            }
        }
        //酒吧 未清台
        for(int k = 0; k < 9; k ++) {
            int sqr_2 = static_chbillno2index(k);
            if(div_bar.contains(QString::number(k))) {
                cell[sqr_2 % 3][sqr_2 / 3].isDensePattern = true;
            }
        }
    }

    ///尾
    //选中
    if(option_state & QStyle::State_Selected) {//选中
        QStringList checklist = State_Selected_checked.split(",", QString::SkipEmptyParts);
        foreach(const QString &check, checklist) {//勾选
            int sqr_2 = static_chbillno2index(check.toInt());
            cell[sqr_2 % 3][sqr_2 / 3].ischecked = true;
        }
    }
    //置灰
    foreach(const QString &sqr, disable_sqr_list) {
        int sqr_2 = static_chbillno2index(sqr.toInt());
        cell[sqr_2 % 3][sqr_2 / 3].isdisabled = true;
    }
    //////////绘制

    //border
    painter->setPen(QPen(QColor("dimgrey"), 4));
    painter->drawPath(getRoundRectPath(parent_rect.adjusted(2, 2, 0, 0), xyRadius, xyRadius, xyRadius, xyRadius));
    //pie
    bool gray_mask = false;
    if(option_state & QStyle::State_Selected) {
        gray_mask = true;
    }
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            painterCell(painter, cell, x, y,
                        parent_rect,
                        true,//painter_linear
                        gray_mask,
                        xyRadius,
                        2//pattern_scale
                        );
        }
    }
    if(rect_is_div) {
        //增加分割线
        painter->setPen(QPen(QColor("white"), 0.5));
        painter->drawLine(QPointF(parent_rect.x() + parent_rect.width() * 1.0/ 3.0, parent_rect.y()), QPointF(parent_rect.x() + parent_rect.width() *1.0 / 3.0, parent_rect.y() + parent_rect.height()) );
        painter->drawLine(QPointF(parent_rect.x() + parent_rect.width() * 2.0/ 3.0, parent_rect.y()), QPointF(parent_rect.x() + parent_rect.width() * 2.0/ 3.0, parent_rect.y() + parent_rect.height()) );

        painter->drawLine(QPointF(parent_rect.x(), parent_rect.y() + parent_rect.height() * 1.0/ 3.0), QPointF(parent_rect.x() + parent_rect.width(), parent_rect.y() + parent_rect.height() *1.0 / 3.0) );
        painter->drawLine(QPointF(parent_rect.x(), parent_rect.y() + parent_rect.height() * 2.0/ 3.0), QPointF(parent_rect.x() + parent_rect.width(), parent_rect.y() + parent_rect.height() *2.0 / 3.0) );
    }

    ///开始绘制文本信息
    parent_rect.adjust(table_border,table_border,-table_border,-table_border);
    QFontMetrics fm(painter->font());
    int x = parent_rect.x();
    int y = parent_rect.y();
    int w = parent_rect.width();
    int h = parent_rect.height();
    int h1 = fm.height();
//    int_person = int_person;
    int w1 = fm.width(int_person)+5;
    QPixmap pixmap_man = QPixmap(":/image/man.png").scaledToHeight(h1);
    int w2 = pixmap_man.width();
//    painter->setPen(QColor(*textColor));
    painter->setPen(QColor("black"));

    ///上一页、下一页
    if(ch_tableno == "-1" || ch_tableno == "-2") {
        painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), vch_tablename);
        return;
    }

    //已经开台
    if(rect_is_div) {//分单
        if(!num_cost.isEmpty()) {//左上角，右下角
            show_info = tableno_n_name;
            painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), show_info);
            return;
        }
    }
    //未分单,开台
    if(!rect_is_div && !ch_billno.isEmpty()) {
        show_info = tableno_n_name;
        painter->drawText(parent_rect, int(Qt::AlignTop | Qt::AlignLeft), show_info);

        //画人
        painter->drawPixmap(QPoint(x+w-w1-w2, y+h-2*h1),  pixmap_man);
        //            painter->fillRect(QRect(x+w-w1, y+h-2*h1, w1, h1), "green");
        painter->drawText(QRect(x+w-w1, y+h-2*h1, w1, h1), Qt::AlignCenter, int_person);

        show_info = "\n"+ pay_unit_prefix + num_cost;
        painter->drawText(parent_rect, int(Qt::AlignBottom | Qt::AlignRight), show_info);
        return;
    }
    //预订
    if(order_time.isValid()) {
        show_info += tableno_n_name;
        show_info += "\n" + order_time.toString("hh:mm:ss");
        painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), show_info);
        return;
    }
    //未使用
    show_info += tableno_n_name +"\n";
    int w3 = fm.width(int_person)+5;
    //画人
    painter->drawPixmap(QPoint(x+(w-w2-w3)/2,  y+(h+h1)/2),  pixmap_man);
    painter->drawText(QRect(x+(w+w2-w3)/2,  y+(h+h1)/2, w3, h1), Qt::AlignCenter, int_person);
    painter->drawText(parent_rect, int(Qt::AlignCenter|Qt::TextWordWrap), show_info);
}

const fexpandmain_model_sqltablemodel *restraurantDelegate::get_cache()
{
    return model_cache;
}

//=======================
restraurant9BoxDelegate::restraurant9BoxDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    border = 2;//2的倍数
}

void restraurant9BoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //1
    QRect rect = option.rect;
    QRect parent_rect=rect.adjusted(border, border, -border, -border);
    QFont f = painter->font();
    QString ch_billno = index.data(Qt::UserRole).toString();
    QString cell_index = index.data(Qt::UserRole+1/*index*/).toString();
    QString int_person = index.data(Qt::UserRole+2).toString();
    QString num_cost = index.data(Qt::UserRole+3).toString();
    bool ischecked = index.data(Qt::UserRole+4/*check*/).toBool();//1,2,3,0

    QString vch_tablename = index.data(Qt::UserRole + 5/*vch_tablename*/).toString();
    QString ch_tableno = index.data(Qt::UserRole+6/*ch_tableno*/).toString();

    bool isdisabled = index.data(Qt::UserRole+7/*disable*/).toBool();//1,2,3,0
    bool div_bar = index.data(Qt::UserRole+8/*div_bar*/).toBool();
    if(2 != lds::sysconf->value("system_setting/yingyemoshi", 0).toInt()) {//酒吧模式
        div_bar = false;
    }


    if(pay_unit_prefix.isEmpty()) {
        pay_unit_prefix = n_func::f_get_sysparm("pay_unit_prefix", " ");
    }

    int y = index.row();
    int x = index.column();
    //    //2
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    //    //3
    tableSTATE tstate ;
    ///////////空闲
    tstate= lds_scr_table::gettableState( tableState_KONGXIAN);
    ////////////选中
    if(!ch_billno.isEmpty()) {
        if(preprinttablemap->value(ch_billno, false)) {//若是不是空闲的
            tstate= lds_scr_table::gettableState( tableState_YUDA);
        } else {
            tstate= lds_scr_table::gettableState(tableState_KAITAI);
        }
    }
    if(isdisabled) {
        tstate= lds_scr_table::gettableState(tableState_DISABLE);
    }
    //border
    painter->setPen(QPen(QColor("dimgrey"), border));
    if(combine_state) {
        //border
        painter->drawPath(getRoundRectPath(parent_rect.adjusted(2, 2, 0, 0), xyRadius, xyRadius, xyRadius, xyRadius));

        //pie
        QLinearGradient linearGrad1(parent_rect.topLeft(),       parent_rect.bottomLeft());
        linearGrad1.setColorAt(0, static_color_light(tstate.colorname1, 3));
        linearGrad1.setColorAt(1, static_color_light(tstate.colorname1, 0));
        QPainterPath linear_path = getRoundRectPath(parent_rect, xyRadius, xyRadius, xyRadius, xyRadius);//
        painter->fillPath(linear_path, linearGrad1);
    } else {
        qreal b_halft = border / 2.0;
        QRectF rect = option.rect;
        if(x == 2 && y == 0) {
            painter->drawPath(getRectLinePath(rect.adjusted(3 * border,  3 * border, -b_halft, 0), x, y));
        }
        if(x == 2 && y == 1) {
            painter->drawPath(getRectLinePath(rect.adjusted(0, 0, -b_halft, 0), x, y));
        }
        if(x == 1 && y == 2) {
            painter->drawPath(getRectLinePath(rect.adjusted(0, 0, 0, -b_halft), x, y));
        }
        if(x == 0 && y == 2) {
            painter->drawPath(getRectLinePath(rect.adjusted(3 * border,  3 * border, 0, -b_halft), x, y));
        }
        if(x == 2 && y == 2) {
            painter->drawPath(getRectLinePath(rect.adjusted(0, 0, -b_halft, -b_halft), x, y));
        }
        //fill
        QLinearGradient linearGrad1(parent_rect.topLeft(),       parent_rect.bottomLeft());
        linearGrad1.setColorAt(0, static_color_light(tstate.colorname1, 3 - y));
        linearGrad1.setColorAt(1, static_color_light(tstate.colorname1, 3 - y - 1));
        QPainterPath linear_path;
        if(0 == x && 0 == y) {
            linear_path = getRoundRectPath(parent_rect, xyRadius, 0, 0, 0);//
        } else if(0 == x && 2 == y) {
            linear_path = getRoundRectPath(parent_rect, 0, xyRadius, 0, 0);//
        } else if(2 == x && 2 == y) {
            linear_path = getRoundRectPath(parent_rect, 0, 0, xyRadius, 0);//
        } else if(2 == x && 0 == y) {
            linear_path = getRoundRectPath(parent_rect, 0, 0, 0, xyRadius);//
        } else {
            linear_path = getRoundRectPath(parent_rect, 0, 0, 0, 0);//
        }
        painter->fillPath(linear_path, linearGrad1);
        if(div_bar) {
            restraurantDelegate::paintDensePattern(painter,
                                                   linear_path,
                                                   QColor(restraurantDelegate::gettableState(tableState_BAR).colorname1),
                                                   Qt::DiagCrossPattern,
                                                   2,
                                                   false);
        }
    }
    //开始绘制
    parent_rect.adjust(border, border, -border, -border);
    painter->setPen(QColor("black"));
    f.setPointSize(10);
    f.setBold(false);
    painter->setFont(f);
    QFontMetrics fm(painter->font());
    int xx = parent_rect.x();
    int yy = parent_rect.y();
    int w = parent_rect.width();
    int h = parent_rect.height();
    int h1 = fm.height();
    int_person = int_person;
    int w1 = fm.width(int_person)+5;
    QPixmap pixmap_man = QPixmap(":/image/man.png").scaledToHeight(h1);
    int w2 = pixmap_man.width();

    //
    if(!ch_tableno.isEmpty() || !vch_tablename.isEmpty()) {
        painter->drawText(parent_rect, int(Qt::AlignCenter), ch_tableno+"\n"+vch_tablename);
    } else {
        if(!int_person.isEmpty() || !num_cost.isEmpty()) {
            painter->drawPixmap(QPoint(xx+w-w1-w2, yy+h-2*h1),  pixmap_man);
            //            painter->fillRect(QRect(x+w-w1, yy+h-2*h1, w1, h1), "green");
            painter->drawText(QRect(xx+w-w1, yy+h-2*h1, w1, h1), Qt::AlignCenter, int_person);

            painter->drawText(parent_rect, int(Qt::AlignBottom | Qt::AlignRight), "\n" + pay_unit_prefix +QString().sprintf("%.02f", num_cost.toFloat()));
        }
        //////////////////////////////////////////////////
        f.setPointSize(20);
        f.setBold(true);
        painter->setFont(f);
        painter->drawText(parent_rect, int(Qt::AlignCenter), cell_index);
    }

    //绘制 选中边框
    if(isdisabled == false) {
        if(option.state & QStyle::State_Selected) {
            QPixmap pg = QPixmap(":/image/widgets_pic/check_on.png").scaledToWidth(20, Qt::SmoothTransformation);
            painter->drawPixmap(QRect(parent_rect.x()+parent_rect.width()-pg.width(), parent_rect.y(), pg.width(), pg.height()), pg);
        } else {
            if(ischecked) {//未选中 但是check
                QPixmap pg = QPixmap(":/image/widgets_pic/check_on.png").scaledToWidth(20, Qt::SmoothTransformation);
                painter->drawPixmap(QRect(parent_rect.x()+parent_rect.width()-pg.width(), parent_rect.y(), pg.width(), pg.height()), pg);
            }
        }
    }
}

void restraurant9BoxDelegate::tryCombine()
{
    combine_state = true;
}

QString restraurantTableStateDelegate::paint_transfer(const QVariant &dd) const
{
    QString text = restraurantDelegate::gettableState((tableState)dd.toInt()).key;
    return text;
}
