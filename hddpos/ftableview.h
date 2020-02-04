#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "lds_tableview_timeout.h"
#include "lds_bool.h"
#include "ftableview_standmodel_virtual.h"

class ftableview_delegate_virutal;
class QTimeLine;
class ftableview : public lds_tableview_timeout
{
    Q_OBJECT
    //////////////////////////////////////样式表//////////////////////////////////////
    Q_PROPERTY(QColor bgNormalColor10 READ getbgNormalColor10 WRITE setbgNormalColor10 DESIGNABLE true)
    Q_PROPERTY(bool bgNormalPlain READ getbgNormalPlain WRITE setbgNormalPlain DESIGNABLE true)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor DESIGNABLE true)
    Q_PROPERTY(QColor bgBorderColor READ getbgBorderColor WRITE setbgBorderColor DESIGNABLE true)

public:

    void setbgNormalColor10(const QColor &c);
    QColor getbgNormalColor10();

    void setbgNormalPlain(bool c);
    bool getbgNormalPlain();

    void setTextColor(const QColor &c);
    QColor getTextColor();

    void setbgBorderColor(const QColor &c);
    QColor getbgBorderColor();

public:
    QColor bgNormalColor10;
    lds_bool_default_false bgNormalPlain;
    QColor textColor;
    QColor bgBorderColor;
    //////////////////////////////////////类定义//////////////////////////////////////
public:
    ftableview(QWidget *parent = 0) ;
    void setModel(ftableview_standmodel_virtual *model);
    void enableSelectOneWhenMultiSelected();//主要是可以选中后，再次点击取消选中效果
    void enable_mouse_press_after_select();//点击后再次点击会发送signal_enable_mouse_press_after_select
    void disConnectCenter();

    void updateFixed(int ccount, int rcount, int width = 0, int height = 0);
    void addAnimation(int ccount, int rcount);
    void fillData(int ccount, int rcount);
    void fillData_addAnimation(int ccount, int rcount);
    void updateFixed_fillData(int ccount, int rcount, int width = 0, int height = 0);
    void updateFixed_fillData_addAnimation(int ccount, int rcount, int width = 0, int height = 0);
    void updateTableRect(int x, int y, int w, int h, int left, int top, int right, int bottom);
    void updateTableRect(int x, int y, int w, int h, int margin);
    void updateTableRect(const QRect &rect, int left, int top, int right, int bottom);
    void updateTableRect(const QRect &rect, int margin);

    void centerControl(const QModelIndex &index);


    QStringList get_usr_list(int role);
    ftableview_standmodel_virtual *m;

    void toSelectTriggerDelay(int row, int column, int mesc);
signals:
    void signalclick(const QModelIndex &index);
    void signalclick(const QString &no);

    void signalselectedclick(const QModelIndex &index);
    void signalclickNew(const QModelIndex &index);
private slots:
    void tocenter(const QModelIndex &index);
    void tocheckoneselect(const QModelIndex &index);
    void toCenterCurrentIndex();

    //做法press后，修改数量
signals:
    void signal_enable_mouse_press_after_select(const QModelIndex &index);
private:
    lds_bool_default_false is_enable_mouse_press_after_select;
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    //~做法press后，修改数量
    //////////////////////////////////////弹窗//////////////////////////////////////
public:
    //    void enable_pop_view_cp_memo(int columncount, int rowcount);
private:
    ///////////////////////////2////////////////////////////
    lds_bool_default_false b_selectone_when_multiselected;
public:
    enum popType{
        pop_mutil = 1,
        pop_single = 2,

        pop_select_KONGXIAN = 4,
        pop_select_KAITAI = 8,
        pop_mutil_KAITAI = pop_mutil |pop_select_KAITAI,


    };
    Q_DECLARE_FLAGS(popTypes, popType)
    void enablePop(popTypes type);
protected slots:
    virtual void toshowpop(const QModelIndex &index);
protected:
    popTypes _type;
    ///////////////////////////2////////////////////////////
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ftableview::popTypes)

#endif // TABLEVIEW_H
