#include "public_short_cut_map.h"
#include "ui_public_short_cut_map.h"
#include "public_sql.h"
#include <QPainter>
#include <QKeyEvent>
#include <QtDebug>
#include "n_func.h"
#include <QSet>


//Qt::DisplayRole/*text*/);
//Qt::UserRole/*desc*/);
//Qt::UserRole + 1/*keytext*/);
//Qt::UserRole + 2/*keyvalue*/);

//text usr usr_1 usr_2
static QStandardItem * get_item_text_usr(const QString &text, QVariant k) {
    QStandardItem *item = new QStandardItem;
    item->setText(text);
    item->setData(k, Qt::UserRole);
    return item;
}

int public_short_cut_map::_row0 = QT_BEGIN;
int public_short_cut_map::_row1 = PAY_END;

public_short_cut_map::public_short_cut_map(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::public_short_cut_map)
{
    ui->setupUi(this);
    ui->tableView->setModel(&public_sql::OPER_KEY_MODEL);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setItemDelegate(new delegate(this));
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif

    //
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(toexit()));
    connect(ui->pushButton_default, SIGNAL(clicked()),this,SLOT(todefault()));

    ui->tableView->setFocus();
    ui->tableView->installEventFilter(this);
    ui->pushButton_cancel->installEventFilter(this);
    ui->pushButton_default->installEventFilter(this);
    ui->pushButton_ok->installEventFilter(this);
    this->setWindowTitle(QObject::tr("快捷键设置"));
}

public_short_cut_map::~public_short_cut_map()
{
    delete ui;
}

QString public_short_cut_map::get_desc_by_keyvalue(int keyvalue, int row0, int row1)
{
    QStandardItemModel *m = & public_sql::OPER_KEY_MODEL;
    //get
    for(int row = row0-1; row < row1; row++) {
        for(int column = 0; column < m->columnCount(); column ++) {
            if(m->data(m->index(row, column), Qt::UserRole + 2/*keyvalue*/).toInt() == keyvalue) {
                return m->data(m->index(row, column), Qt::UserRole).toString();
            }
        }
    }
    return "";
}

void public_short_cut_map::OPER_KEY_MODEL_INIT()
{
    //!
    if(public_sql::OPER_KEY_MODEL.rowCount() == 0) {
        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << new QStandardItem(QObject::tr("前台设置"))
                                             << new QStandardItem
                                             << new QStandardItem
                                             << new QStandardItem
                                             );
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 0)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 1)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 2)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 3)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("会员"), "HY")
                                             << get_item_text_usr(QObject::tr("修改"), "XG")
                                             << get_item_text_usr(QObject::tr("删除"), "SC")
                                             << get_item_text_usr(QObject::tr("挂单"), "GD")
                                             );

        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("切换后台"), "QHHT")
                                             << get_item_text_usr(QObject::tr("锁屏"), "SP")
                                             << get_item_text_usr(QObject::tr("结算"), "JS")
                                             << get_item_text_usr(QObject::tr("折扣"), "ZK")
                                             );

        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("数量")+"-", "SUB")
                                             << get_item_text_usr(QObject::tr("数量")+"+", "ADD")
                                             << get_item_text_usr(QObject::tr("替换"), "HC")
                                             << get_item_text_usr(QObject::tr("取重"), "QZ")
                                             );

        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("常用商品"), "CYC")
                                             << get_item_text_usr(QObject::tr("交班"), "JB")
                                             << get_item_text_usr(QObject::tr("整单取消"), "ZDQX")
                                             << get_item_text_usr(QObject::tr("赠送"), "ZS")
                                             );

        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("开钱箱"), "KQX")
                                             << get_item_text_usr(QObject::tr("会员充值"), "HYCZ")
                                             << get_item_text_usr(QObject::tr("会员取款"), "HYQK")
                                             << get_item_text_usr(QObject::tr("退货"), "TCTK")
                                             );
        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("修改密码"), "XGMM")
                                             );

        //

        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << new QStandardItem(QObject::tr("付款设置"))
                                             << new QStandardItem
                                             << new QStandardItem
                                             << new QStandardItem);
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 0)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 1)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 2)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.item(public_sql::OPER_KEY_MODEL.rowCount()-1, 3)->setBackground(QColor("#ff546b"));
        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("微信"), "WX")
                                             << get_item_text_usr(QObject::tr("支付宝"), "ZFB")
                                             << get_item_text_usr(QObject::tr("银行卡"), "YHK")
                                             << get_item_text_usr(QObject::tr("会员卡"), "HYK")
                                             );
        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("充值"), "CZ")
                                             << get_item_text_usr(QObject::tr("代金券"), "DJQ")
                                             << get_item_text_usr(QObject::tr("美团"), "MT")
                                             << get_item_text_usr(QObject::tr("大众点评"), "DZDP")
                                             );
        public_sql::OPER_KEY_MODEL.appendRow(QList<QStandardItem*>()
                                             << get_item_text_usr(QObject::tr("全场折扣"), "QCZK")
                                             << get_item_text_usr(QObject::tr("优惠"), "YH")
                                             << get_item_text_usr(QObject::tr("会员挂账"), "HYGZ")
                                             );
