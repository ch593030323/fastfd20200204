#ifndef PUBLIC_ENCRYPTION_DOG_H
#define PUBLIC_ENCRYPTION_DOG_H

#include <QWidget>
//加密狗
#ifdef Q_OS_WIN
#include<QLibrary>
//typedef long ( *NoxGetLastError)();
//typedef long ( *NoxClose)(long keyHandle);
//typedef long ( *NoxFind)(long ,long * , long *); //定义函数指针
//typedef long ( *NoxOpen)(long keyHandle, char * userPin);
//typedef long ( *NoxReadStorage)(long keyHandle, unsigned char * pBuffer);

typedef long (__stdcall *NoxGetLastError)();
typedef long (__stdcall *NoxClose)(long keyHandle);
typedef long (__stdcall *NoxFind)(long ,long * , long *); //定义函数指针
typedef long (__stdcall *NoxOpen)(long keyHandle, char * userPin);
typedef long (__stdcall *NoxReadStorage)(long keyHandle,   unsigned char * pBuffer);
#endif

class public_encryption_dog : public QObject
{
    Q_OBJECT
public:
    enum ERROR_CODE {
        ERROR_CODE_WRONGLIB = 0,
        ERROR_CODE_NODOG,
        ERROR_CODE_WRONGDOG,
        ERROR_CODE_NOLIB,
        ERROR_CODE_NOERR
    };
public:
    explicit public_encryption_dog(QObject *parent = 0);
    static ERROR_CODE conductlock(QString &dogID);
    static QString error_name(ERROR_CODE code);
};

#endif // PUBLIC_ENCRYPTION_DOG_H
