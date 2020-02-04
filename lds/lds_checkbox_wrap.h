#ifndef LDS_CHECKBOX_WRAP_H
#define LDS_CHECKBOX_WRAP_H

#include <QCheckBox>

class lds_checkbox_wrap : public QCheckBox
{
    Q_OBJECT
public:
    explicit lds_checkbox_wrap(QWidget *parent = 0);
//    QMap<QString, QString>
protected:
    void paintEvent(QPaintEvent *e);
    bool hitButton(const QPoint &p) const;
    void resizeEvent(QResizeEvent *e);
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *e);
private:
    QRect textrect() const;
    int textleftspacing() const;
    int textflag;
};

#endif // LDS_CHECKBOX_WRAP_H