#ifdef QT_DEBUG
        QSet<QString> usr_set;
        int count = 0;
        for(int row = 0; row < public_sql::OPER_KEY_MODEL.rowCount(); row ++) {
            for(int column = 0; column < public_sql::OPER_KEY_MODEL.columnCount(); column++) {
                if(!public_sql::OPER_KEY_MODEL.data(public_sql::OPER_KEY_MODEL.index(row, column), Qt::UserRole).toString().isEmpty()) {
                    usr_set.insert(public_sql::OPER_KEY_MODEL.data(public_sql::OPER_KEY_MODEL.index(row, column), Qt::UserRole).toString());
                    count++;
                }
            }
        }
        if(usr_set.count() != count) {
            qDebug() << "快捷键有重复";
            QObject *o = 0;
            o->objectName();
        }
#endif
    }
    //~
    //load
    lds_query_hddpos  query;
    query.execSelect("select count(vch_parameter) from cey_sys_parameter where vch_parameter like 'public_short_cut_map_%' ");
    query.next();
    if(query.recordValue(0).toInt() == 0) {
        defaultData();
        saveData2();
    }
    for(int row = 0; row < public_sql::OPER_KEY_MODEL.rowCount(); row ++) {
        for(int column = 0; column < public_sql::OPER_KEY_MODEL.columnCount(); column++) {
            QString desc = public_sql::OPER_KEY_MODEL.data(public_sql::OPER_KEY_MODEL.index(row, column), Qt::UserRole).toString();
            if(!desc.isEmpty()) {
                QStringList key_text_value = n_func::f_get_sysparm("public_short_cut_map_"+desc).split(",");
                QString key = key_text_value.value(0);
                QString value = key_text_value.value(1, defaultData(key));

                public_sql::OPER_KEY_MODEL.setData(public_sql::OPER_KEY_MODEL.index(row, column), key, Qt::UserRole + 1);
                public_sql::OPER_KEY_MODEL.setData(public_sql::OPER_KEY_MODEL.index(row, column), value, Qt::UserRole + 2);
            }
        }
    }
}

void public_short_cut_map::showqt()
{
    for(int k  = PAY_BEGIN-1; k < PAY_END; k++)
        ui->tableView->hideRow(k);
    _row0 = QT_BEGIN;
    _row1 = QT_END;
}

void public_short_cut_map::showpay()
{
    for(int k = QT_BEGIN-1; k < QT_END; k++)
        ui->tableView->hideRow(k);
    _row0 = PAY_BEGIN;
    _row1 = PAY_END;
}

bool public_short_cut_map::keyPressQt(QWidget *qt, QKeyEvent *e)
{
    const int k = e->key();
    if(k == Qt::Key_Escape) {
        e->ignore();
        invokeMethod(qt, "toTuichu");
        return false;
    }
    if(k == Qt::Key_F1) {
        public_short_cut_map dialog(qt);
        dialog.showqt();
        dialog.exec();
        goto end;
    }
    if(k == Qt::Key_Plus) {//+
        invokeMethod(qt, "toadd");
        goto end;

    }
    if(k == Qt::Key_Minus) {//-
        invokeMethod(qt, "tosub");
        goto end;

    }
    if(k == Qt::Key_Asterisk) {//*
        invokeMethod(qt, "todetail");
        goto end;

    }
    if(k == Qt::Key_Slash) {// /
        invokeMethod(qt, "todetail");
        goto end;
    }
    if(k == Qt::Key_Up) {
        invokeMethod(qt, "todishup");
        goto end;
    }
    if(k == Qt::Key_Down) {
        invokeMethod(qt, "todishdown");
        goto end;
    }
    //
    shortCutFunctionQt(qt, get_desc_by_keyvalue(k, public_short_cut_map::QT_BEGIN, public_short_cut_map::QT_END));
end:
    return true;
}

