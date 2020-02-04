#ifndef table_management_MANAGEMENT_PAD_H
#define table_management_MANAGEMENT_PAD_H

#include <QGraphicsItem>
class table_management_desk;
class table_management_pad_control;

class table_management_pad : public QGraphicsItem
{
public:
    table_management_pad();
    ~table_management_pad();
    QRectF boundingRect() const ;
    QPainterPath shape() const ;
    void paint(QPainter */*painter*/, const QStyleOptionGraphicsItem */*item*/, QWidget */*widget*/);


    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

    void setPos(const QPointF &pos);
    QSizeF get_control_size();
    QPointF get_control_pos();
    void set_app_item(table_management_desk *item);

    void update_boundrect(const QRectF &boundrect);
private:
    table_management_pad_control *control;
    QRectF boundrect;
};

class table_management_pad_control : public QGraphicsItem
{
public:
    table_management_pad_control(QGraphicsItem *parent = 0);\
    ~table_management_pad_control();

    QRectF boundingRect() const ;
    QPainterPath shape() const ;

    table_management_desk *app_item;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem */*item*/, QWidget */*widget*/);

    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    void transform_app_item(const QString &text);
private:
    QPixmap pg;
};

class table_management_pad_control_btn : public QGraphicsItem
{
public:
    table_management_pad_control_btn(const QString &text, table_management_pad_control *parent = 0);\
    ~table_management_pad_control_btn();
    QRectF boundingRect() const ;
    QPainterPath shape() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem */*item*/, QWidget */*widget*/);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);


private:
    QString text;
    bool ispressed;
    table_management_pad_control *parent_item;
};

#endif // table_management_MANAGEMENT_PAD_H
