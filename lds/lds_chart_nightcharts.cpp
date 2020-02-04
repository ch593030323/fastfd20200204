#include "lds_chart_nightcharts.h"
#include "nightcharts.h"
#include "ui_lds_chart_nightcharts_dialog.h"
#include <QGraphicsProxyWidget>

lds_chart_nightcharts::lds_chart_nightcharts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_lds_chart_nightcharts_Dialog)
{
    ui->setupUi(this);
    chart=new lds_chart_nightcharts_content;
    ui->comboBox_type->addItems(chart->gettype().keys());
    ui->comboBox_lengendtype->addItems(chart->getlengendtype().keys());

    QGraphicsProxyWidget *backitem=new QGraphicsProxyWidget;
    backitem->setWidget(chart);
    backitem->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    QGraphicsScene *scene=new QGraphicsScene;
    scene->setSceneRect(scene->itemsBoundingRect());
    scene->addItem(backitem);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing
                                     | QPainter::SmoothPixmapTransform
                                     );
    connect(ui->comboBox_lengendtype,SIGNAL(currentIndexChanged(QString)),this,SLOT(updatechart()));
    connect(ui->comboBox_type,SIGNAL(currentIndexChanged(QString)),this,SLOT(updatechart()));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(to_quit()));
}

void lds_chart_nightcharts::updatechart()
{
    chart->type=chart->gettype().value(ui->comboBox_type->currentText());
    chart->lengendtype=chart->getlengendtype().value(ui->comboBox_lengendtype->currentText());
    chart->update();
}

void lds_chart_nightcharts::to_quit()
{
    this->reject();
}
