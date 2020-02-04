#include "kds_main_info_brief_frame.h"
#include <QPainter>
#include "kds_main_view.h"
#include "lds_query_db.h"
#include <QtDebug>

kds_main_info_brief_frame::kds_main_info_brief_frame(QWidget *parent)
    : QFrame(parent)
{
}

QByteArray kds_main_info_brief_frame::getIndexJson()
{
    return p.toByteArray();
}

void kds_main_info_brief_frame::setIndexJson(const QByteArray &json)
{
    this->org_json =json;
    flow_id_state_map.clear();
    p.parse(json);
    p.replaceItemValue("", "row_count", this->height() / 40);//include title row
    p.replaceItemValue("", "item_more_visible", 0);//include title row
    this->update();
}

QSet<QString> kds_main_info_brief_frame::modifiedBill_nos(int state)
{
    QSet<QString> ret;
    for(QMap<int, int>::iterator k = flow_id_state_map.begin(); k != flow_id_state_map.end(); k ++) {
        if(k.value() == state) {
            ret.insert(kds::selectValue("select ch_billno from cey_u_orderdish_print_kt_order where int_flowID = %1 ", k.key()).toString());
        }
    }
    return ret;
}

QSet<int> kds_main_info_brief_frame::modifiedFlow_ids(int state)
{
    QSet<int> ret;
    for(QMap<int, int>::iterator k = flow_id_state_map.begin(); k != flow_id_state_map.end(); k ++) {
        if(k.value() == state) {
            ret.insert(k.key());
        }
    }
    return ret;
}

void kds_main_info_brief_frame::replaceItemValue(const QString &path, const char *key, const QVariant &data)
{
    p.replaceItemValue(path, key, data);
    this->update();
}

void kds_main_info_brief_frame::reset()
{
    setIndexJson(org_json);
}

int kds_main_info_brief_frame::deleteArrayItem(const QString &array, int which)
{
    cJSON *j = p.path_json(array);
    if(j == 0)
        return 0;
    int prev = which - 1;
    int after = which + 1;
    QString prev_title;
    QString after_title;
    QString which_title = p.path_value(QString("%1[%2{type").arg(array).arg(which)).toString();
    int count = p.array_count(array);
    if("item" != which_title) {
        return count;
    }
    if(prev >= 0 &&prev < count) {
        prev_title = p.path_value(QString("%1[%2{type").arg(array).arg(prev)).toString();
        if("item" == prev_title) {
            p.deleteArray(array, which);
            return count - 1;
        }
    }
    if(after >= 0 &&after < count) {
        after_title = p.path_value(QString("%1[%2{type").arg(array).arg(after)).toString();
        if("item" == after_title) {
            p.deleteArray(array, which);
            return count - 1;
        }
    }
    p.deleteArray(array, which);
    if("title" == prev_title) {
        p.deleteArray(array, prev);
        return count - 2;
    }
    return count - 1;
}

void  kds_main_info_brief_frame::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    kds::paintTableBrief(&painter, p.toByteArray(), this->rect());
}
