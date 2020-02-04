#ifndef LDS_ROUNDEDDIALOG_TITLE_FRAME_H
#define LDS_ROUNDEDDIALOG_TITLE_FRAME_H

#include <QFrame>

class lds_roundeddialog_title_frame : public QFrame
{
    Q_OBJECT
public:
    lds_roundeddialog_title_frame(QWidget *parent);

    void setTitleColor(QColor *titleColor);
    void enableBottomBorder();
protected:
    void paintEvent(QPaintEvent *e);
private:
    QColor *titleColor;
    bool isbottomBorder;
};

#endif // LDS_ROUNDEDDIALOG_TITLE_FRAME_H
