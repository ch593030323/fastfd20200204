#ifndef WIN32_PRINTER_ARGOX_PPLB_H
#define WIN32_PRINTER_ARGOX_PPLB_H

#include <QObject>
//各个设备的pid,vid可以到对应文件的，函数 get_pid;get_vid获得
//只支持printer_argox_pplb这个型号的打印机


//example
/**
char data[]=
            "N\nZB\nA98,49,0,966,1,1,N,\"hello\"\nP1\n";//"N\r\nZB\r\nFS\"standalone\"\r\nT20,20,0,6,1,1,N,\"ABC123hello\"\r\nFE\r\nQ240,40\r\nFR\"standalone\"\r\nW1\r\nFK\"standalone\"\r\n";
win32_printer_argox_pplb printer;
if(printer.send_data(printer_label_argox_PPLB::get_vid(), printer_label_argox_PPLB::get_pid(), data)) {
} else {
    //printer.errString();
}

*/
class win32_printer_argox_pplb : public QObject
{
    Q_OBJECT
public:
    explicit win32_printer_argox_pplb(QObject *parent = 0);
    ~win32_printer_argox_pplb();

    bool send_data(quint16 vid, quint16 pid, char *data);//更具pid，vid来识别设备

    QString errString();

private:
    QString err;
};

#endif // WIN32_PRINTER_ARGOX_PPLB_H
