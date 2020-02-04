#include "lds_tableview_delegate_check.h"
#include <QCheckBox>
#include <QApplication>
#include <QTableView>
#include <QPainter>
#include <QVariant>
#include <QtDebug>


lds_tableview_delegate_check::lds_tableview_delegate_check(QVector<int> &checkcolumn_vector, QObject *tableview, bool ischeckable) :
    lds_tableview_delegate_virtual(tableview)
{
    dtype = just_for_column;
    uncheckkey = "N";
    checkkey = "Y";
    this->checkcolumn_vector = checkcolumn_vector;
    this->ischeckable = false;
    if(ischeckable) {
        setClickable();
    }
}

lds_tableview_delegate_check::lds_tableview_delegate_check(int checkrow, int checkcolumn, QObject *tableview, bool ischeckable):
    lds_tableview_delegate_virtual(tableview)
{
    uncheckkey = "N";
    checkkey = "Y";

    dtype = just_for_item;
    this->checkrow = checkrow;
    this->checkcolumn = checkcolumn;
    this->ischeckable = false;
    if(ischeckable) {
        setClickable();
    }
}

lds_tableview_delegate_check::lds_tableview_delegate_check(int checkcolumn, QObject *tableview, bool ischeckable):
    lds_tableview_delegate_virtual(tableview)
{
    dtype = just_for_column;
    uncheckkey = "N";
    checkkey = "Y";
    this->checkcolumn_vector.clear();
    this->checkcolumn_vector << checkcolumn;
    this->ischeckable = false;
    if(ischeckable) {
        setClickable();
    }
}

lds_tableview_delegate_check::~lds_tableview_delegate_check()
{
    if(ischeckable&&this->parent()) {
        QTableView *v=qobject_cast<QTableView *>(this->parent());
        if(v) {
            disconnect(v,SIGNAL(clicked(QModelIndex)),this,SLOT(inter_clicked(QModelIndex)));
        }
    }
}


void lds_tableview_delegate_check::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(just_for_column == dtype) {
        //://image/widgets_pic/check_on.png
        if(checkcolumn_vector.contains(index.column())) {
            goto paintcheck;
        }
    } else  if(just_for_item == dtype) {
        //://image/widgets_pic/check_on.png
        if(checkrow == index.row() && checkcolumn == index.column()) {
            goto paintcheck;
        }
    }
    lds_tableview_delegate_virtual::paintText(painter, option, index, index.data());
    return;
paintcheck:
    {
        lds_tableview_delegate_virtual::paintText(painter, option, index, QVariant());
        //iseditable
        bool iseditable = ischeckable
                &&index.flags() & Qt::ItemIsEditable
                &&option.state & QStyle::State_Enabled;

        //disable_radio_off

        QPixmap p(
                    index.data().toString()==checkkey?
                        (iseditable? ":/image/widgets_pic/check_on":":/image/widgets_pic/disable_check_on" ):
                        (iseditable? ":/image/widgets_pic/check_off":":/image/widgets_pic/disable_check_off")
                        );
        if(!p.isNull()) {
            p=p.scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter->drawPixmap(QStyle::alignedRect(option.direction, Qt::AlignCenter, p.size(), option.rect), p);
        }
        return;
    }
}

QWidget *lds_tableview_delegate_check::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(just_for_column == dtype) {
        if(checkcolumn_vector.contains(index.column())) {
            return 0;
        }
    }
    if(just_for_item == dtype) {
        if(checkrow == index.row() && checkcolumn == index.column()) {
            return 0;
        }
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void lds_tableview_delegate_check::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void lds_tableview_delegate_check::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

void lds_tableview_delegate_check::inter_clicked(const QModelIndex &index)
{
    if(index.flags()&Qt::ItemIsEditable
            && index.flags()&Qt::ItemIsSelectable
            && index.flags()&Qt::ItemIsEnabled) {
        bool f = false;
        if(just_for_column == dtype) {
            if(checkcolumn_vector.contains(index.column())) {
                f = true;
            }
        }
        if(just_for_item == dtype) {
            if(checkrow == index.row()
                    && checkcolumn == index.column()) {
                f = true;
            }
        }
        if(f) {
            QAbstractItemModel *model=qobject_cast<QTableView *>(this->parent())->model();
            if(model) {
                if(index.data().toString()==checkkey) {
                    model->setData(index, uncheckkey);
                } else {
                    model->setData(index, checkkey);
                }
                emit checkclick(index);
            }
        }
    }
}

void lds_tableview_delegate_check::setClickable()
{
    if(ischeckable) return;
    ischeckable=true;
    if(this->parent()) {
        QTableView *v=qobject_cast<QTableView *>(this->parent());
        if(v) {
            connect(v,SIGNAL(clicked(QModelIndex)),this,SLOT(inter_clicked(QModelIndex)));
        }
    }
}

bool lds_tableview_delegate_check::clickable()
{
    return ischeckable;
}
