#include "lds_messagebox.h"
#include <QTimeLine>
#include <QVariant>
#include <QDialog>
#include <QMovie>
#include <QAbstractButton>
#include <QHBoxLayout>
#include<QLabel>
#include <QtDebug>
#include <QTimer>
#include <QEventLoop>
#include <QStyle>
#include <QTextEdit>
#include <QCoreApplication>
#include <QScrollBar>
#include <QTime>
#include <QKeyEvent>

static QPushButton *createMsgButton(QWidget *parent, const QString &text) {
    QPushButton *p=new QPushButton(parent);
    p->setProperty("outer_stylesheet", "pushbutton");
    p->setText(text);
    p->setFixedSize(qMax(80, p->sizeHint().width() +10 + 10), 30);
    return p;
}
QPoint lds_messagebox::messge_pos = QPoint(0, 0);

lds_messagebox::lds_messagebox(QObject *parent) :
    QObject(parent)
{
}

int lds_messagebox::warning(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxWarning, title, text,
                                             button0Text, button1Text, button2Text, "", delayclose);
}

int lds_messagebox::warning(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxWarning, title, text,
                                             "", "", "", "", delayclose);
}

int lds_messagebox::information(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxInformation, title, text,
                                             button0Text, button1Text, button2Text, "", delayclose);
}

int lds_messagebox::information(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxInformation, title, text, "", "", "", "", delayclose);
}

int lds_messagebox::question(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, const QString &button3Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxQuestion, title, text,
                                             button0Text, button1Text, button2Text,button3Text, delayclose);

}

int lds_messagebox::question(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxQuestion, title, text, "", "", "", "", delayclose);
}

int lds_messagebox::critical(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxCritical, title, text,
                                             button0Text, button1Text, button2Text,"", delayclose);

}

int lds_messagebox::critical(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QStyle::SP_MessageBoxCritical, title, text, "", "", "", "", delayclose);
}

int lds_messagebox::showOldMessageBox(QWidget *parent, QStyle::StandardPixmap icon, const QString &title, const QString &text, QString button0Text, const QString &button1Text, const QString &button2Text, const QString &button3Text, bool delayclose)
{
    if(button0Text.isEmpty())
        button0Text=QObject::tr("确定");
    QStringList btn_list;
    if(!button0Text.isEmpty()) btn_list << button0Text;
    if(!button1Text.isEmpty()) btn_list << button1Text;
    if(!button2Text.isEmpty()) btn_list << button2Text;
    if(!button3Text.isEmpty()) btn_list << button3Text;


    QTimeLine timeLine(5000, 0);
    if(delayclose) {
        timeLine.setFrameRange(0, 5);
        timeLine.setDirection(QTimeLine::Backward);
        timeLine.start();
    }

    if(text.length() > 400) {
        lds_messagebox_textedit dialog(parent, title, text, btn_list);
        connect(&timeLine, SIGNAL(frameChanged(int)), &dialog, SLOT(updateTimeframe(int)));
        connect(&dialog,SIGNAL(btnok_finished(int)),&dialog, SLOT(done(int)));
        return lds_roundeddialog_rect_align_200x100(&dialog).exec();
    }
    lds_msgbox dialog(icon, title, text, btn_list, parent);
    connect(&timeLine, SIGNAL(frameChanged(int)), &dialog, SLOT(updateTimeframe(int)));
    connect(&dialog,SIGNAL(btnok_finished(int)),&dialog, SLOT(done(int)));

    dialog.resize(dialog.sizeHint());
    lds_roundeddialog_rect_align_200x100 d(&dialog);
    d.move(messge_pos);
    return d.exec();
}

