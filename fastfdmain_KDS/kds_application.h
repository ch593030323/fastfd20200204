#ifndef KDS_APPLICATION_H
#define KDS_APPLICATION_H

#include <QApplication>

class kds_application : public QApplication
{
    Q_OBJECT
public:
    kds_application(int &argc, char **argv);
    bool notify(QObject *o, QEvent *e);

};

#endif // KDS_APPLICATION_H
