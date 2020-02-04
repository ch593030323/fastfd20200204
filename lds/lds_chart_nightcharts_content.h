#ifndef LDS_CHART_NIGHTCHARTS_CONTENT_H
#define LDS_CHART_NIGHTCHARTS_CONTENT_H

#include <QWidget>

class lds_chart_nightcharts_content : public QWidget
{
    Q_OBJECT
public:
    struct CHART_ITEMS{
        CHART_ITEMS(const QString &t, const QColor &c, float p):
        text(t),color(c),percent(p) {}
        QString text;
        QColor color;
        float percent;
    };

public:
    explicit lds_chart_nightcharts_content(QWidget *parent = 0);

    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    QList<CHART_ITEMS> chart_items;
    void chart_items_after();
    QMap<QString, int> gettype();
    int type;//{Histogramm,Pie,Dpie};
    QMap<QString, int> getlengendtype();
    int lengendtype;//{Round,Vertical}
};

#endif // LDS_CHART_NIGHTCHARTS_CONTENT_H
