#include "qt_xiangshan_setting.h"
#include "ui_qt_xiangshan_setting.h"
#include "lds.h"
#include <QFileInfoList>
#include <QDir>

qt_XIANGSHAN_setting::qt_XIANGSHAN_setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::qt_XIANGSHAN_setting)
{
    ui->setupUi(this);

    ui->comboBox_fuping_skin->addItems(QStringList() << "");/*
    colorPixmap(QColor(255, 255, 255), 30, 30), "skyblue");
    ui->comboBox_fuping_skin->addItem(colorPixmap(QColor(40, 40, 40), 30, 30), "obsidian");*/

    ui->comboBox_scale_bundrate->addItems(QStringList() << "19200" << "9600");
    ui->comboBox_scale_port->addItems(QStringList() << "/dev/ttyS1" << "/dev/ttyS2" << "/dev/ttyS3" << "/dev/ttyS4" << "/dev/ttyS5" << "/dev/ttyS6");

    model_list = new QStandardItemModel(this);
    model_list->setHorizontalHeaderLabels(QStringList() << QObject::tr("名称") << QObject::tr("大小") << QObject::tr("操作"));
    ui->tableView_pic_list->setModel(model_list);
    ui->tableView_pic_list->setEditTriggers(QTableView::DoubleClicked);

    //
    QMenu *menu = new QMenu(this);
    QWidgetAction *aw = new QWidgetAction(ui->pushButton_show_pic_list);
    aw->setDefaultWidget(ui->tableView_pic_list);
    menu->addAction(aw);
    ui->pushButton_show_pic_list->setMenu(menu);
    ui->tableView_pic_list->setFixedSize(700, 340);
    //
    QMovie *m = new QMovie(this);
    m->setFileName("://image/loading.gif");
    ui->label_loading->setScaledContents(true);
    ui->label_loading->setMovie(m);
    //
    dataOp(false);
    //
    QThread *t = new QThread(this);
    copy_file_ex.moveToThread(t);
    t->start();
    //
    ui->tableWidget->selectRow(0);
    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(toCancel()));
    connect(ui->pushButton_ccb_default, SIGNAL(clicked()), this, SLOT(toccbDefault()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(toOk()));
    connect(ui->pushButton_select_adv_path, SIGNAL(clicked()), this, SLOT(toSelectAdvPath()));
    connect(ui->pushButton_select_shop_info, SIGNAL(clicked()), this, SLOT(toSelectShopInfo()));
    connect(ui->tableWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(toSelectPage(QModelIndex)));
    connect(ui->pushButton_upan_refresh, SIGNAL(clicked()), this, SLOT(toUpanRefresh()));
    connect(ui->comboBox_upan, SIGNAL(currentIndexChanged(int)), this, SLOT(toUpanPicListRefresh()));

    //    connect(ui->pushButton_adv_refresh, SIGNAL(clicked()), this, SLOT(toAdvRefresh()));
    //    connect(ui->pushButton_shop_refresh, SIGNAL(clicked()), this, SLOT(toShopRefresh()));
    connect(ui->pushButton_show_pic_list->menu(), SIGNAL(aboutToShow()), this, SLOT(toShowPicList()));
    connect(ui->tableView_pic_list, SIGNAL(clicked(QModelIndex)), this, SLOT(toAdvListClick(QModelIndex)));
    connect(ui->pushButton_pic_list_clearAll, SIGNAL(clicked()), ui->listWidget_upan_list, SLOT(clearSelection()));
    connect(ui->pushButton_pic_list_selectAll, SIGNAL(clicked()), ui->listWidget_upan_list, SLOT(selectAll()));
    connect(this, SIGNAL(signal_copy_file(QStringList, QString)), &copy_file_ex, SLOT(copyFile(QStringList,QString)));
    connect(this, SIGNAL(signal_copy_file(QString, QString, QString)), &copy_file_ex, SLOT(copyFile(QString,QString,QString)));
    connect(&copy_file_ex, SIGNAL(information(bool,QString)), this, SLOT(toUpdateMsg(bool, QString)));
    this->setWindowTitle(QObject::tr("设置"));
    QTimer::singleShot(10, this, SLOT(toUpanRefresh()));
}

qt_XIANGSHAN_setting::~qt_XIANGSHAN_setting()
{
    delete ui;
}

