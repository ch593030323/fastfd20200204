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

#include <qevent.h>
#include <QPainter>
#include <QTextStream>
#include <QUndoStack>
#include "s_document.h"
#include "s_commands.h"
#include <QDebug>
#include <QLineEdit>
#include <QPushButton>
#include <QDialog>


static const int resizeHandleWidth = 16;

/******************************************************************************
** Shape
*/

const QSize Shape::minSize(30, 30);

Shape::Shape(Type type, const QColor &color, const QRect &rect, const QString &text, const QString &fsize, bool ismoving, bool istext, int rotate)
{
    m_type=(type);
    m_rect=(rect);
    m_color=(color);
    _text=(text);
    _fontsize=(fsize);
    _ismoving=(ismoving);
    _istext=(istext);
    this->rotate = rotate;
}

Shape::Type Shape::type() const
{
    return m_type;
}

QRect Shape::rect() const
{
    return m_rect;
}

QColor Shape::color() const
{
    return m_color;
}

QString Shape::text() const
{
    return _text;
}

QString Shape::name() const
{
    return m_name;
}

QRect Shape::resizeHandle() const
{
    QPoint br = m_rect.bottomRight();
    return QRect(br - QPoint(resizeHandleWidth, resizeHandleWidth), br);
}

QString Shape::typeToString(Type type)
{
    QString result;

    switch (type) {
    case Rectangle:
        result = QLatin1String("Rectangle");
        break;
    case Circle:
        result = QLatin1String("Circle");
        break;
    case Triangle:
        result = QLatin1String("Triangle");
        break;
    case TwoBarCode:
        result = QLatin1String("twobarcode");
        break;
    case OneBarCode:
        result = QLatin1String("onebarcode");
        break;
    default:
        break;
    }

    return result;
}

Shape::Type Shape::stringToType(const QString &s, bool *ok)
{
    if (ok != 0)    *ok = true;

    if (s == QLatin1String("Rectangle"))   return Rectangle;
    if (s == QLatin1String("Circle"))  return Circle;
    if (s == QLatin1String("Triangle"))   return Triangle;
    if (s == QLatin1String("twobarcode"))    return TwoBarCode;
    if (s == QLatin1String("onebarcode"))    return OneBarCode;

    if (ok != 0)   *ok = false;
    return Rectangle;
}

/******************************************************************************
** Document
*/

S_Document::S_Document(QWidget *parent)
    : QWidget(parent), m_currentIndex(-1), m_mousePressIndex(-1), m_resizeHandlePressed(false)
{
    m_undoStack = new QUndoStack(this);

    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    QPalette pal = palette();
    pal.setBrush(QPalette::Base, QPixmap(":/icon/ui/icons/background.png"));
    pal.setColor(QPalette::HighlightedText, Qt::red);
    setPalette(pal);
}

QString S_Document::addShape(const Shape &shape)
{
    QString name = Shape::typeToString(shape.type());
    name = uniqueName(name);

    m_shapeList.append(shape);
    m_shapeList[m_shapeList.count() - 1].m_name = name;
    setCurrentShape(m_shapeList.count() - 1);

    return name;
}

void S_Document::deleteShape(const QString &shapeName)
{
    int index = indexOf(shapeName);
    if (index == -1)   return;

    update(m_shapeList.value(index).rect());

    m_shapeList.removeAt(index);

    if (index <= m_currentIndex) {
        m_currentIndex = -1;
        if (index == m_shapeList.count())   --index;
        setCurrentShape(index);
    }
}


void S_Document::deleteAll()
{
}

Shape S_Document::shape(const QString &shapeName) const
{
    int index = indexOf(shapeName);
    if (index == -1)     return Shape();
    return m_shapeList.value(index);
}

void S_Document::setShapeRect(const QString &shapeName, const QRect &rect)
{
    int index = indexOf(shapeName);
    if (index == -1)   return;

    Shape &shape = m_shapeList[index];

    update(shape.rect());
    update(rect);

    shape.m_rect = rect;
}

void S_Document::setShapeColor(const QString &shapeName, const QColor &color)
{

    int index = indexOf(shapeName);
    if (index == -1) return;

    Shape &shape = m_shapeList[index];
    shape.m_color = color;

    update(shape.rect());
}

void S_Document::setShapefontsize(const QString &shapeName, const QString &fs)
{
    int index = indexOf(shapeName);
    if (index == -1)    return;

    Shape &shape = m_shapeList[index];
    shape._fontsize = fs;
}

