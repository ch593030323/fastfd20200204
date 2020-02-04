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

#include <QUndoGroup>
#include <QUndoStack>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QToolButton>
#include "s_document.h"
#include "s_mainwindow.h"
#include "s_commands.h"
#include <QLineEdit>
#include <QDebug>
#include <QSettings>
#include <QPushButton>
#include <QLabel>
#include "labelprint_goodinfo_select_dialog.h"
#include "commonheader.h"
#include "uinormal_message.h"
#include "ui_s_mainwindow.h"
#include "lds.h"

static int min_off = 30;
static QColor randomColor()
{
    int r = (int) (3.0*(rand()/(RAND_MAX + 1.0)));
    switch (r) {
    case 0:
        return Qt::red;
    case 1:
        return Qt::green;
    default:
        break;
    }
    return Qt::blue;
}
static QRect BACKGROUND_rect;
static QRect randomRect(const QSize &s)
{
    QSize min = Shape::minSize;

    int left = (int) ((0.0 + s.width() - min.width())*(rand()/(RAND_MAX + 1.0)));
    int top = (int) ((0.0 + s.height() - min.height())*(rand()/(RAND_MAX + 1.0)));
    int width = (int) ((0.0 + s.width() - left - min.width())*(rand()/(RAND_MAX + 1.0))) + min.width();
    int height = (int) ((0.0 + s.height() - top - min.height())*(rand()/(RAND_MAX + 1.0))) + min.height();

    return QRect(left, top, width, height);
}

S_MainWindow::S_MainWindow(QWidget *parent)
    : QWidget(parent),
      ui(new Ui_S_Mainwindow)
{

    ui->setupUi(this);
    // 0

    connect(ui->actionAddRectangle, SIGNAL(triggered()), this, SLOT(addShape()));
    connect(ui->actionRemoveShape, SIGNAL(triggered()), this, SLOT(removeShape()));
    connect(ui->actionAddRobot, SIGNAL(triggered()), this, SLOT(addRobot()));
    connect(ui->actionAddSnowman, SIGNAL(triggered()), this, SLOT(addSnowman()));
    //connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    //connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(ui->actionAdd_Twobarcode, SIGNAL(triggered()),this,SLOT(addtwobarcode()));
    connect(ui->actionAdd_Onebarcode, SIGNAL(triggered()),this,SLOT(addonebarcode()));
    connect(ui->actionConfirm,SIGNAL(triggered()),this,SLOT(close()));
    //connect(ui->this->actionSetting,SIGNAL(triggered()),this,SLOT(setting()));
    connect(ui->documentTabs, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));

    m_undoGroup = new QUndoGroup(this);
    QAction *undoAction = m_undoGroup->createUndoAction(this);
    QAction *redoAction = m_undoGroup->createRedoAction(this);
    undoAction->setIcon(QIcon(":/image/undo.png"));
    redoAction->setIcon(QIcon(":/image/redo.png"));

    ui->toolBar->addAction(undoAction);
    ui->toolBar->addAction(redoAction);
    ui->toolBar->addWidget(new QLabel("x:"));
    ui->toolBar->addWidget(xline = new QLineEdit);xline->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel(_danwei));

    ui->toolBar->addWidget(new QLabel("y:"));
    ui->toolBar->addWidget(yline = new QLineEdit);yline->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel(_danwei));

    ui->toolBar->addWidget(new QLabel(QObject::tr("宽度") + ":"));
    ui->toolBar->addWidget(wline = new QLineEdit);wline->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel(_danwei));

    ui->toolBar->addWidget(new QLabel(QObject::tr("高度") + ":"));
    ui->toolBar->addWidget(hline = new QLineEdit);hline->setMinimumWidth(50);
    ui->toolBar->addWidget(new QLabel(_danwei));

    ui->toolBar->addWidget(new QLabel(QObject::tr("字体大小") + ":"));
    ui->toolBar->addWidget(fsline = new QComboBox);fsline->setMinimumWidth(50);

    ui->toolBar_2->addWidget(istext = new QCheckBox(QObject::tr("打印文本")));istext->setMinimumWidth(50);

    QLabel *strech_label;
    ui->toolBar_2->addWidget(strech_label = new QLabel);
    ui->toolBar_2->addWidget(new QLabel(QObject::tr("旋转")));
    ui->toolBar_2->addWidget(com_rotate = new QComboBox);com_rotate->setMinimumWidth(50);

    fsline->setEditable(true);
    fsline->addItem(QObject::tr("标准")+"(11)", "11");
    fsline->addItem(QObject::tr("倍高")+"(12)", "12");
    fsline->addItem(QObject::tr("倍宽")+"(21)", "21");
    fsline->addItem(QObject::tr("倍高倍宽")+"(22)", "22");

    com_rotate->addItem("0",    0);
    com_rotate->addItem("90",   1);
    com_rotate->addItem("180",  2);
    com_rotate->addItem("270",  3);



    connect(xline,SIGNAL(editingFinished()),this,SLOT(setXywhInfo()));
    connect(yline,SIGNAL(editingFinished()),this,SLOT(setXywhInfo()));
    connect(wline,SIGNAL(editingFinished()),this,SLOT(setXywhInfo()));
    connect(hline,SIGNAL(editingFinished()),this,SLOT(setXywhInfo()));
    connect(com_rotate,SIGNAL(currentIndexChanged(int)),this,SLOT(setXywhInfo()));
    connect(fsline,SIGNAL(currentIndexChanged(int)),this,SLOT(setXywhInfo()));
    connect(fsline,SIGNAL(editTextChanged(QString)),this,SLOT(setXywhInfo()));
    connect(istext,SIGNAL(toggled(bool)),this,SLOT(setXywhInfo()));

    addDocument(new S_Document());
    updateActions();

    this->setFixedHeight(1500);
}

