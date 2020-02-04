#include "kds_application.h"
#include "kds.h"

kds_application::kds_application(int &argc, char **argv) :
    QApplication(argc, argv)
{

}

bool kds_application::notify(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::MouseTrackingChange
            || e->type() == QEvent::MouseButtonPress
            || e->type() == QEvent::MouseButtonRelease
            || e->type() == QEvent::MouseButtonDblClick
            || e->type() == QEvent::MouseMove){
        kds::kds_model_setdata("cur_parameter", "mouse_running", 1);
    }

    return QApplication::notify(o, e);
}
