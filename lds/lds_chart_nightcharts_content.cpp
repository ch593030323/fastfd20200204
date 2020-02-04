#include "lds_chart_nightcharts_content.h"
#include "nightcharts.h"
#include <QMap>

lds_chart_nightcharts_content::lds_chart_nightcharts_content(QWidget *parent) :
    QWidget(parent)
{
    chart_items.append(CHART_ITEMS("Item1",QColor(200,10,50),34));
    chart_items.append(CHART_ITEMS("Item2",Qt::green,27));
    chart_items.append(CHART_ITEMS("Item3",Qt::cyan,14));
    chart_items.append(CHART_ITEMS("Item4",Qt::yellow,7));
    chart_items.append(CHART_ITEMS("Item5",Qt::blue,4));
    chart_items.append(CHART_ITEMS("Item3",Qt::cyan,14));
    chart_items.append(CHART_ITEMS("Item4",Qt::yellow,7));
    chart_items.append(CHART_ITEMS("Item5",Qt::blue,4));
    type=Nightcharts::Dpie;
    lengendtype=Nightcharts::Round;

    this->setFixedSize(1000, 750);
}

void lds_chart_nightcharts_content::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter;
    painter.begin(this);
    Nightcharts PieChart;
    PieChart.setType(Nightcharts::type(type));//{Histogramm,Pie,Dpie};
    PieChart.setLegendType(Nightcharts::legend_type(lengendtype));//{Round,Vertical}
    PieChart.setCords(150,50,1000/1.5,750/1.5);
    foreach(CHART_ITEMS citem, chart_items) {
        PieChart.addPiece(citem.text, citem.color, citem.percent);
    }
    if(!chart_items.isEmpty()) {
        PieChart.draw(&painter);
        PieChart.drawLegend(&painter);
    }
}

void lds_chart_nightcharts_content::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
}

void lds_chart_nightcharts_content::chart_items_after()
{
    float item_value_total=0.0;
    foreach(CHART_ITEMS citem, chart_items) {
        item_value_total += citem.percent;
    }

    for(int index=0;index<chart_items.count();index++) {
        if(item_value_total != 0.0)
            chart_items[index].percent=chart_items[index].percent/item_value_total*100.0;
    }
}

QMap<QString, int> lds_chart_nightcharts_content::gettype()
{
    QMap<QString, int> intmap;
    intmap.insert("Bar", Nightcharts::Histogramm);
    intmap.insert("Pie", Nightcharts::Pie);
    intmap.insert("3Dpie", Nightcharts::Dpie);
    return intmap;
}

QMap<QString, int> lds_chart_nightcharts_content::getlengendtype()
{
    QMap<QString, int> intmap;
    intmap.insert("Round", Nightcharts::Round);
    intmap.insert("Tagged", Nightcharts::Vertical);
    return intmap;
}
