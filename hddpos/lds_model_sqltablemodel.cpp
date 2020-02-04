#include "lds_model_sqltablemodel.h"
#include "lds_query_db.h"
#include "lds_query_thread.h"
#include "lds_expression2rpn.h"
#include <QSqlRecord>
#include <QPixmap>
#include <QtDebug>
#include <QStack>
#include <QtDebug>
#include <QDateTime>
#include <QSqlError>
#include <QApplication>
#include "math.h"
#include <QFile>
#include <QSqlError>
#include <QSqlIndex>
#include <QSqlField>
#include <QtDebug>


lds_model_sqltablemodel::lds_model_sqltablemodel(QObject *parent)
    : lds_model_sqltablemodel_m(parent, lds_query_thread::getThreadDbAndOpen())
{

}