QLabel *qt_XIANGSHAN_setting::get_label_shop_info_size()
{
    return ui->label_shop_info_size;
}

void qt_XIANGSHAN_setting::toCancel()
{
    this->reject();
}

void qt_XIANGSHAN_setting::toccbDefault()
{
    ui->lineEdit_ccb_BRANCHID->setText("320000000");
    ui->lineEdit_ccb_MERCHANTID->setText("105303000004292");
    ui->lineEdit_ccb_POSID->setText("008980292");
    ui->lineEdit_ccb_PUB->setText("db78d479a09597376265f30d020111");
    ui->lineEdit_ccb_QUPWD->setText("888888");
}

void qt_XIANGSHAN_setting::toOk()
{
    dataOp(true);
    this->accept();
}

void qt_XIANGSHAN_setting::toSelectAdvPath()
{
    QList<QModelIndex> indexs = ui->listWidget_upan_list->selectionModel()->selectedIndexes();
    QStringList pics;
    foreach(const QModelIndex &index, indexs) {
        pics << ui->listWidget_upan_list->item(index.row())->data(Qt::UserRole + 1).toString();
    }
    toCopyFiles(pics, lds::localdataPath + "/image_double_screen_dir");
}

void qt_XIANGSHAN_setting::toSelectShopInfo()
{
    QList<QModelIndex> indexs = ui->listWidget_upan_list->selectionModel()->selectedIndexes();
    QModelIndex  index = indexs.value(0);
    QString pic;
    if(indexs.count() != 1) {
        ui->label_msg->setText(QObject::tr("请选择一张图片"));
        return;
    }
    pic = ui->listWidget_upan_list->item(index.row())->data(Qt::UserRole + 1).toString();
    toCopyFile(pic, lds::localdataPath, "shop_info");
}

void qt_XIANGSHAN_setting::toSelectPage(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(index.row());
}

void qt_XIANGSHAN_setting::toUpanRefresh()
{
    ui->comboBox_upan->clear();
    foreach(const QString &filepath, lds::getUsbPathList()) {
        ui->comboBox_upan->addItem(QPixmap("://image/q8.png"), QFileInfo(filepath).baseName(), filepath);
        qDebug() << __FILE__ << __LINE__ << filepath;
    }
    //
    ui->comboBox_upan->setFocus();
    toUpanPicListRefresh();
}

void qt_XIANGSHAN_setting::toUpanPicListRefresh()
{
    /*Removes all items and selections in the view.
Warning: All items will be permanently deleted.*/
    if(this->focusWidget() != ui->comboBox_upan) {
        return;
    }
    QString filepath = ui->comboBox_upan->itemData(ui->comboBox_upan->currentIndex()).toString();
    if(filepath.isEmpty()) {
        return;
    }

    ui->listWidget_upan_list->clear();

    foreach(const QFileInfo &info, QDir(filepath).entryInfoList(QStringList() << "*.jpg" << "*.png" << "*.bmp")) {
        if(info.fileName() == "."
                || info.fileName() == ".."
                || info.fileName() == "") {
            continue;
        }
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(info.fileName());
        item->setData(Qt::UserRole + 1, info.filePath());
        ui->listWidget_upan_list->insertItem(ui->listWidget_upan_list->count(), item);
    }
}

void qt_XIANGSHAN_setting::toShowPicList()
{
    model_list->removeRows(0, model_list->rowCount());
    foreach(const QFileInfo &info, QDir(lds::localdataPath + "/image_double_screen_dir").entryInfoList(QStringList() << "*.jpg" << "*.png" << "*.bmp")) {
        if(info.fileName() == "."
                || info.fileName() == ".."
                || info.fileName() == "") {
            continue;
        }
        QList<QStandardItem*> items;
        QString pic = info.fileName();
        items << new QStandardItem(QString().append(pic));
        items << new QStandardItem(QString::number(info.size() / 1024) + "KB");
        items << new QStandardItem(QObject::tr("删除"));
//        items << new QStandardItem;
//        items << new QStandardItem;

        int size_kb = info.size() / 1024;
        int size_mb = size_kb / 1024;
        if(size_mb == 0) {
            items[1]->setText(QString::number(size_kb) + "KB");
        } else {
            items[1]->setText(QString::number(size_mb) + "MB");
        }
        items[1]->setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
        items[0]->setData(info.filePath());

        model_list->appendRow(items);
    }
    ui->tableView_pic_list->setColumnWidth(0, 360);
    ui->tableView_pic_list->setColumnWidth(1, 80);
    ui->tableView_pic_list->setColumnWidth(2, 120);
//    ui->tableView_pic_list->setColumnWidth(3, 60);
//    ui->tableView_pic_list->setColumnWidth(4, 60);
}

