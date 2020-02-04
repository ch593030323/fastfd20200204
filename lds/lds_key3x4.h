#ifndef LDS_KEY3X4_H
#define LDS_KEY3X4_H

#include <QWidget>

namespace Ui {
class lds_key3x4;
}

class lds_key3x4 : public QWidget
{
    Q_OBJECT

public:
    explicit lds_key3x4(QWidget *parent = 0);
    ~lds_key3x4();
    void setDotEnabled(bool f);

    void showExtra(bool show);
private slots:
    void set_num();

private:
    Ui::lds_key3x4 *ui;
};

#endif // LDS_KEY3X4_H
