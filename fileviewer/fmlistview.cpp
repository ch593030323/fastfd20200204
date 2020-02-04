/*
The QeFeM project.
Copyright (C) 2010  Karim Pinter

This code is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "fmlistview.h"
#include "defines.h"
#include <QDebug>
#include <QShortcut>
#include <QUrl>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QDir>
#ifdef Q_WS_WIN
#include <windows.h>
#else
#include <QProcess>
#endif
#ifdef Q_WS_MAC
#include <QTimer>
#endif
#include <QMimeData>

FMListView::FMListView( QWidget *parent) :
    QTableView(parent)
{    
    fileEnabled = true;
    QStandardItemModel *mod = new QStandardItemModel(this);
    setModel(mod);
    mod->setColumnCount(2);
    freeSpace.clear();
    verticalHeader()->hide();//隐藏左边那列
    setSelectionBehavior(QAbstractItemView::SelectRows);//整行选择
    setShowGrid(false);
    horizontalHeader()->setVisible(false);
    setAlternatingRowColors(true);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
    horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
#endif
    setSelectionMode( QAbstractItemView::SingleSelection);
    setEditTriggers( QAbstractItemView::NoEditTriggers );
#ifdef Q_WS_WIN
    QString home( "H:\\" );
#else
    QString home( "/usr/desk/mp3" );
#endif
    this->setIconSize(QSize(38,38));
    this->verticalHeader()->setDefaultSectionSize(40);
    setRootPath( home );
    connect( &watcher, SIGNAL(directoryChanged( const QString& )), this, SLOT(setRootPath(const QString&)) );
}

FMListView::~FMListView()
{
}

void FMListView::keyPressEvent( QKeyEvent * event )
{
    QTableView::keyPressEvent( event );
    if(( event->key() == Qt::Key_Up )||( event->key() == Qt::Key_Down )
            ||( event->key() == Qt::Key_Home ) || ( event->key() == Qt::Key_End )
            ||( event->key() == Qt::Key_PageDown )||( event->key() == Qt::Key_PageUp ))
    {
        emit keyUpOrDownPressed();
    }
}

void FMListView::dropEvent( QDropEvent* e )
{
    QStringList files;
    int i;
    for( i=0; i < e->mimeData()->urls().count(); i++ )
    {
        QString file( e->mimeData()->urls()[i].toString() );
        if( file.contains("file:///", Qt::CaseInsensitive ))
        {
#ifdef Q_WS_WIN
            file.remove( 0, 8 );
#else
            file.remove( 0, 7 );
#endif
            files.append( file );
        }
    }
    if( i > 0 )
    {
        emit copyFiles( files );
    }
    e->acceptProposedAction();
}


void FMListView::dragEnterEvent( QDragEnterEvent * event )
{
    if( event->mimeData()->hasUrls() &&
            (( event->proposedAction() == Qt::CopyAction) || ( event->proposedAction() == Qt::MoveAction)))
    {
        event->acceptProposedAction();
    }
}

void FMListView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}


QStringList FMListView::selectedFiles()
{
    QModelIndex index;
    QStringList slist;
    if( this->hasFocus() )
    {

        QItemSelectionModel *selections = selectionModel();
        QModelIndexList list = selections->selectedRows();
        if(list.size()<=0) {
            return slist;
        }
        // QModelIndexList list = selectedIndexes();
        if( list.at(0).data().toString().at(0)=='.' && list.at(0).data().toString().at(1)=='.' )
        {
            list.removeFirst();
        }

        // (QModelIndex index, selected)
        foreach( index, list)
        {

            slist.append( index.data().toString() );
        }
    }
    else
    {
        foreach( index, selectionList)
        {
            if( index.row() == 0 )
            {
                if( !(index.data().toString().at(0)=='.' && index.data().toString().at(1)=='.') )
                {
                    slist.append( index.data().toString() );
                }
            }
            else
            {
                slist.append( index.data().toString() );
            }
        }
    }
    return slist;
}

static inline QString humanReadableSize(int cuantity)
{
    QStringList myunits;
    myunits<<"B"<<"KB"<<"MB"<<"GB"<<"TB";
    int i=0;

    while (cuantity>=1024) {
        cuantity=cuantity/1024;
        i++;
        if(i==4)break;
    }

    return QString("%1 %2").arg(QString::number(cuantity,'f',1)).arg(myunits.at(i));
}

void FMListView::setRootPath(const QString& path,  bool isRoot)
{
    QDir rootpath(path);
    if (!rootpath.exists()/* || path == "/"*/) {
        return;
    }

    bool changed = true;
    selectionList.clear();
    if(isRoot) {
        cur_root_dir = path;
    }
    if( path == rootDir )
    {
        changed = false;
    }
    else
    {
        //TODO: add watcher to different thread slows down the responsiveness
        watcher.removePath( rootDir );
        rootDir.clear();
        rootDir.append( path );
        watcher.addPath( rootDir );
    }
    QDir dir( path );
    QStringList dirs = dir.entryList( QDir::AllEntries | QDir::NoDot,QDir::DirsFirst | QDir::Type);
    //QFileInfoList dirs2 = dir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot,QDir::DirsFirst | QDir::Type | QDir::Size);
    QStandardItemModel* mod = qobject_cast<QStandardItemModel*>( model() );
    mod->removeRows(0, mod->rowCount());

    // getFreeSpace( path );
    QString firstLine;
    firstLine.append("..");
    firstLine.append("\t");
    firstLine.append("..");
    emit dffree(freeSpace);
    //    firstLine.append(QObject::tr("回到上级目录"));

    for( int i = 0; i < dirs.count(); i++ )
    {
        QString full( path );
        full.append("/");
        full.append( dirs.at(i) );
        QFileInfo inf( full );
        QStandardItem *item = NULL;
        QStandardItem *itemsize = NULL;

        if( inf.isDir() || inf.isBundle() )
        {
            if(( (dirs.at(i)!="up")&&(dirs.at(i)!="lang")&& (dirs.at(i)!="data") && (dirs.at(i)!="ie") ) || path == "/mnt/usb") {//非/mnt目录下的data 显示
                if(i==0 && dirs.at(i) == "..") {
                    if(path== cur_root_dir) {//"/usr/desk" || path == "/mnt/usb" || path == "/mnt" ) {
                    } else {
                        item = new QStandardItem( style()->standardIcon( QStyle::SP_ArrowUp ), firstLine  );
                        itemsize = new QStandardItem(QObject::tr("返回上一目录"));
                    }
                }
                else
                {
                    //  zheng  chang   chuang  jian

                    item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), dirs.at(i) );
                    itemsize = new QStandardItem(QObject::tr("进入当前目录"));

                }
            }
        }
        else if( inf.isFile() )
        {
            if((path.left(8) !="/mnt/usb") && (( inf.suffix().compare( "sh", Qt::CaseInsensitive ) == 0)
                                               ||  ( inf.suffix().compare( "", Qt::CaseInsensitive ) == 0)
                                               ||  ( inf.suffix().compare( "qss", Qt::CaseInsensitive ) == 0)
                                               ||  ( inf.suffix().compare( "ini", Qt::CaseInsensitive ) == 0)
                                               ||  ( inf.suffix().endsWith("~"))//隐藏文件
                                               )) {

            }else{
                QString filepath = path+QDir::separator()+dirs.at(i);
                QPixmap p(filepath);
                if(p.isNull()) {
                    item = new QStandardItem( style()->standardIcon( QStyle::SP_FileIcon ), dirs.at(i));
                } else {
                    item = new QStandardItem(p.scaled(this->iconSize()), dirs.at(i));
                }
                itemsize = new QStandardItem( humanReadableSize( inf.size() ) );
            }

            //只有mp3 格式才会显示
            //            if( ( inf.suffix().compare( "mp3", Qt::CaseInsensitive ) == 0) ||
            //                    ( inf.suffix().compare( "wma", Qt::CaseInsensitive ) == 0) ||
            //                    ( inf.suffix().compare( "wav", Qt::CaseInsensitive ) == 0)
            //                    ) {

            //                item = new QStandardItem( style()->standardIcon( QStyle::SP_FileIcon ), dirs.at(i));
            //                itemsize = new QStandardItem( humanReadableSize( inf.size() ) );
            //            }
            if(!fileEnabled) {
                if(item) {item->setFlags(Qt::NoItemFlags);item->setBackground(QColor("aliceblue"));}
                if(itemsize) {itemsize->setFlags(Qt::NoItemFlags);itemsize->setBackground(QColor("aliceblue"));}
            }
        }
        if( item != NULL )
            mod->appendRow( item );
        if( itemsize != NULL )
            mod->setItem(mod->rowCount()-1,1,itemsize);
    }
