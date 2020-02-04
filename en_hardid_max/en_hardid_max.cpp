#include "en_hardid_max.h"
#include "ui_en_hardid_max.h"
#include "lds.h"
#include <QMessageBox>
#include <QFile>
#include <QClipboard>


en_hardid_max::en_hardid_max(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::en_hardid_max)
{
    ui->setupUi(this);
    ui->dateEdit_soft->setDate(QDate::currentDate());
    ui->tabWidget->setCurrentIndex(0);
}

en_hardid_max::~en_hardid_max()
{
    delete ui;
}

void en_hardid_max::on_pushButton_ok_clicked()
{
    togenerate2();
}

void en_hardid_max::on_pushButton_exit_clicked()
{
    this->reject();
}

void en_hardid_max::togenerate2()
{
    QString value_num;
    int index = ui->tabWidget->currentIndex();
    switch(index) {
    case 0://平板连接数
    {
        value_num = QString("%1(%2)")
                .arg(ui->lineEdit_mobile_hardid->text())
                .arg(ui->lineEdit_mobile_connect_max_id->text());
        if(ui->lineEdit_mobile_hardid->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("机器码为空"));
            return;
        }
        if(ui->lineEdit_mobile_connect_max_id->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("最大连接数为空"));
            return;
        }
    }
        break;
    case 1://软件有效期
    {
        value_num = QString("%1%2")
                .arg(ui->lineEdit_soft_hardid->text())
                .arg(ui->dateEdit_soft->date().toString("yyyyMMdd"));
        if(ui->lineEdit_soft_hardid->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("机器码为空"));
            return;
        }
        if(ui->dateEdit_soft->date().isNull()) {
            QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("有效期isNull"));
            return;
        }
    }
        break;
    case 2://语言激活
    {
        value_num = QString("strange_language/%1=1").arg(ui->lineEdit_lan_hardid->text());
        if(ui->lineEdit_lan_hardid->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, QObject::tr("警告"), QObject::tr("机器码为空"));
            return;
        }
    }
        break;
    }


    value_num = lds::of_str_encrypt(value_num);
    value_num = value_num.toLocal8Bit().toHex().toUpper();
    QFile file(ui->tabWidget->tabText(index) + ".txt");
    file.open(QFile::WriteOnly);
    file.write(value_num.toLocal8Bit());
    file.close();

    QApplication::clipboard()->setText(value_num);
    QMessageBox::information(this, MESSAGE_TITLE_VOID, QObject::tr("当前注册码：%1，已复制到剪切板").arg(value_num));
}
