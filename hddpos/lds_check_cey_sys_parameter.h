#ifndef LDS_CHECK_CEY_SYS_PARAMETER_H
#define LDS_CHECK_CEY_SYS_PARAMETER_H

#include <QObject>
#include <QMap>

class lds_check_cey_sys_parameter : public QObject
{
    Q_OBJECT
public:
    explicit lds_check_cey_sys_parameter(QObject *parent = 0);

    void init_buildConnect(QObject *o, const QString &field_name, const char *asignal);
    void buildConnect(QObject *o, const QString &field_name, const char *asignal);

private slots:
    void toCheck();

private:
    QMap<QObject *, QString> obj_field_map;
};

#endif // LDS_CHECK_CEY_SYS_PARAMETER_H
