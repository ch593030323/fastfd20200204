#include "public_showimages.h"
#include "ui_public_showimages.h"
#include <QDir>
#include <QPainter>
#include <QTimer>
#include <QScrollBar>
#include <QtDebug>
#include "lds.h"
#include <QImageReader>

public_showImages::public_showImages(const QString &imagedir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::public_showImages)
{
    ui->setupUi(this);

    //tableview
    model = new ItemModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegate(new delegate(this));
    ui->tableView->horizontalHeader()->setDefaultSectionSize(100);
    ui->tableView->verticalHeader()->setDefaultSectionSize(100);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->viewport()->setFixedSize(600, 500);
    ui->tableView->setFixedSize(620, 520);
    ui->tableView->setSelectionMode(QTableView::MultiSelection);
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setStyleSheet(
                "QTableView, QStandardItem{"
                "background-color:transparent;"
                "}"
                );
    QList<QStandardItem*> items;
    QStringList nameFilters;
    foreach(const QByteArray & format, QImageReader::supportedImageFormats())
        nameFilters << "*." + format.toLower() << "*." + format.toUpper();
    foreach(const QFileInfo &info, QDir(imagedir).entryInfoList(nameFilters)) {
        if(info.isFile()) {
            items << new QStandardItem(info.filePath());
            if(items.count() == 5) {
                model->appendRow(items);
                items.clear();
            }
        }
    }
    if(items.count() > 0) {
        model->appendRow(items);
        items.clear();
    }
    //timer
    imageindex = 0;
    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(toloadimage()));
    //connect
    connect(ui->pushButton_up, SIGNAL(clicked()), ui->tableView, SLOT(toPageUp()));
    connect(ui->pushButton_down, SIGNAL(clicked()), ui->tableView, SLOT(toPageDown()));
    connect(ui->pushButton_clearAll, SIGNAL(clicked()), ui->tableView, SLOT(toClearAll()));
    connect(ui->pushButton_selectAll, SIGNAL(clicked()), ui->tableView, SLOT(toSelectAll()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->tableView->verticalScrollBar(), SIGNAL(sliderPressed()), timer, SLOT(stop()));
    connect(ui->tableView->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(loadtimestart()));
    connect(ui->tableView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(loadtimestart()));
    loadtimestart();

    setWindowTitle(QObject::tr("导入图片"));
}

public_showImages::~public_showImages()
{
    delete ui;
}

QStringList public_showImages::selected_image_path_list()
{
        return image_path_list;
}

void public_showImages::toloadimage()
{
    //row column
    int hcount = ui->tableView->horizontalHeader()->count();
    if(hcount <= 0) return;
    int imageindex = this->imageindex + ui->tableView->verticalScrollBar()->value() * hcount;
    int row = imageindex / hcount;
    int column = imageindex % hcount;
    if(this->imageindex >= hcount * (ui->tableView->verticalScrollBar()->pageStep())) {
        timer->stop();
        return;
    }

    //setData
    QModelIndex index = model->index(row, column);
    QVariant d = model->data(index, Qt::UserRole);
    if(index.isValid()
            && false == d.canConvert<QPixmap>()) {
        timer->setInterval(100);
        QSize itemsize(ui->tableView->horizontalHeader()->defaultSectionSize(), ui->tableView->verticalHeader()->defaultSectionSize());
        QPixmap p(model->data(index).toString());
        if(false == p.isNull())
            model->setData(index, p.scaled(itemsize, Qt::KeepAspectRatio), Qt::UserRole);
    } else {
        timer->setInterval(10);
    }

    //imageindex ++
    this->imageindex ++;
}

void public_showImages::loadtimestart()
{
    imageindex = 0;
    timer->start();
}

void public_showImages::tocancel()
{
    this->reject();
}

void public_showImages::took()
{
    image_path_list.clear();
    foreach(const QModelIndex &index, ui->tableView->selectionModel()->selectedIndexes()) {
        image_path_list << index.data().toString();
    }

    this->accept();
}

void public_showImages::delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() == "obsidian") {
        painter->setPen(QColor("white"));
    } else {
        painter->setPen(QColor("black"));
    }

    QVariant d = index.data(Qt::UserRole);
    if(d.canConvert<QPixmap>()) {
        QPixmap p = qvariant_cast<QPixmap>(d);
        painter->drawPixmap(QStyle::alignedRect(option.direction, Qt::AlignCenter, p.size(), option.rect), p);
    } else {
        painter->drawText(option.rect, Qt::AlignCenter | Qt::TextWordWrap, QFileInfo(index.data().toString()).fileName());
    }

    if(option.state & QStyle::State_Selected) {
        QPixmap p(":/image/widgets_pic/radio_on.png");
        p = p.scaled(30, 30);
        painter->drawPixmap(QStyle::alignedRect(option.direction, Qt::AlignTop | Qt::AlignRight, p.size(), option.rect), p);
    }
}


Qt::ItemFlags public_showImages::ItemModel::flags(const QModelIndex &index) const
{
    if(index.data().toString().isEmpty()) {
        return Qt::NoItemFlags;
    }
    return QStandardItemModel::flags(index);
}
