#include "barscale_main_dialog.h"
#include "ui_barscale_main_dialog.h"
#include <QTimer>
#include <QHostAddress>
#include "electronicscale_dahua.h"
#include "electronicscale_cas.h"
#include "electronicscale_ysheng.h"
#include "electronicscale_djian.h"
#include "electronicscale_kaifeng.h"
#include "electronicscale_rongta.h"
#include <QtCore>
#include "commonheader.h"
#if QT_VERSION > QT_VERSION_CHECK(5,0,0)
#include <QtConcurrent/QtConcurrent>
#endif
#include "barscale_net_set_dialog.h"
#include "barscale_downdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include "ui_labelprint_goodinfo_add_dialog.h"
#include "labelprint_goodinfo_add_dialog.h"
#include "barscale_num_input.h"
#include "lds.h"
#include "uinormal_message.h"
#include "electronicscale_dahua_en.h"
#include "electronicscale_dahua_07.h"
#include "ui_barscale_downdialog.h"

/*
1）您那需要对LSX条码秤升级新版本到：LSX_Chinese_V6.050
2）然后在秤下位机菜单中打开：系统设置（长按此按键3s）->左右切换->参数设置->是否启用简单变价协议? 选择是否生效（我司程序默认关闭）
3）本软件只提供修改价格，修改品名功能,如清除plu,设置
*/
static void ScaleDownInfo(QObject *receiver,
                          QStringList conduct_scale,
                          bool *isbreak,
                          QStandardItemModel *downmodel,
                          QStandardItemModel *ipportmodel)
{
    //! 开始计时
    QMetaObject::invokeMethod(receiver,
                              "startTimer",
                              Qt::QueuedConnection
                              );
    //! 开始计时
    //! 修改pushbutton的 文本
    QMetaObject::invokeMethod(receiver,
                              "updateButtonText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QObject::tr("暂停"))
                              );
    //! 修改pushbutton的 文本
    //! 获取条码秤类别
    QString scaletype = conduct_scale[1];
    QString conduct_type=conduct_scale[0];
    //! 获取条码秤类别

    *isbreak = false;//初始化 手动中断 为 false
    ElectronicScale_DAHUA *dahua_scale = 0;
    ElectronicScale_DAHUA dahua;
    ElectronicScale_DAHUA_EN dahua_EN;
    ElectronicScale_DAHUA_07 dahua_07;
    bool dahua_goin = true;

    switch(CommonHeader::creat_barcode_scaletypes().indexOf(scaletype)) {
    case 0://上海大华 TM-15H
        if(dahua_goin) {
            dahua_scale = &dahua;
            dahua_goin = false;
        }
    case 6://dahua_EN
        if(dahua_goin) {
            dahua_scale = &dahua_EN;
            dahua_goin = false;
        }
    case 7://dahua_07
        if(dahua_goin) {
            dahua_scale = &dahua_07;
            dahua_goin = false;
        }
    {
        if(conduct_type == "down") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     ,  quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(downmodel->rowCount()));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = downmodel->rowCount();
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(dahua_scale->downloadPLUINFO(downmodel, down_row));
                            if(printer.waitForReadyRead(30000, isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                //! 3次读取，读满11为止 <--begin-->
                                for(int c = 0; c < 10; c++) {
                                    if(ret.length() >= 11) break;
                                    if(printer.waitForReadyRead(1000,isbreak)) {
                                        ret += printer.tcpSocket->readAll();
                                    }
                                }
                                //! 3次读取，读满11为止 <--end-->

                                if(ret.startsWith("0v")) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->

        } else if(conduct_type == "clear") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     , quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(1));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = 1;
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_DAHUA::clearPLU());
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                //! 3次读取，读满8为止 <--begin-->
                                int loop_count = 3;
                                while(loop_count >=1 && loop_count --) {
                                    if(ret.length() < 8) {
                                        printer.waitForReadyRead(30000,isbreak);
                                        ret += printer.tcpSocket->readAll();
                                    }
                                }
                                //! 3次读取，读满8为止 <--end-->

                                if(ret.startsWith("0i")) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    isSuccess = true;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        } else if(conduct_type == "setlabel") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     , quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(2));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = 2;
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_DAHUA::setLABEL_40x30_step1());
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                //! 3次读取，读满7为止 <--begin-->
                                int loop_count = 3;
                                while(loop_count >=1 && loop_count --) {
                                    if(ret.length() < 7) {
                                        printer.waitForReadyRead(30000,isbreak);
                                        ret += printer.tcpSocket->readAll();
                                    }
                                }
                                //! 3次读取，读满7为止 <--end-->
                                if(ret.startsWith("0t")) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    printer.writedirect(ElectronicScale_DAHUA::setLABEL_40x30_step2());
                                    if(printer.waitForReadyRead(30000,isbreak)) {
                                        QByteArray ret = printer.tcpSocket->readAll();
                                        //! 3次读取，读满7为止 <--begin-->
                                        int loop_count = 3;
                                        while(loop_count --) {
                                            if(ret.length() < 7) {
                                                printer.waitForReadyRead(30000,isbreak);
                                                ret += printer.tcpSocket->readAll();
                                            }
                                        }
                                        //! 3次读取，读满7为止 <--end-->
                                        if(ret.startsWith("0r")) {
                                            down_row++;
                                            QMetaObject::invokeMethod(receiver,
                                                                      "updateProgressbar",
                                                                      Qt::QueuedConnection,
                                                                      Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                                      Q_ARG(int, ipport_row)
                                                                      );
                                            isSuccess = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        }
    }
        break;
    case 1://凯士 CL5200
    {
        if(conduct_type == "down") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     , quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(downmodel->rowCount()));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = downmodel->rowCount();
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_CAS::downloadPLUINFO(
                                                    downmodel->item(down_row,0)->text()
                                                   ,  downmodel->item(down_row,8)->text()
                                                   ,  downmodel->item(down_row,1)->text()
                                                   ,  downmodel->item(down_row,26)->text()
                                                   ,  downmodel->item(down_row,2)->text()

                                                   ,  downmodel->item(down_row,7)->text()
                                                   ,  downmodel->item(down_row,3)->text()
                                                    )
                                                );
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                if(ret.endsWith("W02:O01")) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        } else if(conduct_type == "clear") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     , quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(downmodel->rowCount()));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = downmodel->rowCount();
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_CAS::clearPLU(
                                                    downmodel->item(down_row, 0)->text()
                                                   ,  downmodel->item(down_row, 8)->text()
                                                    )
                                                );
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                if(ret.endsWith("C003:O13")) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        }
    }
        break;
    case 2: //友声
    {
        if(conduct_type == "down") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     , quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(downmodel->rowCount()));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = downmodel->rowCount();
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_YSHENG::downloadPLUINFO(downmodel, down_row));
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                if(ret.startsWith(ElectronicScale_YSHENG::end_ans())) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        } else if(conduct_type == "clear") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     ,  quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     ,  30000
                                     ,  isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(1));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = 1;
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_YSHENG::clearPLU());
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                if(ret.startsWith(ElectronicScale_YSHENG::end_ans())) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        }
    }
        break;
    case 3://厦门顶尖LS2CX
    {
        if(conduct_type == "down") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            printer.resetSocket(barscale_main_SOCKET::UDP);
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     ,  quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     ,  30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(downmodel->rowCount()));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = downmodel->rowCount();
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_DJIAN::downloadPLUINFO(downmodel->item(down_row, 0)->text(),
                                                                                       downmodel->item(down_row, 26)->text(),
                                                                                       downmodel->item(down_row, 1)->text(),
                                                                                       downmodel->item(down_row, 2)->text(),
                                                                                       downmodel->item(down_row, 3)->text(),
                                                                                       downmodel->item(down_row, 7)->text()
                                                                                       ));
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                if(ret.startsWith(ElectronicScale_DJIAN::downloadPLUINFO_back(downmodel->item(down_row, 1)->text()))) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        } else if(conduct_type == "clear") {

        }
    }
        break;
    case 4://凯丰电子秤
    {
        if(conduct_type == "down") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            printer.resetSocket(barscale_main_SOCKET::UDP);
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     ,  quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     ,  30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(downmodel->rowCount()));

                    if(downmodel->rowCount()) {
                        bool pre_set = false;
                        //设置商店名称
                        printer.writedirect(ElectronicScale_KAIFENG::setShopName(downmodel->item(0,27)->text()));
                        if(printer.waitForReadyRead(30000, isbreak)) {
                            if(printer.tcpSocket->readAll().startsWith(ElectronicScale_KAIFENG::setShopName_back())) {

                                // 和商店prefix
                                printer.writedirect(ElectronicScale_KAIFENG::getSysParam());
                                if(printer.waitForReadyRead(30000,isbreak)) {
                                    QByteArray b = printer.tcpSocket->readAll();
                                    if(b.startsWith(ElectronicScale_KAIFENG::getSysParam_back())) {

                                        printer.writedirect(ElectronicScale_KAIFENG::setShopPrefix(downmodel->item(0,8)->text(), b));
                                        if(printer.waitForReadyRead(30000,isbreak)) {
                                            if(printer.tcpSocket->readAll().startsWith(ElectronicScale_KAIFENG::setShopPrefix_back())) {
                                                pre_set = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if(!pre_set) {
                            ipport_row++;
                            continue;
                        }
                    }


                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = downmodel->rowCount();
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_KAIFENG::downloadPLUINFO(downmodel->item(down_row, 0)->text(),
                                                                                         downmodel->item(down_row, 26)->text(),
                                                                                         downmodel->item(down_row, 1)->text(),
                                                                                         downmodel->item(down_row, 2)->text(),
                                                                                         downmodel->item(down_row, 3)->text(),
                                                                                         downmodel->item(down_row, 7)->text()
                                                                                         ));
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                if(ret.startsWith(ElectronicScale_KAIFENG::downloadPLUINFO_back())) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        } else if(conduct_type == "clear") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            printer.resetSocket(barscale_main_SOCKET::UDP);

            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     ,  quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(1));

                    //! 下载重传 <--begin-->
                    bool isSuccess = false;
                    for(int loop_step = 0; loop_step < 3; loop_step ++) {
                        int down_row = 0;
                        int down_rowcount = 1;
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(loop_step));
                        //! 一次传输 <--begin-->
                        while(down_row < down_rowcount) {
                            printer.writedirect(ElectronicScale_KAIFENG::clearPLU());
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll();
                                if(ret.startsWith(ElectronicScale_KAIFENG::clearPLU_back())) {
                                    down_row++;
                                    QMetaObject::invokeMethod(receiver,
                                                              "updateProgressbar",
                                                              Qt::QueuedConnection,
                                                              Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                              Q_ARG(int, ipport_row)
                                                              );
                                    if(down_row >= down_rowcount) {
                                        isSuccess = true;
                                        break;
                                    }
                                    continue;
                                }
                            }
                            break;
                        }
                        //! 一次传输 <--end-->
                        if(isSuccess)break;
                    }
                    //! 下载重传 <--end-->

                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        }
    }
        break;//switch
    case 5://容大RLS1000
        if(conduct_type == "down") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            barscale_main_SOCKET printer;
            printer.resetSocket(barscale_main_SOCKET::UDP);

            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     , quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(downmodel->rowCount()));

                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(0));
                    bool isSuccess = false;
                    int down_row = 0;
                    int baohao = 0;
                    int down_rowcount = downmodel->rowCount();
                    QVector<ElectronicScale_RONGTA::pluData> pludata4;//上限四个
                    while(down_row < down_rowcount) {
                        //1
                        pludata4.clear();
                        while(down_row < down_rowcount) {
                            pludata4.append(ElectronicScale_RONGTA::pluData(
                                                downmodel->item(down_row, 26)->text(),//商品名称
                                                downmodel->item(down_row, 1)->text(),//生鲜号
                                                downmodel->item(down_row, 2)->text(),//单价
                                                downmodel->item(down_row, 3)->text(),//包装类型

                                                downmodel->item(down_row, 1)->text(),//货号
                                                downmodel->item(down_row, 8)->text(),//部门号
                                                downmodel->item(down_row, 7)->text().toInt(),//有效时间
                                                ElectronicScale_RONGTA::barcodetype,
                                                ElectronicScale_RONGTA::unitcode
                                                )
                                            );
                            down_row++;
                            if(pludata4.count() >= 4) break;
                        }
                        while(pludata4.count() < 4) {
                            pludata4.append(ElectronicScale_RONGTA::pluData());
                        }

                        //2
                        baohao++;
                        printer.writedirect(ElectronicScale_RONGTA::downloadPLUINFO( baohao,
                                                                                     pludata4[0],  pludata4[1], pludata4[2], pludata4[3]
                                )
                                );
                        if(printer.waitForReadyRead(30000,isbreak)) {
                            QByteArray ret = printer.tcpSocket->readAll().toHex();
                            if(ret.startsWith("02")) {

                                QMetaObject::invokeMethod(receiver,
                                                          "updateProgressbar",
                                                          Qt::QueuedConnection,
                                                          Q_ARG(QString, QString("%1/%2").arg(down_row).arg(down_rowcount)),
                                                          Q_ARG(int, ipport_row)
                                                          );
                                if(down_row >= down_rowcount) {
                                }
                            }
                        }
                    }

                    //热键
                    QStringList hotkeys;
                    for(int row = 0, rowcount = downmodel->rowCount(); row < rowcount ;row++) {
                        hotkeys.append(downmodel->item(row, 1)->text());
                    }
                    printer.writedirect(ElectronicScale_RONGTA::creatHotkey(hotkeys));
                    if(printer.waitForReadyRead(30000,isbreak)) {
                        QByteArray ret = printer.tcpSocket->readAll().toHex();
                        if(ret.startsWith("02")) {
                            printer.writedirect(ElectronicScale_RONGTA::creatfinished());
                            if(printer.waitForReadyRead(30000,isbreak)) {
                                QByteArray ret = printer.tcpSocket->readAll().toHex();
                                if(ret.startsWith("02")) {
                                    isSuccess = true;
                                }
                            }
                        }
                    }
                    //下载结束
                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->

        } else if(conduct_type == "clear") {
            int ipport_row = 0;
            int ipport_rowcount = ipportmodel->rowCount();
            bool isSuccess = false;
            barscale_main_SOCKET printer;
            printer.resetSocket(barscale_main_SOCKET::UDP);
            //! 按照ip port  下载数据 <--begin-->
            while(ipport_row < ipport_rowcount) {
                if(printer.tryconnect(ipportmodel->item(ipport_row, 1)->text()
                                     , quint16(ipportmodel->item(ipport_row, 2)->text().toInt())
                                     , 30000
                                     , isbreak
                                      )) {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接成功"));
                    ipportmodel->item(ipport_row, 4)->setText(QString("%1").arg(1));
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输中[%1]...").arg(0));
                    printer.writedirect(ElectronicScale_RONGTA::clearPLU());
                    if(printer.waitForReadyRead(30000,isbreak)) {
                        QByteArray ret = printer.tcpSocket->readAll().toHex();
                        if(ret.startsWith("02")) {
                            QMetaObject::invokeMethod(receiver,
                                                      "updateProgressbar",
                                                      Qt::QueuedConnection,
                                                      Q_ARG(QString, QString("%1/%2").arg(1).arg(1)),
                                                      Q_ARG(int, ipport_row)
                                                      );
                            isSuccess = true;
                        }
                    }
                    if(isSuccess) {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输成功"));
                    } else {
                        ipportmodel->item(ipport_row, 3)->setText(QObject::tr("传输失败"));
                    }
                } else {
                    ipportmodel->item(ipport_row, 3)->setText(QObject::tr("连接失败"));
                }
                printer.closeconnect(30000, isbreak);
                ipport_row++;
            }
            //! 按照ip port  下载数据 <--end-->
        }
        break;
    }//switch

    //! 修改pushbutton的 文本
    // ((barscale_downdialog*)receiver)->updateButtonText(QObject::tr("完成"));
    QMetaObject::invokeMethod(receiver,
                              "updateButtonText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QObject::tr("完成"))
                              );
    //! 修改pushbutton的 文本
    //! 结束计时
    QMetaObject::invokeMethod(receiver,
                              "stopTimer",
                              Qt::QueuedConnection
                              );
    //! 结束计时
}

