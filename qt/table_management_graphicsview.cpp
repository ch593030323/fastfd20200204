#include "table_management_graphicsview.h"
#include "table_management_pad.h"
#include <QtDebug>
#include "table_management_desk.h"
#include <QStyle>
#include <QFile>
#include<QGraphicsScene>
#include "lds_query_hddpos.h"

table_management_graphicsview::table_management_graphicsview(QWidget *parent)
    : QGraphicsView(parent)
{
    pad = 0;
    add_item_off = 0;
    //    btn = 0;
    setRenderHint(QPainter::Antialiasing, false);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    edit_mode = false;
}

table_management_graphicsview::~table_management_graphicsview()
{
}

void table_management_graphicsview::add_pad_once(table_management_pad *pad)
{
    this->curScene()->addItem(pad);
    pad->hide();
    this->pad = pad;
}

table_management_pad *table_management_graphicsview::get_pad()
{
    return this->pad;
}

QStringList table_management_graphicsview::get_item_ch_tableno_list()
{
    QStringList list;
    if(!this->isVisible()) return list;

    QGraphicsScene *scene = this->curScene();
    if(scene == 0) return list;

    for(int  count = scene->items().count() - 1; count >= 0; count--) {
        table_management_desk *item = qgraphicsitem_cast<table_management_desk *>(scene->items()[count]);
        if(item) {
            list << item->data(table_management_desk::item_ch_tableno).toString();
        }
    }
    return list;
}

table_management_desk * table_management_graphicsview::add_item(const QString &ch_tableno, const QString &ch_areano, table_management_desk::shapeType type)
{
    QGraphicsScene *scene = this->curScene();
    table_management_desk *item = new table_management_desk(pad, type);
    item->setData(table_management_desk::item_ch_tableno, ch_tableno);
    item->setData(table_management_desk::item_ch_areano, ch_areano);
    item->setPos(150 + add_item_off, 150 + add_item_off );
    scene->addItem(item);

    if(!is_edit_mode()) {
        item->setFlags(item->flags() & ~QGraphicsItem::ItemIsMovable);
    }

    add_item_off += 30;
    return item;
}

void table_management_graphicsview::delete_item(const QString &ch_tableno)
{
    QGraphicsScene *scene = this->curScene();
    for(int  count = scene->items().count() - 1; count >= 0; count--) {
        table_management_desk *item = qgraphicsitem_cast<table_management_desk *>(scene->items()[count]);
        if(item
                && item->data(table_management_desk::item_ch_tableno).toString() == ch_tableno) {
            //Removes the item item and all its children from the scene. The ownership of item is passed on to the caller (i.e., QGraphicsScene will no longer delete item when destroyed).
            scene->removeItem(item);
            emit_signal_table_deleted(item->data(table_management_desk::item_ch_tableno).toString());
            delete item;
        }
    }
}

void table_management_graphicsview::goto_edit_mode()
{
    add_item_off = 0;

    QGraphicsScene *scene = this->curScene();
    for(int  count = scene->items().count() - 1; count >= 0; count--) {
        table_management_desk *item = qgraphicsitem_cast<table_management_desk *>(scene->items()[count]);
        if(item) {
            item->setFlags(item->flags() | QGraphicsItem::ItemIsMovable);
        }
    }
    edit_mode = true;
}

void table_management_graphicsview::goout_edit_mode()
{
    add_item_off = 0;

    QGraphicsScene *scene = this->curScene();
    for(int  count = scene->items().count() - 1; count >= 0; count--) {
        table_management_desk *item = qgraphicsitem_cast<table_management_desk *>(scene->items()[count]);
        if(item) {
            item->setFlags(item->flags() & ~QGraphicsItem::ItemIsMovable);
        }
    }
    edit_mode = false;
}

bool table_management_graphicsview::is_edit_mode()
{
    return edit_mode;
}

void table_management_graphicsview::emit_signal_table_clicked(const QString &ch_tableno)
{
    emit signal_table_clicked(ch_tableno);
}

void table_management_graphicsview::emit_signal_table_double_clicked(const QString &ch_tableno)
{
    emit signal_table_double_clicked(ch_tableno);
}

void table_management_graphicsview::emit_signal_table_deleted(const QString &ch_tableno)
{
    emit signal_table_deleted(ch_tableno);
}

void table_management_graphicsview::filter_by_ch_areano(const QString &ch_areano)
{
    if(!this->isVisible()) return;

    QGraphicsScene *scene = this->curScene();
    for(int  count = scene->items().count() - 1; count >= 0; count--) {
        table_management_desk *item = qgraphicsitem_cast<table_management_desk *>(scene->items()[count]);
        if(item) {
            if(!ch_areano.isEmpty()
                    && item->data(table_management_desk::item_ch_areano).toString() != ch_areano) {
                item->hide();
            } else {
                item->show();
            }
        }
    }
}

const QMap<QString, QString> &table_management_graphicsview::table_no_name_map_get()
{
    return table_no_name_map;
}

void table_management_graphicsview::table_no_name_map_insert(const QString &ch_tableno, const QString &vch_tablename)
{
    table_no_name_map.insert(ch_tableno, vch_tablename);
}

bool table_management_graphicsview::press_event_do_double_clicked(const QString &ch_tableno)
{
    return do_double_clicked.press_event_do_double_clicked(ch_tableno);
}

void table_management_graphicsview::desk_save(const QString &filepath)
{
    QByteArray ba;
    QDataStream in(&ba, QIODevice::WriteOnly);    // read the data serialized from the file

    QGraphicsScene *scene = this->curScene();
    for(int  count = scene->items().count() - 1; count >= 0; count--) {
        table_management_desk *item = qgraphicsitem_cast<table_management_desk *>(scene->items()[count]);
        if(item) {
            for(int k = table_management_desk::item_hscale_min; k < table_management_desk::item_max; k ++) {
                QVariant d;
                if(k == table_management_desk::item_pos) {
                    d = item->pos();
                } else {
                    d = item->data(k);
                }
                in << d;
            }
        }
    }

    //
    QFile file(filepath);
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << file.errorString();
        return;
    }
    file.write(ba);
    file.close();

    //
    add_item_off = 0;
}

void table_management_graphicsview::desk_load_init_scene(const QString &filepath)
{
    //
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        return;
    }

    //
    QByteArray ba = file.readAll();
    QDataStream in(&ba, QIODevice::ReadOnly);    // read the data serialized from the file

    while(!in.atEnd()) {
        table_management_desk *item = add_item("", "");
        for(int k = table_management_desk::item_hscale_min; k < table_management_desk::item_max; k ++) {
            QVariant d;
            in >> d;
            if(k == table_management_desk::item_pos) {
                item->setPos(d.toPointF());
            } if(k == table_management_desk::item_rotate) {
                item->setRotation(d.toInt());
            }

            item->setData(k, d);
        }
    }
}

void table_management_graphicsview::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if(curScene())
        curScene()->setSceneRect(QRect(0, 0, this->width(), this->height()));

    if(pad) {
        pad->update_boundrect(rect());
    }
}

QGraphicsScene *table_management_graphicsview::curScene()
{
    if(this->scene() == 0) {
        QGraphicsScene *scene = new QGraphicsScene(this);
        setScene(scene);
        setStyleSheet("background:url(:/image/table_bg.png); padding:-2px;border:0px;");
        add_pad_once(new table_management_pad);
    }
    return this->scene();
}
