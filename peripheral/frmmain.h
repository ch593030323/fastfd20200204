#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
class QextSerialPort;

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    QextSerialPort *myCom;
    QTimer *myTimer;

    QTimer *myReadTimer;//定时读取串口数据

    int SendCount;//发送数据计数
    int ReceiveCount;//接收数据计数
    bool IsShow;//是否显示数据
    bool IsDebug;//是否启用调试,接收到数据后模拟发送数据    
    bool IsAutoclear;//是否自动清空
    bool IsHexSend;//是否16进制数据发送
    bool IsHexReceive;//是否16进制数据接收    

    QStringList SendDataList;//转发数据链表
    void InitForm();//初始化界面以及其他处理
     QString jieshi(int a);
signals:
     void weighComming(QString);
     void qupiComming(bool);
     void lingweiComming(bool);
public:
     bool openFrm(QString path,int rate);
     void zhiling();
     void qupi();
private slots:
    void ReadMyCom();//读取串口数据   
};

#endif // FRMMAIN_H
