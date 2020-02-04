#ifndef LDS_DATETIMEEDIT_H
#define LDS_DATETIMEEDIT_H

#include <QDateTimeEdit>

class lds_calendarwidget;

class lds_datetimeEdit : public QDateTimeEdit
{
    Q_OBJECT
public:
    lds_datetimeEdit(QWidget *parent = 0);
    void setDisplayFormat(const QString &format);
    QString dateTimeStr(const QString &format = "yyyy-MM-dd hh:mm:ss");
    void enablePopAdjustPostion();
    QDateTime dateTime();
protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    lds_calendarwidget *calendar;
    bool is_pop_adjust_postion;
};

#endif // LDS_DATETIMEEDIT_H