void S_MainWindow::updateActions()
{
    S_Document *doc = currentDocument();
    m_undoGroup->setActiveStack(doc == 0 ? 0 : doc->undoStack());
    QString shapeName = doc == 0 ? QString() : doc->currentShapeName();

    ui->actionAddRobot->setEnabled(doc != 0);
    ui->actionAddSnowman->setEnabled(doc != 0);
    //actionAddCircle->setEnabled(doc != 0);
    ui->actionAddRectangle->setEnabled(doc != 0);
    //actionAddTriangle->setEnabled(doc != 0);
    //actionClose->setEnabled(doc != 0);
    //actionSave->setEnabled(doc != 0 && !doc->undoStack()->isClean());

    if (shapeName.isEmpty()) {
        //actionRed->setEnabled(false);
        //actionGreen->setEnabled(false);
        //actionBlue->setEnabled(false);
        ui->actionRemoveShape->setEnabled(false);
    } else {
        Shape shape = doc->shape(shapeName);
        //actionRed->setEnabled(shape.color() != Qt::red);
        //actionGreen->setEnabled(shape.color() != Qt::green);
        //actionBlue->setEnabled(shape.color() != Qt::blue);
        ui->actionRemoveShape->setEnabled(true);
    }

    if (doc != 0) {
        int index = ui->documentTabs->indexOf(doc);
        Q_ASSERT(index != -1);
        static const QIcon unsavedIcon(":/ui/filesave.png");
        ui->documentTabs->setTabIcon(index, doc->undoStack()->isClean() ? QIcon() : unsavedIcon);

        //        if (doc->undoStack()->count() == 0)
        //            doc->undoStack()->setUndoLimit(undoLimit->value());
    }
}

void S_MainWindow::openDocument()
{
    //    QString fileName ;//= QFileDi1alog::getOpenFil1eName(this);
    //    if (fileName.isEmpty())  return;

    //    QFile file(fileName);
    //    if (!file.open(QIODevice::ReadOnly)) {
    //        uinormal_message::warning(this,
    //                                  QObject::tr("File error"),
    //                                  QObject::tr("Failed to open\n%1").arg(fileName));
    //        return;
    //    }
    //    QTextStream stream(&file);

    //    S_Document *doc = new S_Document();
    //    if (!doc->load(stream)) {
    //        uinormal_message::warning(this,
    //                                  QObject::tr("Parse error"),
    //                                  QObject::tr("Failed to parse\n%1").arg(fileName));
    //        delete doc;
    //        return;
    //    }

    //    doc->setFileName(fileName);
    //    addDocument(doc);
}

QString S_MainWindow::fixedWindowTitle(const S_Document *doc) const
{
    QString title = doc->fileName();

    if (title.isEmpty())   title = QObject::tr("Unnamed");
    else  title = QFileInfo(title).fileName();

    QString result;

    for (int i = 0; ; ++i) {
        result = title;
        if (i > 0) result += QString::number(i);

        bool unique = true;
        for (int j = 0; j < ui->documentTabs->count(); ++j) {
            const QWidget *widget = ui->documentTabs->widget(j);
            if (widget == doc) {
                continue;
            }
            if (result == ui->documentTabs->tabText(j)) {
                unique = false;
                break;
            }
        }
        if (unique) {
            break;
        }
    }

    return result;
}