void public_short_cut_map::shortCutFunctionQt(QObject *qt, const QString &desc)
{
    if(desc == "HY") {
        invokeMethod(qt, "toMRead");
    } else if(desc == "XG") {
        invokeMethod(qt, "todetail");
    } else if(desc == "SC") {
        invokeMethod(qt, "toquitdish");
    } else if(desc == "GD") {
        invokeMethod(qt, "toTakebill");
    }

    else if(desc == "QHHT") {
        invokeMethod(qt, "tosysswitchback");
    } else if(desc == "SP") {
        invokeMethod(qt, "tosyssuoping");
    } else if(desc == "JS") {
        invokeMethod(qt, "topaybill");
    } else if(desc == "ZK") {
        invokeMethod(qt, "todetail");
    }

    else if(desc == "SUB") {
        invokeMethod(qt, "tosub");
    } else if(desc == "ADD") {
        invokeMethod(qt, "toadd");
    } else if(desc == "HC") {
        invokeMethod(qt, "tochange");
    } else if(desc == "QZ") {
        invokeMethod(qt, "toTakeweight");
    }

    else if(desc == "CYC") {
        invokeMethod(qt, "tochangyongcai");
    } else if(desc == "JB") {
        invokeMethod(qt, "tosysjiaoban");
    } else if(desc == "ZDQX") {
        invokeMethod(qt, "toAllbillcancel");
    } else if(desc == "ZS") {
        invokeMethod(qt, "topresent");
    }

    else if(desc == "KQX") {
        invokeMethod(qt, "tosyskaiqianxiang");
    } else if(desc == "HYCZ") {
        invokeMethod(qt, "tosyschongzhi");
    } else if(desc == "HYQK") {
        invokeMethod(qt, "tosysqukuan");
    } else if(desc == "TCTK") {
        invokeMethod(qt, "toExitbill");
    } else if(desc == "XGMM") {
        invokeMethod(qt, "tosysxiugaimima");
    }
}

void public_short_cut_map::invokeMethod(const char *slot)
{
    invokeMethod(this->parent(), slot);
}

void public_short_cut_map::invokeMethod(QObject *o, const char *slot)
{
    QMetaObject::invokeMethod(o,
                              slot,
                              Qt::QueuedConnection
                              );
}

bool public_short_cut_map::eventFilter(QObject *o, QEvent *e)
{
    QModelIndex index = ui->tableView->currentIndex();
    //表格没有被选中
    if(!index.isValid()) goto end;

    if(QEvent::KeyPress == e->type()) {
        QKeyEvent *ke = ((QKeyEvent *)e);
        QString ktext = ke->text();
        const int kvalue = ke->key();
        //退出
        if(kvalue == Qt::Key_Escape) {
            toexit();
            e->ignore();
            return true;
        }
        if(kvalue == Qt::Key_Return
                || kvalue == Qt::Key_Enter ) {
            took();
            e->ignore();
            return true;
        }
        //数字不计入
        if(kvalue >= Qt::Key_0 && kvalue <= Qt::Key_9) {
            goto end;
        }
        //上下移动不计
        if(kvalue == Qt::Key_Left
                || kvalue == Qt::Key_Right
                || kvalue == Qt::Key_Up
                || kvalue == Qt::Key_Down
                || kvalue == Qt::Key_Tab
                ) {
            goto end;
        }

        //F键2-12
        switch(kvalue) {
        //        case Qt::Key_F1:
        case Qt::Key_F2:
        case Qt::Key_F3:
        case Qt::Key_F4:
        case Qt::Key_F5:
        case Qt::Key_F6:
        case Qt::Key_F7:
        case Qt::Key_F8:
        case Qt::Key_F9:
        case Qt::Key_F10:
        case Qt::Key_F11:
        case Qt::Key_F12:
            ktext = QString("F%1").arg(1+kvalue-Qt::Key_F1);
            break;
        }

        ktext = ktext.trimmed().toUpper();
        //没有简码描述
        if(!public_sql::OPER_KEY_MODEL.data(index, Qt::UserRole).toString().isEmpty()) {
            public_sql::OPER_KEY_MODEL.setData(index, ktext, Qt::UserRole + 1/*keytext*/);
            if(!ktext.isEmpty()) {
                public_sql::OPER_KEY_MODEL.setData(index, kvalue, Qt::UserRole + 2/*keyvalue*/);
            } else {
                public_sql::OPER_KEY_MODEL.setData(index, QVariant(), Qt::UserRole + 2/*keyvalue*/);
            }
        }
    }

end:
    return QDialog::eventFilter(o, e);
}

