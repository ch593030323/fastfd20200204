#ifndef LDS_ENCRYPT_H
#define LDS_ENCRYPT_H

#include <QObject>

class lds_encrypt
{
public:
    lds_encrypt();

    //科瑞达
    //解密
    static QString CREATE_de(QString as_source);
    //加密
    static QString CREATE_en(QString as_source);

    static QByteArray SHA256_en(QByteArray code);

    static QByteArray BASE62_en(qlonglong num);
    static qlonglong BASE62_de(QByteArray code);
};

#endif // LDS_ENCRYPT_H
