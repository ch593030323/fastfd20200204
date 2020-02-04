#ifndef SAAS_GUIDE_DIRECT_H
#define SAAS_GUIDE_DIRECT_H

#include <QDialog>
#include <QFrame>
#include <QState>

namespace Ui {
class saas_guide_direct;
}

class saas_guide_direct : public QFrame
{
    Q_OBJECT

public:
    explicit saas_guide_direct(int ssqr, QWidget *parent);
    ~saas_guide_direct();

private slots:
    void toUpdateS3();
    void to_s3_down();
    void to_s3_up();
private:
    void appItem(const QString &p1, const QString &p2, const QString &p3);
    Ui::saas_guide_direct *ui;
    QState *s1, *s2, *s3;
};

#endif // SAAS_GUIDE_DIRECT_H
