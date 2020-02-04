#ifndef KDS_MAIN_INFO_PRINT_H
#define KDS_MAIN_INFO_PRINT_H

#include <QDialog>

namespace Ui {
class kds_main_info_print;
}

class kds_main_info_print : public QDialog
{
    Q_OBJECT

public:
    explicit kds_main_info_print(QWidget *parent = 0);
    ~kds_main_info_print();

private slots:
    void tocancel();
    void took();
    void toprintset();
private:
    Ui::kds_main_info_print *ui;
};

#endif // KDS_MAIN_INFO_PRINT_H
