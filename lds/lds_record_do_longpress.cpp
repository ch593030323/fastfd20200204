#include "lds_record_do_longpress.h"
#include <QMovie>
#include <QTableView>

lds_record_do_longpress::lds_record_do_longpress(QWidget *button) :
    QObject(button),
    button(button)
{
    enable = false;
#ifdef QT_DEBUG
    timeline = new QTimeLine(1000, this);
#else
    timeline = new QTimeLine(3000, this);
#endif
    loading_label = new QLabel(button);
    loading_label->setMovie(new QMovie(":/image/loading.gif", "", this));
    loading_label->setScaledContents(true);
    loading_label->resize(50, 50);
    loading_label->hide();

    timeline->setLoopCount(1);
    timeline->setFrameRange(0, timeline->duration());

    connect(timeline,SIGNAL(frameChanged(int)),this,SLOT(framerun(int)));
}

void lds_record_do_longpress::setEnable(bool enable)
{
    this->enable = enable;
}

void lds_record_do_longpress::start(const QPoint &pos)
{
    if(!enable) return;

    press_pos = pos;
    timeline->start();
}

void lds_record_do_longpress::stop()
{
    if(!enable) return;

    timeline->stop();
    loading_label->hide();
}

bool lds_record_do_longpress::isprepared()
{
    if(!enable) return true;

    return loading_label->isHidden() && timeline->state() == QTimeLine::Running;
}

bool lds_record_do_longpress::isprepared_stop()
{
    bool f = isprepared();
    stop();
    return f;
}

void lds_record_do_longpress::framerun(int frame)
{
    if(frame >= timeline->duration() / 2) {
        if(loading_label->parent() == button) {
            loading_label->move(press_pos - QPoint(loading_label->width() / 2, loading_label->height() / 2));
        } else {
            loading_label->move(press_pos - QPoint(loading_label->width() / 2, loading_label->height() / 2) + button->pos());
        }
        loading_label->movie()->jumpToNextFrame();
        loading_label->show();
    }
    if(frame == timeline->duration()) {
        timeline->stop();
        loading_label->hide();
        emit timeout();
        if(this->parent()->inherits("QTableView")) {
            emit timeout(qobject_cast<QTableView *>(this->parent())->indexAt(press_pos));
        }
    }
}