void S_MainWindow::addDocument(S_Document *doc)
{
    if (ui->documentTabs->indexOf(doc) != -1) {
        return;
    }
    m_undoGroup->addStack(doc->undoStack());
    ui->documentTabs->addTab(doc, fixedWindowTitle(doc));
    connect(doc, SIGNAL(currentShapeChanged(QString)), this, SLOT(updateActions()));
    connect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    connect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));

    connect(doc,SIGNAL(ShapeClicked(QRect)),this,SLOT(updateXywhInfo(QRect)));

    setCurrentDocument(doc);
}

void S_MainWindow::setCurrentDocument(S_Document *doc)
{
    ui->documentTabs->setCurrentWidget(doc);
}

S_Document *S_MainWindow::currentDocument() const
{
    return qobject_cast<S_Document*>(ui->documentTabs->currentWidget());
}

SSRPairMap S_MainWindow::get_curprintMap()
{
    return _ssrmap;
}

void S_MainWindow::removeDocument(S_Document *doc)
{
    int index = ui->documentTabs->indexOf(doc);
    if (index == -1) {
        return;
    }

    ui->documentTabs->removeTab(index);
    m_undoGroup->removeStack(doc->undoStack());
    disconnect(doc, SIGNAL(currentShapeChanged(QString)), this, SLOT(updateActions()));
    disconnect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    disconnect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));

    if (ui->documentTabs->count() == 0) {
        newDocument();
        updateActions();
    }
}

void S_MainWindow::saveDocument()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    for (; ;) {
        QString fileName = doc->fileName();

        //        if (fileName.isEmpty())
        //            fileName = QFileDi1alog::getSave1FileName(this);
        if (fileName.isEmpty()) {
            break;
        }
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)) {
            uinormal_message::warning(this, "message", "Failed to open" + fileName);
            doc->setFileName(QString());
        } else {
            QTextStream stream(&file);
            doc->save(stream);
            doc->setFileName(fileName);

            int index = ui->documentTabs->indexOf(doc);
            Q_ASSERT(index != -1);
            ui->documentTabs->setTabText(index, fixedWindowTitle(doc));
            break;
        }
    }
}

void S_MainWindow::closeDocument()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;


    if (!doc->undoStack()->isClean()) {
        int button
                = uinormal_message::warning(this,
                                            QObject::tr("Unsaved changes"),
                                            QObject::tr("Would you like to save this document?"),
                                            QObject::tr("Yes"), QObject::tr("No"));
        if (button == 0) {
            saveDocument();
        }
    }

    removeDocument(doc);
    delete doc;
}

void S_MainWindow::newDocument()
{
    addDocument(new S_Document());
    S_Document *doc = currentDocument();
    if (doc == 0) return;
}

void S_MainWindow::addShape()
{
    labelprint_goodinfo_select_dialog dialog(keysMap(), this);
    QStringList headerlabels;
    for(int c=0,count=printmodel->columnCount();c<count;c++)headerlabels.append(printmodel->horizontalHeaderItem(c)->text());
    dialog.setup(headerlabels);
    dialog.setexplaintext(QObject::tr("打印内容"));
    if(QDialog::Accepted !=dialog.exec()) return;
    QString key = dialog.whoischecked();
    if(key.isEmpty()) return;
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    Shape::Type type;

    //if (sender() == actionAddCircle)
    //type = Shape::Circle;
    /*else*/ if (sender() == ui->actionAddRectangle)
        type = Shape::Rectangle;
    //else if (sender() == actionAddTriangle)
    // type = Shape::Triangle;
    else return;

    Shape newShape(type, randomColor(), randomRect(BACKGROUND_rect.size()));
    newShape._fontsize = "11";
    newShape._text = key;
    doc->undoStack()->push(new AddShapeCommand(doc, newShape));
    //    doc->setShapefontsize(newShape.name(),"1");//初始化字体1
}

void S_MainWindow::removeShape()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())    return;
    doc->undoStack()->push(new RemoveShapeCommand(doc, shapeName));
}

void S_MainWindow::removeall()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    QList<Shape> shapes = doc->shapes();
    foreach(Shape shape, shapes) {
        doc->undoStack()->push(new RemoveShapeCommand(doc, shape.name()));
    }
    doc->undoStack()->clear();
}

