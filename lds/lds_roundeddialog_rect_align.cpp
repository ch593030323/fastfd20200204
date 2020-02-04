#include "lds_roundeddialog_rect_align.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QVariant>
#include <QPainter>
#include "lds.h"
#include <QDialogButtonBox>
#include <QEvent>
#include <QKeyEvent>
#include "lds_roundeddialog_title.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QtDebug>
#include <QApplication>
#include "lds_messagebox.h"

int lds_roundeddialog_rect_align::titleHeight = 30;

void lds_roundeddialog_rect_align::setbgColor(const QColor &color)
{
    bgColor = color;
}

QColor lds_roundeddialog_rect_align::getbgColor() const
{
    return bgColor;
}

void lds_roundeddialog_rect_align::settitleColor(const QColor &color)
{
    titleColor = color;
}

QColor lds_roundeddialog_rect_align::gettitleColor() const
{
    return titleColor;
}

void lds_roundeddialog_rect_align::setbgPixmap(const QPixmap &pixmap)
{
    bgPixmap = pixmap;
}

QPixmap lds_roundeddialog_rect_align::getbgPixmap()
{
    return bgPixmap;
}

void lds_roundeddialog_rect_align::setSizeNotResize(QDialog *buddy)
{
    buddy->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

lds_roundeddialog_rect_align:: lds_roundeddialog_rect_align(QDialog *buddy, void (*paintLeftTop)(const QRect &, QPainter *), bool show_title, bool click_empty_reject, const QSize &buddy_min_size)
    : lds_dialog_drog(buddy->parentWidget())
{
    this->click_empty_reject = click_empty_reject;
    this->click_dialog_accept = false;
    this->buddy_min_size = buddy_min_size;
    this->old_parent = buddy->parentWidget();
#ifdef QT_DEBUG
    if(old_parent == 0) {
        qDebug() << "buddy parent is null" << buddy->windowTitle();
        //        QObject *o = 0;
        //        o->children();
    }
#endif

    old_w_flags = buddy->windowFlags();
    this->setProperty("outer_stylesheet", "lds_roundeddialog_rect_align");
    this->setGeometry(QRect(qApp->desktop()->availableGeometry().topLeft(), lds::MAIN_WINDOW_SIZE));
    //1
    setAttribute(Qt::WA_TranslucentBackground, true);
    lds::setwflagFrameless(this);
    setbgColor("slategrey");

    //2
    label_title = 0;
    if(show_title &&
            MESSAGE_TITLE_VOID != buddy->windowTitle()) {
        label_title = new lds_roundeddialog_title(this, paintLeftTop, buddy->windowTitle(), &titleColor);
        label_title->setFixedHeight(titleHeight);
    }

    this->buddy = buddy;
    this->buddy->setParent(this);
    this->buddy->setMinimumHeight(qMin(lds::MAIN_WINDOW_SIZE.height() -
                                       (label_title == 0 ? 0 : label_title->height()),   this->buddy->height()));
    if(label_title) label_title->setFixedWidth(this->buddy->width());
    this->buddy->setWindowFlags(Qt::Widget);
    this->buddy->installEventFilter(this);
    this->buddy->show();

    //3
    connect(buddy,SIGNAL(finished(int)),this,SLOT(todone(int)));

    isdestroy = true;
}

lds_roundeddialog_rect_align::~lds_roundeddialog_rect_align()
{
    qDebug() << __FUNCTION__;
}

void lds_roundeddialog_rect_align::setTitle(const QString &title)
{
    //    buddy->show();
    if(label_title) label_title->setTitle(title);
}

void lds_roundeddialog_rect_align::disableAutoDestory()
{
    isdestroy = false;
}

void lds_roundeddialog_rect_align::enable_click_empty_reject()
{
    click_empty_reject = true;
}

void lds_roundeddialog_rect_align::enable_click_dialog_accept()
{
    click_dialog_accept = true;
}

QDialog *lds_roundeddialog_rect_align::get_buddy()
{
    return buddy;
}

void lds_roundeddialog_rect_align::buddyResize()
{
    QSize size = buddy->size();
    if(size.width() < buddy->sizeHint().width()) {
        size.setWidth(buddy->sizeHint().width());
    }
    if(size.height() < buddy->sizeHint().height()) {
        size.setHeight(buddy->sizeHint().height());
    }
    size = size.boundedTo(lds::MAIN_WINDOW_SIZE - (label_title ? QSize(0, label_title->height()) : QSize(0, 0)));

    if(buddy->inherits("lds_msgbox")) {
        buddy->setFixedSize(qMax(buddy_min_size.width(), size.width()), qMax( buddy_min_size.height(), size.height()));
    } else {
        buddy->setFixedSize(size);
    }
    //
    toUpdatePos();
}

void lds_roundeddialog_rect_align::toUpdatePos()
{
    int h = this->height();
    int w = this->width();
    int h1 = buddy->height();
    int w1 = buddy->width();
    int h2 = 0;
    if(label_title) {
        h2 = titleHeight;
    }
    int x1, x2, y1, y2;
    x1 = x2 = ( w - w1 ) /2;
    y1 = (h - h1 - h2) / 2;
    y2 = y1 + h2;
    buddy->move(x2, y2);
    if(label_title) label_title->move(x1, y1);
}

int lds_roundeddialog_rect_align::exec()
{
#ifdef QT_DEBUG
    qDebug() << buddy->metaObject()->className();
#endif
    buddy->show();
    return QDialog::exec();
}

void lds_roundeddialog_rect_align::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.fillRect(this->rect(), bgColor);
}

