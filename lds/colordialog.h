#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QComboBox>
#include <QTableView>
#include <QItemDelegate>

namespace Ui {
class colordialog;
}
class QColorPicker;
class QColorLuminancePicker;

class colordialog : public QDialog
{
    Q_OBJECT

public:
    explicit colordialog(QWidget *parent = 0);
    ~colordialog();
    QString result_color_name();
private slots:
    void took();
    void tocancel();
    void toupdate(const QModelIndex &current, const QModelIndex &previous);
    void toupdateHtml(const QString &html_color);
    void toupdateRed(int html_color);
    void toupdateBlue(int html_color);
    void toupdateGreen(int html_color);
    void toupdate(int h, int s, int v);
    void toupdate(int h, int s);

    void toupdateColor(const QColor &color);
private:
    QPixmap pix_color(const QString &colorname);
    void com_add_item(QComboBox *com, const QString &colorname);
private:
    QString color_name;
    QColorPicker *picker;
    QColorLuminancePicker *mpicker;
    Ui::colordialog *ui;

    class itemDelegate : public QItemDelegate
    {
    public:
        itemDelegate(QObject *parent) : QItemDelegate(parent) {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    };
};


class QColorPicker : public QFrame
{
    Q_OBJECT
public:
    QColorPicker(QWidget* parent);
    ~QColorPicker();

public slots:
    void setCol(int h, int s);

signals:
    void newCol(int h, int s);

protected:
    QSize sizeHint() const;
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);

private:
    int hue;
    int sat;

    QPoint colPt();
    int huePt(const QPoint &pt);
    int satPt(const QPoint &pt);
    void setCol(const QPoint &pt);

    QPixmap pix;
};



class QColorLuminancePicker : public QWidget
{
    Q_OBJECT
public:
    QColorLuminancePicker(QWidget* parent = 0);
    ~QColorLuminancePicker();

    int value();
public slots:
    void setCol(int h, int s, int v);
    void setCol(int h, int s);

signals:
    void newHsv(int h, int s, int v);

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    enum { foff = 3, coff = 4 }; //frame and contents offset
    int val;
    int hue;
    int sat;

    int y2val(int y);
    int val2y(int val);
    void setVal(int v);

    QPixmap *pix;
};

#endif // COLORDIALOG_H
