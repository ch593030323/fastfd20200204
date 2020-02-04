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

#ifndef QFMLISTVIEW_H
#define QFMLISTVIEW_H

#include <QListView>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include <QFileSystemWatcher>
#include<QTableView>
#include<QStandardItemModel>
#include<QScrollBar>
class FMListViewModel;
class FMListView : public QTableView
{
Q_OBJECT
public:
    explicit FMListView(QWidget *parent = 0);
    ~FMListView();
    QStringList selectedFiles();
    QString getRootDir();
    bool fileEnabled;
protected:
    void keyPressEvent( QKeyEvent * event );
    void dropEvent( QDropEvent * e );
    void dragEnterEvent( QDragEnterEvent * event );
    void dragMoveEvent(QDragMoveEvent *event);
    void getFreeSpace(const QString& path);
    void focusInEvent( QFocusEvent * event );
    void focusOutEvent( QFocusEvent * event );
signals:
    void keyUpOrDownPressed();
    void copyFiles( const QStringList& files );
    void rootPathChanged ( const QString& );
    void focusGained();
    void dffree(QString free);
    void foucsout(bool test);
public slots:
    void setRootPath( const QString& path , bool isRoot = false);
#ifdef Q_WS_MAC
    void lateUpdate();
#endif
protected:
    QString cur_root_dir;//当前的根目录
    QString rootDir;
    QString freeSpace;
    QFileSystemWatcher watcher;
    QModelIndexList selectionList;
    QModelIndex curIndex;
};


#endif // QFMLISTVIEW_H