#ifdef QEFEM_MAEMO_DEV
    QStandardItem *item = NULL;
    item = new QStandardItem( "" );
    mod->appendRow( item );
    item = new QStandardItem( style()->standardIcon( QStyle::SP_MessageBoxInformation ), rootDir );
    mod->appendRow( item );
#endif
    if(( changed )&&( this->hasFocus() ))
    {
        curIndex = model()->index(0,0);
        // selectionModel()->setCurrentIndex( curIndex, QItemSelectionModel::Select );
        selectRow(curIndex.row());
        emit rootPathChanged( rootDir );
    }
#ifdef Q_WS_MAC
    QTimer::singleShot(100, this, SLOT(lateUpdate()));
#endif
    //    this->resizeColumnToContents(1);
}

QString FMListView::getRootDir()
{
    return rootDir;
}

#ifdef Q_WS_WIN
void FMListView::getFreeSpace(const QString& path)
{
    quint64 availUser, total, avail;
    QFileInfo fi(path);
    QString dir = QDir::toNativeSeparators(fi.absoluteDir().canonicalPath());
    freeSpace.clear();
    if(GetDiskFreeSpaceExW((LPCWSTR)dir.utf16(),
                           (PULARGE_INTEGER)&availUser,
                           (PULARGE_INTEGER)&total,
                           (PULARGE_INTEGER)&avail) != 0)
    {
        availUser = availUser / 1048576;
        total = total / 1048576;
        avail = avail / 1048576;
        freeSpace.append(QString::number(availUser) );
        freeSpace.append(" mb/ ");
        freeSpace.append(QString::number(total) );
        freeSpace.append(" mb free");
    }
}

