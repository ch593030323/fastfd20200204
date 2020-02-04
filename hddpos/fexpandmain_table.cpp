#include "fexpandmain_table.h"
#include <QtDebug>
#include <QHeaderView>
#include <QPainter>
#include <QApplication>

const int fexpandmain_table::tableRowHeight = 61;

QColor fexpandmain_table::getdivColor()
{
    return divColor;
}

void fexpandmain_table::setdivColor(const QColor &c)
{
    divColor = c;
}

QColor fexpandmain_table::getnameColor()
{
    return nameColor;
}

void fexpandmain_table::setnameColor(const QColor &c)
{
    nameColor = c;
}

QColor fexpandmain_table::getnameSubColor()
{
    return nameSubColor;
}

void fexpandmain_table::setnameSubColor(const QColor &c)
{
    nameSubColor = c;
}

QColor fexpandmain_table::getdefColor()
{
    return defColor;
}

void fexpandmain_table::setdefColor(const QColor &c)
{
    defColor = c;
}

QColor fexpandmain_table::getitemBgColor()
{
    return itemBgColor;
}

void fexpandmain_table::setitemBgColor(const QColor &c)
{
    itemBgColor = c;
}

fexpandmain_table::fexpandmain_table(QWidget *parent): lds_tableview_timeout(parent)
{
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setAlternatingRowColors(false);
    this->verticalHeader()->setDefaultSectionSize(tableRowHeight);
    border = 2;
}

fexpandmain_table::~fexpandmain_table()
{
}

