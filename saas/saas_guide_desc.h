#ifndef SAAS_GUIDE_DESC_H
#define SAAS_GUIDE_DESC_H

#include <QDialog>

namespace Ui {
class saas_guide_desc;
}

class saas_guide_desc : public QDialog
{
    Q_OBJECT

public:
    explicit saas_guide_desc(QWidget *parent = 0);
    ~saas_guide_desc();

private:
    Ui::saas_guide_desc *ui;
};

#endif // SAAS_GUIDE_DESC_H