barscale_main_dialog::barscale_main_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::barscale_main_dialog),
    down_isbreak(false)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    {
        if(QSqlDatabase::connectionNames().contains("barscale_main_dialog")) {

        } else {
            QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE", "barscale_main_dialog");
            db.setDatabaseName(lds::localdataPath +"/db_barscale_main_dialog.db");
        }
    }
    QSqlQuery query(QSqlDatabase::database("barscale_main_dialog"));
    query.exec("create table if not exists barscale_main_dialog_table ("
               "vch_plu char(20) primary key,"
               "vch_name char(200),"
               "vch_no char(20), "
               "num_price char(20), "
               "vch_weighttype char(20),"

               "vch_validdate char(20),"
               "vch_flag char(20)"
               ")"
               );
    downmodel0=new barscaleModel(0, QSqlDatabase::database("barscale_main_dialog"));
    downmodel0->setTable("barscale_main_dialog_table");
    downmodel0->setEditStrategy(barscaleModel::OnManualSubmit);
    downmodel0->select();
    downmodel0->setHeaderData(downmodel0->fieldIndex("vch_plu"), Qt::Horizontal, QObject::tr("PLU编号"));
    downmodel0->setHeaderData(downmodel0->fieldIndex("vch_name"), Qt::Horizontal, QObject::tr("商品名"));
    downmodel0->setHeaderData(downmodel0->fieldIndex("vch_no"), Qt::Horizontal, QObject::tr("商品代码"));
    downmodel0->setHeaderData(downmodel0->fieldIndex("num_price"), Qt::Horizontal, QObject::tr("单价(/kg)"));

    downmodel0->setHeaderData(downmodel0->fieldIndex("vch_weighttype"), Qt::Horizontal, QObject::tr("称重方式"));
    downmodel0->setHeaderData(downmodel0->fieldIndex("vch_validdate"), Qt::Horizontal, QObject::tr("有效日期"));
    downmodel0->setHeaderData(downmodel0->fieldIndex("vch_flag"), Qt::Horizontal, QObject::tr("标识号"));

    ui->tableView->setModel(downmodel0);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectItems);
    ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
    ui->tableView->setItemDelegate(new barscale_main_delegate(this, ui->tableView));

    downmodel=new QStandardItemModel(this);//下载用
    downmodel->setHorizontalHeaderLabels(QStringList()
                                         << QObject::tr("PLU编号") << QObject::tr("商品代码") << QObject::tr("单价") << QObject::tr("称重方式")
                                         << QObject::tr("特殊信息") + "1" << QObject::tr("特殊信息") + "2" << QObject::tr("特殊信息")+ "3" << QObject::tr("有效日期")
                                         << QObject::tr("店名号") << QObject::tr("部门号") << QObject::tr("数字代号") << QObject::tr("皮重")
                                         << QObject::tr("标签号") << QObject::tr("是否打折") << "1" + QObject::tr("时间段开始") << "1" + QObject::tr("时间段结束")

                                         << "1" + QObject::tr("时间段折扣")  << "2" + QObject::tr("时间段开始") << "2" + QObject::tr("时间段结束") << "2" + QObject::tr("时间段折扣")
                                         << "3" + QObject::tr("时间段开始") << "3" + QObject::tr("时间段结束") << "3" + QObject::tr("时间段折扣") << "4" + QObject::tr("时间段开始")
                                         << "4" + QObject::tr("时间段结束") << "4" + QObject::tr("时间段折扣") << QObject::tr("商品名") + "1" << QObject::tr("商品名") + "2"
                                         << QObject::tr("商品名3"));


    //    name(_name),//商品名称
    //            barcode(_barcode),//生鲜号
    //            price(_price),//单价
    //            validday(_validday),//有效时间
    //            packageweight(_packageweight),//包装类型

    //            Tareweight(0),//皮重默认是0
    //            plucode(_plucode),//货号
    //            flag(_flag),//部门号
    //            barcodetype(_barcodetype)//条码编号


    downdialog=new barscale_downdialog(this);
    connect(downdialog->finishButton(),SIGNAL(clicked()),SLOT(downdialog_exit()));

    ui->comboBox_barcode_scaletypes->addItems(CommonHeader::creat_barcode_scaletypes());
    ui->comboBox_barcode_scaletypes->setCurrentIndex(ui->comboBox_barcode_scaletypes->findText(lds::sysconf->value("barscale/scaletype").toString()));
    update_barcodetype(ui->comboBox_barcode_scaletypes->currentText());
    ui->comboBox_barcode_scalebarcodetype->setCurrentIndex(ui->comboBox_barcode_scalebarcodetype->findText(lds::sysconf->value("barscale/scalebartype").toString()));
    ui->lineEdit_barcode_scaleflag->setText(lds::sysconf->value("barscale/scaleflag", 21).toString());
    ui->spinBox_precision_amount->setValue(lds::sysconf->value("barscale/precision_amount", 2).toInt());
    ui->spinBox_precision_weight->setValue(lds::sysconf->value("barscale/precision_weight", 3).toInt());
    ui->spinBox_precision_price->setValue(lds::sysconf->value("barscale/precision_price", 2).toInt());
    ui->comboBox_unitno->setCurrentIndex(lds::sysconf->value("barscale/scaleunitno").toInt());

    update_scaleflag(ui->lineEdit_barcode_scaleflag->text());
    switch_mode = 1;
    on_pushButton_switch_mode_clicked();
    ui->frame_other->hide();

    //! connect
    connect(ui->lineEdit_barcode_scaleflag,SIGNAL(textChanged(QString)),this,SLOT(update_scaleflag(QString)));
    connect(ui->comboBox_barcode_scaletypes,SIGNAL(currentIndexChanged(QString)),this,SLOT(update_barcodetype(QString)));
    connect(ui->pushButton_add,SIGNAL(clicked()),this,SLOT(togoodadd()));
    connect(ui->toolButton_other,SIGNAL(clicked()),this,SLOT(toshowother()));
    connect(ui->pushButton_plu_keep_code, SIGNAL(clicked()),this,SLOT(to_plu_keep_code()));
}

