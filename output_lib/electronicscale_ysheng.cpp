#include "electronicscale_ysheng.h"
#include "printer_0.h"

QMap<int, QString> ElectronicScale_YSHENG::_wtype;
QStringList ElectronicScale_YSHENG::_btype;

ElectronicScale_YSHENG::ElectronicScale_YSHENG()
{
}

QByteArray ElectronicScale_YSHENG::clearPLU()
{
    QByteArray s0;
    s0 += _tx1("CLR") + _tx1("PLU") + "\n";
    s0 += _tx1("CLR") + _tx1("END") + "\n";
    s0 += "UPL\tTIM\t\r\n";

    return s0;
}

/*

"DWL	PLU
//  11	0	1	2	3	4	5	6	7	8	9
    PLU	1	10010		1	12000,5	0,0	0,0	0	0	0
//10	11	12	13	14	15	16	17	18	19	20
   0	0	0	9	38272591
//21	22	23	24	25	26	27	28	29	30	31
   0	0	0	0	1	0	0	0	0	10	0
//32	33	34	35	36	37	38	39	40	41	42
   0	0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0
//43	44	45	46	47	48	49	50	51	52	53
   0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0	0,0
//54	55	56	57	58	59	60	61
   0,0	0	0	0	0	0	0	0
END	PLU
"
*/

/*
    s0 += _t("DWL") + _t("PLU") + "\n";
    s0 += _t("PLU") +
            _t("1") + //新版 PLU编号 1～9 999 999
            _t_t("10001") + //货号
            _t("1") + //设置为重量：【0】【1】默认计重单位【2】默认计数单位【3】kg千克计重单位【4】g克计重单位【5】ton吨计重单位【6】ib英镑计重单位【7】500g市斤计重单位【8】100g百克计重单位
            _t("3,0");//设置单价 可输 或手动输入
    s0 += _t("0,0") + //设置成本
            _t("0,0") + //
            _t("0") +
            _t("0") +
            _t("0");
    s0 += _t("0") +
            _t("0") +
            _t("0") +
            _t("9") +
            _t_t(_t_t(_t_t(_t(printer_0::hanzi2quweima("水果")))));//设置商品名 5477
    s0 += _t("0") + _t("0") + _t("0") + _t("0") + _t("1");
    s0 += _t("0") + _t("0") + _t("0") + _t("0") + _t("10");
    s0 += _t("0") + _t("0") + _t("0,0") + _t("0,0") + _t("0");
    s0 += _t("127") + _t("0,0") + _t("0,0") + _t("0,0") + _t("0");
    s0 += _t("127") + _t("0,0") + _t("0,0") + _t("0,0") + _t("0");
    s0 += _t("127") + _t("0,0") + _t("0,0") + _t("0,0") + _t("0");
    s0 += _t("127") + _t("0,0") + _t("0,0") + _t("0,0") + _t("0");
    s0 += _t("0") + _t("0") + _t("0") + _t("0") + _t("0");
    s0 += _t("0") + "\n";
    s0 += _t("END") + _t("PLU");*/

