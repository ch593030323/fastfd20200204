#ifndef LDS_CHECKBOX_UNWRAP_H
#define LDS_CHECKBOX_UNWRAP_H

#include <QCheckBox>

class lds_checkbox_unwrap : public QCheckBox
{
    Q_OBJECT
    Q_PROPERTY(QString checked1 READ isChecked1 WRITE setChecked1 USER true)

signals:
    void toggled(const QString &YN);
    void clicked(const QString &YN);
public:
    explicit lds_checkbox_unwrap(QWidget *parent = 0);
    virtual ~lds_checkbox_unwrap();

    void setChecked1(QString c);
    QString isChecked1();
    /////////////////
private slots:
    void toToggled(bool f);
    void toClicked(bool f);
};

#endif // LDS_CHECKBOX_UNWRAP_H
