#ifndef RESTAURANTTABLEVIEW_H
#define RESTAURANTTABLEVIEW_H
#include "ftableview.h"
#include <QItemDelegate>
#include "lds_bool.h"
#include "lds_tableview_delegate_virtual.h"
#include "lds_model_sqltablemodel.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "lds_scr_table.h"

class restraurantDelegate;

class restaurantTableView : public ftableview
{
    Q_OBJECT
public:
    restaurantTableView(QWidget *parent = 0);
    void setCurrentIndex(const QString &ch_tableno);
    void setCurrentIndex ( const QModelIndex & index );

    bool find_selected_ch_tableno(const QString &ch_tableno);
    QModelIndex getIndexByChtableno(const QString &ch_tableno);
    void setItemDelegate(restraurantDelegate *delegate);
    restraurantDelegate *get_delegate();

    QStringList getCheckList(const QModelIndex &index);
    QString getChbillnoBySqr(const QModelIndex &index, int sqr);
    QString getChbillnoBySqr(const QString &ch_billnos, int sqr);

    ///////////////////////////1////////////////////////////
    void enableMutiSelected(bool f = true);
    QSet<QString> tablenolist;
    void loadTableList(const QStringList &tableList);
public slots:
    void tocurindex_checklist(const QString &checklist);
signals:
    void signalSaveTableFinished();
    void siganlmodelcheck(const QModelIndex & index);
private slots:
    void toSaveCurTable(const QModelIndex &index);
    void toloadTableNo();
    ///////////////////////////1////////////////////////////

private:
    restraurantDelegate *d;

    ///////////////////////////2////////////////////////////
protected slots:
    void toshowpop(const QModelIndex &index);
    ///////////////////////////2////////////////////////////

protected:
    void keyPressEvent(QKeyEvent *e);
};

class restraurantDelegate : public QItemDelegate{
    Q_OBJECT
public:
    ///////////////////////////1////////////////////////////
    static tableSTATE gettableState(tableState state);
    static QPixmap getPixmapByStateList(QList<tableState> statelist);
    static QPixmap getPixmapByStateList(tableState state);

    static QList<tableSTATE> &getTablestatelist();
    static void clearCache();
    ///////////////////////////1////////////////////////////
public:
    static  QColor color_light(const QString &c, double light, int a = 255);//0 1 2 3 原 亮 很亮 最亮
    static  QColor color_light(const QColor &c, double light, int a = 255);//0 1 2 3 原 亮 很亮 最亮
    static  int chbillno2index(int v);
    static  int index2chbillno(int v);
    static const int table_border;

    static void paintDensePattern(QPainter *painter, const QPainterPath &path, const QColor &color, Qt::BrushStyle pattern, qreal pattern_scale, bool drawPen);
    static void paintDensePattern(QPainter *painter, const QRectF &rect, const QColor &color, Qt::BrushStyle pattern, qreal pattern_scale, bool drawPen);

    static void painterCell(QPainter *painter, cell_content cell[][3], int x, int y, const QRect &rect, bool painter_linear, bool gray_mask, qreal xyRadius, qreal  pattern_scale);
    static void cell_init_showed_color(cell_content cell[][3], const QString &showed);
public:
    restraurantDelegate(QObject *parent);
    ~restraurantDelegate();
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    void keepConnect(restaurantTableView *tableview);

    static QMap<int/*sqr*/, QString/*ch_billno*/> get_ch_billno_div_map(const QString &ch_billnos) ;
    static bool ch_billno_is_div(const QMap<int, QString> &map) ;
    static bool ch_billno_is_div(const QString &ch_billnos);

    static const QString sql_ch_billnos;
    //酒吧
    static bool bar_is_div(const QStringList &int_div_id_list) ;
    //
    struct paintCellData {
        //index
        QString vch_tablename;
        QString ch_tableno;
        QString ch_billnos;

        QString int_person;
        QString num_cost;
        QString order_tableno;
        QTime order_time;
        QString State_Selected_checked;
        QStringList disable_sqr_list;
        QStringList div_bar;
        //other
        QString pay_unit_prefix;
        const QMap<QString/*ch_tableno*/, bool> *preprinttablemap;
        //paint
        QRect rect;
        QStyle::State option_state;

    };

    void paintCellFill(paintCellData &p_dta, const QSqlRecord &index) const;
    void paintCellFill(paintCellData &p_dta, const QModelIndex &index) const;
    static void paintTable(QPainter *painter, const paintCellData &p_dta);
    //
    const fexpandmain_model_sqltablemodel  *get_cache();
private:
    QColor *textColor;
    const QMap<QString/*ch_tableno*/, bool> *preprinttablemap;
    const fexpandmain_model_sqltablemodel  *model_cache;
};



//九宫格
class restraurant9BoxDelegate : public QItemDelegate{
    Q_OBJECT
public:
    restraurant9BoxDelegate(QObject *parent = 0);
    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    void tryCombine();

public:
    QMap<QString/*ch_tableno*/, bool> *preprinttablemap;//预打印状态
private:
    mutable QString pay_unit_prefix;
    lds_bool_default_false combine_state;
    int border;
};

//九宫格
class restraurantTableStateDelegate : public lds_tableview_delegate_virtual{
    Q_OBJECT
public:
    restraurantTableStateDelegate(QObject *parent = 0)
        : lds_tableview_delegate_virtual(parent) {}
    virtual QString paint_transfer(const QVariant &dd) const ;
};
#endif // RESTAURANTTABLEVIEW_H