barscale_main_dialog::~barscale_main_dialog()
{
    delete ui;
    delete downmodel0;
}

void barscale_main_dialog::set_shop_name(const QString &shop_name)
{
    ui->lineEdit_shopname->setText(shop_name);
}

QString barscale_main_dialog::shop_name()
{
    return ui->lineEdit_shopname->text();
}

void barscale_main_dialog::keep(QSqlQuery &queryouter, const QString sqlwherebarcode)
{

    //! init_start_transaction
    QSqlDatabase::database("barscale_main_dialog").transaction();
    QSqlQuery query(QSqlDatabase::database("barscale_main_dialog"));
    QSqlQuery querygo(QSqlDatabase::database("barscale_main_dialog"));
    query.exec("select vch_no from barscale_main_dialog_table");

    QString vch_no;
    while(query.next()) {
        vch_no = query.record().value("vch_no").toString();

        queryouter.exec(sqlwherebarcode.arg(vch_no));
        if(queryouter.next()) {//存在，同步
            if(!querygo.exec(QString("update barscale_main_dialog_table set vch_name='%1', num_price='%2',  vch_validdate = '%4' where vch_no='%3' ")
                             .arg(queryouter.record().value(0).toString())
                             .arg(queryouter.record().value(1).toString())
                             .arg(vch_no)
                             .arg(queryouter.record().value(2).toString())
                             )) {
                goto failure;
            }
        } else {//不存在，删除
            if(!querygo.exec(QString("delete from barscale_main_dialog_table where vch_no='%1' ").arg(vch_no))) {
                goto failure;
            }
        }
    }

    goto success;
success:
    QSqlDatabase::database("barscale_main_dialog").commit();
    downmodel0->select();
    return;
failure:
    QSqlDatabase::database("barscale_main_dialog").rollback();
    uinormal_message::warning(this, QObject::tr("条码秤"), QObject::tr("条码秤错误") + "\n" + querygo.lastError().text());
    return;
}