lds_msgbox::lds_msgbox(QStyle::StandardPixmap icon, const QString &title, const QString &text, QStringList buttonTexts, QWidget *parent)
    : lds_msgbox_virtual(icon, title, text, buttonTexts, parent)
{

    QLabel *label_title = new QLabel(this);
    QLabel *label_icon = new QLabel(this);
    QLabel *edit = new QLabel(this);
    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    //icon_title
    hlayout = new QHBoxLayout;
    hlayout->addWidget(label_title);
    vlayout->addLayout(hlayout);
    label_title->setAlignment(Qt::AlignCenter);

    //edit
    hlayout = new QHBoxLayout;
    hlayout->addWidget(label_icon);
    hlayout->addWidget(edit, 1);
    vlayout->addLayout(hlayout, 1);

    //btn
    hlayout = new QHBoxLayout;
    hlayout->addStretch(1);
    if(buttonTexts.isEmpty())
        buttonTexts.prepend(QObject::tr("确定"));
    foreach(const QString &btn_text, buttonTexts) {
        QPushButton *p = createMsgButton(this, btn_text);

        hlayout->addWidget(p);
        btnlist.append(p);
        connect(p, SIGNAL(clicked()), this, SLOT(todone()));

        if(btnok == 0)
            btnok = p;
    }
    vlayout->addLayout(hlayout);

    this->setLayout(vlayout);

    label_title->setText(title);
    label_icon->setPixmap(style()->standardIcon(icon).pixmap(40, 40));
    edit->setText(text);
}

lds_messagebox_loading::lds_messagebox_loading(QWidget *parent, const QString &title, const QString &text)
    : QDialog(parent)
{
    this->setWindowTitle(title);

    mv = new QMovie(this);
    mv->setFileName(":/image/loading.gif");
    label_image = new QLabel(this);
    label_image->setFixedSize(40, 40);
    label_image->setScaledContents(true);
    label_image->setMovie(mv);
    label_text = new QLabel(this);
    hlayout = new QHBoxLayout;
    hlayout->addWidget(label_image);
    hlayout->addWidget(label_text, 1);
    this->setLayout(hlayout);

    label_text->setText(text);
    mv->start();
    this->setFixedHeight(60);
}

lds_messagebox_loading::~lds_messagebox_loading()
{
}

void lds_messagebox_loading::image_dynamic_run()
{
    mv->jumpToNextFrame();
    label_image->repaint();
    this->repaint();
}

QString lds_messagebox_loading::text()
{
    return label_text->text();
}

QPushButton* lds_messagebox_loading::layoutAddButton(const QString &btn_txt)
{
    QPushButton *b = createMsgButton(this, btn_txt);

    hlayout->addWidget(b);
    btnlist << b;

    return b;
}

int lds_messagebox_loading::layoutButtonIndexOf(QObject *obj)
{
    return btnlist.indexOf(obj);
}

QObject *lds_messagebox_loading::layoutButton(int index)
{
    return btnlist.value(index);
}

void lds_messagebox_loading::setText(const QString &text)
{
    label_text->setText(text);
    label_text->repaint();
    this->repaint();
}

void lds_messagebox_loading::update_image()
{

}

lds_messagebox_loading_show::lds_messagebox_loading_show(QWidget *parent, const QString &title, const QString &text)
    : QObject(parent)
{
    _title = title;
    loading = new lds_messagebox_loading(parent, MESSAGE_TITLE_VOID, text);
    dialog = new lds_roundeddialog_rect_align_200x100(loading);
    dialog->setModal(true);
    btn_click_index = -1;
    loading->setFixedWidth(300);
    setText(text);
}

lds_messagebox_loading_show::~lds_messagebox_loading_show()
{
    delete dialog;
}

void lds_messagebox_loading_show::show()
{
    dialog->show();
}

void lds_messagebox_loading_show::hide()
{
    dialog->hide();
}

lds_roundeddialog_rect_align *lds_messagebox_loading_show::widget()
{
    return dialog;
}

void lds_messagebox_loading_show::show_delay()
{
    show();
    QEventLoop eloop;
    QTimer::singleShot(100, &eloop, SLOT(quit()));
    eloop.exec();
}

