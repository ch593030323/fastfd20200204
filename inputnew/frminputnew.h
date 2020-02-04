#ifndef FRMINPUTNEW_H
#define FRMINPUTNEW_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QPushButton>
#include <QTimer>
#include <QShowEvent>
#include <QMutexLocker>
#include<QDBusInterface>

#include<qdebug.h>

struct firstwidget
{
    QWidget *first;
    int y;

};




class frmInputNew : public QWidget
{
    Q_OBJECT

public:
    explicit frmInputNew(QWidget *parent = 0);
    ~frmInputNew();

    //单例模式,保证一个程序只存在一个输入法实例对象
    static frmInputNew *Instance()
    {
        if (!_instance) {
            static QMutex mutex;
            QMutexLocker locker(&mutex);

            if (!_instance) {
                _instance = new frmInputNew;
            }
        }

        return _instance;
    }



protected:
    //事件过滤器
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    //焦点改变事件槽函数处理
    void focusChanged(QWidget *oldWidget, QWidget *nowWidget);

    void changehide();

      void keyboardstatus(bool status);
private:
     QDBusInterface *dbusInterface;

    bool keyboard_show;

    static frmInputNew *_instance;  //实例对象
    int deskWidth;                  //桌面宽度
    int deskHeight;                 //桌面高度
    bool isFirst;                   //是否首次加载
    void InitForm();                //初始化窗体数据
    void ShowPanel();               //显示输入法面板

    QWidget *currentWidget;         //当前焦点的对象
    QLineEdit *currentLineEdit;     //当前焦点的单行文本框
    QTextEdit *currentTextEdit;     //当前焦点的多行文本框
    QPlainTextEdit *currentPlain;   //当前焦点的富文本框
    QTextBrowser *currentBrowser;   //当前焦点的文本浏览框


    QString currentPosition;        //当前输入法面板位置类型
    void huifu();
    QList<firstwidget *> listfirstwindows;

    bool mylistcontains(QWidget * now);


};

#endif // FRMINPUTNEW_H