void barscale_main_dialog::update_downmodel()
{
    downmodel->removeRows(0, downmodel->rowCount());
    for(int row=0,rowcount=downmodel0->rowCount();row<rowcount;row++) {
        QList<QStandardItem *> items;
        creatItem(items, QStringList()
                  << downmodel0->record(row).value("vch_plu").toString()
                  << downmodel0->record(row).value("vch_no").toString()
                  << QString::number((downmodel0->record(row).value("num_price").toFloat() * 100))
                  << downmodel0->record(row).value("vch_weighttype").toString());
        creatItem(items, QStringList() << QString("00") << QString("00") << QString("00")
                  << downmodel0->record(row).value("vch_validdate").toString());
        creatItem(items, QStringList()
                  << downmodel0->record(row).value("vch_flag").toString()
                  << QString("00") << QString("0000000000000")  << QString("000000"));
        creatItem(items, QStringList() << QString("00") << QString("00") << QString("00")  << QString("00"));

        creatItem(items, QStringList() << QString("00") << QString("00") << QString("00")  << QString("00"));
        creatItem(items, QStringList() << QString("00") << QString("00") << QString("00")  << QString("00"));
        creatItem(items, QStringList() << QString("00") << QString("00")
                  << downmodel0->record(row).value("vch_name").toString()
                  << ui->lineEdit_shopname->text());

        creatItem(items, QStringList() << QString(QObject::tr("测试三")));
        downmodel->appendRow(items);
    }

    QString scaletype = ui->comboBox_barcode_scaletypes->currentText();
    switch(CommonHeader::creat_barcode_scaletypes().indexOf(scaletype)) {
    case 0://上海大华 TM-15H
        break;
    case 1://凯士 CL5200
        break;
    case 2://友声
        break;
    case 3://厦门顶尖LS2CX
        break;
    case 4://凯丰条码秤
        break;
    case 5://容大RLS1000
        ElectronicScale_RONGTA::unitcode = ElectronicScale_RONGTA::UnitType().key(ui->comboBox_unitno->currentText(), ui->comboBox_unitno->currentText().toInt());
        ElectronicScale_RONGTA::barcodetype = ElectronicScale_RONGTA::BarType().key(ui->comboBox_barcode_scalebarcodetype->currentText());
        break;
    }
}

