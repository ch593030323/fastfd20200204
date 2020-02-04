#ifndef W_RPT_REPORT_FRAME_BTN_PAGEINDEX__H
#define W_RPT_REPORT_FRAME_BTN_PAGEINDEX__H
#include "lds_toolbar_button.h"
#include <QMap>
#include <QSet>
class w_rpt_report_frame_btn_pageindex_ : public lds_toolbar_button
{
    Q_OBJECT
public:
    struct total_desc_value_Data{
        inline total_desc_value_Data() {}
        QString fieldname;
        float value;
    };

    struct step_count_Data{
        inline step_count_Data():curstep(0), pagecount(0), onepagestep(0), rowcount(0) {}
        inline bool isValid() {
            return rowcount != 0 ||!sql.isEmpty();
        }
        inline void init() {
            curstep = 0;
            pagecount = 0;
            onepagestep = 0;
            rowcount = 0;
            sql = "";
        }
        //初始化
        int curstep;
        int pagecount;
        int onepagestep;
        int rowcount;
        QString sql;

        //保留
        QList<total_desc_value_Data> total_list;//summodel
        QSet<int> set_column_delegate;//代理
        QMap<QWidget */*widget*/, bool /*isvisiabled*/> map_widget_visiable;
    };

public:
public:
    w_rpt_report_frame_btn_pageindex_();
    QString get_desc(int index);
    QString get_limit_sql_update_desc(int index);
    QString get_limit_sql(int index);
    int get_cur_page(int index);
    int get_rowcount(int index);
    bool isValid(int index);
    void init(int index, int rowcount, int onepagestep, const QString &sql);
    void init(int index);
    void insert_d(int index);
    bool can_page_move(int index, int step);//==-1 default:curstep
    void up(int index);
    void down(int index);
    void setpage(int index, int page_value);

    void sum_clear(int index);
    void sum_append(int index, const QString &fieldname);
    void sum_genrate(int index);
    QList< total_desc_value_Data> get_sum_list_total_data(int index);

    void insert_map_widget_visiable(int index, QWidget *w, bool isvisiabled);
    void update_map_widget_visiable(int index);
private:
    //必须预先调用insert_d初始化
    QList<step_count_Data> d_list;
signals:
    void signal_step_change();
};

#endif // W_RPT_REPORT_FRAME_BTN_PAGEINDEX__H
