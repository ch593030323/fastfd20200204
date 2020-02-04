#include "uinormal_message.h"
#include "commonheader.h"


uinormal_message::uinormal_message(QWidget *parent) :
    QDialog(parent)
{
}

int uinormal_message::warning(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text)
{
    return uinormal_message::showOldMessageBox(parent, QMessageBox::Warning, title, text,
                                               button0Text, button1Text, button2Text);

}

int uinormal_message::information(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text)
{
    return uinormal_message::showOldMessageBox(parent, QMessageBox::Information, title, text,
                                               button0Text, button1Text, button2Text);
}


int uinormal_message::showOldMessageBox(QWidget *parent, QMessageBox::Icon icon, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text)
{
    QMessageBox messageBox(icon, title, text, QMessageBox::NoButton, parent);
    messageBox.setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    messageBox.setStyleSheet("QPushButton{min-width:60px;}");
    QString myButton0Text = button0Text;
    if (myButton0Text.isEmpty()) {
        myButton0Text = QDialogButtonBox::QObject::tr("OK");
    }
    messageBox.addButton(myButton0Text, QMessageBox::ActionRole);
    if (!button1Text.isEmpty()) {
        messageBox.addButton(button1Text, QMessageBox::ActionRole);
    }
    if (!button2Text.isEmpty()) {
        messageBox.addButton(button2Text, QMessageBox::ActionRole);
    }

    return messageBox.exec();
}
