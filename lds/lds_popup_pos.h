#ifndef LDS_POPUP_POS_H
#define LDS_POPUP_POS_H

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QRect>

class lds_popup_pos : public QObject
{
    Q_OBJECT
public:
    enum SIZETYPE{
        SIZEHNIT,
        SIZE
    };
public:
    explicit lds_popup_pos(QObject *parent = 0);

    static QPoint getPopPoint(const QPoint &gpos, const QPoint &gpos2, const QSize &popsize, Qt::LayoutDirection dir);
    static QPoint getPopPoint(QWidget *parent, QWidget *popwidget);
    static QPoint getPopPoint(QWidget *parent, const QRect &section, QWidget *popwidget);
    static QPoint getPopPoint_size(QWidget *parent, QWidget *popwidget);
    static QPoint getPopPoint0(QWidget *parent, QWidget *popwidget, SIZETYPE type);
    static QPoint getGlobalCenterPopup(QWidget *parent, QWidget *pop);
    static QPoint getParentCenterPopup(QWidget *parent, QWidget *pop);

};

#endif // LDS_POPUP_POS_H
