#ifndef FEXPANDMAIN_TABLEVIEW_H
#define FEXPANDMAIN_TABLEVIEW_H


#include <QTableView>
#include <QMouseEvent>
#include "fexpandmain_table.h"
#include <QStyledItemDelegate>
#include "fexpandmain_model_sqltablemodel_data.h"
#include <QTimer>

class fexpandmain_tableview : public fexpandmain_table
{
    Q_OBJECT
public:
    explicit fexpandmain_tableview(QWidget *parent = 0) ;
    ~fexpandmain_tableview();

    int mutableHeight(int row);
    void updateTableHeight(int row);
    void updateTableHeightAll();

    static int intIdWidth();
    static QRect intIdRect();
    static QRect nameRect();
    static QRect numRect();
    static QRect totalRect();

    static QRect memoNameRect();
    static QRect memoTotalRect();
    static QRect dishDescRect();

    static QRect alignRect(const QRect &rect);
public slots:
    void to_show_full_zero();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void updatecurIndex();
private:
    class fexpandmain_delegate : public QStyledItemDelegate{
    public:
        fexpandmain_delegate(QObject *parent = 0);
        void keepConnect(fexpandmain_tableview *tableview);

        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

        QString show_full_going_name(QFontMetrics fm, QString text, const QRect &rect, QString &cut, const QStyle::State &state) const;
        int *border;
        QColor *divColor;
        QColor *nameColor;
        QColor *nameSubColor;
        QColor *defColor;
        QColor *itemBgColor;
    };

    static void setUnderline(QPainter *painter, bool underline);
    static void drawText(QPainter *painter, const QRect &rect, int flags, const QString &text, bool underline);
public:

    static int text_height;
    static void drawNum(const fexpandmain_model_sqltablemodel_data *tablemodel, QPainter *painter, const QStyleOptionViewItem &option, int row);
    static void drawTotal(const fexpandmain_model_sqltablemodel_data *tablemodel, QPainter *painter, const QStyleOptionViewItem &option, int row);


    static QString show_full_name_cut;
    static QString show_full_cook_cut;
    //    //////////////////
    static QPainterPath pathEllipse(const QRectF &rect);
};

#endif // FEXPANDMAIN_TABLEVIEW_H
