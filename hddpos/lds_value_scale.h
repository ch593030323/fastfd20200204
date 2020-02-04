#ifndef LDS_VALUE_SCALE_H
#define LDS_VALUE_SCALE_H

#include <QObject>

class lds_value_scale : public QObject
{
    Q_OBJECT
public:
    explicit lds_value_scale(int scale, QObject *parent = 0);
    void make_connect(const QObject *asender, const char *asignal, const QObject *areceiver, const char *amember);
signals:
    void newValue(double value);
public slots:
    void receiveValue(double value);
private:
    int _scale;
};

#endif // LDS_VALUE_SCALE_H
