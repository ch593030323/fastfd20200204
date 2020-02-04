#ifndef LDS_PUSHBUTTON_WRAP_H
#define LDS_PUSHBUTTON_WRAP_H

#include <QPushButton>
#include "lds_bool.h"
#include <QMap>
#include <QVariant>
#include "lds_pushbutton.h"

class lds_pushbutton_wrap : public lds_pushbutton
{
    Q_OBJECT
public:
    explicit lds_pushbutton_wrap(QWidget *parent = 0);
    virtual ~lds_pushbutton_wrap();
    void show_flag_top_right(bool f);//云端未上传时,会有用
    void show_pixmap(Qt::Alignment pixmap_flag, const QPixmap &pixmap);

    int textflags;
    int xp1, yp1, xp2, yp2;


    void disableFrame();
    lds_bool_default_true b_enable_frame;


    void enableFlash(bool show_flash);
protected:
    lds_bool_default_false show_top_right;
    lds_bool_default_false show_flash;
    QPixmap pixmap;
    Qt::Alignment pixmap_flag;

    /////////////////
    void paintEvent(QPaintEvent *e);
};

#endif // LDS_PUSHBUTTON_WRAP_H