void S_Document::setShapeistext(const QString &shapeName, bool value)
{
    int index = indexOf(shapeName);
    if (index == -1)    return;

    Shape &shape = m_shapeList[index];
    shape._istext = value;
}

void S_Document::setShapeRotate(const QString &shapeName, int rotate)
{
    int index = indexOf(shapeName);
    if (index == -1)    return;

    Shape &shape = m_shapeList[index];
    shape.rotate = rotate;
}


QUndoStack *S_Document::undoStack() const
{
    return m_undoStack;
}

bool S_Document::load(QTextStream &stream)
{
    m_shapeList.clear();

    while (!stream.atEnd()) {
        QString shapeType, shapeName, colorName;
        int left, top, width, height;
        stream >> shapeType >> shapeName >> colorName >> left >> top >> width >> height;
        if (stream.status() != QTextStream::Ok)    return false;
        bool ok;
        Shape::Type type = Shape::stringToType(shapeType, &ok);
        if (!ok)    return false;
        QColor color(colorName);
        if (!color.isValid())    return false;

        Shape shape(type);
        shape.m_name = shapeName;
        shape.m_color = color;
        shape.m_rect = QRect(left, top, width, height);

        m_shapeList.append(shape);
    }

    m_currentIndex = m_shapeList.isEmpty() ? -1 : 0;

    return true;
}

void S_Document::save(QTextStream &stream)
{
    for (int i = 0; i < m_shapeList.count(); ++i) {
        const Shape &shape = m_shapeList.value(i);
        QRect r = shape.rect();
        stream << Shape::typeToString(shape.type()) << QLatin1Char(' ')
               << shape.name() << QLatin1Char(' ')
               << shape.color().name() << QLatin1Char(' ')
               << r.left() << QLatin1Char(' ')
               << r.top() << QLatin1Char(' ')
               << r.width() << QLatin1Char(' ')
               << r.height();
        if (i != m_shapeList.count() - 1)      stream << QLatin1Char('\n');
    }
    m_undoStack->setClean();
}

QString S_Document::fileName() const
{
    return m_fileName;
}

void S_Document::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

int S_Document::indexAt(const QPoint &pos) const
{
    for (int i = m_shapeList.count() - 1; i >= 0; --i) {
        if (m_shapeList.value(i).rect().contains(pos))    return i;
    }
    return -1;
}

void S_Document::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    int index = indexAt(event->pos());
    if (index != -1) {
        setCurrentShape(index);

        const Shape &shape = m_shapeList.value(index);
        m_resizeHandlePressed = shape.resizeHandle().contains(event->pos());

        if (m_resizeHandlePressed)     m_mousePressOffset = shape.rect().bottomRight() - event->pos();
        else    m_mousePressOffset = event->pos() - shape.rect().topLeft();
        emit ShapeClicked(shape.rect());
    }
    m_mousePressIndex = index;
}

void S_Document::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    m_mousePressIndex = -1;
}

void S_Document::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();

    if (m_mousePressIndex == -1)   return;

    const Shape &shape = m_shapeList.value(m_mousePressIndex);
    //若不是移动打shap，则返回
    if(!shape._ismoving)return;

    QRect rect;
    if (m_resizeHandlePressed) {
        rect = QRect(shape.rect().topLeft(), event->pos() + m_mousePressOffset);
    } else {
        rect = shape.rect();
        QPoint p(event->pos() - m_mousePressOffset);
        rect.moveTopLeft(QPoint(qMax(0, p.x()), qMax(0, p.y())));
    }

    QSize size = rect.size().expandedTo(Shape::minSize);
    rect.setSize(size);

    m_undoStack->push(new SetShapeRectCommand(this, shape.name(), rect));
    emit ShapeClicked(rect);
}

void S_Document::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
//    int index = indexAt(event->pos());
//    if (index != -1) {
//        setCurrentShape(index);


//        Shape &shape = m_shapeList[index];

//        GetDataDialog d(this);
//        d.setText(QObject::tr("输入字段信息"));
//        d.setEditText(shape._text);

//        if(d.exec() == QDialog::Accepted) {
//            shape._text = d.getText();
//            update();
//        }
//    }
//    m_mousePressIndex = index;
}

