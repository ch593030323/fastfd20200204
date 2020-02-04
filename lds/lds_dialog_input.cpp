#include "ui_lds_dialog_input_dialog.h"
#include "lds_dialog_input.h"
#include "lds_messagebox.h"
#include <QTimer>
#include <QtDebug>
#include <QKeyEvent>
#include "lds.h"
#include "ui_lds_dialog_input_dialog.h"

lds_dialog_input::lds_dialog_input(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_lds_dialog_input_Dialog)
{
    ui->setupUi(this);
    ui->label->setText("");
    ui->label_2->setText("");
    //1
    ui->tableView->setFixedWidth(280);
    //2
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(took()));
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(toexit()));

    //    connect(ui->pushButton_self, SIGNAL(clicked()),this,SLOT(toself()));
    //3
    ui->tableView->setFixedWidth(280);
    ui->frame->setFixedHeight(40);
    lds::hideWidget(ui->label_3);
    lds::hideWidget(ui->lineEdit_3);
    lds::hideWidget(ui->label_com);
    lds::hideWidget(ui->comboBox);
    lds::hideWidget(ui->pushButton_link);

    hideSecondLineEdt();
    closeDetail();
    delay_set_focus();
    this->setWindowTitle("");
}

lds_dialog_input::~lds_dialog_input()
{
}

void lds_dialog_input::setDotEnabled(bool f)
{
    ui->widget_key3x4->setDotEnabled(f);
}

void lds_dialog_input::delay_set_focus()
{
    QTimer::singleShot(100,this,SLOT(lineselectall()));
}

void lds_dialog_input::showSecondLineEdt()
{
    ui->label_2->setVisible(true);
    ui->lineEdit_2->setVisible(true);
    this->setFixedHeight(curFixedHeight());
}

void lds_dialog_input::hideSecondLineEdt()
{
    ui->label_2->setVisible(false);
    ui->lineEdit_2->setVisible(false);
    this->setFixedHeight(curFixedHeight());

}

void lds_dialog_input::hideKey()
{
    lds::hideWidget(ui->widget_key3x4);
    this->setFixedHeight(curFixedHeight());
}

void lds_dialog_input::closeDetail()
{
    lds::hideWidget(ui->tableView);
    this->setFixedWidth(280);
}

void lds_dialog_input::openDetail()
{
    lds::showWidget(ui->tableView);
    this->setFixedWidth(280 + ui->tableView->width());
}

void lds_dialog_input::disableESC()
{
    this->installEventFilter(this);
}

void lds_dialog_input::hideCancel()
{
    ui->pushButton_cancel->hide();
}

void lds_dialog_input::resetButtons(const QStringList &names)
{
    QBoxLayout *blayout = qobject_cast<QBoxLayout *>(ui->frame->layout());
    while(blayout->count() > 0)
        delete blayout->takeAt(0)->widget();
    //
    foreach(const QString &name, names) {
        lds_toolbar_button *b = new lds_toolbar_button;
        b->setProperty("outer_stylesheet", "pushbutton");
        b->setText(name);
        blayout->insertWidget(blayout->count(), b);
        connect(b, SIGNAL(clicked()),this,SLOT(todo()));
    }
}

void lds_dialog_input::showView(int flags)
{
    lds::showWidget(ui->label,              flags & viewShow1);
    lds::showWidget(ui->lineEdit,              flags & viewShow1);

    lds::showWidget(ui->label_2,            flags & viewShow2);
    lds::showWidget(ui->lineEdit_2,         flags & viewShow2);

    lds::showWidget(ui->label_3,            flags & viewShow3);
    lds::showWidget(ui->lineEdit_3,         flags & viewShow3);

    lds::showWidget(ui->label_com,          flags & viewShowCom);
    lds::showWidget(ui->comboBox,           flags & viewShowCom);

    lds::showWidget(ui->widget_key3x4,      flags & viewShowKey);

    lds::showWidget(ui->pushButton_link,      flags & viewShowLink);

    this->setFixedHeight(curFixedHeight());
}

void lds_dialog_input::toLineEditSetNum(double num)
{
    ui->lineEdit->setText(QString::number(num));
}

void lds_dialog_input::todo()
{
    QPushButton *b = qobject_cast<QPushButton *>(this->sender());
    QBoxLayout *blayout = qobject_cast<QBoxLayout *>(ui->frame->layout());
    if(b && blayout) {
        this->done(blayout->indexOf(b));
    }
}

int lds_dialog_input::getlineheight()
{
    int h = 0;
    if(lds::widgetIsShow(ui->lineEdit)) {
        h += 40;
    }
    if(lds::widgetIsShow(ui->lineEdit_2)) {
        h += 40;
    }
    if(lds::widgetIsShow(ui->lineEdit_3)) {
        h += 40;
    }
    if(lds::widgetIsShow(ui->comboBox)) {
        h += 40;
    }
    if(lds::widgetIsShow(ui->pushButton_link)) {
        h += 40;
    }
    if(lds::widgetIsShow(ui->widget_key3x4)) {
        h += 200;
    }
    return h;
}

int lds_dialog_input::curFixedHeight()
{
    return ui->frame->height() + getlineheight() + lds_roundeddialog_rect_align::titleHeight;
}

bool lds_dialog_input::eventFilter(QObject *o, QEvent *e)
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

bool lds_dialog_input::checkdiscount(const QString &value, QWidget *parent)
{
    if(checknum(value, parent)) {
        if(value.toFloat() <= 100) {
            return true;
        }
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("折扣不能大于100"));
    }
    return false;
}

bool lds_dialog_input::checknum(const QString &value, QWidget *parent)
{
    if(!lds::isSeptNumber(value)) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("修改数据必须为数字"));
        return false;
    }
    return true;
}

bool lds_dialog_input::checknumWith0(const QString &value, QWidget *parent)
{
    bool ok = false;
    float v = value.toFloat(&ok);
    if(!ok) {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("数据必须为数字"));
        return false;
    }
    if(v  < 0)
    {
        lds_messagebox::warning(parent, MESSAGE_TITLE_VOID, QObject::tr("数据不能小于0"));
        return false;
    }
    return true;
}

void lds_dialog_input::took()
{
    took_cardno = ui->lineEdit->text();
    this->accept();//1
}

void lds_dialog_input::toexit()
{
    this->reject();//0
}

void lds_dialog_input::toself()
{
    this->done(2);//2
}

void lds_dialog_input::update_state()
{
}

void lds_dialog_input::update_state2()
{
}

void lds_dialog_input::lineselectall()
{
    if(ui->lineEdit->isEnabled()
            && !ui->lineEdit->isReadOnly()
            && !ui->lineEdit->isHidden())  {
        ui->lineEdit->selectAll();
        ui->lineEdit->setFocus();
    } else{
        ui->lineEdit_2->selectAll();
        ui->lineEdit_2->setFocus();
    }
}
