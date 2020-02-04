#include "lds_popup_pos.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>

lds_popup_pos::lds_popup_pos(QObject *parent) :
    QObject(parent)
{
}

QPoint lds_popup_pos::getPopPoint(const QPoint &gpos,const QPoint &gpos2, const QSize &popsize, Qt::LayoutDirection dir)
{
    QPoint pos = gpos;
    QPoint pos2 =  gpos2;
    QSize main_window;
#if defined(Q_OS_UNIX) && defined(QT_NO_DEBUG)
    main_window = QApplication::desktop()->availableGeometry().adjusted(0, 0, 0, -40).size();
#else
    main_window = QApplication::desktop()->availableGeometry(pos).size();
#endif
    QRect screen = QRect(QApplication::desktop()->availableGeometry().topLeft(),  main_window);
//    if (dir == Qt::RightToLeft) {
//        pos.setX(pos.x()-popsize.width());
//        pos2.setX(pos2.x()-popsize.width());
//        if (pos.x() < screen.left())
//            pos.setX(qMax(pos.x(), screen.left()));
//        else if (pos.x()+popsize.width() > screen.right())
//            pos.setX(qMax(pos.x()-popsize.width(), screen.right()-popsize.width()));
//    } else {
        if (pos.x()+popsize.width() > screen.right())
            pos.setX(screen.right()-popsize.width());
        pos.setX(qMax(pos.x(), screen.left()));
//    }
    if (pos.y() + popsize.height() > screen.bottom())
        pos.setY(pos2.y() - popsize.height());
    else if (pos.y() < screen.top())
        pos.setY(screen.top());
    if (pos.y() < screen.top())
        pos.setY(screen.top());
    if (pos.y()+popsize.height() > screen.bottom())
        pos.setY(screen.bottom()-popsize.height());

    return pos;
}

QPoint lds_popup_pos::getPopPoint(QWidget *parent, QWidget *popwidget)
{
    return getPopPoint(parent, parent->rect(), popwidget);
}

QPoint lds_popup_pos::getPopPoint(QWidget *parent, const QRect &section, QWidget *popwidget)
{
    QPoint bottomLeft = section.bottomLeft();
    QPoint topLeft = section.topLeft();

    bottomLeft = parent->mapToGlobal(bottomLeft);
    topLeft = parent->mapToGlobal(topLeft);

    return lds_popup_pos::getPopPoint(bottomLeft, topLeft, popwidget->size(), popwidget->layoutDirection());
}

QPoint lds_popup_pos::getPopPoint_size(QWidget *parent, QWidget *popwidget)
{
    return lds_popup_pos::getPopPoint0(parent, popwidget, SIZE);
}

QPoint lds_popup_pos::getPopPoint0(QWidget *parent, QWidget *popwidget, SIZETYPE type)
{
    QPoint pos = (parent->layoutDirection() == Qt::RightToLeft) ? parent->rect().bottomRight() : parent->rect().bottomLeft();
    QPoint pos2 = (parent->layoutDirection() == Qt::RightToLeft) ? parent->rect().topRight() : parent->rect().topLeft();

    pos = parent->mapToGlobal(pos);
    pos2 = parent->mapToGlobal(pos2);

    QSize size;
    switch(type) {
    case SIZEHNIT:
        size = popwidget->sizeHint();
        break;
    case SIZE:
        size = popwidget->size();
        break;
    }

    return getPopPoint(pos, pos2, size, parent->layoutDirection());
}

/**
pop居中与父类的的左上角的坐标*/
QPoint lds_popup_pos::getGlobalCenterPopup(QWidget *parent, QWidget *pop) {
    QPoint p = parent->mapToGlobal(QPoint(0,0));
    return QStyle::alignedRect(parent->layoutDirection(), Qt::AlignCenter, pop->size(), QRect(p, QSize(parent->size()))).topLeft();
}

QPoint lds_popup_pos::getParentCenterPopup(QWidget *parent, QWidget *pop)
{
    QPoint p =QPoint(0,0);
    return QStyle::alignedRect(parent->layoutDirection(), Qt::AlignCenter, pop->size(), QRect(p, QSize(parent->size()))).topLeft();

}
