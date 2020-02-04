#ifndef BTNKEYSWICTH_H
#define BTNKEYSWICTH_H

#include "lds_pushbutton_wrap.h"

class btnkeyswicth : public lds_pushbutton_wrap
{
    Q_OBJECT
public:
    btnkeyswicth(QWidget *parent = 0);

    void init(const QString &k1, const QString &v1, const QString &k2, const QString &v2, const QString &k_cur = "");
    void init(const QString &k1, const QString &k2, const QString &k_cur = "");
    void setValues(const QString &v1, const QString &v2);

    void enabledSwitchKey(bool enabled);

    QString firstKey();
    QString secondKey();
    QString firstValue();
    QString secondValue();

    void emitKeyChanged();
signals:
    void keyChanged(const QString &key);
    void keyChanged();

    //---------------------virtual---------------------
protected:
    virtual bool switchData();
    //---------------------virtual---------------------

private slots:
    void switchKey();
private:
    void paintEvent(QPaintEvent *e);
private:
    QString key1;
    QString key2;
    QString value1;
    QString value2;
    int cur_index;
};

#endif // BTNKEYSWICTH_H
