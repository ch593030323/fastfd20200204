#include "w_scr_dish_dish_change.h"
#include "ui_w_scr_dish_dish_change.h"
#include <QSqlRecord>
#include "lds_messagebox.h"
#include "backheader.h"
#include <QKeyEvent>

w_scr_dish_dish_change::w_scr_dish_dish_change(const QString &ch_dishno_cur, float num_price_cur, const QString &ch_suitno, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_scr_dish_dish_change)
{
    ui->setupUi(this);
    lds_query_hddpos  query;
    _ch_suitno = ch_suitno;
    wtitle = QObject::tr("修改商品");
    _num_price_cur = num_price_cur;
    _ch_dishno_cur = ch_dishno_cur;
    suitreplace = false;
    //1
    querymodel = new QSqlQueryModel;
    ui->tableView->setModel(querymodel);
    //2
    query.execSelect(QString("select vch_dishname from cey_bt_dish where ch_dishno = '%1' ").arg(ch_dishno_cur));
    query.next();
    QString vch_dishname = query.recordValue("vch_dishname").toString();


    //4
    ui->frame_2->setup(QStringList() << QObject::tr("等价商品") << QObject::tr("所有商品")<< QObject::tr("可替换商品"), Qt::AlignRight);
    ui->label_desc->setText(QObject::tr("当前商品(%1)将不会显示").arg(vch_dishname+"/"+_ch_dishno_cur));
    query.execSelect(QString("select count(0) from cey_bt_dish_suit where ch_suitno = '%1' and ch_dishno = '%2'").arg(_ch_suitno).arg(_ch_dishno_cur));
    query.next();
    if(query.recordValue(0).toLongLong() == 0) {
        ui->frame_2->index_widget(QObject::tr("可替换商品"))->setVisible(false);
        ui->frame_2->index_widget(QObject::tr("等价商品"))->animateClick();
    } else {
        ui->frame_2->index_widget(QObject::tr("可替换商品"))->animateClick();
        ui->label->setText(QObject::tr("可替换商品"));
    }
    ui->lineEdit_cur->setText(QString("%1(%2)").arg(vch_dishname).arg(ch_dishno_cur));
    ui->lineEdit_cur->setEnabled(false);
    ui->lineEdit_destination->setEnabled(false);

    //3
    connect(ui->tableView,SIGNAL(selectChanged(int)),this,SLOT(totableclicked(int)));
    connect(ui->pushButton_cancel,SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_ok,SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->frame_2->index_widget(QObject::tr("等价商品")),SIGNAL(clicked()),this,SLOT(toselectequal()));
    connect(ui->frame_2->index_widget(QObject::tr("所有商品")),SIGNAL(clicked()),this,SLOT(toSelectAll()));
    connect(ui->frame_2->index_widget(QObject::tr("可替换商品")),SIGNAL(clicked()),this,SLOT(tosuitchange()));
    connect(ui->lineEdit_filter,SIGNAL(textChanged(QString)),this,SLOT(toselect()));
    connect(ui->pushButton_clearfilter,SIGNAL(clicked()),ui->lineEdit_filter,SLOT(clear()));
    ui->frame_2->index_widget(QObject::tr("等价商品"))->setShortcut(Qt::Key_E);
    ui->frame_2->index_widget(QObject::tr("等价商品"))->setShortcut(Qt::Key_A);
    ui->frame_2->index_widget(QObject::tr("等价商品"))->setShortcut(Qt::Key_R);
    ui->pushButton_ok->setShortcut(Qt::Key_Return);
    ui->pushButton_cancel->setShortcut(Qt::Key_C);
    ui->tableView->installEventFilter(this);
    ui->lineEdit_filter->installEventFilter(this);
}

w_scr_dish_dish_change::~w_scr_dish_dish_change()
{
    delete ui;
}

void w_scr_dish_dish_change::tocancel()
{
    this->reject();
}

