#include "business_data_upload_isoft.h"
#include "qtsoap.h"
#include "lds_query_hddpos.h"
#include <QObject>
#include "lds.h"
#include <QDir>
#include "n_func.h"
#include "fexpandmain_model_sqltablemodel_data.h"
#include "public_sql.h"

business_data_upload_isoft::business_data_upload_isoft()
{

}

void business_data_upload_isoft::uploadBill()
{
    QString path = lds::localdataPath + "/business_data_upload_isoft";
    QFileInfoList fileinfos =QDir(path).entryInfoList(QStringList() << "*.txt");
    QEventLoop eloop;
    QTimer      timer;
    QtSoapHttpTransport http;
    QString errstring;
    timer.setSingleShot(true);
    QObject::connect(&timer,SIGNAL(timeout()),&eloop,SLOT(quit()));
    QObject::connect(&http, SIGNAL(responseReady()), &eloop,SLOT(quit()));
    QString upload_filepath;
    QString upload_filepath_error;
    QFile file;
    QByteArray upload_data;
    int total = 0;
    int success = 0;

#ifndef QT_NO_DEBUG
    http.setHost("59.46.112.18",9052);
#else
    http.setHost("10.1.13.2",8082);
#endif
    http.setAction("http://tempuri.org/PostSalesInfo");

    for(int k = 0; k < fileinfos.count(); k++) {
        QFileInfo info = fileinfos[k];
        upload_filepath = info.filePath();
        upload_filepath_error = info.path() + "/" +info.baseName()+"_error.txt";
        if(info.baseName().endsWith("_error"))
            continue;

        file.setFileName(upload_filepath);
        if(!file.open(QFile::ReadOnly)) {
            qDebug() << file.errorString();
        }
        upload_data = file.readAll();
        file.close();

#ifndef QT_NO_DEBUG
        http.submitRequest(upload_data,"/Service/POSServerWebService2.asmx");
#else
        http.submitRequest(upload_data,"/Service/POSServerWebService.asmx");
#endif
        total++;
        QMetaObject::invokeMethod(public_sql::save_login_object(),
                                  "signal_isoft_upload_polling_upload",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, info.fileName()+QString("    %1/%2").arg(k).arg(fileinfos.count()))
                                  );

        timer.start(10*1000);
        eloop.exec(QEventLoop::ExcludeUserInputEvents);
        if(timer.isActive())
        {
            const QtSoapMessage &message = http.getResponse();

            if (!message.isFault())
            {
                QString strXML = message.toXmlString();
                QDomDocument doc;
                doc.setContent(strXML);

                // 接在在线状态
                QDomNodeList statusList = doc.elementsByTagName("Status");
                if(statusList.count() > 0)
                {
                    QString Message;
                    QDomNodeList messageList = doc.elementsByTagName("Message");
                    if(messageList.count() > 0)
                        Message = messageList.at(0).toElement().text();
                    if("SUCCESS" == statusList.at(0).toElement().text()
                            || Message.contains("Sales record exists"))
                    {
                        success ++;
                        QFile::remove(upload_filepath);
                        QFile::remove(upload_filepath_error);
                        continue;
                    }
                    else
                    {
                        QDomNodeList messageList = doc.elementsByTagName("Message");
                        if(messageList.count() > 0)
                        {
                            //失败信息记录
                            errstring = (info.baseName() + ":" + messageList.at(0).toElement().text());
                        }
                    }
                }
                else{
                    errstring = QObject::tr("解析失败");
                }
            }
            else
            {
                errstring ="";
                QString strXML = message.toXmlString();
                QDomDocument doc;
                doc.setContent(strXML);
                QDomNodeList faultstrings = doc.elementsByTagName("faultstring");
                if(!faultstrings.isEmpty()) {
                    errstring += faultstrings.at(0).toElement().text();
                }
                errstring += QObject::tr("上传失败");
            }
        }
        else
        {
            errstring = QObject::tr("超时");
        }

        file.setFileName(upload_filepath_error);
        if(!file.open(QFile::WriteOnly)) {
            qDebug() << file.errorString();
        }
        file.write(errstring.toLocal8Bit());
        file.close();
    }

    qDebug() << __LINE__ << "finished";
    QMetaObject::invokeMethod(public_sql::save_login_object(),
                              "signal_isoft_upload_polling_finished",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QString("SUCCESS/TOTAL: %1/%2").arg(success).arg(total))
                              );
}

