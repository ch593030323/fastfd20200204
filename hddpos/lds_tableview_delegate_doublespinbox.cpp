#include "lds_tableview_delegate_doublespinbox.h"
#include <QDoubleSpinBox>
#include "lds_doublespinbox.h"
#include <QtDebug>

lds_tableview_delegate_doublespinbox::lds_tableview_delegate_doublespinbox(QObject *parent)
    : lds_tableview_delegate_virtual(parent)
{
    prec = 2;
}

lds_tableview_delegate_doublespinbox::lds_tableview_delegate_doublespinbox(int decimal, QObject *parent)
    : lds_tableview_delegate_virtual(parent)
{
    prec = decimal;
}

QWidget *lds_tableview_delegate_doublespinbox::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    lds_doublespinbox *editor = new lds_doublespinbox(aParent);
    editor->setDecimals(prec);
    editor->setValue(index.data().toDouble());
    return editor;
}

void lds_tableview_delegate_doublespinbox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    lds_doublespinbox *editor2 = qobject_cast<lds_doublespinbox *>(editor);

    model->setData(index, editor2->value(), Qt::EditRole);
}

void lds_tableview_delegate_doublespinbox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
        lds_doublespinbox *editor2 = qobject_cast<lds_doublespinbox *>(editor);
        editor2->setValue(index.data(Qt::EditRole).toDouble());
}

void lds_tableview_delegate_doublespinbox::setDecimals(int prec)
{
    this->prec = prec;
}
