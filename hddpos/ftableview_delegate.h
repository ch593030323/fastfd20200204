#ifndef FTABLEVIEW_DELEGATE_H
#define FTABLEVIEW_DELEGATE_H

#include <QWidget>
#include <QItemDelegate>
#include <QLinearGradient>
#include <QSettings>
#include <QSet>
#include "lds_bool.h"
#include <QTableView>
#include <QNetworkAccessManager>
#include <QStack>

class ftableview;
class ftableview_delegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum IconStyle{
        AutoIcon,// icon isvalid? TextUnderIcon : TextOnly;
    };

    static void setTableViewImageScaled(double imageScaled, QTableView *tableview);
    static double getTableViewImageScaled(QTableView *tableview);
public:
    ftableview_delegate(ftableview *tableview);
    ftableview_delegate(ftableview *tableview, bool indexisEmptyDrawGrad);
    ~ftableview_delegate();
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    void setIconStyle(IconStyle style);

    void setImageScaled(double imageScaled);
    double getImageScaled();
public:
    QLinearGradient getLinerBypropety(QStyle::State state , const QString &vch_color) const;
    void keepConnect(ftableview *tableview);
    void enableRoundRected();
    void disableDrawBorder();
    void setColorLight(double light);
    void setImageMargin(int m);
    void set_pay_unit_prefix(const QString &pay_unit_prefix);
private:
    QPainterPath roundedRect(const QRect &rect, qreal xRadius, qreal yRadius) const;
    void init();
    static void drawBorder(QPainter *p, bool drawTableViewLeftTopBorder, const QModelIndex &index, const QRect &rect, QColor color);
    static void fillRect(QPainter *p, const QModelIndex &index, const QRect &rect, QBrush color) ;
    static void fillRect(QPainter *p, const QModelIndex &index, const QRect &rect, QColor color) ;

//    void setMargin(int m);
//    void setMargin(int border_top_bottom, int border_left_right);
//    void setMargin(int left, int top, int right, int bottom);
private:
    QColor *bgNormalColor10;
    lds_bool *bgNormalPlain;
    QColor *textColor;
    QColor *bgBorderColor;
    QString tableview_property;
public:
    static const int CELL_BORDER_SPACE;
    lds_bool_default_false b_shwoGrid;
    lds_bool_default_false indexisEmptyDrawGrad;
    lds_bool_default_false indexisEmptyDrawRect;
    lds_bool_default_true drawTableViewLeftTopBorder;
private:
    IconStyle icon_style;
    double colorLight;
    int imageMargin;

    QString pay_unit_prefix;
    lds_bool_default_false b_roundRected;

    mutable QMap<QString, QPixmap> temp_save_pixmap_map;

    mutable double imageScaled;

    ftableview *parentView;
};
#endif // FTABLEVIEW_DELEGATE_H
