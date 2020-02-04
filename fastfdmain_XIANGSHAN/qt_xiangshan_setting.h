#ifndef QT_XIANGSHAN_SETTING_H
#define QT_XIANGSHAN_SETTING_H

#include <QDialog>
#include "qt_xiangshan_copy_file.h"

namespace Ui {
class qt_XIANGSHAN_setting;
}

class qt_XIANGSHAN_setting : public QDialog
{
    Q_OBJECT

public:
    explicit qt_XIANGSHAN_setting(QWidget *parent = 0);
    ~qt_XIANGSHAN_setting();


    QLabel *get_label_shop_info_size();
signals:
    void signal_copy_file(const QStringList &fileInfoFrom, const QString &dirTo);
    void signal_copy_file(const QString &filepathFrom, const QString &dirTo, const QString &baseNameTo);
private slots:
    void toCancel();
    void toccbDefault();
    void toOk();
    void toSelectAdvPath();
    void toSelectShopInfo();
    void toSelectPage(const QModelIndex &index);

    void toUpanRefresh();
    void toUpanPicListRefresh();

    void toShowPicList();
    void toAdvListClick(const QModelIndex &index);

    void toUpdateMsg(bool success, const QString &msg);
    void toCopyFiles(const QStringList &filepaths, const QString &dirTo);
    void toCopyFile(const QString &filepath, const QString &dirTo, const QString &baseNameTo);
private:
    void dataOp(bool save);
    void comSetValue(QComboBox *com, const QString &value);

    void showLoading();
    void hideLoading();
private:
    QPixmap colorPixmap(const QColor &color, int w, int h);
private:
    Ui::qt_XIANGSHAN_setting *ui;
    QStandardItemModel *model_list;
    qt_XIANGSHAN_copy_file copy_file_ex;
};

#endif // QT_XIANGSHAN_SETTING_H
