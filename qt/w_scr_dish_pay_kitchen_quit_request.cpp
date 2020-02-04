#include "w_scr_dish_pay_kitchen_quit_request.h"
#include "ui_w_scr_dish_pay_kitchen_quit_request.h"
#include <QLabel>
#include "lds_query_hddpos.h"
#include <QtDebug>
#include <QMovie>


w_scr_dish_pay_kitchen_quit_request::w_scr_dish_pay_kitchen_quit_request(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_pay_kitchen_quit_request)
{
    ui->setupUi(this);
    lds_query_hddpos  query;
    QList<int> int_flow_id_list;
    for(int k = 0; k < 20; k++) {
        int_flow_id_list << 816 + k;
    }
    QMap<QString, QMap<QString, double> > vch_plan_id_ch_dishno_num_map;
    foreach(int int_flow_id, int_flow_id_list) {
        QString vch_plan_k3o3_id;
        QStringList vch_plan_id_list;

        query.execSelect(QString(" select int_flowID, ch_billno, ch_dishno, vch_plan_k3o3_id, num_num-num_back as num_dish from cey_u_orderdish where int_flowid = %1 ")
                         .arg(int_flow_id));
        if(query.next()) {
            vch_plan_k3o3_id = query.recordValue("vch_plan_k3o3_id").toString();
            vch_plan_id_list << vch_plan_k3o3_id.mid(0, 2);
            vch_plan_id_list << vch_plan_k3o3_id.mid(2, 2);
            vch_plan_id_list << vch_plan_k3o3_id.mid(4, 2);

            foreach(const QString &vch_plan_id, vch_plan_id_list) {
                if(lds_query_hddpos::selectHasNext(QString(" SELECT b.ch_kitchen_view FROM cey_bt_kitchen_plan a, cey_bt_kitchen_print b where a.int_print_id = b.int_id and a.vch_plan_id = '%1' and  b.ch_kitchen_view = 'Y';  ").arg(vch_plan_id))) {
                    if(vch_plan_id_ch_dishno_num_map.find(vch_plan_id) == vch_plan_id_ch_dishno_num_map.end())
                        vch_plan_id_ch_dishno_num_map.insert(vch_plan_id, QMap<QString, double>());
                    vch_plan_id_ch_dishno_num_map[vch_plan_id].insert(query.recordValue("ch_dishno").toString(), vch_plan_id_ch_dishno_num_map[vch_plan_id].value(query.recordValue("ch_dishno").toString(), 0) + query.recordValue("num_dish").toDouble());
                }
            }
        }
    }

    QLabel *last_dish = 0;
    int index = 0;
    for(QMap<QString, QMap<QString, double> >::iterator k = vch_plan_id_ch_dishno_num_map.begin(); k != vch_plan_id_ch_dishno_num_map.end(); k++, index++) {
        QLabel *label = new QLabel(ui->scrollAreaWidgetContents);
        QLabel *loading = new QLabel(ui->scrollAreaWidgetContents);
        QLabel *dish = new QLabel(ui->scrollAreaWidgetContents);
        QMovie *movie = new QMovie(":/image/loading.gif", "", this);

        label->setGeometry(10, 10 + index * 50, 100, 40);
        loading->setGeometry(120, 10 + index * 50, 40, 40);
        dish->setGeometry(170, 10 + index * 50, 800, 40);

        label->setStyleSheet("background:blue;");
        loading->setStyleSheet("background:yellow;");
        dish->setStyleSheet("background:red;");

        label->setText(k.key());
        loading->setMovie(movie);
        movie->start();
        QMapIterator<QString, double> mapitr(k.value());
        while(mapitr.hasNext()) {
            mapitr.next();
            dish->setText(dish->text() + lds_query_hddpos::selectValue(QString(" select vch_dishname from cey_bt_dish where ch_dishno = '%1' ").arg(mapitr.key())).toString() + "X" + QString::number(mapitr.value()) + ",");
        }

        last_dish = dish;
    }
    ui->scrollAreaWidgetContents->setMinimumSize(last_dish->geometry().right() + 10, last_dish->geometry().bottom() + 10);
}

w_scr_dish_pay_kitchen_quit_request::~w_scr_dish_pay_kitchen_quit_request()
{
    delete ui;
}
