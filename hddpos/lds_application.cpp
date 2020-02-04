#include "lds_application.h"
#include "public_sql.h"

lds_application::lds_application(int &argc, char **argv)
    : QApplication(argc, argv)
{

}

bool lds_application::notify(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::MouseTrackingChange
            || e->type() == QEvent::MouseButtonPress
            || e->type() == QEvent::MouseButtonRelease
            || e->type() == QEvent::MouseButtonDblClick
            || e->type() == QEvent::MouseMove){
        public_sql::restrant_auto_lock_flag = false;
    }

    return QApplication::notify(o, e);
}