void S_MainWindow::setShapeColor()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())    return;

    QColor color;

    //if (sender() == actionRed)
    //color = Qt::red;
    //else if (sender() == actionGreen)
    // color = Qt::green;
    //else if (sender() == actionBlue)
    //color = Qt::blue;
    //else
    return;

    if (color == doc->shape(shapeName).color()) {
        return;
    }

    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, color));
}

void S_MainWindow::addSnowman()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    // Create a macro command using beginMacro() and endMacro()

    doc->undoStack()->beginMacro(QObject::tr("Add snowman"));
    doc->undoStack()->push(new AddShapeCommand(doc,
                                               Shape(Shape::Circle, Qt::blue, QRect(51, 30, 97, 95))));
    doc->undoStack()->push(new AddShapeCommand(doc,
                                               Shape(Shape::Circle, Qt::blue, QRect(27, 123, 150, 133))));
    doc->undoStack()->push(new AddShapeCommand(doc,
                                               Shape(Shape::Circle, Qt::blue, QRect(11, 253, 188, 146))));
    doc->undoStack()->endMacro();
}

void S_MainWindow::addRobot()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    // Compose a macro command by explicitly adding children to a parent command

    QUndoCommand *parent = new QUndoCommand(QObject::tr("Add robot"));

    new AddShapeCommand(doc, Shape(Shape::Rectangle, Qt::green, QRect(115, 15, 81, 70)), parent);
    new AddShapeCommand(doc, Shape(Shape::Rectangle, Qt::green, QRect(82, 89, 148, 188)), parent);
    new AddShapeCommand(doc, Shape(Shape::Rectangle, Qt::green, QRect(76, 280, 80, 165)), parent);
    new AddShapeCommand(doc, Shape(Shape::Rectangle, Qt::green, QRect(163, 280, 80, 164)), parent);
    new AddShapeCommand(doc, Shape(Shape::Circle, Qt::blue, QRect(116, 25, 80, 50)), parent);
    new AddShapeCommand(doc, Shape(Shape::Rectangle, Qt::green, QRect(232, 92, 80, 127)), parent);
    new AddShapeCommand(doc, Shape(Shape::Rectangle, Qt::green, QRect(2, 92, 80, 125)), parent);

    doc->undoStack()->push(parent);
}

void S_MainWindow::about()
{
    QMessageBox::about(this, QObject::tr("About Undo"), QObject::tr("The Undo demonstration shows how to use the Qt Undo framework."));
}

void S_MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, QObject::tr("About Qt"));
}

void S_MainWindow::updateXywhInfo(const QRect &rect)
{
    xline->setFocus();//强制焦点切换，触发保存
    S_Document *doc = qobject_cast<S_Document *>(sender());
    if (doc == 0) return;
    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())    return;
    //单位  实时更新
    //长宽是会影响到实际打长宽的
    //mm dots(200) dots(300)
    Shape shape=doc->shape(shapeName);

    xline->setText(QString::number(rect.x() * _factor));
    yline->setText(QString::number(rect.y() * _factor));
    wline->setText(QString::number((rect.width() - min_off) * _factor));
    hline->setText(QString::number((rect.height() - min_off) * _factor));
    com_rotate->setCurrentIndex(com_rotate->findData(shape.rotate));

    int fsindex = fsline->findData(shape._fontsize);
    if(fsindex == -1)
        fsline->setEditText(shape._fontsize);
    else
        fsline->setCurrentIndex(fsline->findData(shape._fontsize));

    istext->setChecked(shape._istext);
    fsline->setFocus();

    istext->setEnabled(shape.type()==Shape::Rectangle);
    if(!istext->isEnabled())istext->setChecked(false);
    return;
}

void S_MainWindow::setXywhInfo()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())    return;

    int x = xline->text().toDouble()/_factor;
    int y = yline->text().toDouble()/_factor;
    int w = wline->text().toDouble()/_factor + min_off;
    int h = hline->text().toDouble()/_factor + min_off;

    doc->setShapeRect(shapeName,QRect(qMax(0,x), qMax(0,y), qMax(0,w), qMax(0,h)));
    QString font = fsline->itemText(fsline->currentIndex());
    if(font != fsline->currentText()) {
        font = fsline->currentText();
    } else {
        font = fsline->itemData(fsline->currentIndex()).toString();
        if(font.isEmpty()) {
            font = fsline->itemText(fsline->currentIndex());
        }
    }


    doc->setShapefontsize(shapeName, font);
    doc->setShapeistext(shapeName,istext->isChecked());
    doc->setShapeRotate(shapeName,com_rotate->itemData(com_rotate->currentIndex()).toInt());

    return;
}

