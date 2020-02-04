#ifndef READER_RF_H
#define READER_RF_H

#include <QObject>
#include <QByteArray>
#include <QStringList>
#ifdef Q_OS_UNIX
#include <sys/types.h>
#include "libusb-1.0/libusb.h"
#include <linux/fs.h>
#include <stdio.h>
#endif

/**
exmple:
            Reader_RF reader;
            reader.setDefPwd("ffffffffffff");
            QString p0,p1,p2;
             if(reader.readcard(p0, p1, p2)) {
             } else {
                //reader.lastError();
             }
*/
class Reader_RF : public QObject
{
    Q_OBJECT
public:
    explicit Reader_RF(QObject *parent = 0);
    ~Reader_RF();
    /*writecard("1234567812345678","1234567812345678","1234567812345678");*/
    bool writecard(const QString &part0,const  QString &part1,const  QString &part2);       //将part0,part1,part2分别写到卡内三块
    bool writeSqr(int block_id, const QString &part);//将part 写到卡内block_id块

    /*return    "1234567812345678","1234567812345678","1234567812345678"true,true*/
    bool readcard(QString &part0,QString &part1,QString &part2);                                         //读卡 将卡内三块数据回读到 part0,part1,part2

    bool setDefPwd(const QString &defPwd/*ffffffffffff*/);                                                        //默认密码修改,第一步总是要执行,用于读写是,匹配卡密码
    QByteArray defPwd();/*ffffffffffff*/                                                                                         //读取默认密码
    bool pwdChange(const QString &oldPwd/*ffffffffffff*/,                                                       //修改密码
                   const QString &newPwd/*ffffffffffff*/);

    bool cardisready();                                                                                                                     //判断是否有卡放在读卡器上
    bool pwdiscorrect();                                                                                                                   //判断卡密码是否正确

    QString lastError();
    bool beep();                                                                                                                                 //蜂鸣０
    void setsection(char section/*0~f*/);                                                                                      //修改扇区默认是0x01
private:
#ifdef Q_OS_UNIX
    int getRespone(libusb_device_handle * connection,const QByteArray &tmp);                   //向读卡器发指令，并返回返回值

    QByteArray getBccList(char fuction,char length, const QByteArray &original);                  //编辑指令，如蜂鸣等

    QByteArray getBccWriteList(char fuction,char length,char area,                                         //写入
                               const QByteArray &original);

    QString simpleError() const;
    int indexof_simpleError() const;
    //打印所有的bus上的接口
private:
    void print_devs(libusb_device **devs);
    void printdev(libusb_device *dev);

    libusb_device_handle *dev_handle;//usb
    libusb_context *ctx;//usb context
#endif
    bool open();
    void close();

    int sqr;//自增序列号
    QByteArray byte2;//接受数据

    QStringList read_card_error;
    QString otherError;
    QByteArray _DefPwd;
    bool isopen;
};

#endif // READER_RF_H
