#include "public_printerr_dialog.h"
#include "ui_public_printerr_dialog.h"
#include <QTimer>
#include "public_sql.h"
#include <QMouseEvent>
#include "lds.h"
public_printerr_Dialog::public_printerr_Dialog(QWidget *parent) :
    lds_widget_drog(parent),
    ui(new Ui::public_printerr_Dialog),
    ispressed(false)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint|Qt::Tool);
    this->setAttribute(Qt::WA_DeleteOnClose);

    QTimer::singleShot(5000, this, SLOT(toclose()));

}

public_printerr_Dialog::~public_printerr_Dialog()
{
    delete ui;
}

void public_printerr_Dialog::setContent(const QString &text, const QString &title)
{
    ui->label->setText(text);
    ui->label_title->setText(title);
}

void public_printerr_Dialog::toclose()
{
/*    if(ispressed) {
        QTimer::singleShot(5000, this, SLOT(toclose()));
    } else */{
        this->close();
    }
}


void public_printerr_Dialog::mousePressEvent(QMouseEvent *event)
{
    ispressed = true;
    lds_widget_drog::mousePressEvent(event);
}

void public_printerr_Dialog::mouseReleaseEvent(QMouseEvent *event)
{
    lds_widget_drog::mouseReleaseEvent(event);
    lds::sysconf->setValue("public_printerr/pos", this->mapToGlobal(QPoint(0, 0)));
    ispressed = false;
}
