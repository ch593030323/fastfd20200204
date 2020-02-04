#include "getfilepath.h"
#include "ui_getfilepath.h"
#include <QLabel>
#include <QTableView>
#include <QFileInfo>
#include <QInputDialog>
#include "lds_messagebox.h"
#include <QTimer>
#include <QPixmapCache>
#include <QtDebug>
#include "lds.h"

getFilePath::getFilePath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::getFilePath)
{
    ui->setupUi(this);
    ui->tableWidget_dir->getGuideLabel()->clear();
    copychecked = false;
    ui->pushButton_paste->setEnabled(copychecked);
    ui->pushButton_copy->setEnabled(false);
    ui->pushButton_rename->setEnabled(false);
    ui->pushButton_del->setEnabled(false);
    path_arg  =
            "<span style=\" color:#ff546b;\">%1%2</span>"
            "<span style=\" color:#ff546b;font-weight: bold;\">%3</span>";
    connect(ui->tableWidget_dir,SIGNAL(clickstat(QString,QString)),this,SLOT(toupdateGuide(QString,QString)));
    connect(this,SIGNAL(signalconduct(int,int)),this,SLOT(toupdateprogress(int, int)));
    connect(ui->tableWidget_dir->getFileTable(),SIGNAL(clicked(QModelIndex)),this,SLOT(toquicklygoin(QModelIndex)));

    //
    QRadioButton *r;
    radio_str_pair_list.append(radio_strPair(r = new QRadioButton(QObject::tr("系统目录"), this), "/usr/desk")); r->setStyleSheet("QRadioButton:indicator{ height:20px;width:20px; image: url(:/image/q6.png);} ");r->setFixedHeight(30);
    radio_str_pair_list.append(radio_strPair(r = new QRadioButton(QObject::tr("SD卡"), this), "/mnt/sdcard"));r->setStyleSheet("QRadioButton:indicator{ height:20px;width:20px; image: url(:/image/q7.png);} ");r->setFixedHeight(30);
    foreach(const QString &k,  lds::getUsbPathList()) {
        radio_str_pair_list.append(radio_strPair(r = new QRadioButton(QFileInfo(k).baseName(), this), k)); r->setStyleSheet("QRadioButton:indicator{ height:20px;width:20px; image: url(:/image/q8.png);} ");r->setFixedHeight(30);
    }
    for(int k = 0; k < radio_str_pair_list.count(); k++) {
        ui->verticalLayout_pan->insertWidget(ui->verticalLayout_pan->count(),  radio_str_pair_list[k].first);
        connect(radio_str_pair_list[k].first, SIGNAL(clicked(bool)), this, SLOT(toPanToggled(bool)));
    }
    showDirsOnly(false);

    radio_str_pair_list.first().first->setChecked(true);
}

getFilePath::~getFilePath()
{
    delete ui;
}

void getFilePath::setCurFilePath(const QString &filepath)
{
    //
    bool list_has_checked = false;
    for(int k = 0; k < radio_str_pair_list.count(); k++) {
        if(radio_str_pair_list[k].second.contains("/mnt/usb")
                && filepath.contains("/mnt/usb")) {
            radio_str_pair_list[k].first->setChecked(true);
            list_has_checked = true;
            break;
        }
        if(radio_str_pair_list[k].second.contains("/mnt/sdcard")
                && filepath.contains("/mnt/sdcard")) {
            radio_str_pair_list[k].first->setChecked(true);
            list_has_checked = true;
            break;
        }
    }
    if(false == list_has_checked) {
        radio_str_pair_list[0].first->setChecked(true);
    }
    QFileInfo filepathinfo(filepath);

    if(!ui->tableWidget_dir->lineEditFilename->isHidden()) {
        ui->tableWidget_dir->lineEditFilename->setText(filepathinfo.fileName());
        ui->tableWidget_dir->setRootPath(filepathinfo.path());
    } else {
        if(filepathinfo.isFile()) {
            ui->tableWidget_dir->setRootPath(filepathinfo.path());
            ui->tableWidget_dir->getGuideLabel()->setText(filepathinfo.filePath());
        } else {
            ui->tableWidget_dir->setRootPath(filepath);
        }
    }
    //若是文件存在，则切到这行
    if(QFile::exists(filepath)) {
        QAbstractItemModel *model = ui->tableWidget_dir->getFileTable()->model();
        for(int row = 0, rowcount = model->rowCount(); row < rowcount; row++) {
            if(model->data(model->index(row, 0)).toString() == filepathinfo.fileName()) {
                ui->tableWidget_dir->getFileTable()->selectRow(row);
                break;
            }
        }
    }
}

void getFilePath::showDirsOnly(bool f)
{
    if(f)*(ui->tableWidget_dir->fileEnabled) = false;
}

void getFilePath::setOkButtonText(const QString &text)
{
    ui->pushButton_ok->setText(text);
}