void barscale_main_dialog::creatItem(QList<QStandardItem *> &items, const QStringList &list)
{
    foreach(QString s, list)
        items <<new QStandardItem(s);
}

bool barscale_main_dialog::brefore_getscaletypes()
{
    labelprint_goodinfo_add_dialog dialog(this);
    dialog.setWindowTitle(QObject::tr("选择条码秤"));
    dialog.ui->label->setText(QObject::tr("如果没有找到想要条码秤，请到上一窗口修改秤的型号"));
    dialog.model->setHorizontalHeaderLabels(QStringList() << QObject::tr("秤号") << QObject::tr("秤名") << QObject::tr("地址/端口") << QObject::tr("状态"));
    QByteArray ba = lds::sysconf->value("barscale/netinfo").toByteArray();
    QDataStream out(&ba, QIODevice::ReadOnly);
    while(!out.atEnd()) {
        QList<QStandardItem *> items;
        QString p1, p2, p3, p4;
        out >> p1 >> p2 >> p3 >> p4;
        items <<new QStandardItem(p1);
        items <<new QStandardItem(p2);
        items <<new QStandardItem(p3);
        items <<new QStandardItem(p4);
        if(p4==QObject::tr("使用")
                && p2==ui->comboBox_barcode_scaletypes->currentText())
            dialog.model->appendRow(items);
    }

    if(QDialog::Accepted!=dialog.exec()) {
        return false;
    }
    //2
    update_downmodel();
    //2.5
    downdialog->model->removeRows(0, downdialog->model->rowCount());
    QModelIndexList indexlist=dialog.ui->tableView->selectionModel()->selectedIndexes();
    QSet<int> row_set;
    foreach(QModelIndex index, indexlist)row_set.insert(index.row());
    foreach(int row, row_set) {
        downdialog->model->appendRow(QList<QStandardItem*>()
                                     <<new QStandardItem(dialog.model->item(row, 0)->text())
                                     <<new QStandardItem(dialog.model->item(row, 2)->text().split(":").value(0))
                                     <<new QStandardItem(dialog.model->item(row, 2)->text().split(":").value(1))
                                     <<new QStandardItem(QObject::tr("未连接")) //状态
                                     <<new QStandardItem("0") //计数
                                     <<new QStandardItem("0/1") //百分比
                                     );
    }
    if(downdialog->model->rowCount()<=0) {
        return false;
    }
    downdialog->ui->timeEdit->setTime(QTime(0,0,0));
    downdialog->show();
    return true;
    //    downdialog->showUpdate();
}

void barscale_main_dialog::goodsadd(const QList<BARSCAEL_ITEMS> items)
{
    ///重载步骤：
    /// 1调用其他窗口，生成对应的itemlist，
    /// 2调用goodsadd(itemslist);
    //1
    QSqlQuery querygo(QSqlDatabase::database("barscale_main_dialog"));
    querygo.exec("select max(cast(vch_plu as decimal)) from barscale_main_dialog_table");
    querygo.next();
    qlonglong row;
    qlonglong plu = querygo.record().value(0).toLongLong();
    foreach(BARSCAEL_ITEMS item, items) {
        downmodel0->insertRow(row=downmodel0->rowCount());
        downmodel0->setData(downmodel0->index(row, BARSCALE_PLU), ++plu);
        downmodel0->setData(downmodel0->index(row, BARSCALE_NAME), item.dishname);
        downmodel0->setData(downmodel0->index(row, BARSCALE_CODE), item.dishno);
        downmodel0->setData(downmodel0->index(row, BARSCALE_PRICE), item.price);
        downmodel0->setData(downmodel0->index(row, BARSCALE_VALIDDATE), item.validdate);
        downmodel0->setData(downmodel0->index(row, BARSCALE_FLAG), ui->lineEdit_barcode_scaleflag->text());
    }
    downmodel0->submitAll();
}

void barscale_main_dialog::on_pushButton_clicked()
{
    if(!brefore_getscaletypes()) return;
    QFuture<void> future = QtConcurrent::run(ScaleDownInfo,
                                             downdialog,
                                             QStringList()<<"down"<<lds::sysconf->value("barscale/scaletype").toString(),
                                             &down_isbreak,
                                             downmodel,
                                             downdialog->model);
}

void barscale_main_dialog::on_pushButton_2_clicked()
{
    if(!brefore_getscaletypes()) return;
    QFuture<void> future = QtConcurrent::run(ScaleDownInfo,
                                             downdialog,
                                             QStringList()<<"clear"<<lds::sysconf->value("barscale/scaletype").toString(),
                                             &down_isbreak,
                                             downmodel,
                                             downdialog->model);
    //    future.waitForFinished();
}

void barscale_main_dialog::on_pushButton_3_clicked()
{
    if(!brefore_getscaletypes()) return;
    QFuture<void> future = QtConcurrent::run(ScaleDownInfo,
                                             downdialog,
                                             QStringList()<<"setlabel"<<lds::sysconf->value("barscale/scaletype").toString(),
                                             &down_isbreak,
                                             downmodel,
                                             downdialog->model);
    //    future.waitForFinished();
}

void barscale_main_dialog::on_pushButton_4_clicked()
{
    //保存配置文件避免第一次进入时网络管理时,无法选中秤类型的情况
    save_barscale();
    //外部输入
    barscale_net_set_dialog dialog(this);
    dialog.exec();
}

