#ifndef TABLE_MANAGEMENT_GRAPHICSVIEW_H
#define TABLE_MANAGEMENT_GRAPHICSVIEW_H

#include <QGraphicsView>
#include "table_management_desk.h"
#include "table_management_pad.h"
#include "lds.h"

class table_management_graphicsview : public QGraphicsView
{
    Q_OBJECT
public:
    table_management_graphicsview(QWidget *parent = 0);
    ~table_management_graphicsview();

    void add_pad_once(table_management_pad *pad);
    table_management_pad *get_pad();

    QStringList get_item_ch_tableno_list();
    table_management_desk *add_item(const QString &ch_tableno, const QString &ch_areano, table_management_desk::shapeType type = table_management_desk::shape_rectangle_min);
    void delete_item(const QString &ch_tableno);
    void goto_edit_mode();
    void goout_edit_mode();
    bool is_edit_mode();

    void emit_signal_table_clicked(const QString &ch_tableno);
    void emit_signal_table_double_clicked(const QString &ch_tableno);
    void emit_signal_table_deleted(const QString &ch_tableno);

    void filter_by_ch_areano(const QString &ch_areano);

    const QMap<QString/*ch_tableno*/, QString/*vch_tablename*/> &table_no_name_map_get();
    void table_no_name_map_insert(const QString &ch_tableno, const QString &vch_tablename);
    lds_bool_default_true is_desk_show_code;

    bool press_event_do_double_clicked(const QString &ch_tableno);
public slots:
    void desk_save(const QString &filepath);
    void desk_load_init_scene(const QString &filepath);
signals:
    void signal_table_clicked(const QString &ch_tableno);
    void signal_table_double_clicked(const QString &ch_tableno);
    void signal_table_deleted(const QString &ch_tableno);
protected:
    void resizeEvent(QResizeEvent *event);
private:
    QGraphicsScene *curScene();
    table_management_pad *pad;
    bool edit_mode;
    int add_item_off;
    QMap<QString/*ch_tableno*/, QString/*vch_tablename*/> table_no_name_map;
private:
    lds_record_do_double_clicked do_double_clicked;
};
#endif // TABLE_MANAGEMENT_GRAPHICSVIEW_H