void getFilePath::showFilename()
{
    ui->tableWidget_dir->getLineEditFilename_show();
}

void getFilePath::on_pushButton_ok_clicked()
{
    retpath = ui->tableWidget_dir->getGuideLabel()->text();
    if(!ui->tableWidget_dir->lineEditFilename->isHidden()) {
        retpath += "/"+ui->tableWidget_dir->lineEditFilename->text();
    }
    this->accept();
}

void getFilePath::on_pushButton_close_clicked()
{
    this->reject();
}

void getFilePath::on_pushButton_copy_pressed()
{
    filepathfrom = ui->tableWidget_dir->selectedFiles().value(0);
    if(filepathfrom.isEmpty()) {
        copychecked = false;
        updatecopy_paste(copychecked);
        return;
    }

    copy_pasteupdate();

    if(copychecked) {
        if(pathto.isEmpty()) pathto = QFileInfo(filepathfrom).path();
        ui->tableWidget_dir->getGuideLabel()->setText(path_arg.arg(QObject::tr("复制")+filepathfrom).arg(QObject::tr("到")).arg(pathto+QDir::separator()));
    }
}

void getFilePath::on_pushButton_paste_clicked()
{
    QString filepathto = pathto+QDir::separator()+QFileInfo(filepathfrom).fileName();
    QString error;
    QFile file0(filepathfrom);
    QFileInfo filepathtoinfo(filepathto);
    QFileInfo filepathfrominfo(filepathfrom);
    file0.open(QFile::ReadOnly);
    //1
    error = QObject::tr("源文件和目标文件一致");
    if(filepathfrom == filepathto) {
        goto failure;
    }
    error.clear();
    //1
    error = QObject::tr("目标文件路径不能包含源文件");
    if(filepathto.contains(filepathfrom)) {
        goto failure;
    }
    error.clear();
    //2
    if(QFile::exists(filepathto)) {
        if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("删除同名文件?"), QObject::tr("是"),QObject::tr("否"))) {

            if(filepathtoinfo.isDir()) {
                error=QObject::tr("删除同名文件夹");
                if(deleteDir(filepathto)) {
                } else {
                    goto failure;
                }
            } else if(filepathtoinfo.isFile()) {
                error=QObject::tr("删除同名文件");
                if(deleteFile(filepathto)) {
                } else {
                    goto failure;
                }
            }
        } else {
            goto ret;
        }
    }

    //3
    if(filepathfrominfo.isDir()) {
        error = QObject::tr("粘贴文件夹");
        if(copyDir(filepathfrominfo.filePath(), filepathtoinfo.path())) {
            lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            goto ret;
        } else {
            goto failure;
        }
    } else if(filepathfrominfo.isFile()) {
        error = QObject::tr("粘贴文件");
        if(copyFile(filepathfrominfo.filePath(), filepathtoinfo.path())) {
            lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            goto ret;
        } else {
            goto failure;
        }
    }
failure:
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, error+QObject::tr("操作失败"));
ret:
    copy_pasteupdate();
    return;
}

void getFilePath::on_pushButton_del_clicked()
{
    QString filepath = ui->tableWidget_dir->selectedFiles().value(0);
    if(filepath.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有文件被选中"));
        return;
    }
    QFileInfo filepathinfo = QFileInfo(filepath);
    if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认删除"),QObject::tr("是"),QObject::tr("否"))) {
        if(filepathinfo.isDir()) {
            if(deleteDir(filepath))
                lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            else
                lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        } else if(filepathinfo.isFile()) {
            if(deleteFile(filepath))
                lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
            else
                lds_messagebox::information(this,MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        }
    }
}

void getFilePath::toupdateGuide(QString path,QString name)
{
    QString filepath;
    if(!copychecked) {
        if(name.isEmpty()
                ||name == "."
                || name == ".."
                || !QFile(path+QDir::separator()+name).exists()) {
            ui->pushButton_copy->setEnabled(false);
        } else {
            ui->pushButton_copy->setEnabled(true);
        }
    }
    if(name.isEmpty()
            ||name == "."
            || name == ".."
            || !QFile(path+QDir::separator()+name).exists()) {
        ui->pushButton_rename->setEnabled(false);
        ui->pushButton_del->setEnabled(false);
        name.clear();
        filepath = path;
    } else {
        filepath = path+QDir::separator()+name;
        ui->pushButton_rename->setEnabled(true);
        ui->pushButton_del->setEnabled(true);
    }

    if(filepathfrom.isEmpty() && pathto.isEmpty()) {

        QFileInfo filepathinfo(filepath);
        ui->tableWidget_dir->getGuideLabel()->setText(filepathinfo.filePath());
        if(!ui->tableWidget_dir->lineEditFilename->isHidden()) {
            if(filepathinfo.isFile()) {
                ui->tableWidget_dir->lineEditFilename->setText(filepathinfo.fileName());
                ui->tableWidget_dir->getGuideLabel()->setText(filepathinfo.path());
            }
        }

    } else {
        pathto = filepath;
        ui->tableWidget_dir->getGuideLabel()->setText(path_arg.arg(filepathfrom).arg(QObject::tr("到")).arg(pathto+QDir::separator()));
    }
}

