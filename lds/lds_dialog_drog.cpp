#include "lds_dialog_drog.h"
#include <QMouseEvent>

lds_dialog_drog::lds_dialog_drog(QWidget *parent) :
    QDialog(parent),
    mouse_press(false)
{

}
void lds_dialog_drog::mousePressEvent( QMouseEvent * event )
{
    //只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
    }

    //窗口移动距离
    move_point = event->globalPos() - pos();
}

void lds_dialog_drog::mouseReleaseEvent(QMouseEvent *)
{
    mouse_press = false;
}

void lds_dialog_drog::mouseMoveEvent(QMouseEvent *event)
{
#ifdef QT_DEBUG
    //移动窗口
    if(mouse_press)
    {
        QPoint move_pos = event->globalPos();
        move(move_pos - move_point);
    }
#endif
}