void w_scr_dish_dish_change::took()
{
    int row = ui->tableView->currentIndex().row();
    if(row < 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有商品被选中"));
        return;
    }

    ch_dishno_new = querymodel->record(row).value("ch_dishno").toString();

    rate = 1.0;
    QStringList list = querymodel->record(row).value("rate").toString().split("/");
    if(list.value(1).toFloat() != 0)rate = list.value(0).toFloat()/list.value(1).toFloat();

    this->accept();
}

void w_scr_dish_dish_change::totableclicked(int row)
{
    ui->lineEdit_destination->setText(querymodel->record(row).value("vch_dishname").toString()
                                      + "("+ querymodel->record(row).value("ch_dishno").toString() + ")");
}

void w_scr_dish_dish_change::toselectequal()
{
    suitreplace = false;
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    ui->label->setText(b->text());
    _sql = (QString("select  ch_dishno, vch_dishname, num_price, '1.00/1.00'  as rate  from cey_bt_dish where "
                    " ch_stopflag <> 'Y' and num_price = '%1' and ch_dishno <> '%2' ")
            .arg(_num_price_cur)
            .arg(_ch_dishno_cur)
            + " and ch_dishno like '%1%'  ");
    toselect();
}

void w_scr_dish_dish_change::toSelectAll()
{
    suitreplace = false;
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    ui->label->setText(b->text());
    _sql = (QString("select  ch_dishno, vch_dishname, num_price, '1.00/1.00' as rate  from cey_bt_dish where "
                    " ch_stopflag <> 'Y' and ch_dishno <> '%1' ")
            .arg(_ch_dishno_cur)
            + " and ch_dishno like '%1%'  ");
    toselect();
}

void w_scr_dish_dish_change::tosuitchange()
{
    suitreplace = true;
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    ui->label->setText(b->text());
    _sql = (QString(
                "   select  b.ch_dishno, b.vch_dishname, b.num_price, concat(a.num_num_change,'/',c.num_num) as rate  from cey_bt_suit_change  a"
                "   left join cey_bt_dish b on a.ch_dishno = b.ch_dishno left join cey_bt_dish_suit c on a.int_flowid = c.int_flowid"
                "   where c.ch_suitno = '%1' and c.ch_dishno = '%2' and b.ch_stopflag <> 'Y' ")
            .arg(_ch_suitno).arg(_ch_dishno_cur)
            + " and a.ch_dishno like '%1%'  ");
    toselect();
}

void w_scr_dish_dish_change::toselect()
{
    ui->lineEdit_destination->clear();
    querymodel->setQuery(_sql.arg(ui->lineEdit_filter->text()));

    querymodel->setHeaderData(0, Qt::Horizontal, QObject::tr("编号"));
    querymodel->setHeaderData(1, Qt::Horizontal, QObject::tr("名称"));
    querymodel->setHeaderData(2, Qt::Horizontal, QObject::tr("价格"));
    querymodel->setHeaderData(3, Qt::Horizontal, QObject::tr("替换比例"));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnWidth(1, 200);
    if(!ui->tableView->currentIndex().isValid()) {
        ui->tableView->selectRow(0);
    } else {
        ui->tableView->restore();
    }
}

bool w_scr_dish_dish_change::eventFilter(QObject *o, QEvent *e)
{
    if(QEvent::KeyPress == e->type()) {
        QKeyEvent *ke = ((QKeyEvent *)e);
        const int kvalue = ke->key();
        if(kvalue == Qt::Key_Up) {
            ui->tableView->selectRow(qMax(ui->tableView->currentIndex().row() - 1, 0));
            e->ignore();
            return true;
        }
        if(kvalue == Qt::Key_Down) {
            ui->tableView->selectRow(qMin(ui->tableView->currentIndex().row() + 1, querymodel->rowCount()));
            e->ignore();
            return true;
        }

        //        if(kvalue == Qt::Key_Left
        //                || kvalue == Qt::Key_Right) {
        //            ui->pushButton_ok->setFocus();
        //            e->ignore();
        //            return true;
        //        }
    }
    return QDialog::eventFilter(o, e);
}
