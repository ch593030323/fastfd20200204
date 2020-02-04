#ifndef Lds_DIALOG_DROG_H
#define Lds_DIALOG_DROG_H

#include <QDialog>

class lds_dialog_drog : public QDialog
{
    Q_OBJECT
public:
    explicit lds_dialog_drog(QWidget *parent = 0);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    bool mouse_press; //按下鼠标左键
    QPoint move_point; //移动的距离

};

#endif // Lds_DIALOG_DROG_H
