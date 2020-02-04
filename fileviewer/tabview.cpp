#include "tabview.h"
#include "ui_tabview.h"
#include <QtDebug>

tabview::tabview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tabview)
{

    ui->setupUi(this);
    fileEnabled = &ui->dirList->fileEnabled;
    lineEditFilename = ui->lineEdit_filename;
    currentDir.clear();
#ifdef Q_WS_WIN
    currentDir.append("H:\\");
#else
    currentDir.append("/usr/desk/mp3");
#endif
    connect(ui->dirList, SIGNAL(clicked( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(ui->dirList, SIGNAL(activated( const QModelIndex& )), this, SLOT( dirDoubleClicked( const QModelIndex & ) ));
    //  connect(ui->dirList, SIGNAL(entered( const QModelIndex& )), this, SLOT( dirClicked( const QModelIndex & ) ));
    connect(ui->dirList, SIGNAL(copyFiles(const QStringList&)),this,SLOT( copy(const QStringList& ) ));
    connect(ui->dirList, SIGNAL(focusGained()),this,SLOT( listGotFocus() ));

    //connect(ui->dirList,SIGNAL(dffree(QString)),this,SLOT(freelabel(QString)));
#ifdef Q_WS_WIN
    setPathEditText( "H:\\");
#else
    setPathEditText( "/usr/desk/mp3");
#endif

    ui->pathEdit->hide();
    //    ui->pushButton_down->setAutoRepeat(true);
    //    ui->pushButton_up->setAutoRepeat(true);
    //    ui->pushButton_open->hide();

    ui->frame_filename->hide();
    ui->lineEdit_filename->hide();

//    ui->splitter->setStretchFactor(0, 0);
//    ui->splitter->setStretchFactor(1, 1);
}

tabview::~tabview()
{
    delete ui;
}

QStringList tabview::selectedFiles()
{
    QStringList list = ui->dirList->selectedFiles();

    list.removeOne("");

    QStringList res;
    QString fileName;
    QString full;
    foreach( fileName, list)
    {
        full.clear();
        full.append( currentDir );
        full.append("/");
        full.append( fileName );
        res.append( full );
    }
    return res;
}

QStringList tabview::selectedFilesname()
{
    QStringList list = ui->dirList->selectedFiles();

    list.removeOne("");
    return list;
}

QString tabview::curDir()
{
    return currentDir;
}

QStringList tabview::getStatus()
{
    return QStringList();
}

void tabview::setRootPath(const QString &path, bool isRoot)
{
    QString defpath = path;
    if(!QDir().exists(defpath)) {
#ifdef Q_OS_WIN
        defpath = "D:";
#else
        defpath = "/usr/desk";
#endif

#ifdef QT_DEBUG
        defpath = "/home";
#endif
    }

    currentFile.clear();
    ui->dirList->setRootPath(defpath, isRoot);
    currentDir.clear();
    currentDir.append(defpath);
    setPathEditText(defpath);
}

QTime tabview::lastFocus()
{
    return lastClick;
}

QLineEdit *tabview::getPathEdit()
{
    return ui->pathEdit;
}

QLabel *tabview::getGuideLabel()
{
    return ui->label_guide;
}

QTableView *tabview::getFileTable()
{
    return ui->dirList;
}

QLineEdit *tabview::getLineEditFilename_show()
{
    ui->lineEdit_filename->setVisible(true);
    ui->frame_filename->setVisible(true);

    return ui->lineEdit_filename;
}

void tabview::setEditMode(EditMode emode)
{
    mode = emode;
    switch( mode )
    {
    case None:
    {
        return;
    }
    case Rename:
    {

        break;
    }
    case MkDir:
    {
        //TODO generate dir names
        //  ui->pathEdit->setText("New_Dir");
        //  ui->pathEdit->setFocus();
        //  ui->pathEdit->selectAll();
        break;
    }
    case Zip:
    {
        //TODO generate dir names
        currentFile.clear();
        currentFile.append(  ui->pathEdit->text() );
        ui->pathEdit->setText("New.zip");
        ui->pathEdit->setFocus();
        ui->pathEdit->selectAll();
        break;
    }
    case Search:
    {
        //TODO generate dir names
        ui->pathEdit->setText("*.*");
        ui->pathEdit->setFocus();
        ui->pathEdit->selectAll();
        break;
    }
    case Create:
        break;
    default:
        return;
    }
    return;
}

void tabview::dirClicked( const QModelIndex &  index )
{
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>( index.model());
    if( model != 0 )
    {

        lastClick = QTime::currentTime ();
        QStandardItem* item = model->itemFromIndex( index.sibling(index.row(),0) );
        QString path( ui->dirList->getRootDir() );
        if(path.trimmed().isEmpty()) return;
        QString path2=path;
        QString pathname="";
        if( path.at(path.size()-1)!='/')
        {
            path.append("/");
        }
        // if( index.row() == 0 )
        // {
        //     path.append( ".." );
        // }
        // else
        {
            pathname=item->data(Qt::DisplayRole ).toString();
            path.append(pathname );

        }
        filepathze=path;
        currentFile.clear();
        currentFile.append( path );
        if(index.flags() != Qt::NoItemFlags) {
            emit clickstat(path2,pathname);
        } else {
            //            setPathEditText( path2 );
        }
        QFileInfo inf( path ) ;
        if( inf.isFile() )
        {
            if(( inf.suffix().compare( "png", Qt::CaseInsensitive ) == 0)||
                    ( inf.suffix().compare( "jpg", Qt::CaseInsensitive ) == 0)||
                    //  ( inf.suffix().compare( "bmp", Qt::CaseInsensitive ) == 0)||
                    //   ( inf.suffix().compare( "bzip2", Qt::CaseInsensitive ) == 0)||
                    //  ( inf.suffix().compare( "tar", Qt::CaseInsensitive ) == 0)||
                    //  ( inf.suffix().compare( "arj", Qt::CaseInsensitive ) == 0)||
                    ( inf.suffix().compare( "bmp", Qt::CaseInsensitive ) == 0) )
            {
                //                ui->pushButton_open->setEnabled(true);
                // mainW->startAnimation();
                //TODO make a lin version
            }
            else
            {
                //                ui->pushButton_open->setEnabled(false);

            }
        } else if(inf.isDir()) {
            //            ui->pushButton_open->setEnabled(true);
        }else{
            //            ui->pushButton_open->setEnabled(true);
        }
        //  if(pathname == "") {
        //      dirDoubleClicked(index);
        // }
    }
}

void tabview::dirDoubleClicked(const QModelIndex &index)
{
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(index.model());
    if( model != 0 )
    {
        lastClick = QTime::currentTime ();
        QStandardItem* item = model->itemFromIndex(  index.sibling(index.row(),0) );
        QString path( ui->dirList->getRootDir() );
        if(path.trimmed().isEmpty()) return;
        if( path.at(path.size()-1)!='/')
        {
            path.append("/");
        }
        QString dirs( item->data(Qt::DisplayRole ).toString() );
        if(( dirs[0] == '.' )&&( dirs[1] == '.' )&&( dirs[2] == '\t' ))
        {
            QDir dir( path);
            if( dir.cdUp() )
            {
                path.clear();
                path.append( dir.canonicalPath () );
                currentDir.clear();
                currentDir.append( path  );
                ui->dirList->setRootPath( currentDir );
                setPathEditText( currentDir );
                return;
            }
            else
            {
                return;
            }
        }
        path.append(dirs);
        QFileInfo inf( path ) ;
        //isAbsolute()
        //isBundle()
        if( inf.isDir() )
        {

            //mainW->startAnimation();
            currentFile.clear();
            //TODO check the size
            currentDir.clear();
            currentDir.append( path );
            ui->dirList->setRootPath( currentDir );
            setPathEditText( currentDir );
            return;
        }
        // if( inf.isExecutable() )
        // {
        //     QProcess::startDetached( inf.absoluteFilePath() );
        // }
        if( inf.isFile() )
        {




            //  QString url("file:///");
            //  url.append( inf.filePath() );
            //QDesktopServices::openUrl( url );

        }

    }
}

void tabview::copy(const QStringList &files)
{
    emit copyFiles( files, currentDir, left );
}

void tabview::listGotFocus()
{
    lastClick = QTime::currentTime();
}

void tabview::rootChanged(const QString &newPath)
{

    // mainW->stopAnimation();

#ifndef QEFEM_MAEMO_DEV
    //hlist->addHistoryItem( newPath );
#endif
    currentDir.clear();
    currentDir.append( newPath );
    currentFile.clear();
    currentFile.append( newPath );

    setPathEditText( newPath );
}

void tabview::editFinished()
{
#if defined(QEFEM_MAEMO_DEV)
    hidePathEdit();
#endif
    switch( mode )
    {
    case None:
    {
        QString dirName( ui->pathEdit->text() );
        QFileInfo finfo( dirName );
        if( finfo.exists() && finfo.isDir() )
        {
            currentFile.clear();
            currentFile.append( dirName );
            currentDir.clear();
            currentDir.append( dirName );
            ui->dirList->setRootPath( dirName );
        }
        else
        {
            setPathEditText( currentDir );
        }
        ui->dirList->setFocus();
        return;
    }
    case Rename:
    {
        QFileInfo info( currentFile );
        if( info.isDir() )
        {
            QDir dir( currentFile );
            QString fileName( currentDir );
            if( fileName.at(fileName.size()-1) != '/')
                fileName.append("/");
            fileName.append( ui->pathEdit->text() );
            ui->dirList->setFocus();
            if( !dir.rename( currentFile, fileName ) )
            {
                QMessageBox msgBox;
                msgBox.setText( currentFile );
                msgBox.setInformativeText("Renaming failed.");
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                setPathEditText( currentFile );
            }
            else
            {
                setPathEditText( fileName  );
            }
        }
        else if( info.isFile() )
        {
            QFile file( currentFile );
            QString fileName( currentDir );
            if( fileName.at(fileName.size()-1) != '/')
                fileName.append("/");
            fileName.append( ui->pathEdit->text() );
            ui->dirList->setFocus();
            if( !file.rename( fileName ) )
            {
                setPathEditText( currentFile );
            }
            else
            {
                setPathEditText( file.fileName() );
            }
        }
        //reload();
        //  mainW->updateStatus();
        break;
    }
    case MkDir:
    {
        mode = None;
        QDir newdir( currentDir );
        QString newPath(currentDir);

        newPath.append("/");
        newPath.append( ui->pathEdit->text() );
        if( !newdir.mkdir( newPath ) )
        {
            qDebug()<<"Creating dir failed :"<<newPath;
            //QToolTip::showText( window()->pos(), "Creating dir failed!");
        }

        setPathEditText( currentDir );
        ui->dirList->setFocus();
        //mainW->updateStatus();
        break;
    }
    case Zip:
    {
        mode = None;



        break;
    }
    case Create:
        break;
    case Search:
    {
        mode = None;
        //findTask* ftask = new findTask( currentDir,pathEdit->text() );
        // findVector.append(QPair<findTask *,QPair<QString, bool> >( ftask, QPair<QString, bool>( pathEdit->text(), true)) );
        //connect( ftask, SIGNAL( finished() ), this, SLOT( findTaskFinished() ));
        // connect( ftask, SIGNAL( getSearchResult( const QStringList& ) ), this, SLOT( readFindResult( const QStringList& ) ));
        // ftask->run();
        // setPathEditText( currentDir );
        // dirList->setFocus();
        // mainW->updateStatus();
        // break;
    }
    }
    mode = None;
    return;
}

void tabview::freelabel(QString free)
{
    //ui->label_free->setText(QObject::tr("free: ")+free);
}

void tabview::setPathEditText( QString text)
{
#ifdef Q_WS_WIN
    text.replace( QString("/"), QString("\\"));
    ui->pathEdit->setText( text );
#else
    emit   clickstat(text,"");
    // qDebug()<<"+++++++++++";
    //filepathze=text;
    ui->pathEdit->setText( text );
#endif
}

void tabview::dirListUpPage()
{
    ui->dirList->verticalScrollBar()->setValue(ui->dirList->verticalScrollBar()->value()-ui->dirList->verticalScrollBar()->pageStep());
}

void tabview::dirListDownPage()
{
    ui->dirList->verticalScrollBar()->setValue(ui->dirList->verticalScrollBar()->value()+ui->dirList->verticalScrollBar()->pageStep());
}

