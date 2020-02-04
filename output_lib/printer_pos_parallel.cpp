#include "printer_pos_parallel.h"

QString Printer_POS_PARALLEL::printPort()
{
    return "LPT1:";
}

Printer_POS_PARALLEL::Printer_POS_PARALLEL()
{
    m_file = 0;
    _open = false;
}

Printer_POS_PARALLEL::~Printer_POS_PARALLEL()
{
    qDebug() << __FUNCTION__;
    close();
}

qint64 Printer_POS_PARALLEL::writedirect(const QByteArray &byte)
{
    return fwrite(byte.data(),byte.size(),1,m_file);
}

qint64 Printer_POS_PARALLEL::write(const QString & str, QString languageKey)
{
    return writedirect(printer_0::trans_u2codepage(str, languageKey, "GBK"));

}

qint64 Printer_POS_PARALLEL::writeHex(const QString &hex)
{
    return writedirect(QByteArray::fromHex(QByteArray().append(hex)));
}

bool Printer_POS_PARALLEL::tryOpen(const QString &ipport)
{
    _open = false;
    m_error = "";
    m_port = ipport;
    //    m_file = fopen("LPT1:","r+");
    m_file = fopen(ipport.toLocal8Bit(),"r+");
    if(m_file == NULL)
    {
        m_error =  "sorry! open LPT1 error!!";
        return false;
    }

    _open = true;
    return true;
}

bool Printer_POS_PARALLEL::isOpen()
{
    return _open;
}

void Printer_POS_PARALLEL::close()
{
    if(_open) {
        _open = false;
        fclose(m_file);
        m_file = NULL;
    }
}

QString Printer_POS_PARALLEL::lastError()
{
    return m_error;
}

QString Printer_POS_PARALLEL::port()
{
    return m_port;
}

bool Printer_POS_PARALLEL::waitForReadyReadv(int msecs)
{
    Q_UNUSED(msecs)

    return false;
}

QByteArray Printer_POS_PARALLEL::readAll()
{
    QByteArray ret;
    return ret;
}

