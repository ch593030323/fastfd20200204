#include "frminputnew.h"
#include "qdesktopwidget.h"
#include<QApplication>
#include<QtGui>

frmInputNew *frmInputNew::_instance = 0;
frmInputNew::frmInputNew(QWidget *parent) :
    QWidget(parent)

{

    this->InitForm();

}

frmInputNew::~frmInputNew()
{

}


void frmInputNew::InitForm()
{

    // this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);


    QDesktopWidget desk;
    deskWidth = desk.availableGeometry().width();
    deskHeight = desk.height();//desk.availableGeometry().height();

    isFirst = true;


    currentWidget = 0;
    currentLineEdit = 0;
    currentTextEdit = 0;
    currentPlain = 0;
    currentBrowser = 0;

    //如果需要更改输入法面板的显示位置,改变currentPosition这个变量即可
    //control--显示在对应输入框的正下方 bottom--填充显示在底部  center--窗体居中显示
    currentPosition = "control";
    //通信
    dbusInterface = new QDBusInterface("com.scpos.system", "/system/desktop", "com.scpos.desktop", QDBusConnection::systemBus(), this);
    connect(dbusInterface, SIGNAL(keyboardstatus(bool)), SLOT(keyboardstatus(bool)));
    //    qDebug()<<"dbus  fish";

    //	//绑定全局改变焦点信号槽
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
            this, SLOT(focusChanged(QWidget *, QWidget *)));
    //绑定按键事件过滤器
    qApp->installEventFilter(this);
}


void frmInputNew::ShowPanel()
{

    dbusInterface->call("showkeyboard");
}

//事件过滤器,用于识别鼠标单击汉字标签处获取对应汉字
bool frmInputNew::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::Hide && obj->objectName()!="frmInputNew") {
        //          qDebug()<<"====================== == ";
        //为防止空指针 窗体隐藏移除list
        for(int i=0;i<listfirstwindows.size();i++) {
            if(listfirstwindows[i]->first==(QWidget *)obj) {
                listfirstwindows.removeAt(i);
                break;
            }
        }
    }

    if (event->type() == QEvent::RequestSoftwareInputPanel) {
        if((obj->objectName() != "qt_spinbox_lineedit"
            &&!obj->inherits("QDateTimeEdit")
            &&!obj->inherits("QDateEdit")
            &&!obj->inherits("QTimeEdit")
            &&!obj->inherits("QDoubleSpinBox")
            &&!obj->inherits("QTextEdit")
            &&!obj->inherits("QPlainTextEdit")
            &&!obj->inherits("QTextBrowser")
            && (obj->inherits("QSpinBox")
                || obj->inherits("QComboBox")
                || obj->inherits("QLineEdit")))) {

            if(currentWidget) {
                //先存储list      currentWidget = nowWidget;
                if(!mylistcontains(currentWidget->window())) {
                    firstwidget *listfirst=new firstwidget;
                    listfirst->first=currentWidget->window();
                    listfirst->y=currentWidget->window()->y();
                    listfirstwindows.append(listfirst);
                }
                ShowPanel(); //显示软键盘
                //      }

                QPoint linexy=currentWidget->mapToGlobal(QPoint(0,0));
                // qDebug()<<linexy.y()<<(deskHeight-370);
                if(linexy.y()>=(deskHeight-370)) { //284
                    // qDebug()<<"yidong----------------------";
                    currentWidget->window()->move(currentWidget->window()->x(),currentWidget->window()->y()-(linexy.y()-(deskHeight-450)));

                }
            }
        }

    } else
        if (event->type() == QEvent::CloseSoftwareInputPanel) {
            //            qDebug()<<"xuyao  close  ----------";
        }
    return QWidget::eventFilter(obj, event);
}


void frmInputNew::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
    if (nowWidget != 0 && !this->isAncestorOf(nowWidget)) {

        //如果对应属性noinput为真则不显示
        if (nowWidget->property("noinput").toBool()) {
            QTimer::singleShot(0, this, SLOT(changehide()));
            return;
        }

        isFirst = false;

        if((!nowWidget->inherits("QDateTimeEdit")
            &&!nowWidget->inherits("QDateEdit")
            &&!nowWidget->inherits("QTimeEdit")
            &&!nowWidget->inherits("QDoubleSpinBox")
            &&!nowWidget->inherits("QTextEdit")
            &&!nowWidget->inherits("QPlainTextEdit")
            &&!nowWidget->inherits("QTextBrowser")
            && (nowWidget->inherits("QSpinBox")
                || nowWidget->inherits("QComboBox")
                || nowWidget->inherits("QLineEdit")))) {

            currentWidget = nowWidget;
            //获取相对屏幕位置
            QPoint linexy=currentWidget->mapToGlobal(QPoint(0,0));

            if(linexy.y()>=(deskHeight-370)) { //284

                if(mylistcontains(currentWidget->window())  && keyboard_show )//判断是否已经包含 包含了才需要移动
                {
                    currentWidget->window()->move(currentWidget->window()->x(),currentWidget->window()->y()-(linexy.y()-(deskHeight-370)));
                }
            }

        }

    }else{
    }
}

void frmInputNew::changehide()
{

    dbusInterface->call("hideKeyboard");
}


void frmInputNew::huifu()
{
    // qDebug()<<"------------"<<listfirstwindows.size();
    for(int i=0;i<listfirstwindows.size();++i) {
        listfirstwindows[i]->first->move(listfirstwindows[i]->first->x(),listfirstwindows[i]->y);

    }
}

bool frmInputNew::mylistcontains( QWidget *now)
{
    bool ret=false;

    for(int i=0;i<listfirstwindows.size();i++) {
        if(listfirstwindows[i]->first==now) {
            ret=true;
            break;
        }
    }
    //  qDebug()<<ret;
    return ret;
}

void frmInputNew::keyboardstatus(bool status)
{

    if(status==false) {
        huifu();
        keyboard_show=false;
    }else{
        keyboard_show=true;
        // ShowPanel();
    }
}



