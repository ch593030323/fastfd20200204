#ifndef UINORMAL_MESSAGE_H
#define UINORMAL_MESSAGE_H

#include <QDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
class uinormal_message : public QDialog
{
    Q_OBJECT
public:
    explicit uinormal_message(QWidget *parent = 0);


    static int warning(QWidget *parent, const QString &title,
                       const QString& text,
                       const QString& button0Text = QString(),
                       const QString& button1Text = QString(),
                       const QString& button2Text = QString()
            );
    static int information(QWidget *parent, const QString &title,
                                     const QString& text,
                                     const QString& button0Text = QString(),
                                     const QString& button1Text = QString(),
                                     const QString& button2Text = QString()
                          );
private:
    static int showOldMessageBox(QWidget *parent, QMessageBox::Icon icon,
                                 const QString &title, const QString &text,
                                 const QString &button0Text,
                                 const QString &button1Text,
                                 const QString &button2Text);
};

#endif // UINORMAL_MESSAGE_H
