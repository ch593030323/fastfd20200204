#include "lock_screen.h"
#include "ui_lock_screen.h"
#include <QSqlRecord>
#include "lds_messagebox.h"
#include "n_func.h"
#include <QDebug>
#include "backheader.h"
#include "lds.h"
#include <QSqlQueryModel>

lock_screen::lock_screen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lock_screen)
{
    ui->setupUi(this);
    setWindowTitle(QObject::tr("前台锁屏"));
#ifdef QT_DEBUG
    setWindowTitle(this->metaObject()->className());
#endif
    ui->lineEdit->setText(lds::gs_operid);
    ui->lineEdit->selectAll();
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    ui->lineEdit_2->installEventFilter(this);
    this->installEventFilter(this);

    if(lds::sysconf->value("system_setting/jiesuoyuangong").toString() == "1"){
        ui->lineEdit->setEnabled(false);
    }
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_13, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(tonum()));
    connect(ui->pushButton_backspace, SIGNAL(clicked()), this, SLOT(tonum()));

    //
    lds::hideWidget(ui->pushButton_cancel);
    lds::hideWidget(ui->lineEdit_2);
    lds::hideWidget(ui->label_3);
}

lock_screen::~lock_screen()
{
    delete ui;
}

QString lock_screen::getOperator()
{
    return ui->lineEdit->text().trimmed();
}

void lock_screen::tonum()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    QLineEdit *l = qobject_cast<QLineEdit *>(qApp->focusWidget());
    if(l == 0)
        return;
    if(b == ui->pushButton_backspace) {
        l->backspace();
    } else {
        l->insert(b->text());
    }
}

bool lock_screen::eventFilter(QObject *o, QEvent *e)
{
    if(QEvent::KeyPress == e->type()) {
        QKeyEvent *ke = ((QKeyEvent *)e);
        const int kvalue = ke->key();
        if(kvalue == Qt::Key_Escape) {
            e->ignore();
            return true;
        }
    }
    return QDialog::eventFilter(o, e);
}

void lock_screen::on_pushButton_ok_clicked()
{
    QString vch_operid = ui->lineEdit->text().trimmed();
    QString vch_pwd = ui->lineEdit_2->text().trimmed();
    if(vch_operid == "") {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("用户名为空"));
        return;
    }
    if(vch_operid == "0000") {
        this->accept();
        return;
    }
    if(lds_query_hddpos::selectValue(QString("select count(0)  from cey_sys_operator  where vch_operID = '%1' and ch_flag ='Y' ").arg(vch_operid)).toInt() == 0) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("用户名代码不存在"));
        return;
    }
    this->accept();


    //    QSqlQueryModel model;

    //    if(ui->lineEdit->text().trimmed().isEmpty() || ui->lineEdit_2->text().trimmed().isEmpty())
    //    {
    //        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("请输入完整的用户名和密码"));

    //        return;
    //    }

    //    //    if(lds::sysconf->value("system_setting/jiesuoyuangong").toString() == "1")
    //    //    {
    //    //        if(ui->lineEdit->text().trimmed() != lds::gs_operid)
    //    //        {
    //    //            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("锁屏解锁必须是系统登录员工"));

    //    //            return;
    //    //        }
    //    //    }

    //    if(ui->lineEdit->text().trimmed() == "0000")
    //    {
    //        if(!n_func::f_cmp_syspwd(ui->lineEdit_2->text().trimmed()))//ui->lineEdit_2->text().trimmed() != n_func::f_get_sysparm("system_pass", "0000"))
    //        {
    //            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码输入错误"));
    //            ui->lineEdit_2->setFocus();
    //            return;
    //        }
    //    }
    //    else
    //    {
    //        model.setQuery(QString("select count(0)  from cey_sys_operator  where vch_operID = '%1' and ch_flag ='%2' ").arg(ui->lineEdit->text().trimmed()).arg("Y"));
    //        if(model.record(0).value(0).toInt() == 0)
    //        {
    //            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("用户名代码不存在"));

    //            return;
    //        }

    //        model.clear();
    //        model.setQuery(QString("select vch_password  from cey_sys_operator  where vch_operID = '%1' and ch_flag ='%2' ").arg(ui->lineEdit->text().trimmed()).arg("Y"));
    //        if(model.record(0).value(0).toString().trimmed() != ui->lineEdit_2->text().trimmed())
    //        {
    //            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码输入错误"));

    //            return;
    //        }
    //    }

    //    this->accept();
}


lock_screen_servant::lock_screen_servant(QWidget *parent)
    : lock_screen(parent)
{
    this->setWindowTitle(MESSAGE_TITLE_VOID);
#ifdef QT_DEBUG
    setWindowTitle(this->metaObject()->className());
#endif
    ui->label_2->setText(QObject::tr("服务员"));
    ui->lineEdit->setEnabled(true);
    lds::hideWidget(ui->pushButton_cancel, false);
}

void lock_screen::on_pushButton_cancel_clicked()
{
    this->reject();
}
