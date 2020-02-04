#include "label_img_header_dialog.h"
#include "ui_label_img_header_dialog.h"
#include "label_img_header.h"
#include "label_img_checkbox.h"

label_img_header_dialog::label_img_header_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::label_img_header_dialog)
{
    ui->setupUi(this);
    ui->comboBox->addItems(label_img_header::printerNames());
    ui->spinBox_printer_MMwidth->setValue(40);
    ui->spinBox_printer_MMheight->setValue(30);
    //
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(updatePrinter(QString)));
    connect(ui->spinBox_printer_MMheight, SIGNAL(valueChanged(int)), this, SLOT(updatePrinter()));
    connect(ui->spinBox_printer_MMwidth, SIGNAL(valueChanged(int)), this, SLOT(updatePrinter()));
    //
    checkInit(ui->plainTextEdit->toPlainText());
    updatePrinter(ui->comboBox->currentText());

}

label_img_header_dialog::~label_img_header_dialog()
{
    delete ui;
}

void label_img_header_dialog::on_pushButton_printPage_clicked()
{

    if(ui->checkBox_printAll->isChecked()) {
        ui->design->printAll();
        return;
    }
    ui->design->printFirst();
}

void label_img_header_dialog::on_pushButton_design_clicked()
{
    checkInit(ui->plainTextEdit->toPlainText());
}

void label_img_header_dialog::on_pushButton_left_clicked()
{
    checkTransfer("left", 1);
}

void label_img_header_dialog::on_pushButton_down_clicked()
{
    checkTransfer("bottom", 1);
}

void label_img_header_dialog::on_pushButton_top_clicked()
{
    checkTransfer("top", 1);
}

void label_img_header_dialog::on_pushButton_right_clicked()
{
    checkTransfer("right", 1);
}

void label_img_header_dialog::updateCheck()
{
}

void label_img_header_dialog::checkTransfer(const QString &member, int op)
{
    QList<label_img_checkbox *> list = checkList();

    for(int k = 0; k < list.count(); k ++) {
        if(list[k]->isChecked()) {
            if(member == "scale")
                ui->design->scale(list[k]->curID(), op);
            if(member == "left")
                ui->design->left(list[k]->curID());
            if(member == "right")
                ui->design->right(list[k]->curID());
            if(member == "top")
                ui->design->top(list[k]->curID());
            if(member == "bottom")
                ui->design->bottom(list[k]->curID());
        }
    }
    ui->plainTextEdit->setPlainText(ui->design->curTable().toString());
}

void label_img_header_dialog::checkInit(const QString &text)
{
    label_img_header::LabelTable d = label_img_header::resolveLabel(text);
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(d.printerName));
    ui->spinBox_printer_MMwidth->setValue(d.MMwidth);
    ui->spinBox_printer_MMheight->setValue(d.MMheight);
    //
    for(label_img_checkbox *o : checkList())
        delete o;
    for(int k = 0; k < d.content.count(); k ++) {
        const label_img_header::LabelLine &label  = d.content[k];
        label_img_checkbox *check = new label_img_checkbox(k, ui->frame_check_list);
        check->setText(label.name);
        check->setGeometry(10, 10 + k * 40, ui->frame_check_list->width() * 0.7, 30);
        check->setStyleSheet("border:1px solid black;");
        check->show();
        ui->frame_check_list->setMinimumHeight(10 + k * 40);
    }
    //
    ui->design->setTable(d);
}

void label_img_header_dialog::updatePrinter(const QString &printerName, int MMwidth, int MMheight)
{
    label_img_header::LabelTable table = ui->design->curTable();
    table.printerName = printerName;
    table.MMheight = MMheight;
    table.MMwidth = MMwidth;
    ui->plainTextEdit->setPlainText(table.toString());
    ui->design->setTable(table);
}

QList<label_img_checkbox *> label_img_header_dialog::checkList()
{
    QList<label_img_checkbox *> list;
    for(QObject *o :ui->frame_check_list->children()) {
        label_img_checkbox *c = qobject_cast<label_img_checkbox *>(o);
        if(c != 0)
            list << c;
    }
    return list;
}

void label_img_header_dialog::on_pushButton_scale_minus_clicked()
{
    checkTransfer("scale", -1);
}

void label_img_header_dialog::on_pushButton_scale_plus_clicked()
{
    checkTransfer("scale", 1);
}

void label_img_header_dialog::updatePrinter()
{
    updatePrinter(ui->comboBox->currentText());
}

void label_img_header_dialog::updatePrinter(const QString &printerName)
{
    updatePrinter(printerName, ui->spinBox_printer_MMwidth->value(), ui->spinBox_printer_MMheight->value());
}

void label_img_header_dialog::on_pushButton_printer_MMwidth_sub_clicked()
{
    ui->spinBox_printer_MMwidth->setValue(ui->spinBox_printer_MMwidth->value() - 1);
}

void label_img_header_dialog::on_pushButton_printer_MMheight_sub_clicked()
{
    ui->spinBox_printer_MMheight->setValue(ui->spinBox_printer_MMheight->value() - 1);
}

void label_img_header_dialog::on_pushButton_printer_MMheight_add_clicked()
{
    ui->spinBox_printer_MMheight->setValue(ui->spinBox_printer_MMheight->value() + 1);
}

void label_img_header_dialog::on_pushButton_printer_MMwidth_add_clicked()
{
    ui->spinBox_printer_MMwidth->setValue(ui->spinBox_printer_MMwidth->value() + 1);
}

void label_img_header_dialog::on_pushButton_selectAll_clicked()
{
    int k = -1;
    for(label_img_checkbox *o : checkList()) {
        if(k == -1)
            k = !o->isChecked();
        o->setChecked(k);
    }
}
