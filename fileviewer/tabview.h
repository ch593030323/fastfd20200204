#ifndef TABVIEW_H
#define TABVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTime>
#include <QFileInfo>
#include <QProcess>
#include <QShortcut>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QDesktopServices>
#include<QMessageBox>

#include<QToolTip>
class QTableView;
namespace Ui {
class tabview;
}

class tabview : public QWidget
{
    Q_OBJECT
    Q_ENUMS(EditMode)
public:
    explicit tabview(QWidget *parent = 0);
    ~tabview();
    QStringList selectedFiles();
    QStringList selectedFilesname();
    QString curDir();
    QString filepathze;
    QStringList getStatus();
    void setRootPath( const QString& path, bool isRoot = false );
    QTime lastFocus();
    QLineEdit *getPathEdit();
    QLabel *getGuideLabel();
    QTableView *getFileTable();
    QLineEdit *getLineEditFilename_show();

    bool *fileEnabled;//table的控件
    QLineEdit *lineEditFilename;//table的控件
public:
    enum EditMode{ None, Rename, MkDir, Zip, Create, Search };
    void setEditMode( EditMode emode );

private:
    Ui::tabview *ui;
public slots:
    void dirClicked( const QModelIndex &  index );
    void dirDoubleClicked( const QModelIndex &  index );
    void copy( const QStringList& files );
    void listGotFocus();
    void rootChanged( const QString & newPath );
    void editFinished();
    void freelabel(QString free);
    void dirListUpPage();
    void dirListDownPage();

signals:
    void copyFiles( const QStringList& files, const QString& dest, bool left );
    void clickstat(QString path,QString name);
private:
    QTime lastClick;
    void setPathEditText(const QString text);
    QString currentFile;
    QString currentDir;
    bool left;
    EditMode mode;
};

#endif // TABVIEW_H