void S_MainWindow::initDocument(const SSRPairMap &ssrmap)
{
    //!==========================1begin
    _yoff = lds::sysconf->value("label_print/height", "320").toInt();
    _xoff = lds::sysconf->value("label_print/width", "720").toInt();

    //将分辨率转为 200dpi dots 的模式
    _danwei = lds::sysconf->value("label_print/danwei", "dots").toString();
    QString fengbianlv = lds::sysconf->value("label_print/fengbianlv", "200DPI").toString();

    _factor = 1.0;
    if(_danwei == "mm") {//to dots 200dpi
        _factor = 1.0 / 8.0;
    } else if(_danwei == "dots" && fengbianlv == "200DPI") {
        _factor = 1.0;
    } else if(_danwei == "dots" && fengbianlv == "300DPI") {
        _factor = 3.0 / 2.0;
    }
    _xoff =_xoff / _factor + min_off;
    _yoff =_yoff / _factor + min_off;
    _danwei += "  ";
    //!==========================1end
    removeall();
    _ssrmap = ssrmap;
    _ssrmap.remove("BACKGROUND");
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    //RENEW ALL
    int w = lds::sysconf->value("label_print/width", "720").toFloat();
    int h = lds::sysconf->value("label_print/height", "320").toFloat();
    BACKGROUND_rect = QRect(0,0,w / _factor  + min_off,h / _factor  + min_off);
    Shape newShape(Shape::Rectangle, randomColor()
                   ,  BACKGROUND_rect, "BACKGROUND");
    newShape._ismoving = false;
    doc->undoStack()->push(new AddShapeCommand(doc, newShape));

    QStringList names = _ssrmap.keys();
    foreach(QString name, names) {
        Shape::Type type;
        type = (Shape::Type)_ssrmap[name].value(0).toInt();
        QRectF rect = _ssrmap[name].value(1).toRectF();
        Shape newShape(type, randomColor()
                       ,  QRect(qMax(0.0, double(_xoff - min_off - rect.x()))
                                ,  qMax(0.0, double(_yoff -min_off - rect.y()))
                                ,  qMax(0.0, double(rect.width() + min_off))
                                ,  qMax(0.0, double(rect.height() + min_off)))
                       ,  name
                       ,  _ssrmap[name].value(2, "11").toString()
                       ,  true
                       ,  _ssrmap[name].value(3, false).toBool()
                       ,  _ssrmap[name].value(4, 0).toInt());

        doc->undoStack()->push(new AddShapeCommand(doc, newShape));
    }
}

void S_MainWindow::set_BACKGROUND_rect(QRect rect)
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;

    doc->setShapeRect("BACKGROUND", rect);
}

void S_MainWindow::addtwobarcode()
{
    labelprint_goodinfo_select_dialog dialog(keysMap(), this);
    QStringList headerlabels;
    for(int c=0,count=printmodel->columnCount();c<count;c++)headerlabels.append(printmodel->horizontalHeaderItem(c)->text());
    dialog.setup(headerlabels);
    dialog.setexplaintext(QObject::tr("打印二维码"));
    if(QDialog::Accepted !=dialog.exec()) return;
    QString key = dialog.whoischecked();
    if(key.isEmpty()) return;

    S_Document *doc = currentDocument();
    if (doc == 0) return;

    Shape newShape(Shape::TwoBarCode, randomColor(), randomRect(BACKGROUND_rect.size()));
    newShape._text = key;
    doc->undoStack()->push(new AddShapeCommand(doc, newShape));
}

void S_MainWindow::addonebarcode()
{
    labelprint_goodinfo_select_dialog dialog(keysMap(), this);
    QStringList headerlabels;
    for(int c=0,count=printmodel->columnCount();c<count;c++)headerlabels.append(printmodel->horizontalHeaderItem(c)->text());
    dialog.setup(headerlabels);
    dialog.setexplaintext(QObject::tr("打印一维码"));
    if(QDialog::Accepted !=dialog.exec()) return;
    QString key = dialog.whoischecked();
    if(key.isEmpty()) return;

    S_Document *doc = currentDocument();
    if (doc == 0) return;

    Shape newShape(Shape::OneBarCode, randomColor(), randomRect(BACKGROUND_rect.size()));
    newShape._text = key;
    doc->undoStack()->push(new AddShapeCommand(doc, newShape));
}

