#ifndef BTNLANGUAGESWICTH_H
#define BTNLANGUAGESWICTH_H

#include "btnkeyswicth.h"

class btnlanguageswicth : public btnkeyswicth
{
    Q_OBJECT
public:
    btnlanguageswicth(QWidget *parent = 0) ;
    void initKeys(const QString &first_key, const QString &second_key);
    void initKeys_conf();

    //---------------------virtual---------------------
protected:
    virtual bool switchData();
    //---------------------virtual---------------------
};

#endif // BTNLANGUAGESWICTH_H