#elif defined(Q_WS_MAC)
void FMListView::getFreeSpace(const QString& path)
{

    QProcess df;
    df.start("df", QStringList() << "-g"<<"-t"<<path);
    if (!df.waitForStarted())
        return;

    if (!df.waitForFinished())
        return;

    df.readLine();
    QString res( df.readLine() );
    QStringList data = res.split(' ', QString::SkipEmptyParts);
    freeSpace.clear();
    freeSpace.append( data[3] );
    freeSpace.append(" gb/ ");
    freeSpace.append( data[2] );
    freeSpace.append(" gb free ");
    freeSpace.append( data[4] );
}
#elif defined(Q_OS_UNIX)
void FMListView::getFreeSpace(const QString& path)
{

    QProcess df;
    df.start("df", QStringList() <<"-h"<<path);
    if (!df.waitForStarted())
        return;

    if (!df.waitForFinished())
        return;

    df.readLine();
    QString res( df.readLine() );
    QStringList data = res.split(' ', QString::SkipEmptyParts);
    freeSpace.clear();
    freeSpace.append( data[3] );
    // freeSpace.append(" gb/ ");
    //  freeSpace.append( data[2] );
    //  freeSpace.append(" gb free ");
    //  freeSpace.append( data[4] );
}
#endif

void FMListView::focusInEvent( QFocusEvent * event )
{
    QTableView::focusInEvent(event);
    //    if( selectionModel() == NULL )
    //        return;
    //    if( !selectionList.isEmpty() )
    //    {
    //        foreach (QModelIndex index, selectionList) {
    //            selectionModel()->select( index, QItemSelectionModel::Select );
    //            //selectRow(index.row());
    //        }
    //    }
    //    else
    //    {
    //        curIndex = model()->index(0,0);
    //    }
    //    //selectionModel()->setCurrentIndex( curIndex, QItemSelectionModel::Rows );

    //    selectRow(curIndex.row());
    //    emit focusGained();
    //     emit foucsout(true);
}

void FMListView::focusOutEvent( QFocusEvent * event )
{
    QTableView::focusOutEvent(event);
    //QItemSelectionModel *selections = selectionModel();
    //    selectionList = selectionModel()->selectedRows();

    //    // selectionList = selectedIndexes();
    //    curIndex = selectionModel()->currentIndex();
    //    selectionModel()->clear();
    //    emit foucsout(false);
}

#ifdef Q_WS_MAC
void FMListView::lateUpdate()
{
    QDir dir( rootDir );
    QStringList dirs = dir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot,QDir::DirsFirst | QDir::Type);
    QStandardItemModel* mod = qobject_cast<QStandardItemModel*>( model() );
    mod->removeRows(0, mod->rowCount());
    QString firstLine("..");
    firstLine.append("\t");
    firstLine.append(freeSpace);
    QStandardItem *itemUp = new QStandardItem( style()->standardIcon( QStyle::SP_ArrowUp ), firstLine );
    mod->appendRow( itemUp );
    for( int i = 0; i < dirs.count(); i++ )
    {
        QString full( rootDir );
        full.append("/");
        full.append( dirs.at(i) );
        QFileInfo inf( full );
        QStandardItem *item = NULL;
        if( inf.isDir() || inf.isBundle() )
        {
            item = new QStandardItem( style()->standardIcon( QStyle::SP_DirIcon ), dirs.at(i) );

        }
        else //if( inf.isFile() )
        {
            item = new QStandardItem( style()->standardIcon( QStyle::SP_FileIcon ), dirs.at(i) );

        }
        if( item != NULL )
            mod->appendRow( item );
    }
    if( this->hasFocus() )
    {
        curIndex = model()->index(0,0);
        selectionModel()->setCurrentIndex( curIndex, QItemSelectionModel::Select );
    }
}
#endif
