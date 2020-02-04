#include "labelprint_goodinfo_select_dialog.h"
#include "ui_labelprint_goodinfo_select_dialog.h"
#include <QRadioButton>
#include <QtDebug>
#include "commonheader.h"

labelprint_goodinfo_select_dialog::labelprint_goodinfo_select_dialog(const QStringList selecteds, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::labelprint_goodinfo_select_dialog),
    _selecteds(selecteds)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setWindowTitle(QObject::tr("选择商品"));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(reject()));
}

labelprint_goodinfo_select_dialog::~labelprint_goodinfo_select_dialog()
{
    delete ui;
}

void labelprint_goodinfo_select_dialog::setup(const QStringList &checks)
{
    QGridLayout *glayout=new QGridLayout;
    ui->widget->setLayout(glayout);

    //count 15 =4*3+3
    for(int row=0,rowcount=checks.count()/4+(checks.count()%4!=0);row<rowcount;row++) {
        for(int c=0;c<4;c++) {
            int index=row*4+c;
            if(index < checks.count()) {
                QRadioButton *rb=new QRadioButton;
                rb->setText(checks[index]);
                glayout->addWidget(rb, row, c);
                rbs.append(rb);
                if(rb->text()==QObject::tr("打印次数"))rb->setVisible(false);
                if(_selecteds.contains(rb->text())) {
                    rb->setAutoExclusive(true);
                    rb->setChecked(true);
                    rb->setEnabled(false);
                }
            }
        }
    }
}

void labelprint_goodinfo_select_dialog::setexplaintext(const QString &text)
{
    ui->label_explain->setText(text);
}

QString labelprint_goodinfo_select_dialog::whoischecked()
{
    foreach(QRadioButton *rb, rbs) {
        if(rb->isEnabled()
                &&rb->isChecked())
            return rb->text();
    }
    return "";
}
