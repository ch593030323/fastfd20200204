#include "lds_tableview_delegate_time.h"
#include <QDateTimeEdit>
#include <QTimeEdit>
#include "lds_datetimeedit.h"

lds_tableview_delegate_time::lds_tableview_delegate_time(QVariant::Type type, QObject *parent)
    :lds_tableview_delegate_virtual(parent)
    , _type(type)
{

}

void lds_tableview_delegate_time::setDisplayFormat(QVariant::Type type)
{
    _type = type;
}

QWidget *lds_tableview_delegate_time::createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    lds_datetimeEdit *editor=new lds_datetimeEdit(aParent);
    editor->setDisplayFormat(dataTime2Format());
    editor->enablePopAdjustPostion();
    editor->setDateTime(index.data().toDateTime());
    return editor;
}

void lds_tableview_delegate_time::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    lds_datetimeEdit *editor0 = qobject_cast<lds_datetimeEdit *>(editor);
    model->setData(index, dateTime2Varint(editor0->dateTime()));
}

void lds_tableview_delegate_time::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    lds_datetimeEdit *editor0=qobject_cast<lds_datetimeEdit *>(editor);
    editor0->setDateTime(index.data().toDateTime());
}

QVariant lds_tableview_delegate_time::dateTime2Varint(const QDateTime &dt) const
{
        if(_type == QVariant::Date) {
            return dt.date();
        }
        if(_type == QVariant::Time) {
            return dt.time();
        }
        return dt;
}

QString lds_tableview_delegate_time::dataTime2String(const QDateTime &dt) const
{
    return dt.toString(dataTime2Format());
}

QString lds_tableview_delegate_time::dataTime2Format() const
{
    if(_type == QVariant::Date) {
        return "yyyy-MM-dd";
    }
    if(_type == QVariant::Time) {
        return "hh:mm:ss";
    }
    return "yyyy-MM-dd hh:mm:ss";
}
