#include "kds_dishtype_setting.h"
#include "ui_kds_dishtype_setting.h"
#include "lds_combobox.h"
#include <QLineEdit>
#include "lds_pushbutton.h"
#include <QtDebug>
#include <QStyleOptionViewItem>
#include "lds.h"
#include "cjson_help.h"

kds_dishtype_setting::kds_dishtype_setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kds_dishtype_setting)
{
    ui->setupUi(this);
    ui->tableWidget->appHorizontalHeader("row", "");
    ui->tableWidget->appHorizontalHeader("dish_type", QObject::tr("小类编码"));
    ui->tableWidget->appHorizontalHeader("kitchen_view", QObject::tr("厨显"));
    ui->tableWidget->appHorizontalHeader("button", "");
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("row"), 30);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("dish_type"), 100);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("kitchen_view"), 150);
    ui->tableWidget->setColumnWidth(ui->tableWidget->fieldIndex("button"), 40);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setItemDelegateForColumn(ui->tableWidget->fieldIndex("row"), new itemDelegate(ui->tableWidget));
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);

    cJSON_path p(lds::sysconf->value("kds_dishtype_setting/type_kitchen_map").toByteArray());

    connect(ui->toolButton_add, SIGNAL(clicked()), this, SLOT(toadd()));
    connect(ui->toolButton_ok, SIGNAL(clicked()), this, SLOT(took()));
}

kds_dishtype_setting::~kds_dishtype_setting()
{
    delete ui;
}

//Sets the given widget on the item at the given index, passing the ownership of the widget to the viewport.
void kds_dishtype_setting::toadd()
{
    int row = ui->tableWidget->rowCount();
    lds_pushbutton *button = lds_pushbutton::createButton(QPixmap(":/kds/image/list-remove.png"), QSize(20, 20));
    connect(button, SIGNAL(clicked()), this, SLOT(toremove()));
    button->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->insertRow(row);
    ui->tableWidget->setIndexWidget(ui->tableWidget->index(row, "dish_type"), new QLineEdit);
    ui->tableWidget->setIndexWidget(ui->tableWidget->index(row, "kitchen_view"),
                                    lds_ComboBox::createCom(" SELECT vch_plan_id, vch_plan_name FROM cey_bt_kitchen_view; "));
    ui->tableWidget->setIndexWidget(ui->tableWidget->index(row, "button"), button);
}

void kds_dishtype_setting::toremove()
{
    lds_pushbutton *button = qobject_cast<lds_pushbutton *>(this->sender());
    if(button == 0)
        return;
    QModelIndex index = ui->tableWidget->indexAt(button->pos());
    if(!index.isValid())
        return;
    ui->tableWidget->removeRow(index.row());
}

void kds_dishtype_setting::took()
{
    //dish_type_no 必须为数字
    //dish_type_kitchen_view_no 必须为数字
    // ["01=12", "25=26"]
    QString json_value;
    for(int row = 0; row < ui->tableWidget->rowCount(); row ++) {
        json_value += "\"" + ui->tableWidget->text(row, "dish_type") + "=" + ui->tableWidget->text(row, "kitchen_view") + "\",";
    }
    lds::sysconf->setValue("kds_dishtype_setting/type_kitchen_map", "[" + json_value + "]");

    this->accept();
}

void kds_dishtype_setting::itemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt;
    opt = option;
    initStyleOption(&opt, index);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    const QWidget *widget = option.widget;
#else
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option);
    const QWidget *widget = v3? v3->widget : 0;
#endif
    QStyle *style = widget? widget->style() : QApplication::style();

    if(opt.state & QStyle::State_Selected)
        painter->setPen(opt.palette.brush(QPalette::HighlightedText).color());
    else
        painter->setPen(opt.palette.brush(QPalette::Text).color());
    opt.text = QString::number(index.row() + 1);
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}
