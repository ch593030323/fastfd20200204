#ifndef LDS_ROUNDEDDIALOG_RECT_FRAME_H
#define LDS_ROUNDEDDIALOG_RECT_FRAME_H

#include <QFrame>

class lds_roundeddialog_rect_frame : public QFrame
{
    Q_OBJECT
public:
    explicit lds_roundeddialog_rect_frame(QWidget *parent = 0);

    void paintEvent(QPaintEvent *e);

    QColor *bgColor;
};

#endif // LDS_ROUNDEDDIALOG_RECT_FRAME_H