bool lds_roundeddialog_rect_align::eventFilter(QObject *o, QEvent *e)
{
    if(buddy == o) {
        if(QEvent::Hide == e->type()) {
            this->hide();
            return true;
        }
        if(QEvent::Resize == e->type()) {
            if(label_title) {
                label_title->setFixedWidth(buddy->width());
            }
        }
    }
    return QObject::eventFilter(0, e);
}

void lds_roundeddialog_rect_align::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    //    QTimer::singleShot(0, this, SLOT(toBuddyResize()));
}

void lds_roundeddialog_rect_align::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
    buddyResize();
}

void lds_roundeddialog_rect_align::todone(int result)
{
    if(isdestroy) {
        buddy->setParent(old_parent);
        buddy->setWindowFlags(old_w_flags);
    }
    if(this != this->sender()) {
        this->done(result);
    }
}

void lds_roundeddialog_rect_align::toBuddyResize()
{
    buddyResize();
}

void lds_roundeddialog_rect_align::printerPath(QPainter *painter, const QList<QPoint> &pointlist, const QColor &color)
{
    QPainterPath path;
    for(int k = 0; k < pointlist.count(); k++) {
        if(k == 0) {
            path.moveTo(pointlist[k]);
        } else {
            path.lineTo(pointlist[k]);
        }
    }
    path.closeSubpath();
    painter->fillPath(path, color);
}

void lds_roundeddialog_rect_align::keyPressEvent(QKeyEvent *e)
{
    const int kvalue = e->key();
    if(
            kvalue == Qt::Key_Escape) {
        e->ignore();
        return;
    }

    return QDialog::keyPressEvent(e);
}

void lds_roundeddialog_rect_align::mousePressEvent(QMouseEvent *e)
{
    bool click_dialog = buddy->rect().translated(buddy->mapTo(this, QPoint(0,0))).contains(e->pos());

    if(click_dialog) {
        if(click_dialog_accept)
            this->accept();
    } else {
        if(click_empty_reject)
            this->reject();
    }
    lds_dialog_drog::mousePressEvent(e);
}


lds_roundeddialog_rect_align_200x100::lds_roundeddialog_rect_align_200x100(QDialog *buddy)
    : lds_roundeddialog_rect_align(buddy, paintLeftTopNull, true, false, QSize(200, 100))
{

}


lds_roundeddialog_rect_align_600x400::lds_roundeddialog_rect_align_600x400(QDialog *buddy)
    : lds_roundeddialog_rect_align(buddy, paintLeftTopNull, true, false, QSize(600, 400))
{

}