void public_short_cut_map::took()
{
    QModelIndexList indexs;
    QStandardItemModel *m = & public_sql::OPER_KEY_MODEL;
    //get
    for(int row = 0; row < m->rowCount(); row++) {
        if(ui->tableView->isRowHidden(row)) continue;//隐藏的不判断
        for(int column = 0; column < m->columnCount(); column ++) {
            QString keytext = m->data(m->index(row, column), Qt::UserRole + 1/*keytext*/).toString();
            if(!keytext.isEmpty()) {
                indexs << m->index(row, column);
            }
        }
    }

    //check
    foreach(const QModelIndex &index, indexs) {
        QString keytext = m->data(index, Qt::UserRole + 1/*keytext*/).toString();
        foreach(const QModelIndex &index0, indexs) {
            if(index0 == index) continue;
            QString keytext0 = m->data(index0, Qt::UserRole + 1/*keytext*/).toString();
            if(keytext == keytext0) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("按键冲突")+":" +m->data(index, Qt::DisplayRole).toString() +"," + m->data(index0, Qt::DisplayRole).toString());
                return;
            }
        }
    }

    //save
    lds_query_hddpos::tran_saction();
    for(int row = 0; row < m->rowCount(); row++) {
        for(int column = 0; column < m->columnCount(); column ++) {
            QString desc = m->data(m->index(row, column), Qt::UserRole).toString();
            if(!desc.isEmpty()) {
                n_func::f_set_sysparm("public_short_cut_map_"+desc, m->data(m->index(row, column), Qt::UserRole + 1).toString() + "," + m->data(m->index(row, column), Qt::UserRole + 2).toString());
            }
        }
    }
    lds_query_hddpos::com_mit();
    lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("保存成功"));
    this->accept();
}

void public_short_cut_map::toexit()
{
    //
    public_sql::OPER_KEY_MODEL_INIT();
    this->reject();
}

void public_short_cut_map::todefault()
{
    defaultData();
}

