#ifndef PUBLIC_PRINTBY_CH_BILLNO_OVER_VIEW_H
#define PUBLIC_PRINTBY_CH_BILLNO_OVER_VIEW_H

#include <QDialog>
#include "printer_0.h"
#include "public_printby_ch_billno.h"

class public_printby_ch_billno_over_view_data
{
public:
    public_printby_ch_billno_over_view_data();
    void initData(lds_model_sqltablemodel *model_cache);

    QImage getImage(const QString &logo_file_path = "");

    virtual void setPaperWidth(int paperwidth);
    int curPaperWidth();
    const blockIns & get_ins();
    void _refresh();
    QSize paint(QPainter *painter);
    QString f_get_sysparm(const QString &as_parameter, const QVariant &as_default = QVariant());

    blockIns ins;
    int char_w;
    int char_h;
    int line_char_count;
    lds_model_sqltablemodel *model_cache;
    int bg_count;
    int yscroll_off;
    QFont SONGTI_font;


    QRect scaleRect(const QRect &rect, double scale);
    int textWidth(const QString &text);
    int textCharaters(const QString &text);
    QString textMid(const QString &text, int pos, int length);
    QString textAppendBlank(const QString &text, int ideal_width);
    void drawText(QPainter *painter,  QRect rect, int align, const QString &text);
    void scale(QPainter *painter, qreal sx, qreal sy);
    QList<QByteArray> textSplit_n_detect_width(int xoff, int xscale, const QByteArray &text);
    QList<QString> text_detect_blank(const QString &text);
    QVariantMap createMap(const QString &table_key, const QString &table_value);
    void setPainterFont(QPainter *p, int weight);
    void setPainterFontPixel(QPainter *p, int pixel);
    int fontm_width(QPainter *p, const QString &text);
};

class public_printby_ch_billno_over_view : public QDialog, public public_printby_ch_billno_over_view_data
{
    Q_OBJECT
public:
    explicit public_printby_ch_billno_over_view(QWidget *parent = 0);

    void setPaperWidth(int paperwidth);
signals:
    void signal_resize();
public slots:
    void refresh();
    void print();
protected:
    void paintEvent(QPaintEvent *);

    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // PUBLIC_PRINTBY_CH_BILLNO_OVER_VIEW_H
