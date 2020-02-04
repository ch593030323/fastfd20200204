#ifndef LDS_RECORD_DO_LONGPRESS_H
#define LDS_RECORD_DO_LONGPRESS_H

#include <QObject>
#include <QTimeLine>
#include <QLabel>
#include <QPoint>
#include <QModelIndex>

class lds_record_do_longpress : public QObject
{
    Q_OBJECT
public:
    explicit lds_record_do_longpress(QWidget *button);

    void setEnable(bool enable);

    void start(const QPoint &pos);
    void stop();

    bool isprepared();
    bool isprepared_stop();

    QLabel *loading_label;
signals:
    void timeout();
    void timeout(const QModelIndex &index);
private slots:
    void framerun(int frame);
private:
    QTimeLine *timeline;
    QPoint press_pos;
    bool enable;
    QWidget*button;
};

#endif // LDS_RECORD_DO_LONGPRESS_H
