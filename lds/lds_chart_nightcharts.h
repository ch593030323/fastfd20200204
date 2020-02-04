#ifndef LDS_CHART_NIGHTCHARTS_H
#define LDS_CHART_NIGHTCHARTS_H

#include <QDialog>
#include "lds_chart_nightcharts_content.h"

class Ui_lds_chart_nightcharts_Dialog;

class lds_chart_nightcharts : public QDialog
{
    Q_OBJECT
public:
    explicit lds_chart_nightcharts(QWidget *parent = 0);

    lds_chart_nightcharts_content *chart;
private slots:
    void updatechart();

    void to_quit();
private:
    Ui_lds_chart_nightcharts_Dialog *ui;
};

#endif // LDS_CHART_NIGHTCHARTS_H