SSRPairMap S_MainWindow::printMap()
{
    S_Document *doc = currentDocument();
    if (doc == 0) {
        return SSRPairMap();
    }
    QList<Shape> shapes = doc->shapes();

    SSRPairMap map;
    foreach(Shape shape, shapes) {
        //背景的话 不返回
        if(shape._text == "BACKGROUND") continue;
        QPoint p = shape.rect().topLeft();
        p = QPoint(_xoff - min_off,_yoff - min_off) - p;
        map.insert(shape._text, QVariantList()
                   << (int)shape.type()
                   << QRect(p,shape.rect().size() - QSize(min_off, min_off))
                   << shape._fontsize
                   << shape._istext
                   << shape.rotate);
    }

    return map;
}

QString S_MainWindow::printMapJson()
{
    return Map2Json(printMap());
}

QString S_MainWindow::Map2Json(const SSRPairMap &m)
{
    QString json;
    for(QMap<QString, QVariantList>::const_iterator k = m.constBegin(); k != m.end(); k ++) {
        int type = k.value().value(0).toInt();
        QRect r = k.value().value(1).toRect();
        QString fontsize = k.value().value(2).toString();
        bool istext = k.value().value(3).toBool();
        int rotate = k.value().value(4).toInt();

        json += "{";
        json += "\"name\":\"" + k.key() + "\",";
        json += "\"type\":" + QString::number(type) + ",";
        json += "\"x\":" + QString::number(r.x()) + ",";
        json += "\"y\":" + QString::number(r.y()) + ",";
        json += "\"width\":" + QString::number(r.width()) + ",";
        json += "\"height\":" + QString::number(r.height()) + ",";
        json += "\"fontsize\":\"" + fontsize + "\",";
        json += "\"istext\":" + QString::number(istext) + ",";
        json += "\"rotate\":" + QString::number(rotate);
        json += "},";
    }
    json.chop(1);
    return "{\"rect_convert\":1,\"items\":[" + json + "]}";
}

void S_MainWindow::debugMap()
{
    S_Document *doc = currentDocument();
    if (doc == 0) return;
    QList<Shape> shapes = doc->shapes();
    foreach(Shape shape, shapes) {
        //背景的话 不返回
        if(shape._text == "BACKGROUND") continue;
        QPoint p = shape.rect().topLeft();
        p = QPoint(_xoff - min_off,_yoff - min_off) - p;
        //qDebug() << "_text" << shape._text;
        //qDebug() <<"type" << (int)shape.type();
        //qDebug() <<"rect" <<  QRect(p,shape.rect().size() - QSize(min_off, min_off));
        //qDebug() <<"_fontsize" <<  shape._fontsize;
        //qDebug() <<"_istext" <<  shape._istext;
    }
}

QStringList S_MainWindow::keysMap()
{
    S_Document *doc = currentDocument();
    if (doc == 0) {
        return QStringList();
    }
    QList<Shape> shapes = doc->shapes();

    SSRPairMap map;
    QStringList keys;
    foreach(Shape shape, shapes) {
        //背景的话 不返回
        if(shape._text == "BACKGROUND") continue;
        keys << shape._text;
    }

    return keys;
}

bool S_MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    S_Document *doc = currentDocument();
    if (doc == 0) {
        return S_MainWindow::eventFilter(obj, event);
    }
    QString name = doc->currentShapeName();
    QRect rect = doc->shape(name).rect();
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    switch(keyEvent->key()) {
    case Qt::Key_Up:
        doc->setShapeRect(name, QRect(rect.x(), rect.y() - 1, rect.width(), rect.height()));
        updateXywhInfo(doc->shape(name).rect());

        return true;
    case Qt::Key_Down:
        doc->setShapeRect(name, QRect(rect.x(), rect.y() + 1, rect.width(), rect.height()));
        updateXywhInfo(doc->shape(name).rect());

        return true;
    case Qt::Key_Left:
        doc->setShapeRect(name, QRect(rect.x() - 1, rect.y(), rect.width(), rect.height()));
        updateXywhInfo(doc->shape(name).rect());

        return true;
    case Qt::Key_Right:
        doc->setShapeRect(name, QRect(rect.x() + 1, rect.y(), rect.width(), rect.height()));
        updateXywhInfo(doc->shape(name).rect());

        return true;
    default:
        return S_MainWindow::eventFilter(obj, event);
    }
}
