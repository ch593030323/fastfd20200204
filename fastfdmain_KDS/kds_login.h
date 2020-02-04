#ifndef KDS_LOGIN_H
#define KDS_LOGIN_H

#include <QFrame>

namespace Ui {
class kds_login;
}

class kds_login : public QFrame
{
    Q_OBJECT

public:
    explicit kds_login(QWidget *parent = 0);
    ~kds_login();

public slots:
    void tologin();
public:
    bool login();
private slots:
    void toexit();
    void tonum();
    void toauto_login();
signals:
    void signalExit();
    void signalLogin();

private:
    Ui::kds_login *ui;
};

#endif // KDS_LOGIN_H
