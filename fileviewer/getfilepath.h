#ifndef GETFILEPATH_H
#define GETFILEPATH_H

#include <QDialog>
#include <QModelIndex>
#include <QFileDialog>
#include <QRadioButton>

namespace Ui {
class getFilePath;
}

class getFilePath : public QDialog
{
    Q_OBJECT

public:
    explicit getFilePath(QWidget *parent = 0);
    ~getFilePath();

    void setCurFilePath(const QString &filepath);
    void showDirsOnly(bool f);
    QString retpath;//点确定是返回的路径
    void setOkButtonText(const QString &text);
    void showFilename();
signals:
    void signalconduct(int step, int total);
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_close_clicked();

    void on_pushButton_copy_pressed();

    void on_pushButton_paste_clicked();

    void on_pushButton_del_clicked();

    void on_pushButton_rename_clicked();

    void toupdateGuide(QString path,QString name);

    void toquicklygoin(const QModelIndex &index);

    bool deleteFile(const QString &filepath);
    bool deleteDir(const QString &path); //若是文件 则什么都不做，返回true
    bool copyDir(const QString &frompath, const QString &topath);//文件夹 copy 文件夹
    bool copyFile(const QString &fromfilepath, const QString &topath);//文件 copy 文件
    bool newDir(const QString &filepath);//新建一个空的文件夹
    bool newFile(const QString &filepath);//新建一个文件


//    void on_pushButton_go_clicked();

    void toupdateprogress(int step, int total);

    void toPanToggled(bool checked);

    void on_pushButton_down_clicked();

    void on_pushButton_up_clicked();

private:
    void copy_pasteupdate();
    void updatecopy_paste(bool f);
    bool checkfile(const QString &filepath);
private:
    Ui::getFilePath *ui;
    QString   path_arg;
    QString filepathfrom;
    QString pathto;
    bool copychecked;
    typedef QPair<QRadioButton *, QString> radio_strPair;
    QList<radio_strPair> radio_str_pair_list;
    QPoint getGlobalCenterPopup(QWidget *parent, QWidget *pop);

public:
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    static QString	getExistingDirectory ( QWidget * parent = 0,
                                           const QString & caption = QString(),
                                           const QString & dir = QString(),
                                           QFileDialog::Options options = QFileDialog::ShowDirsOnly );
    static QString	getOpenFileName ( QWidget * parent = 0,
                                      const QString & caption = QString(),
                                      const QString & dir = QString(),
                                      const QString & filter = QString(),
                                      QString * selectedFilter = 0,
                                      QFileDialog::Options options = 0 );
    static QStringList	getOpenFileNames ( QWidget * parent = 0, const QString & caption = QString(), const QString & dir = QString(), const QString & filter = QString(), QString * selectedFilter = 0, QFileDialog::Options options = 0 );
    static QString	getSaveFileName ( QWidget * parent = 0,
                                      const QString & caption = QString(),
                                      const QString & dir = QString(),
                                      const QString & filter = QString(),
                                      QString * selectedFilter = 0,
                                      QFileDialog::Options options = 0 );
    //////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif // GETFILEPATH_H
