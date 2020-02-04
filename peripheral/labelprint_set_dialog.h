#ifndef LABELPRINT_SET_DIALOG_H
#define LABELPRINT_SET_DIALOG_H

#include <QDialog>
#include <QComboBox>
class labelprint_set_tagbarcode_dialog;
//1.之后需要单独判断是否有端口重复

class Ui_labelprint_set_dialog;
class labelprint_set_dialog : public QWidget
{
    Q_OBJECT

public:
    explicit labelprint_set_dialog(QWidget *parent = 0);
    ~labelprint_set_dialog();
    labelprint_set_tagbarcode_dialog *st_dialog;
    Ui_labelprint_set_dialog *ui;
public slots:
    void tosave();

    void toexit();
private slots:
    void test_labelprint();

    void update_labelprint_path(const QString &pathtype);

    void update_labelprint_onebartype(const QString &printtype);

    void update_combobox_textcolor(QComboBox *com, QColor color);

    void toportbyvidpid();
private:
    //若是com是可编辑的，直接赋值；否则索引赋值
    void update_combobox(QComboBox *com, const QString &index);

private:
};

#endif // LABELPRINT_SET_DIALOG_H