void getFilePath::toquicklygoin(const QModelIndex &index)
{
    if(index.column() == 1) {
        ui->tableWidget_dir->dirDoubleClicked(index);
    }
}

bool getFilePath::deleteFile(const QString &filepath)
{
    return QFile::remove(filepath);
}

bool getFilePath::deleteDir(const QString &path)
{
    QFileInfo parent_info(path);
    if(path.isEmpty()) return false; //当treeview没有数据被选中时
    if(parent_info.isDir() && parent_info.fileName() == ".") return true;
    if(parent_info.isDir() && parent_info.fileName() == "..") return true;

    QFileInfoList infos = QDir(path).entryInfoList();
    bool flag = true;

    int step = 0;
    int count = infos.count();
    foreach(const QFileInfo &info, infos) {
        step ++;
        if(info.isDir()) {
            flag = flag && deleteDir(info.filePath());
        } else if(info.isFile()) {
            flag = flag && deleteFile(info.filePath());
        }
        toupdateprogress(step, count);

        if(!flag) break;
    }
    if(parent_info.isDir()) {
        flag = flag && QDir().rmdir(path);
    }
    return flag;
}

bool getFilePath::copyDir(const QString &frompath, const QString &topath)
{
    QFileInfo fromInfo(frompath);
    QFileInfo toInfo(topath);
    if(fromInfo.isDir() && fromInfo.fileName() == ".") return true;
    if(fromInfo.isDir() && fromInfo.fileName() == "..") return true;
    QString newfilepath = toInfo.filePath() + QDir::separator() + fromInfo.fileName();

    //开始copy 文件夹内容
    bool flag = true;
    flag = flag && newDir(newfilepath);

    QFileInfoList infos = QDir(frompath).entryInfoList();
    int step = 0;
    int count = infos.count();
    foreach(const QFileInfo &info, infos) {
        step++;
        if(info.isDir()) {
            flag = flag && copyDir(info.filePath(), newfilepath);
        } else if(info.isFile()) {
            flag = flag && copyFile(info.filePath(), newfilepath);
        }
        toupdateprogress(step, count);
        if(!flag) break;
    }

    return flag;
}

bool getFilePath::copyFile(const QString &fromfilepath, const QString &topath)
{
    QString filepathto = topath+QDir::separator()+QFileInfo(fromfilepath).fileName();
    QFile file0(fromfilepath);
    bool flag = file0.copy(fromfilepath, filepathto);
    if(!flag)
        qDebug() << file0.errorString();
    return flag;
}

bool getFilePath::newDir(const QString &filepath)
{
    return QDir().mkdir(filepath);
}

bool getFilePath::newFile(const QString &filepath)
{
    QFile f(filepath);
    f.open(QFile::WriteOnly);
    f.close();

    bool flag = QFile::exists(filepath);
    return flag;
}

void getFilePath::on_pushButton_rename_clicked()
{
    QString filepath = ui->tableWidget_dir->selectedFiles().value(0);
    if(filepath.isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("没有文件被选中"));
        return;
    }

    QFileInfo filepathinfo = QFileInfo(filepath);

    QInputDialog dialog(this);
    dialog.setWindowTitle(ui->pushButton_rename->text());
    dialog.setLabelText(QObject::tr("文件名"));
    dialog.setTextEchoMode(QLineEdit::Normal);
    dialog.setTextValue(QFileInfo(filepath).fileName());
    dialog.setOkButtonText(QObject::tr("确定"));
    dialog.setCancelButtonText(QObject::tr("取消"));
    dialog.resize(this->width()*2/3.0, 100);
    if(QDialog::Accepted == lds_roundeddialog_rect_align_200x100(&dialog).exec()) {
        if(dialog.textValue() == QFileInfo(filepath).fileName()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("名字没有改变"));
            return;
        }
        if(QDir().rename(filepath, filepathinfo.path()+QDir::separator()+dialog.textValue())) {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作成功"));
        } else {
            lds_messagebox::information(this, MESSAGE_TITLE_VOID, QObject::tr("操作失败"));
        }
    }
}

void getFilePath::copy_pasteupdate()
{
    copychecked = !copychecked;
    updatecopy_paste(copychecked);
}