//delay 是 倒计时的问题
void lds_messagebox_loading_show::setText(const QString &text, int delay)
{
    setText(text);
    if(delay > 0) {
        bool first = true;
        QTime dieTime = QTime::currentTime().addMSecs(delay);
        while( QTime::currentTime() < dieTime ) {
            int remain_secs = QTime::currentTime().msecsTo(dieTime)/1000;
            if(remain_secs > 0)
                loading->setText(text+QString("(%1)").arg(remain_secs));
            else
                loading->setText(text);

            if(first) {
                dialog->buddyResize();
                first = false;
            }
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
}

void lds_messagebox_loading_show::setText(const QString &text)
{
    _text = text;

    if(_title.length() > 0) {
        loading->setText(_title + "\n" + text);
    } else {
        loading->setText(text);
    }
    dialog->buddyResize();
}

void lds_messagebox_loading_show::showMessage(const QString &text)
{
    setText(text);
}

void lds_messagebox_loading_show::toBtnClicked()
{
    btn_click_index = loading->layoutButtonIndexOf(this->sender());
    emit signal_btn_clicked(btn_click_index);
}

void lds_messagebox_loading_show::image_dynamic_run()
{
    loading->image_dynamic_run();
}

QString lds_messagebox_loading_show::text()
{
    return _text;
}

void lds_messagebox_loading_show::setTile(const QString &title)
{
    this->_title = title;
}

QString lds_messagebox_loading_show::title()
{
    return _title;
}

void lds_messagebox_loading_show::addButton(const QString &btn_txt)
{
    connect(loading->layoutAddButton(btn_txt), SIGNAL(clicked()), this, SLOT(toBtnClicked()));
}

void lds_messagebox_loading_show::hideButton(int index)
{
    QPushButton *b = qobject_cast<QPushButton *>(loading->layoutButton(index));
    if(b) {
        b->hide();
    }
}

void lds_messagebox_loading_show::setButtonText(int index, const QString &text)
{
    QPushButton *b = qobject_cast<QPushButton *>(loading->layoutButton(index));
    if(b) {
        b->setText(text);
    }
}

void lds_messagebox_loading_show::setMSGWindowFlags(Qt::WindowFlags flags)
{
    loading->setWindowFlags(flags);
}

int lds_messagebox_loading_show::result()
{
    return btn_click_index;
}

void lds_messagebox_loading_show::updateProgress(qint64 bytesStep, qint64 bytesTotal)
{
    setText(QString("%1/%2").arg(bytesStep).arg(bytesTotal));
    image_dynamic_run();
}

void lds_messagebox_loading_show::updateProgress(qint64 bytesStep, qint64 bytesTotal, const QString &title)
{
    setTile(title);
    updateProgress(bytesStep, bytesTotal);
}

lds_messagebox_textedit::lds_messagebox_textedit(QWidget *parent, const QString &title, const QString &text, QStringList buttonTexts)
    : lds_msgbox_virtual(QStyle::SP_CustomBase, title, text, buttonTexts,  parent)
{
    QLabel *label_title = new QLabel(this);
    QLabel *label_icon = new QLabel(this);
    QTextEdit *edit = new QTextEdit(this);
    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    edit->setReadOnly(true);

    //icon_title
    hlayout = new QHBoxLayout;
    hlayout->addWidget(label_icon);
    hlayout->addWidget(label_title, 1);
    vlayout->addLayout(hlayout);

    //edit
    vlayout->addWidget(edit, 1);

    //btn
    hlayout = new QHBoxLayout;
    hlayout->addStretch(1);
    btnok = 0;
    if(buttonTexts.isEmpty())
        buttonTexts.prepend(QObject::tr("确定"));
    foreach(const QString &btn_text, buttonTexts) {
        QPushButton *p = createMsgButton(this, btn_text);
        hlayout->addWidget(p);
        btnlist.append(p);
        connect(p, SIGNAL(clicked()), this, SLOT(todone()));

        if(btnok == 0)
            btnok = p;
    }
    vlayout->addLayout(hlayout);

    this->setLayout(vlayout);

    label_title->setText(title);
    label_icon->setPixmap(
                style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, this).pixmap(40, 40)
                );
    edit->setText(text);

    this->setFixedSize(400, 300);//大小不确定的话，垂直滚动条的位置可能会变
}

lds_msgbox_virtual::lds_msgbox_virtual(QStyle::StandardPixmap icon, const QString &title, const QString &text, QStringList buttonTexts, QWidget *parent)
    : QDialog(parent)
    , btnok(0)
{

}

void lds_msgbox_virtual::updateTimeframe(int index)
{
    if(btnok) {
        btnok->setText(btnok->text().split(":").value(0)+":"+QString::number(index));
        if(index==0)
            emit btnok_finished(0);
    }
}

void lds_msgbox_virtual::todone()
{
    this->done(btnlist.indexOf(this->sender()));
}

void lds_msgbox_virtual::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape) {
        e->ignore();
        return;
    }
    QDialog::keyPressEvent(e);
}
