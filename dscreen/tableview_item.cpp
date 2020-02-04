#include "tableview_item.h"
#include <QFileInfoList>
#include <QDir>
#include <QTimer>
#include <QHeaderView>
#include <QPainter>
#include "tableview_item.h"
#include <QHideEvent>
#include <QtDebug>
#include <QApplication>
#include <qdesktopwidget.h>

static QString pix_path;
static QList<tableview_item::BG_RUN_SET>
run_list;
static int time_step = 0;
static QStringList image_filepath_list;
static int image_step = 0;
static bool time_running = false;
static QPixmap *pix = 0;

tableview_item::tableview_item(QWidget *parent)
    : QTableView(parent)
{
    model = new tableview_item_model(parent);
    setModel(model);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    verticalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setItemDelegate(new tableview_item_delegate(this, this));
    setShowGrid(false);
    setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //
    int rowColumnCount = model->rowCount() * model->columnCount();
    int rowcount = model->rowCount();
    int columncount = model->columnCount();
    BG_RUN_SET run0;
    //左上-》右上  左下-》右下
    run0.clear();
    for(int column = 0; column < rowColumnCount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        run0[column][column] = true;
    }
    run_list.append(run0);

    //上-》下
    run0.clear();
    for(int column = 0; column < rowcount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        for(int row = 0; row < columncount; row++) {
            run0[column][column*columncount + row] = true;
        }
    }
    run_list.append(run0);
    //左-》右
    run0.clear();
    for(int column = 0; column < columncount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        for(int row = 0; row < rowcount; row++) {
            run0[column][row*columncount + column] = true;
        }
    }
    run_list.append(run0);
    //左上-》右下
    run0.clear();
    for(int column = 0; column < columncount + rowcount; column++) {
        run0.append(QBitArray(rowColumnCount, false));
        for(int row = 0; row < column + 1; row++) {
            int k = row*columncount + (column-row);
            if(k >= 0 && k < rowColumnCount) {
                run0[column][k] = true;
            }
        }
    }
    run_list.append(run0);

    //
    timer = new QTimer(this);
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()),this,SLOT(togo()));
    //
}

tableview_item::~tableview_item()
{
    if(pix) delete pix; pix = 0;
    if(pix) delete pix;
}

void tableview_item::initScene(const QString &imagepath)
{
    ///
    image_filepath_list.clear();
    image_step = 0;

    QFileInfoList fileinfos=QDir(imagepath).entryInfoList();
    foreach(const QFileInfo &info, fileinfos) {
        if(info.fileName() == "."
                || info.fileName() == ".."
                || info.fileName().endsWith(".ini"))
            continue;
        image_filepath_list.append(info.filePath());
    }
}

void tableview_item::showPixmap(const QString &mpath)
{
    showPixmap(QPixmap(mpath));
}

void tableview_item::showPixmap(const QPixmap &m)
{
    if(pix) delete pix;
    pix  = new QPixmap(m);

    stop();
    for(int row = 0; row < model->rowCount(); row ++) {
        for(int column = 0; column < model->columnCount(); column ++) {
            this->update(model->index(row, column));
        }
    }
}

void tableview_item::start(int msec)
{
    timer->start(msec);
}

void tableview_item::start()
{
    if(timer->isActive()) {
    } else {
        timer->start();
    }
}

void tableview_item::stop()
{
    timer->stop();
}

void tableview_item::closeEvent(QCloseEvent *e)
{
    QTableView::closeEvent(e);
}

void tableview_item::showEvent(QShowEvent *e)
{
    QTableView::showEvent(e);
}

void tableview_item::hideEvent(QHideEvent *e)
{
    QTableView::hideEvent(e);
}

void tableview_item::mousePressEvent(QMouseEvent *e)
{
    QTableView::mousePressEvent(e);
    emit signal_mouse_pressed();
}

void tableview_item::togo()
{
    static int run_going = 0;
    static int time_pause_count = 4;
    static int time_pause_k = 0;
    //
    if(false == time_running)
        time_running = true;

    //
    if(timer->interval() == 500) {
        //动画停止倒计时
        if(time_pause_k < time_pause_count) {
            time_pause_k ++;
            return;
        }
        //动画启动
        time_pause_k = 0;
    }

    BG_RUN_SET *run0 = &run_list[run_going];
    int loop = time_step / run0->count();//0 1
    if(loop == 1) {
        run_going = rand()%run_list.count();
        run0 = &run_list[run_going];
        time_step = 0;
        image_step++;
        if(image_filepath_list.count() > 0)
            image_step = image_step % image_filepath_list.count();
        else
            image_step = 0;


        //        default
        timer->setInterval(80);
        //左上-》右上  左下-》右下
        if(0 == run_going) timer->setInterval(80);
        //上-》下
        if(1 == run_going) timer->setInterval(150);
        //左-》右
        if(2 == run_going) timer->setInterval(150);
        //左上-》右下
        if(3 == run_going) timer->setInterval(100);
    }
    ///
    const QBitArray &bit = run0->at(time_step);

    if(pix_path != image_filepath_list.value(image_step)) {
        pix_path = image_filepath_list.value(image_step);
        if(pix) delete pix; pix = 0;
        pix = new QPixmap(pix_path);
    }
    //
    for(int k = 0; k < bit.count(); k++) {
        if(bit[k] == true) {
            update(model->index(k/model->columnCount(),k%model->columnCount()));
        }
    }
    //
    //绘制最后一张图
    if(run0->count() == time_step + 1) {
        timer->setInterval(500);
    }

    time_step ++;
}

tableview_item_delegate::tableview_item_delegate(QTableView *tableview, QObject *parent)
    : QItemDelegate(parent)
{
    this->tableview = tableview;
}

void tableview_item_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(time_running == false) return;

    if(pix && !pix->isNull()) {
        //
        qreal tableview_radio = tableview->viewport()->width() * 1.0 / tableview->viewport()->height();
        qreal p_radio = pix->width() * 1.0 / pix->height();

        //
        QRect prect;
        if(tableview_radio > p_radio) {
            prect = QRect(0, 0, tableview->viewport()->height() * p_radio, tableview->viewport()->height());
        } else {
            prect = QRect(0, 0, tableview->viewport()->width(), tableview->viewport()->width() / p_radio);
        }

        qreal radio = pix->width()*1.0 / prect.width();
        //
        QPoint pos_origin = QPoint((tableview->viewport()->width() - prect.width()) / 2, (tableview->viewport()->height() - prect.height()) / 2);
        prect.translate(pos_origin);
        prect =  prect.intersected(option.rect);
        if(!prect.isEmpty()) {
            painter->drawPixmap(prect, pix->copy(QRect((prect.topLeft() - pos_origin) * radio, prect.size() * radio)));
        }
    }

}

int tableview_item_model::rowCount(const QModelIndex &parent) const
{
    return 6;
}

int tableview_item_model::columnCount(const QModelIndex &parent) const
{
    return 8;
}

QVariant tableview_item_model::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant tableview_item_model::headerData(int, Qt::Orientation, int role) const
{
    return QVariant();
}