void barscale_main_dialog::on_pushButton_5_clicked()
{
    //1
    QSqlQuery querygo(QSqlDatabase::database("barscale_main_dialog"));
    QSet<QString> pluset;
    for(int row = 0, rowcount = downmodel0->rowCount(); row < rowcount ;row ++) {
        pluset.insert(downmodel0->data(downmodel0->index(row, 0)).toString());
    }
    //2
    if(pluset.count() != downmodel0->rowCount()) {
        int row, rowsub,rowcount;
        for(row = 0,rowcount = downmodel0->rowCount(); row < rowcount ;row++) {
            for(rowsub = row+1; rowsub < rowcount; rowsub++) {
                if(downmodel0->data(downmodel0->index(row, 0)).toString()
                       == downmodel0->data(downmodel0->index(rowsub, 0)).toString()) {
                    if(0 == uinormal_message::warning(this, QObject::tr("提示"),  QObject::tr("%1，%2行，PLU编号不能重复").arg(row+1).arg(rowsub+1), QObject::tr("返回"),QObject::tr("继续退出"))) {
                        return;
                    }
                    goto end;
                }
            }
        }
    }
    //3
    if(!downmodel0->submitAll()) {
        if(0 == uinormal_message::warning(this, QObject::tr("提示"), downmodel0->lastError().text(),QObject::tr("强制修改"), QObject::tr("继续退出"))) {
            QSqlDatabase::database("barscale_main_dialog").transaction();
            querygo.exec("delete from barscale_main_dialog_table");
            for(int row = 0, rowcount = downmodel0->rowCount(); row < rowcount ;row ++) {
                querygo.prepare("insert into barscale_main_dialog_table(vch_plu, vch_name, vch_no,num_price,  vch_weighttype, vch_validdate,vch_flag  ) values(?,?,?,?,?, ?,?)");
                querygo.addBindValue(downmodel0->data(downmodel0->index(row, BARSCALE_PLU)));
                querygo.addBindValue(downmodel0->data(downmodel0->index(row, BARSCALE_NAME)));
                querygo.addBindValue(downmodel0->data(downmodel0->index(row, BARSCALE_CODE)));
                querygo.addBindValue(downmodel0->data(downmodel0->index(row, BARSCALE_PRICE)));
                querygo.addBindValue(downmodel0->data(downmodel0->index(row, BARSCALE_WEIGHTTYPE)));

                querygo.addBindValue(downmodel0->data(downmodel0->index(row, BARSCALE_VALIDDATE)));
                querygo.addBindValue(downmodel0->data(downmodel0->index(row, BARSCALE_FLAG)));

                if(querygo.exec()) continue;

                QSqlDatabase::database("barscale_main_dialog").rollback();
                if(0 == uinormal_message::warning(this, QObject::tr("提示"),querygo.lastError().text(),QObject::tr("返回"), QObject::tr("继续退出"))) {
                    return;
                }
                goto end;
            }
            QSqlDatabase::database("barscale_main_dialog").commit();
            uinormal_message::warning(this, QObject::tr("提示"),QObject::tr("保存成功"));
        }
    }
    //4
end:
    save_barscale();
    this->reject();
}

void barscale_main_dialog::downdialog_exit()
{
    if(qobject_cast<QPushButton *>(downdialog->finishButton())->text() == QObject::tr("完成")) {
        downdialog->accept();
    } else {
        down_isbreak =  true;
    }
}

void barscale_main_dialog::update_barcodetype(const QString scaletype)
{
    ui->comboBox_barcode_scalebarcodetype->clear();
    ui->comboBox_barcode_scalebarcodetype->addItems(CommonHeader::creat_barcode_scalebarcodetype(scaletype));
    ui->comboBox_unitno->setVisible(false);
    ui->label_unitno->setVisible(false);
    ui->tableView->setColumnHidden(0, false);
    switch(CommonHeader::creat_barcode_scaletypes().indexOf(scaletype)) {
    case 0://上海大华 TM-15H
        break;
    case 1://凯士 CL5200
        break;
    case 2://友声
        break;
    case 3://厦门顶尖LS2CX
    {
    }
        break;
    case 4://凯丰条码秤
        break;
    case 5://容大RLS1000
        if(this->sender() != 0) {
            uinormal_message::information(this, QObject::tr("提示"), QObject::tr("容大条码秤PLU号为行号,最多252个;PLU号同商品代码一一对应"));
        }
        ui->label_unitno->setVisible(true);
        ui->comboBox_unitno->setVisible(true);
        ui->comboBox_unitno->clear();
        ui->comboBox_unitno->addItems(ElectronicScale_RONGTA::UnitType().values());
        ui->tableView->setColumnHidden(0, true);
        break;
    }
}

void barscale_main_dialog::save_barscale()
{
    lds::sysconf->setValue("barscale/scaletype", ui->comboBox_barcode_scaletypes->currentText());
    lds::sysconf->setValue("barscale/scalebartype", ui->comboBox_barcode_scalebarcodetype->currentText());
    lds::sysconf->setValue("barscale/scaleflag",ui->lineEdit_barcode_scaleflag->text());
    lds::sysconf->setValue("barscale/precision_amount",ui->spinBox_precision_amount->value());
    lds::sysconf->setValue("barscale/precision_weight",ui->spinBox_precision_weight->value());
    lds::sysconf->setValue("barscale/precision_price",ui->spinBox_precision_price->value());
    lds::sysconf->setValue("barscale/scaleunitno",ui->comboBox_unitno->currentIndex());
}

void barscale_main_dialog::update_scaleflag(const QString &flag)
{
    for(int row=0,rowcount=downmodel0->rowCount();row<rowcount;row++) {
        downmodel0->setData(downmodel0->index(row, 6), flag);
    }
}

barscale_main_delegate::barscale_main_delegate(barscale_main_dialog *parent, QObject *parent1):
    QItemDelegate(parent1),
    _parent(parent)
{
}

