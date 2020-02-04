#ifndef LABELPRINT_GOODINFO_SELECT_DIALOG_H
#define LABELPRINT_GOODINFO_SELECT_DIALOG_H

#include <QDialog>
class QRadioButton;
namespace Ui {
class labelprint_goodinfo_select_dialog;
}

class labelprint_goodinfo_select_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit labelprint_goodinfo_select_dialog(const QStringList selecteds, QWidget *parent = 0);
    ~labelprint_goodinfo_select_dialog();
    void setup(const QStringList &checks);
    void setexplaintext(const QString &text);
    QString whoischecked();
private:
    Ui::labelprint_goodinfo_select_dialog *ui;
    QStringList _selecteds;
    QList<QRadioButton*> rbs;
};

#endif // LABELPRINT_GOODINFO_SELECT_DIALOG_H
