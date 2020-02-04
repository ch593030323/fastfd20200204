#ifndef LDS_CHECKBOX_PHONESTYLE_H
#define LDS_CHECKBOX_PHONESTYLE_H

#include <QPushButton>
#include <QMap>
#include <QVariant>

class lds_checkbox_phonestyle : public QPushButton
{
    Q_OBJECT
public:
    lds_checkbox_phonestyle(QWidget *parent = 0);

    void setUnderline(bool underline);
    void setData(int role, const QVariant &value);
    QVariant getData(int role);
protected:
    void paintEvent(QPaintEvent *);
private:
    QMap<int, QVariant> map;
    int icon_width;
};

#endif // LDS_CHECKBOX_PHONESTYLE_H
