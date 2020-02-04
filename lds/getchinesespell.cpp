#include "getchinesespell.h"
#include <QTextCodec>

static bool In(wchar_t start, wchar_t end, wchar_t code)
{
    if (code >= start && code <= end)
    {
        return true;
    }
    return false;
}
static char convert(int n)
{
    if (In(0xB0A1,0xB0C4,n)) return 'a';
    if (In(0XB0C5,0XB2C0,n)) return 'b';
    if (In(0xB2C1,0xB4ED,n)) return 'c';
    if (In(0xB4EE,0xB6E9,n)) return 'd';
    if (In(0xB6EA,0xB7A1,n)) return 'e';
    if (In(0xB7A2,0xB8c0,n)) return 'f';
    if (In(0xB8C1,0xB9FD,n)) return 'g';
    if (In(0xB9FE,0xBBF6,n)) return 'h';
    if (In(0xBBF7,0xBFA5,n)) return 'j';
    if (In(0xBFA6,0xC0AB,n)) return 'k';
    if (In(0xC0AC,0xC2E7,n)) return 'l';
    if (In(0xC2E8,0xC4C2,n)) return 'm';
    if (In(0xC4C3,0xC5B5,n)) return 'n';
    if (In(0xC5B6,0xC5BD,n)) return 'o';
    if (In(0xC5BE,0xC6D9,n)) return 'p';
    if (In(0xC6DA,0xC8BA,n)) return 'q';
    if (In(0xC8BB,0xC8F5,n)) return 'r';
    if (In(0xC8F6,0xCBF0,n)) return 's';
    if (In(0xCBFA,0xCDD9,n)) return 't';
    if (In(0xCDDA,0xCEF3,n)) return 'w';
    if (In(0xCEF4,0xD188,n)) return 'x';
    if (In(0xD1B9,0xD4D0,n)) return 'y';
    if (In(0xD4D1,0xD7F9,n)) return 'z';
    return '\0';
}
getChineseSpell::getChineseSpell(QObject *parent) :
    QObject(parent)
{
}

QString getChineseSpell::ChineseSpell(QString src)
{
    wchar_t wchr;
    QString strPinYin;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gbk"));
    for (int i=0; i<src.length(); i++)
    {
        QString str = src.at(i);
        QByteArray arr = str.toLocal8Bit();

        if(arr.size() == 2)
        {
            wchr = (arr.at(0) & 0xff) << 8;
            wchr |= (arr.at(1) & 0xff);
            strPinYin.append(convert(wchr));
        }
        else
        {
            strPinYin.append(arr);
        }
    }
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

    //过滤如雪花啤酒第一个雪字
    QString tablevalue;
    foreach(const QChar &c, strPinYin) {
        if(!c.isNull()) {
            tablevalue += c;
        }
    }
    return tablevalue;
}
