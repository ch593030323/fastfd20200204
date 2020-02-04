#ifndef LABELPRINT_DESIGN_IMPORT_DIALOG_H
#define LABELPRINT_DESIGN_IMPORT_DIALOG_H

#include <QDialog>
class Ui_labelprint_design_import_dialog;
class labelprint_design_import_dialog : public QDialog
{
    Q_OBJECT
public:
    explicit labelprint_design_import_dialog(QWidget *parent = 0);

    void updatecom(const QString &value="");//dir为空则通过_dir来更新；否则通过dir来更新
    QString getfilename() const;
public slots:
    void tocancel();
    void todel();
    void took();
private:
    Ui_labelprint_design_import_dialog *ui;
    QString _dir;
    QString _fname;
};

#endif // LABELPRINT_DESIGN_IMPORT_DIALOG_H