QWidget *barscale_main_delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int c = index.column();
    if(c == 4) {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(CommonHeader::creat_barcode_scale_weightype(lds::sysconf->value("barscale/scaletype").toString()).values());
        return editor;
    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void barscale_main_delegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int c = index.column();
    if(c == 4) {
        QString value = index.model()->data(index, Qt::EditRole).toString();

        QComboBox *spinBox = static_cast<QComboBox*>(editor);
        spinBox->setCurrentIndex(spinBox->findText(CommonHeader::creat_barcode_scale_weightype(lds::sysconf->value("barscale/scaletype").toString())[value.toInt()]));
    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

void barscale_main_delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int c = index.column();
    if(c == 4) {
        QComboBox *spinBox = static_cast<QComboBox*>(editor);
        model->setData(index, CommonHeader::creat_barcode_scale_weightype(lds::sysconf->value("barscale/scaletype").toString()).key(spinBox->currentText()), Qt::EditRole);
    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

void barscale_main_delegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int c = index.column();
    if(c == 4) {
        editor->setGeometry(option.rect);
    } else {
        QItemDelegate::updateEditorGeometry(editor, option, index);
    }
}

void barscale_main_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int  c = index.column();
    if(c != 4) {
        QItemDelegate::paint(painter,option,index);
    } else {
        int key = index.data(Qt::EditRole).toInt();
        QRect x = option.rect;
        int flag = Qt::AlignCenter | Qt::TextWordWrap;
        QColor textcolor= index.model()->data(index, Qt::TextColorRole).value<QColor>();
        if(textcolor.isValid())
            painter->setPen(textcolor);
        painter->drawText(x, flag, CommonHeader::creat_barcode_scale_weightype(lds::sysconf->value("barscale/scaletype").toString())[key]);//);
    }
}

void barscale_main_dialog::on_pushButton_export_clicked()
{
    downmodel0->submitAll();

    //获取路径
    QString filepath = QFileDialog::getSaveFileName(this, QObject::tr("导出文件"),lds::localdataPath +QDir::separator()+"barscale_main.csv");
    if(filepath.isEmpty()) {
        return;
    }
    QFile file(filepath);
    if(!file.open(QFile::WriteOnly)) {
        qDebug() << file.errorString();
        uinormal_message::warning(this, QObject::tr("条码秤"), file.errorString());
        return;
    }
    QString errtext;
    QString table_column_name;
    QString table_column_value;
    QString tablename=downmodel0->tableName();
    qlonglong columncount;
    qlonglong rowcount;
    QSqlQuery query(QSqlDatabase::database("barscale_main_dialog"));
    while(1) {///while

        //!--------------begin0--------------
        if(!query.exec("select * from "+tablename)) {
            errtext= query.lastError().text();
            break;
        }
        columncount=query.record().count();
        for(int c=0; c < columncount; c++) {
            table_column_name = query.record().fieldName(c);
            file.write(table_column_name.toLocal8Bit());
            //是否到结尾
            if(c==columncount-1) {
                file.write("\n");
            } else {
                file.write(",");
            }
        }
        //!--------------end0--------------

        //!--------------begin1--------------
        if(!query.exec("select count(0) from "+tablename)) {
            errtext= query.lastError().text();
            break;
        }
        query.next();
        rowcount=query.record().value(0).toLongLong();
        //!--------------end1--------------

        //!--------------begin2--------------
        if(!query.exec("select * from "+tablename)) {
            errtext= query.lastError().text();
            break;
        }
        for(int r=0;r<rowcount;r++) {
            query.next();
            for(int c=0; c < columncount; c++) {
                table_column_value = query.record().value(c).toString();

                if(query.record().value(c).type()==QVariant::DateTime
                        && table_column_value.isEmpty()) {
                    table_column_value="null";
                } else {
                    table_column_value.replace("'", "\\'");
                    table_column_value.replace("\"", "\\\"");
                    table_column_value=QString(" '%1' ").arg(table_column_value);
                    table_column_value=table_column_value.trimmed();
                }

                file.write(table_column_value.toLocal8Bit());
                //是否到结尾
                if(c==columncount-1) {
                    file.write("\n");
                } else {
                    file.write(",");
                }
            }
        }
        //!--------------end2--------------
        //!--------------begin3--------------
        //!--------------end3--------------
        //!--------------begin4--------------
        //!--------------end4--------------
        //!--------------begin5--------------
        //!--------------end5--------------
        break;
    }///while

    file.close();
    uinormal_message::warning(this, QObject::tr("条码秤"), QObject::tr("导出成功"));
}

void barscale_main_dialog::on_pushButton_import_clicked()//导入
{
    //获取路径
    QString filepath = QFileDialog::getOpenFileName(this, QObject::tr("导入文件"),lds::localdataPath);
    if(filepath.isEmpty())   return;
    QFile file(filepath);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << file.errorString();
        uinormal_message::warning(this, QObject::tr("条码秤"), file.errorString());
        return;
    }

    QSqlQuery query(QSqlDatabase::database("barscale_main_dialog"));
    QByteArray file_read;
    QStringList tablename_dawei;
    QString sql_language;
    QString tablename=downmodel0->tableName();
    QString errtext;
    //! <--transaction begin-->
    QSqlDatabase::database("barscale_main_dialog").transaction();
    while(1) {///while
        //!--------------begin0--------------
        if(!query.exec("delete from "+tablename)) {
            errtext=query.lastError().text();
            break;
        }
        //!--------------end0--------------
        //!--------------begin1--------------
        file_read=file.readLine();//先读一行/去除表头后继续
        sql_language=QString("insert into %1(%2)")
                .arg(tablename)
                .arg(QString(file_read))
                +"values(%1)"
                ;
        //开始插入数据
        while(!file.atEnd()) {
            file_read=file.readLine();
            if(!query.exec(sql_language.arg(QString(file_read)))) {
                errtext=query.lastError().text();
                break;
            }
        }
        //!--------------end1--------------
        //!--------------begin2--------------
        //!--------------end2--------------
        //!--------------begin3--------------
        //!--------------end3--------------
        break;
    }///while

    //! <--transaction begin-->
    QSqlDatabase::database("barscale_main_dialog").commit();
    file.close();
    downmodel0->select();
    uinormal_message::warning(this, QObject::tr("条码秤"), QObject::tr("导入成功"));
}

void barscale_main_dialog::togoodadd()
{
    //    //1
    //    QList<BARSCAEL_ITEMS> items;
    //    labelprint_goodinfo_add_dialog dialog(this);
    //    dialog.model->setHorizontalHeaderLabels(QStringList() << QObject::tr("商品名") << QObject::tr("商品代码") << QObject::tr("价格"));
    //    foreach(BARSCAEL_ITEMS item, items_list)
    //        dialog.model->appendRow(QList<QStandardItem*>()
    //                                <<new QStandardItem(item.dishname)
    //                                <<new QStandardItem(item.dishno)
    //                                <<new QStandardItem(item.price)
    //                                );
    //    //2
    //    if(QDialog::Accepted==dialog.exec()) {
    //        QModelIndexList indexlist=dialog.ui->tableView->selectionModel()->selectedIndexes();
    //        QSet<int> index_list;
    //        foreach(QModelIndex index, indexlist) {
    //            index_list.insert(index.row());
    //        }
    //        foreach(int index, index_list) {
    //            items.append(BARSCAEL_ITEMS(
    //                             ""/*plu为空*/,
    //                             dialog.model->item(index, 0)->text(),
    //                             dialog.model->item(index, 1)->text(),
    //                             dialog.model->item(index, 2)->text())
    //                         );
    //        }
    //    }
    //    goodsadd(items);
}

void barscale_main_dialog::on_pushButton_remove_clicked()
{
    //1
    QModelIndexList indexlist=ui->tableView->selectionModel()->selectedIndexes();
    QSet<int> index_list;
    foreach(QModelIndex index, indexlist) {
        index_list.insert(index.row());
    }
    //2
    foreach(int index, index_list) {
        downmodel0->removeRow(index);
        downmodel0->submit();
    }
    //3
    downmodel0->submitAll();
}

