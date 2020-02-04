#ifndef FEXPANDMAIN_MODEL_SQLTABLEMODEL_DATA_DD_H
#define FEXPANDMAIN_MODEL_SQLTABLEMODEL_DATA_DD_H

#include "fexpandmain_model_sqltablemodel_data.h"

class fexpandmain_model_sqltablemodel_data_dd : public fexpandmain_model_sqltablemodel_data
{
    Q_OBJECT
public:
    fexpandmain_model_sqltablemodel_data_dd(QObject *parent = 0);
protected:
    void senddd(const lds::DOUBLE_DATA  &data, float dish_num_old = 0, const QString &SENDD_DATA_FROM_DATABASE = "");
    void senddd_update_insert(int row,
                              const QString &udp_request_type,
                              const fexpandmain_keepData &keepd,
                              double dish_num_old ,
                              const QString &SENDD_DATA_FROM_DATABASE = "");
    void emit_num_total_change_DD(const ldsVariantMap &num, const ldsVariantMap &total) const;
};



#endif // FEXPANDMAIN_MODEL_SQLTABLEMODEL_DATA_DD_H
