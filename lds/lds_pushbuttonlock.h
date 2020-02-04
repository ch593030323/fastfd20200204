#ifndef LDS_PUSHBUTTONLOCK_H
#define LDS_PUSHBUTTONLOCK_H

#include <QObject>
#include <QWidget>

class lds_pushbuttonlock : public QObject
{
    Q_OBJECT
public:
    explicit lds_pushbuttonlock(QWidget *lockwidget, QObject *parent = 0);
    void toLock();
    ~lds_pushbuttonlock();
private:
    QWidget *l;

};

#endif // LDS_PUSHBUTTONLOCK_H