void barscale_main_dialog::on_pushButton_allcontrol_clicked()
{
    QModelIndexList indexlist=ui->tableView->selectionModel()->selectedIndexes();
    if(indexlist.isEmpty()) {
        uinormal_message::warning(this, QObject::tr("批量修改"), QObject::tr("没有数据被选中"));
        return;
    }
    barscale_num_input inputdialog(this);
    inputdialog.setWindowTitle(QObject::tr("批量修改"));
    if(QDialog::Accepted == inputdialog.exec()) {
        QString retvalue = inputdialog.retvalue();
        bool ok;
        int value = retvalue.toInt(&ok);
        if(!ok)  value = retvalue.toInt(&ok, 16);
        foreach(QModelIndex index, indexlist) {
            if(index.isValid()) {
                if(inputdialog.mode == 0) {
                    if(ok) {
                        downmodel0->setData(index, value);
                    } else {
                        downmodel0->setData(index, retvalue);
                    }
                } else {
                    downmodel0->setData(index, downmodel0->data(index).toFloat()*retvalue.toFloat());
                }
            }
        }
    }
}

void barscale_main_dialog::on_pushButton_clearsection_clicked()
{
    ui->tableView->selectionModel()->clear();
}

void barscale_main_dialog::on_pushButton_switch_mode_clicked()
{
    ui->tableView->selectionModel()->clear();
    if(switch_mode == 0) {
        ui->pushButton_switch_mode->setText(QObject::tr("批处理模式(点击切换)"));
        switch_mode = 1;
        ui->pushButton_allcontrol->setVisible(true);
        ui->pushButton_clearsection->setVisible(true);
        ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
        ui->tableView->setSelectionMode(QTableView::MultiSelection);
        ui->tableView->setSelectionBehavior(QTableView::SelectItems);
    } else
        if(switch_mode == 1) {
            ui->pushButton_switch_mode->setText(QObject::tr("正常模式(点击切换)"));
            switch_mode = 0;
            ui->pushButton_allcontrol->setVisible(false);
            ui->pushButton_clearsection->setVisible(false);
            ui->tableView->setEditTriggers(QTableView::AllEditTriggers);
            ui->tableView->setSelectionMode(QTableView::SingleSelection);
            ui->tableView->setSelectionBehavior(QTableView::SelectItems);
        }
}

void barscale_main_dialog::toshowother()
{
    if(ui->toolButton_other->arrowType() == Qt::DownArrow) {
        ui->toolButton_other->setArrowType(Qt::UpArrow);
        ui->frame_other->show();
    } else if(ui->toolButton_other->arrowType() == Qt::UpArrow) {
        ui->toolButton_other->setArrowType(Qt::DownArrow);
        ui->frame_other->hide();
    }
}

void barscale_main_dialog::to_plu_keep_code()
{
    for(int row = 0, rowcount = downmodel0->rowCount(); row < rowcount; row++) {
        downmodel0->setData(downmodel0->index(row, downmodel0->fieldIndex("vch_plu")), downmodel0->data(downmodel0->index(row, downmodel0->fieldIndex("vch_no"))));
    }

    //    "vch_plu char(20) primary key,"
    //    "vch_name char(200),"
    //    "vch_no char(20), "
}


Qt::ItemFlags barscale_main_dialog::barscaleModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QSqlTableModel::flags(index);
    if(data_is_editing) {
        return f;
    }

    if(index.column() == barscale_main_dialog::BARSCALE_CODE
            ||index.column() == barscale_main_dialog::BARSCALE_NAME
            ||index.column() == barscale_main_dialog::BARSCALE_PRICE
            ||index.column() == barscale_main_dialog::BARSCALE_VALIDDATE
            )
        return f &~(Qt::ItemIsEnabled);

    return f;
}

QVariant barscale_main_dialog::barscaleModel::data(const QModelIndex &idx, int role) const
{
    if(role == Qt::TextColorRole) {
        if(flags(idx) & Qt::ItemIsEnabled) {
            return QColor("green");
        }
        return QColor("black");
    }
    return QSqlTableModel::data(idx, role);
}

bool barscale_main_dialog::barscaleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    data_is_editing = true;
#endif
    bool f = QSqlTableModel::setData(index, value, role);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    data_is_editing = false;
#endif
    return f;
}

barscale_main_SOCKET::barscale_main_SOCKET():tcpSocket(new QTcpSocket)
{
}

barscale_main_SOCKET::~barscale_main_SOCKET()
{
    closeconnect();
    delete tcpSocket;
}

bool barscale_main_SOCKET::tryconnect(const QString &hostaddress, quint16 port, int msecs, bool *manual_break)
{
    //manual_break优先级最高
    if(manual_break && *manual_break) return false;

    //连接服务器
    int step = 1000;
    int loopcount = msecs / step;
    while(loopcount >= 1 && loopcount --) {
        if(manual_break && *manual_break) return false;
        tcpSocket->connectToHost(QHostAddress(hostaddress),port);
        if(tcpSocket->state() == QAbstractSocket::ConnectedState
                || tcpSocket->waitForConnected(step)) {
            return true;
        } else {
            qDebug() << tcpSocket->errorString();
        }
    }
    return false;
}

bool barscale_main_SOCKET::closeconnect(int msecs, bool *manual_break)
{
    //manual_break优先级最高
    if(manual_break && *manual_break) return false;
    //若未连接
    if(tcpSocket->state() == QAbstractSocket::UnconnectedState)
        return true;
    //断开服务器
    int step = 1000;
    int loopcount = msecs / step;
    while(loopcount >= 1 && loopcount --) {
        if(manual_break && *manual_break) return false;
        tcpSocket->disconnectFromHost();
        if (tcpSocket->state() == QAbstractSocket::UnconnectedState
                || tcpSocket->waitForDisconnected(step)) {
            return true;
        }
    }
    return false;
}

bool barscale_main_SOCKET::waitForReadyRead(int msecs, bool *manual_break)
{
    //manual_break优先级最高
    if(manual_break && *manual_break) return false;

    int step = 1000;
    int loopcount = msecs / step;
    while(loopcount >= 1 && loopcount --) {
        if(manual_break && *manual_break) return false;
        if(tcpSocket->waitForReadyRead(step)) {
            return true;
        }
    }
    qDebug() << tcpSocket->errorString();
    return false;
}

qint64 barscale_main_SOCKET::writedirect(const QByteArray &byte)
{
    return tcpSocket->write(byte);
}
void barscale_main_SOCKET::resetSocket(barscale_main_SOCKET::TYPE _type)
{
    delete tcpSocket;
    tcpSocket = 0;
    switch(_type) {
    case TCP:
        tcpSocket = new QTcpSocket;
        break;
    case UDP:
        tcpSocket = new QUdpSocket;
        break;
    }
}