void business_data_upload_isoft::genrateBill(const QString &ch_payno)
{

    QString PropertyId = "1";
    QString TenantId = "185";
    QString ContractId = "184";
    QString PosId = "001";
    lds_query_hddpos  query;
    QString errstring;
    QString ch_billno;
    QDateTime dt_operdate;

    PropertyId = n_func::f_get_sysparm("isoft_PropertyId");
    TenantId = n_func::f_get_sysparm("isoft_TenantId");
    ContractId = n_func::f_get_sysparm("isoft_ContractId");
    PosId = n_func::f_get_sysparm("isoft_PosId");



    query.execSelect(QString("select   ch_billno, ch_payno, dt_operdate from cey_u_checkout_master where ch_payno = '%1';").arg(ch_payno));
    query.next();
    ch_billno = query.recordValue("ch_billno").toString();
    dt_operdate = query.recordValue("dt_operdate").toDateTime();

    fexpandmain_model_sqltablemodel_data tablemodel;
    tablemodel.setTable("cey_u_orderdish");
    tablemodel.select(ch_billno);


    //    query.execSelect(QString("select   num_cost, dt_operdate, num_present, num_discount from cey_u_checkout_master where ch_payno = '%1';").arg(ch_payno));
    //    if(!query.next()) {
    //        qDebug() << "business_data_upload_isoft:no business data";
    //        return;
    //    }
    
    //    float num_cost = query.recordValue("num_cost").toFloat();
    //    float num_present = query.recordValue("num_present").toFloat();
    //    float num_discount = query.recordValue("num_discount").toFloat();
    //sales
    QtSoapStruct *sales = new QtSoapStruct(QtSoapQName("sales", ""));

    sales->insert(new QtSoapSimpleType(QtSoapQName("PropertyId",""),PropertyId));
    sales->insert(new QtSoapSimpleType(QtSoapQName("TenantId",""),TenantId));
    sales->insert(new QtSoapSimpleType(QtSoapQName("ContractId",""), ContractId));
    sales->insert(new QtSoapSimpleType(QtSoapQName("PosId",""), PosId));
    sales->insert(new QtSoapSimpleType(QtSoapQName("SaleNo",""), ch_payno));

    sales->insert(new QtSoapSimpleType(QtSoapQName("SaleDate",""), dt_operdate.toString("yyyy-MM-ddThh:mm:ss")));
    sales->insert(new QtSoapSimpleType(QtSoapQName("DiscAmt",""), QString::number(tablemodel.getKeepdata().present + tablemodel.getKeepdata().discount)));
    sales->insert(new QtSoapSimpleType(QtSoapQName("Amount",""), QString::number(tablemodel.getKeepdata().dish_total())));
    sales->insert(new QtSoapSimpleType(QtSoapQName("TaxAmt",""), 0));

    //items
    int    line = 1;
    QtSoapStruct *Items = new QtSoapStruct(QtSoapQName("Items", ""));
    sales->insert(Items);
    for(int row = 0; row < tablemodel.rowCount(); row ++) {
        fexpandmain_keepData kdata = tablemodel.getKeepdata(row);

        QtSoapStruct *SalesDetailModel = new QtSoapStruct(QtSoapQName("SalesDetailModel", ""));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("PropertyId",""),PropertyId));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("TenantId",""),TenantId));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("ContractId",""), ContractId));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("PosId",""), PosId));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("SaleNo",""), ch_payno));

        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("Lines",""),line++));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("ItemName",""), tablemodel.get_vch_dishname_by_map(tablemodel.model_data(row, "ch_dishno").toString())));// query.recordValue("vch_dishname").toString()));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("Qty",""), kdata.num));// tablemodel.model_data(row, "num_num - num_back").toString()));//QString::number(num_dish)));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("Price",""), tablemodel.model_data(row, "num_price").toString()));//QString::number(num_price)));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("DiscAmt",""), QString::number(kdata.discount + kdata.present)));//QString::number(num_discount + num_present)));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("Amount",""),QString::number(kdata.dish_total())));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("TaxAmt",""),QString::number(0)));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("Type",""),1));
        SalesDetailModel->insert(new QtSoapSimpleType(QtSoapQName("Flag",""),1));

        Items->insert(SalesDetailModel);
    }

    //payments
    line = 1;
    QtSoapStruct *Payments = new QtSoapStruct(QtSoapQName("Payments", ""));
    sales->insert(Payments);
    QtSoapStruct *SalesPayModel = new QtSoapStruct(QtSoapQName("SalesPayModel", ""));
    QString PayCode = "C";
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("PropertyId",""),PropertyId));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("TenantId",""),TenantId));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("ContractId",""), ContractId));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("PosId",""), PosId));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("SaleNo",""), ch_payno));

    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("Lines",""),line++));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("PayCode",""),PayCode));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("PayAmt",""), QString::number(tablemodel.getKeepdata().dish_total())));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("Flag",""),0));
    SalesPayModel->insert(new QtSoapSimpleType(QtSoapQName("OtherAmt",""),0));
    Payments->insert(SalesPayModel);

    //upload data
    QtSoapMessage request;
    QtSoapStruct *soapstruct = request.setMethod("PostSalesInfo","http://tempuri.org/");
    soapstruct->insert(sales);

    QByteArray upload_data = request.toXmlString().toLocal8Bit();
    QString path = lds::localdataPath + "/business_data_upload_isoft";
    if(!QDir(path).exists()) {
        QDir().mkpath(path);
    }
    QString upload_filepath = path+"/"+ch_payno+".txt";
    QFile file(upload_filepath);
    if(!file.open(QFile::WriteOnly)) {
        errstring = "file write error" + file.errorString();
        return;
    }
    file.write(upload_data);
    file.close();
}


void business_data_upload_isoft_thread::run()
{

    business_data_upload_isoft_thread_event event;
    execEvent(&event);
}

void business_data_upload_isoft_thread_event::toExec()
{
    business_data_upload_isoft::uploadBill();
}