QByteArray ElectronicScale_YSHENG::downloadPLUINFO(QStandardItemModel *model, int row)
{
    static bool isfirst = true;
    QByteArray s0;
    /*PLU编号*/QByteArray plu = QByteArray().append(model->item(row, 0)->text());
    /*条码*/QByteArray barcode =QByteArray().append( model->item(row, 1)->text());
    /*称重方式*/QByteArray weightype = QByteArray().append(model->item(row, 3)->text());
    /*单价*/QByteArray price = QByteArray().append(_price(model->item(row, 2)->text()));
    /*商品名称*/QByteArray barcodename = printer_0::trans_UTF82GBK(model->item(row, 26)->text());
    /*是否打印保质期*/QByteArray isprintbaozhiqi = "1";
    /*保质期*/QByteArray baozhiqi = QByteArray().append(model->item(row, 7)->text());

    /*标识位*/QByteArray flag = QByteArray().append(model->item(row, 8)->text());
    /*商店名称*/QByteArray shopname = printer_0::trans_UTF82GBK(model->item(row, 27)->text());

    //    商品信息
    s0 += _tx1("DWL") + _tx1("PLU") + "\n";
    s0 += _tx1("PLU") +_tx1(plu) + _tx2(barcode) +  _tx1(weightype) +  _tx1(price);//单价(元/kg)        -1 +2 + 4
    s0 += _tx1("0,0") + _tx1("0,0") + _tx1("0") + _tx1("0") + _tx1("0");//5 + 5
    s0 += _tx1("0") + _tx1("0") + _tx1("0") + _tx1("9") + _tx7(barcodename);//商品代码 10+4+7
    s0 += _tx1("0") + _tx1("0") + _tx1("0") + _tx1("0") + _tx1("1");//21 + 5
    s0 += _tx1(isprintbaozhiqi) + _tx1("0") + _tx1("0") + _tx1("0") + _tx1("10");//26 + 5
    s0 += _tx1(baozhiqi) + _tx1("0") + _tx1("0,0") + _tx1("0,0") + _tx1("0");//31 + 5
    s0 += _tx1("127") + _tx1("0,0") + _tx1("0,0") + _tx1("0,0") + _tx1("0");//36 + 5
    s0 += _tx1("127") + _tx1("0,0") + _tx1("0,0") + _tx1("0,0") + _tx1("0");//41 + 5
    s0 += _tx1("127") + _tx1("0,0") + _tx1("0,0") + _tx1("0,0") + _tx1("0");//46 + 5
    s0 += _tx1("127") + _tx1("0,0") + _tx1("0,0") + _tx1("0,0") + _tx1("0");//51 + 5
    s0 += _tx1("0") + _tx1("0") + _tx1("0") + _tx1("0") + _tx1("0");//56 + 5
    s0 += _tx1("0") + "\n";
    s0 += _tx1("END") + _tx1("PLU") + "\n";

    if(isfirst) {
        //一次
        //标志位
        s0 += _tx1("DWL") + _tx1("TMS") + "\n";
        s0 += _tx1("TMS") + _tx1("2") +_tx1(flag) + "\n";
        s0 += _tx1("END") + _tx1("TMS") + "\n";
        //商店名称
        s0 += _tx1("DWL") + _tx1("TMT") + "\n";
        s0 += _tx1("TMT") + _tx1("0") + _tx1(shopname) + "\n";
        s0 += _tx1("TMT") + _tx1("1") + _tx1("") + "\n";
        s0 += _tx1("END") + _tx1("TMT") + "\n";
        // isfirst = false;
    }

    s0 += "UPL\tTIM\t\r\n";
    return s0;
}

QByteArray ElectronicScale_YSHENG::_tx1(const QByteArray &data)
{
    return data + "\t";
}

QByteArray ElectronicScale_YSHENG::_tx2(const QByteArray &data)
{
    return data + "\t\t";
}

QByteArray ElectronicScale_YSHENG::_tx7(const QByteArray &data)
{
    return data + "\t\t\t\t\t\t\t";
}

QMap<int, QString> ElectronicScale_YSHENG::WeighType()
{
    if(_wtype.isEmpty()) {
        _wtype.insert(1, QObject::tr("计重(1)"));
        _wtype.insert(2, QObject::tr("计件(2)"));

    }
    return _wtype;
}

QStringList ElectronicScale_YSHENG::BarType()
{
    //B全局标志位 E货物编号 L销售总价
    if(_btype.isEmpty()) {
        _btype << QObject::tr("前缀(2)+货号(5)+金额(5)+校验(1)")  ;// ("BBEEEEELLLLLC");
        _btype << QObject::tr("前缀(2)+货号(5)+金额(5)+重量(5)+校验(1)");
    }
    return _btype;
}
bool ElectronicScale_YSHENG::barcode_scale_resolve(QString &barcode, QString &price, QString &weight, QString &amount, const QString &code, const QString &type)
{
    Q_UNUSED(price)
    bool ret = false;
    int index = BarType().indexOf(type);
    switch(index) {
    case 0:
        barcode = code.mid(2, 5);
        amount = code.mid(7, 5);
        ret = true;
        break;
    case 1:
        barcode = code.mid(2, 5);
        amount = code.mid(7, 5);
        weight = code.mid(12, 5);
        ret = true;
        break;
    }
    return ret;
}

QString ElectronicScale_YSHENG::barcode_default_port()
{
    return "";
}

QString ElectronicScale_YSHENG::_price(const QString &data)
{
    //1.3kg -> 130
    QString _data = QString().sprintf("%0.2f", data.toFloat() / 100);
    return   QString("%1,%2").arg(_data.remove(".")).arg(2);
}

QStringList ElectronicScale_YSHENG::scaleName()
{
    return QStringList() << QObject::tr("上海精涵友声 TM");
}
/*
DWL	TMT
TMT	0	南京商朝
TMT	1	江苏南京
END	TMT
*/

/*
DWL	PLU
PLU	1	10001		1	2,0	0,0	0,0	0	0	0	0	0	0	9	水果								0	0	0	0	1	0	0	0	0	10	0	0	0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0	0,0	0,0	0	0	0	0	0	0	0
PLU	2	10002		2	3,0	0,0	0,0	0	0	0	0	0	0	9	矿泉水								0	0	0	0	0	0	0	0	0	0	0	0	0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0	0,0	0,0	0	127	0,0	0,0	0,0	0	0	0	0	0	0	0
END	PLU
*/

/*
CLR PLU
CLR END
*/