static QPolygon triangle(const QRect &rect)
{
    QPolygon result(3);
    result.setPoint(0, rect.center().x(), rect.top());
    result.setPoint(1, rect.right(), rect.bottom());
    result.setPoint(2, rect.left(), rect.bottom());
    return result;
}

void S_Document::paintEvent(QPaintEvent *event)
{
    QRegion paintRegion = event->region();
    QPainter painter(this);
    QPalette pal = palette();

    for (int i = 0; i < m_shapeList.count(); ++i) {
        const Shape &shape = m_shapeList.value(i);
        QRect rect = shape.rect();
        rect.adjust(1, 1, -resizeHandleWidth/2, -resizeHandleWidth/2);

        if (!paintRegion.contains(shape.rect()))   continue;
        QPen pen = pal.text().color();
        pen.setWidth(i == m_currentIndex ? 2 : 1);
        if(shape.text() == "BACKGROUND") {
            painter.fillRect(rect, QColor("snow"));
            pen.setWidth(1);
        } else {
            painter.setOpacity(0.7);
            painter.fillRect(rect, QColor("powderblue"));
            painter.setOpacity(1.0);
        }
        painter.setPen(pen);
//        if(!shape._ismoving)
//            painter.setBrush(gradient(shape.color(), shape.rect()));


        // paint the shape
        switch (shape.type()) {
        case Shape::Rectangle:
            painter.drawRect(rect);
            break;
        case Shape::Circle:
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawEllipse(rect);
            painter.setRenderHint(QPainter::Antialiasing, false);
            break;
        case Shape::Triangle:
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawPolygon(triangle(rect));
            painter.setRenderHint(QPainter::Antialiasing, false);
            break;
        case Shape::OneBarCode:
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawPixmap(rect.adjusted(0,15,0,0), QPixmap(":/image/onebar.png"));
            painter.setRenderHint(QPainter::Antialiasing, false);

            break;
        case Shape::TwoBarCode:
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawPixmap(rect.adjusted(0,15,0,0), QPixmap(":/image/twobar.png"));
            painter.setRenderHint(QPainter::Antialiasing, false);

            break;
        default:
            break;
        }

        // paint the resize handle
        painter.save();
        painter.setPen(Qt::transparent);
        painter.setBrush(QColor("skyblue"));
        painter.drawRect(shape.resizeHandle().adjusted(0, 0, -1, -1));
        painter.restore();
        // paint the shape name
        //painter.setBrush(pal.text());
        if (shape.type() == Shape::Triangle)    rect.adjust(0, rect.height()/2, 0, 0);
        painter.setPen(Qt::red);
        painter.drawText(rect, Qt::AlignLeft | Qt::AlignTop, shape._text);

        emit currentShapeChanged(rect);
    }
}

void S_Document::setCurrentShape(int index)
{
    QString currentName;

    if (m_currentIndex != -1)  update(m_shapeList.value(m_currentIndex).rect());

    m_currentIndex = index;

    if (m_currentIndex != -1) {
        const Shape &current = m_shapeList.value(m_currentIndex);
        update(current.rect());
        currentName = current.name();
    }

    emit currentShapeChanged(currentName);
}

int S_Document::indexOf(const QString &shapeName) const
{
    for (int i = 0; i < m_shapeList.count(); ++i) {
        if (m_shapeList.value(i).name() == shapeName)   return i;
    }
    return -1;
}

QString S_Document::uniqueName(const QString &name) const
{
    QString unique;

    for (int i = 0; ; ++i) {
        unique = name;
        if (i > 0)     unique += QString::number(i);
        if (indexOf(unique) == -1)   break;
    }

    return unique;
}

QString S_Document::currentShapeName() const
{
    if (m_currentIndex == -1)    return QString();
    return m_shapeList.value(m_currentIndex).name();
}

QList<Shape> S_Document::shapes()
{
    return m_shapeList;
}

void S_Document::keyPressEvent(QKeyEvent *e)
{
    e->accept();
    //    QString shapeName = this->currentShapeName();
    //    if (shapeName.isEmpty())
    //        return;
    //    QRect rect = this->shape(shapeName).rect();


    //    switch(e->key()) {
    //    case Qt::Key_Left:
    //        rect.setX(rect.x() - 1);
    //        this->setShapeRect(shapeName,rect);
    //        break;
    //    case Qt::Key_Up:
    //        break;
    //    case Qt::Key_Right:
    //        break;
    //    case Qt::Key_Down:
    //        break;
    //    default:break;
    //    }
}
