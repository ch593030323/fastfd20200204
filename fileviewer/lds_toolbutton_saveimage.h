#ifndef LDS_TOOLBUTTON_SAVEIMAGE_H
#define LDS_TOOLBUTTON_SAVEIMAGE_H

#include <QToolButton>

class lds_toolbutton_saveimage : public QToolButton
{
    Q_OBJECT
public:
    explicit lds_toolbutton_saveimage(QWidget *parent = 0);
    void setIcon_savefile(const QString &filepath);
    QString _filepath;
};

#endif // LDS_TOOLBUTTON_SAVEIMAGE_H
