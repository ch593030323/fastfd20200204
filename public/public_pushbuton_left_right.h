#ifndef PUBLIC_PUSHBUTON_LEFT_RIGHT_H
#define PUBLIC_PUSHBUTON_LEFT_RIGHT_H

#include <QPushButton>
#include "lds_bool.h"
#include <QVariant>


class public_pushbuton_left_right : public QPushButton
{
    Q_OBJECT
public:
    explicit public_pushbuton_left_right(QWidget *parent = 0);

    void setTitleKey(const QString &title, const QVariant &value);
    void disableClicked();

    void setCheckedByIni(const QString &config_path, const QVariant &config_default_value, bool is_n = false);
    void setCheckedByIniN(const QString &config_path, const QVariant &config_default_value);

    QVariant checkedValue();
signals:
    void signal_switch(const QVariant &value);
    ///////////enableConnectCheck///////////
public:
    void enableConnectCheck(const QString &check_title, const QVariant &check_key_def);
    void enableConnectCheck(const QString &check_title, const QVariant &check_key_def, const QList< QPair<QVariant, QString> > &key_value_list );
private slots:
    void toSwitchChecked();
private:
    QString key_value_list_value_get(const QVariant &key, const QString &defvalue = "");
    int key_value_list_index_get(const QVariant &key);
private:
    QString check_title;
    QVariant check_value;
    QList< QPair<QVariant, QString> > key_value_list;

    ///////////enableConnectCheck///////////
protected:
    void paintEvent(QPaintEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    bool isChecked() const;
private:
    int xp1;
    int yp1;
    int xp2;
    int yp2;
    bool clickabled;
    lds_bool_default_true is_n;
    QString config_path;
};

#endif // PUBLIC_PUSHBUTON_LEFT_RIGHT_H
