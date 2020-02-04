#ifndef FEXPANDMAIN_PAYING_H
#define FEXPANDMAIN_PAYING_H

#include <QWidget>

namespace Ui {
class fexpandmain_paying;
}

class fexpandmain_paying : public QWidget
{
    Q_OBJECT

public:
    explicit fexpandmain_paying(QWidget *parent = 0);
    ~fexpandmain_paying();

    void setMasterValue(const QString &json);
    void setMasterValue(const QString &no, const QString &label, const QString &text);
    void appendDetailValue(const QString &no, const QString &label, const QString &text);

    void updateStyle(const QString &skin_name);
    void setAllControl2Zero();
private:
    Ui::fexpandmain_paying *ui;
};

#endif // FEXPANDMAIN_PAYING_H
