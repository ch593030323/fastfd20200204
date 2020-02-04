#ifndef LDS_DIALOG_INPUT3_H
#define LDS_DIALOG_INPUT3_H

#include <QDialog>

namespace Ui {
class lds_dialog_input3;
}

class lds_dialog_input3 : public QDialog
{
    Q_OBJECT

public:
    explicit lds_dialog_input3(QWidget *parent = 0);
    ~lds_dialog_input3();

public slots:
    void took();
    void toexit();
public:
    Ui::lds_dialog_input3 *ui;
};

#endif // LDS_DIALOG_INPUT3_H
