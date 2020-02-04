#ifndef LDS_CHECKBOX_H
#define LDS_CHECKBOX_H

#include <QCheckBox>
#include "lds_checkbox_wrap.h"



//数据库mapper时，需要使用





class lds_CheckBox : public lds_checkbox_wrap
{
    Q_OBJECT
    Q_PROPERTY(QString checked1 READ isChecked1 WRITE setChecked1 USER true)
    Q_PROPERTY(QString checkYN READ isCheckYN WRITE setCheckYN USER true)
public:
    static QString png_h30_path_appcheckon();
    static QString png_h30_path_appcheckoff();

    static QString png_path_appcheckon();
    static QString png_path_appcheckoff();
signals:
    void toggled(const QString &YN);
    void clicked(const QString &YN);
public:
    explicit lds_CheckBox(QWidget *parent = 0);
    void setChecked1(QString YN);
    QString isChecked1();

    void setCheckYN(QString YN);
    QString isCheckYN();
public slots:
    void setUnchecked(bool uncheck);
private slots:
    void toToggled(bool f);
    void toClicked(bool f);
};

#endif // LDS_CHECKBOX_H
