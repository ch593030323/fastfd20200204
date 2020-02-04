#include "lds_encrypt.h"
#include <openssl/sha.h>
#include <qmath.h>
#include <QtDebug>

lds_encrypt::lds_encrypt()
{
}

QString lds_encrypt::CREATE_de(QString as_source)
{
    //Public function of_str_decrypt (string as_source) returns string
    //string as_source
    //    long i;
    int ll_len;
    QString ls_text;
    QString ls_result;
    QString ls_return;

    //Has been Shielded.
    ls_text = as_source.trimmed();
    ll_len = ls_text.length();
    ls_result = "";
    for(int i = 0; i < ll_len; i++) { // 123  -> 321
        //前插入
        ls_result.push_front(QChar((ls_text.at(i).unicode() -  (2 * (ll_len - i - 3)))));//char(asc(mid(ls_text,i,1)) - (2 * (ll_len - i - 2)));
    }
    return ls_result;
}

QString lds_encrypt::CREATE_en(QString as_source)
{
    //Public function of_str_encrypt (string as_source) returns string
    //string as_source
    //    long i;
    int ll_len;
    QString ls_text;
    QString ls_result;
    //Has been Shielded.
    ls_text = as_source.replace(" ", "").trimmed();
    ll_len = ls_text.length();
    ls_result = "";
    for(int i = 0; i < ll_len ; i++) {//321  -> 123
        ls_result.push_front(QChar((ls_text.at(i).unicode() +  (2 * (i - 2)))));

        qDebug() << __LINE__ << i << ls_result.left(1) << (ls_text.at(i).unicode() +  (2 * (i - 2)));
    }
    return ls_result;
}

QByteArray lds_encrypt::SHA256_en(QByteArray code)
{
    QByteArray sigret;
    sigret.resize(32);
    SHA256((unsigned char*)code.data(), code.length(), (unsigned char*)sigret.data());
    return sigret;
}

QByteArray lds_encrypt::BASE62_en(qlonglong num)
{
    QByteArray result;
//    QByteArray char62 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    //shuffle
    QByteArray char62 = "mfS4ODl7ejMkRJsP81ahITHygW9wLvKn2QFqGr6iAYEuxBZ0dct3b5NXpCUzoV";
    do {
        result.prepend(char62.at(num % 62));
        num = num / 62;
    } while(num > 0);

    return result;
}

qlonglong lds_encrypt::BASE62_de(QByteArray code)
{
    qlonglong result = 0;
//    QByteArray char62 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    //shuffle
    QByteArray char62 = "mfS4ODl7ejMkRJsP81ahITHygW9wLvKn2QFqGr6iAYEuxBZ0dct3b5NXpCUzoV";
    while(code.count() > 0) {
         result += qPow(62, code.count() - 1) * char62.indexOf(code[0]);
         code.remove(0, 1);
    }
    return result;
}
