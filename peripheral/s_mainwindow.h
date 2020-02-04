/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S_MAINWINDOW_H
#define S_MAINWINDOW_H

#include <QMainWindow>
#include "s_document.h"
#include <QKeyEvent>
#include <QUndoGroup>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

class Ui_S_Mainwindow;
typedef QMap<QString, QVariantList> SSRPairMap;
typedef QMap<QString, QString> SSMap;

class  S_MainWindow : public QWidget
{
    Q_OBJECT

public:
    S_MainWindow(QWidget *parent = 0);

    void addDocument(S_Document *doc);
    void removeDocument(S_Document *doc);
    void setCurrentDocument(S_Document *doc);
    S_Document *currentDocument() const;

    SSRPairMap get_curprintMap();
    SSRPairMap printMap();
    QString printMapJson();

    static QString Map2Json(const SSRPairMap &m);

    void debugMap();
    QStringList keysMap();
    void initDocument(const SSRPairMap &ssrmap);
    void set_BACKGROUND_rect(QRect rect);
    bool eventFilter(QObject *obj, QEvent *event);
    QStandardItemModel *printmodel;
public slots:
    void openDocument();
    void saveDocument();
    void closeDocument();
    void newDocument();

    void addShape();
    void addtwobarcode();
    void addonebarcode();
    void removeShape();
    void removeall();
    void setShapeColor();

    void addSnowman();
    void addRobot();

    void about();
    void aboutQt();

private slots:
    void updateActions();
    void updateXywhInfo(const QRect &rect);
    void setXywhInfo();

private:
    QUndoGroup *m_undoGroup;
    QLineEdit *xline, *yline, *wline, *hline;QCheckBox *istext;
    QComboBox *fsline;
    QComboBox *com_rotate;
    QString fixedWindowTitle(const S_Document *doc) const;

    SSRPairMap _ssrmap;

    int _xoff, _yoff;
    QString _danwei;
    double _factor;
    Ui_S_Mainwindow *ui;
};

#endif // S_MAINWINDOW_H