QString public_short_cut_map::defaultData(const QString &key)
{
    if(key.length() == 0 && key == "ADD") return "E,69"; else default2("ADD","E,69");
    if(key.length() == 0 && key == "CYC") return "N,78"; else default2("CYC", "N,78");
    if(key.length() == 0 && key == "CZ") return "R,82"; else default2("CZ", "R,82");
    if(key.length() == 0 && key == "DJQ") return "J,74"; else default2("DJQ", "J,74");
    if(key.length() == 0 && key == "DZDP") return "D,68"; else default2("DZDP", "D,68");
    if(key.length() == 0 && key == "GD") return "I,73"; else default2("GD", "I,73");
    if(key.length() == 0 && key == "HC") return "H,72"; else default2("HC", "H,72");
    if(key.length() == 0 && key == "HY") return "M,77"; else default2("HY", "M,77");
    if(key.length() == 0 && key == "HYCZ") return "R,82"; else default2("HYCZ", "R,82");
    if(key.length() == 0 && key == "HYQK") return "T,84"; else default2("HYQK", "T,84");
    if(key.length() == 0 && key == "HYGZ") return "G,71"; else default2("HYGZ", "G,71");
    if(key.length() == 0 && key == "HYK") return "M,77"; else default2("HYK", "M,77");
    if(key.length() == 0 && key == "JB") return "J,74"; else default2("JB", "J,74");
    if(key.length() == 0 && key == "JS") return "P,80"; else default2("JS", "P,80");
    if(key.length() == 0 && key == "KQX") return "S,83"; else default2("KQX", "S,83");
    if(key.length() == 0 && key == "MT") return "V,86"; else default2("MT", "V,86");
    if(key.length() == 0 && key == "QCZK") return "Z,90"; else default2("QCZK", "Z,90");
    if(key.length() == 0 && key == "QHHT") return "B,66"; else default2("QHHT", "B,66");
    if(key.length() == 0 && key == "QZ") return "W,87"; else default2("QZ", "W,87");
    if(key.length() == 0 && key == "SC") return "D,68"; else default2("SC", "D,68");
    if(key.length() == 0 && key == "SP") return "L,76"; else default2("SP", "L,76");
    if(key.length() == 0 && key == "SUB") return "Q,81"; else default2("SUB", "Q,81");
    if(key.length() == 0 && key == "TCTK") return "K,75"; else default2("TCTK", "K,75");
    if(key.length() == 0 && key == "WX") return "W,87"; else default2("WX", "W,87");
    if(key.length() == 0 && key == "XG") return "C,67"; else default2("XG", "C,67");
    if(key.length() == 0 && key == "XGMM") return "O,79"; else default2("XGMM", "O,79");
    if(key.length() == 0 && key == "YH") return "H,72"; else default2("YH", "H,72");
    if(key.length() == 0 && key == "YHK") return "Y,89"; else default2("YHK", "Y,89");
    if(key.length() == 0 && key == "ZDQX") return "X,88"; else default2("ZDQX", "X,88");
    if(key.length() == 0 && key == "ZFB") return "Q,81"; else default2("ZFB", "Q,81");
    if(key.length() == 0 && key == "ZK") return "Z,90"; else default2("ZK", "Z,90");
    if(key.length() == 0 && key == "ZS") return "G,71"; else default2("ZS", "G,71");

    return "";
}

void public_short_cut_map::default2(const QString &desc, const QString text_value)
{
    for(int row = _row0 - 1; row < _row1; row ++)    {
        for(int column = 0; column < public_sql::OPER_KEY_MODEL.columnCount(); column ++) {
            QModelIndex index = public_sql::OPER_KEY_MODEL.index(row, column);
            if(public_sql::OPER_KEY_MODEL.data(index, Qt::UserRole).toString() == desc) {
                public_sql::OPER_KEY_MODEL.setData(index, text_value.split(",").value(0), Qt::UserRole + 1);
                public_sql::OPER_KEY_MODEL.setData(index, text_value.split(",").value(1), Qt::UserRole + 2);
            }
        }
    }
}

void public_short_cut_map::saveData2()
{
    QStandardItemModel *m = & public_sql::OPER_KEY_MODEL;
    lds_query_hddpos::tran_saction();
    for(int row = 0; row < m->rowCount(); row++) {
        for(int column = 0; column < m->columnCount(); column ++) {
            QString desc = m->data(m->index(row, column), Qt::UserRole).toString();
            if(!desc.isEmpty()) {
                n_func::f_set_sysparm("public_short_cut_map_"+desc, m->data(m->index(row, column), Qt::UserRole + 1).toString() + "," + m->data(m->index(row, column), Qt::UserRole + 2).toString());
            }
        }
    }
    lds_query_hddpos::com_mit();
}

void public_short_cut_map::delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QString desc = index.data(Qt::DisplayRole).toString();
    QString key = index.data(Qt::UserRole + 1).toString();

    QVariant bgcolor = index.data(Qt::BackgroundRole);
    QBrush bgbrush = bgcolor.value<QBrush>();
    painter->fillRect(option.rect, bgbrush);

    QFont f = painter->font();
    f.setPointSize(15);
    painter->setFont(f);
    if(option.state&QStyle::State_Selected) {
        painter->setPen("white");
    }
    painter->drawText(option.rect,int(Qt::AlignVCenter | Qt::AlignLeft),  " " + desc);

    if(!(option.state&QStyle::State_Selected)) {
        painter->setPen("#ff546b");
    }
    painter->drawText(option.rect,int(Qt::AlignVCenter | Qt::AlignRight),  key+" ");

    painter->restore();
}
