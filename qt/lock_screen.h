#ifndef LOCK_SCREEN_H
#define LOCK_SCREEN_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class lock_screen;
}

class lock_screen : public QDialog
{
    Q_OBJECT
    
public:
    explicit lock_screen(QWidget *parent);
    ~lock_screen();
    QString getOperator();

private slots:
    void tonum();
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();

public:
    Ui::lock_screen *ui;

    /////////
protected:
    bool eventFilter(QObject *o, QEvent *e);
};

class lock_screen_servant : public lock_screen {
    Q_OBJECT

public:
    lock_screen_servant(QWidget*parent = 0);
};

#endif // LOCK_SCREEN_H
