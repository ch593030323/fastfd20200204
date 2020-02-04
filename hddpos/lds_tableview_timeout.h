#ifndef LDS_TABLEVIEW_TIMEOUT_H
#define LDS_TABLEVIEW_TIMEOUT_H


#include "lds_tableview_virtual.h"
#include <QTimeLine>
#include <QHeaderView>
#include "lds_record_do_longpress.h"

class QLabel;
class lds_tableview_timeout : public lds_tableview_virtual
{
    Q_OBJECT
public:
    lds_tableview_timeout(QWidget *parent = 0);
    void enable_release_clear_selection(bool enable);//释放时清除选区
    void enableLongPress(bool enable);

signals:
    void double_clicked(const QModelIndex &index);
private:
    bool is_release_clear_selection;
protected:
    QModelIndex pressIndex;
public:
    //////////////////////////////////////表格超时按钮//////////////////////////////////////
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void released(const QModelIndex &index);
    void timeout(const QModelIndex &index);
    //////////////////////////////////////表格超时按钮//////////////////////////////////////

private:
    lds_record_do_double_clicked do_double_clicked;
    lds_record_do_longpress *do_longpress;
};

#endif // LDS_TABLEVIEW_TIMEOUT_H
