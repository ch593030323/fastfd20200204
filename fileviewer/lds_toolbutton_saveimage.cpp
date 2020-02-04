#include "lds_toolbutton_saveimage.h"

lds_toolbutton_saveimage::lds_toolbutton_saveimage(QWidget *parent) :
    QToolButton(parent)
{
}

void lds_toolbutton_saveimage::setIcon_savefile(const QString &filepath)
{
    QPixmap p(filepath);
    if(!p.isNull()) {
        this->setIcon(p);
    } else {
        this->setIcon(QIcon());
    }
    _filepath = filepath;
}
