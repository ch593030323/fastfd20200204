#include "lds_value_scale.h"

lds_value_scale::lds_value_scale(int scale, QObject *parent) :
    QObject(parent)
{
    _scale = scale;
}

void lds_value_scale::make_connect(const QObject *asender, const char *asignal, const QObject *areceiver, const char *amember)
{
    connect(asender, asignal, this, SLOT(receiveValue(double)));
    connect(this, SIGNAL(newValue(double)), areceiver, amember);
}

void lds_value_scale::receiveValue(double value)
{
    emit newValue(value * _scale);
}