void qt_XIANGSHAN_setting::toAdvListClick(const QModelIndex &index)
{
    int row = index.row();

    if(index.column() == 2) {//删除
        QFile::remove(model_list->item(row, 0)->data().toString());
        model_list->removeRow(row);
    }
}

void qt_XIANGSHAN_setting::toUpdateMsg(bool success, const QString &msg)
{
    ui->label_msg->setText((success ? QObject::tr("操作成功") : QObject::tr("操作失败")) + ":" + msg);
    hideLoading();
}

void qt_XIANGSHAN_setting::toCopyFiles(const QStringList &filepaths, const QString &dirTo)
{
    showLoading();
    emit signal_copy_file(filepaths, dirTo);
}

void qt_XIANGSHAN_setting::toCopyFile(const QString &filepath, const QString &dirTo, const QString &baseNameTo)
{
    showLoading();
    emit signal_copy_file(filepath, dirTo, baseNameTo);

}

void qt_XIANGSHAN_setting::dataOp(bool save)
{
    if(save) {
        lds::sysconf->setValue("system_setting/skin_name", ui->comboBox_fuping_skin->currentText());

        lds::sysconf->setValue("system_setting/dianzicheng_baudrate", ui->comboBox_scale_bundrate->currentText());
        lds::sysconf->setValue("system_setting/dianzichengduankou", ui->comboBox_scale_port->currentText());

        lds::sysconf->setValue("system_setting/ccb_BRANCHID", ui->lineEdit_ccb_BRANCHID->text());
        lds::sysconf->setValue("system_setting/ccb_MERCHANTID", ui->lineEdit_ccb_MERCHANTID->text());
        lds::sysconf->setValue("system_setting/ccb_POSID", ui->lineEdit_ccb_POSID->text());
        lds::sysconf->setValue("system_setting/ccb_PUB", ui->lineEdit_ccb_PUB->text());
        lds::sysconf->setValue("system_setting/ccb_QUPWD", ui->lineEdit_ccb_QUPWD->text());
    } else {
        comSetValue(ui->comboBox_fuping_skin, lds::sysconf->value("system_setting/skin_name").toString());

        comSetValue(ui->comboBox_scale_bundrate, lds::sysconf->value("system_setting/dianzicheng_baudrate").toString());
        comSetValue(ui->comboBox_scale_port, lds::sysconf->value("system_setting/dianzichengduankou").toString());

        ui->lineEdit_ccb_BRANCHID->setText(lds::sysconf->value("system_setting/ccb_BRANCHID").toString());
        ui->lineEdit_ccb_MERCHANTID->setText(lds::sysconf->value("system_setting/ccb_MERCHANTID").toString());
        ui->lineEdit_ccb_POSID->setText(lds::sysconf->value("system_setting/ccb_POSID").toString());
        ui->lineEdit_ccb_PUB->setText(lds::sysconf->value("system_setting/ccb_PUB").toString());
        ui->lineEdit_ccb_QUPWD->setText(lds::sysconf->value("system_setting/ccb_QUPWD").toString());
    }
}

QPixmap qt_XIANGSHAN_setting::colorPixmap(const QColor &color, int w, int h)
{
    QPixmap p(w, h);
    p.fill(color);
    return p;
}

void qt_XIANGSHAN_setting::comSetValue(QComboBox *com, const QString &value)
{
    int index = com->findText(value);
    com->setCurrentIndex(index);
    if(index < 0 && com->isEditable()) {
        com->setEditText(value);
    }
}

void qt_XIANGSHAN_setting::showLoading()
{
    ui->label_loading->show();
    ui->label_loading->movie()->start();
}

void qt_XIANGSHAN_setting::hideLoading()
{
    ui->label_loading->hide();
    ui->label_loading->movie()->stop();
}
