#include "lds_check_cey_sys_parameter.h"
#include "n_func.h"

lds_check_cey_sys_parameter::lds_check_cey_sys_parameter(QObject *parent) :
    QObject(parent)
{
}

void lds_check_cey_sys_parameter::init_buildConnect(QObject *o, const QString &field_name, const char *asignal)
{
    o->setProperty("checked", n_func::f_get_sysparm(field_name) == "1");
    buildConnect(o, field_name, asignal);
}

void lds_check_cey_sys_parameter::buildConnect(QObject *o, const QString &field_name, const char *asignal)
{
    obj_field_map.insert(o, field_name);
    connect(o, asignal, this,SLOT(toCheck()));
}

void lds_check_cey_sys_parameter::toCheck()
{
    QObject *o = this->sender();
    if(0 == o)
        return;
    bool check = o->property("checked").toBool();
    QString field_name = obj_field_map.value(o);
    n_func::f_set_sysparm(field_name,  check ?  1 : 0);
}