void getFilePath::updatecopy_paste(bool f)
{
    ui->pushButton_copy->setDown(copychecked);
    if(copychecked)ui->pushButton_copy->setText(QObject::tr("取消复制"));
    else ui->pushButton_copy->setText(QObject::tr("复制"));

    ui->pushButton_ok->setEnabled(!copychecked);
    ui->pushButton_paste->setEnabled(copychecked);
    if(!copychecked) {
        ui->tableWidget_dir->getGuideLabel()->clear();
        filepathfrom.clear();
        pathto.clear();
    }
}

bool getFilePath::checkfile(const QString &filepath)
{
    QFileInfo filepathinfo(filepath);
    if(filepathinfo.fileName().isEmpty()
            ||filepathinfo.fileName() == "."
            || filepathinfo.fileName() == ".."
            || !QFile(filepathinfo.filePath()).exists()) {
        return false;
    }
    return true;
}

//void getFilePath::on_pushButton_go_clicked()
//{
//    ui->tableWidget_dir->dirDoubleClicked(ui->tableWidget_dir->getFileTable()->currentIndex());
//}

void getFilePath::toupdateprogress(int step, int total)
{
    ui->label_desc->setText(QString("%1/%2").arg(step).arg(total));
    ui->label_desc->repaint();
}

void getFilePath::toPanToggled(bool checked)
{
    if(!checked) return;
    for(int k = 0; k < radio_str_pair_list.count(); k ++) {
        if(radio_str_pair_list[k].first == this->sender()) {
            ui->tableWidget_dir->setRootPath(radio_str_pair_list[k].second, true);
        }
    }
}

QPoint getFilePath::getGlobalCenterPopup(QWidget *parent, QWidget *pop) {
    QPoint p = parent->mapToGlobal(QPoint(0,0));
    return QStyle::alignedRect(parent->layoutDirection(), Qt::AlignCenter, pop->size(), QRect(p, QSize(parent->size()))).topLeft();
}

QString getFilePath::getExistingDirectory(QWidget *parent, const QString &caption, const QString &dir, QFileDialog::Options options)
{
#if defined(Q_OS_WIN)// || defined(QT_DEBUG)
    return QFileDialog::getExistingDirectory(parent, caption, dir, options);
#endif

#if defined(Q_OS_UNIX)
    getFilePath dialog(parent);
    if(options == QFileDialog::ShowDirsOnly)dialog.showDirsOnly(true);
    dialog.setWindowTitle(caption);
    dialog.setCurFilePath(dir);
    dialog.setOkButtonText(QObject::tr("选择"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align_200x100(&dialog).exec()) {
        if(!QPixmap(dialog.retpath).isNull()) {
            QPixmapCache::clear();
        }
        return QFileInfo(dialog.retpath).isFile() ? QFileInfo(dialog.retpath).path() : dialog.retpath;
    }
    return "";
#endif
}

QString getFilePath::getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
#if defined(Q_OS_WIN)// || defined(QT_DEBUG)
    return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
#endif

#if defined(Q_OS_UNIX)
    getFilePath dialog(parent);
    dialog.setWindowTitle(caption);
    dialog.setCurFilePath(dir);
    dialog.setOkButtonText(QObject::tr("选择"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align_200x100(&dialog).exec()) {
        if(!QPixmap(dialog.retpath).isNull()) {
            QPixmapCache::clear();
        }
        if(QFileInfo(dialog.retpath).isDir()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, "NOT FILE"  "," + QObject::tr("操作失败"));
            return "";
        }
        return dialog.retpath;
    }
    return "";
#endif
}

QStringList getFilePath::getOpenFileNames(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    return QFileDialog::getOpenFileNames(parent,caption,dir,filter,selectedFilter,options);
}

QString getFilePath::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
#if defined(Q_OS_WIN)// || defined(QT_DEBUG)
    return QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options);
#endif

#if defined(Q_OS_UNIX)
    getFilePath dialog(parent);
    if(options == QFileDialog::ShowDirsOnly)dialog.showDirsOnly(true);
    dialog.showFilename();
    dialog.setWindowTitle(caption);
    dialog.setCurFilePath(dir);

    dialog.setOkButtonText(QObject::tr("选择"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align_200x100(&dialog).exec()) {
        if(QFile::exists(dialog.retpath)) {
            if(1 == lds_messagebox::warning(parent, caption,QObject::tr("有同名文件,该文件将会被替换"), QObject::tr("确定"), QObject::tr("取消"))) {
                return "";
            }
        }
        if(!QPixmap(dialog.retpath).isNull()) {
            QPixmapCache::clear();
        }
        if(QFileInfo(dialog.retpath).isDir()) {
            lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, "NOT FILE," + QObject::tr("操作失败"));
            return "";
        }
        return dialog.retpath;
    }
    return "";
#endif
}

void getFilePath::on_pushButton_down_clicked()
{
    ui->tableWidget_dir->dirListDownPage();
}

void getFilePath::on_pushButton_up_clicked()
{
    ui->tableWidget_dir->dirListUpPage();
}
