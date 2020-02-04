#ifndef LDS_APPLICATION_H
#define LDS_APPLICATION_H

#include <QApplication>

class lds_application : public QApplication
{
public:
    lds_application(int &argc, char **argv);
    bool notify(QObject *o, QEvent *e);
};

#endif // LDS_APPLICATION_H
