#ifndef KDS_MAIN_INFO_BRIEF_FRAME_H
#define KDS_MAIN_INFO_BRIEF_FRAME_H

#include <QFrame>
#include <QByteArray>
#include "cjson_help.h"

class kds_main_info_brief_frame : public QFrame
{
    Q_OBJECT
public:
    kds_main_info_brief_frame(QWidget *parent = 0);

    QByteArray getIndexJson();
    void setIndexJson(const QByteArray &json);

    QSet<QString>modifiedBill_nos(int state);
    QSet<int>modifiedFlow_ids(int state);

    void replaceItemValue(const QString &path, const char *key, const QVariant &data);
    void reset();
    int deleteArrayItem(const QString &array, int which);

    cJSON_path p;
private:
    QMap<int, int> flow_id_state_map;
    QByteArray org_json;
protected:
    void paintEvent(QPaintEvent *);
};

#endif // KDS_MAIN_INFO_BRIEF_FRAME_H
