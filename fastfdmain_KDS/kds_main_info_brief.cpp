#include "kds_main_info_brief.h"
#include "ui_kds_main_info_brief.h"
#include "kds_main_view.h"
#include "lds.h"
#include "lds_pushbutton.h"
#include "lds_query_db.h"
#include "lds_messagebox.h"
#include "w_bt_dish_kitchenprinter_printerset_port_dialog.h"
#include "kds_main_info_print.h"

kds_main_info_brief::kds_main_info_brief(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::kds_main_info_brief)
{
    ui->setupUi(this);
    save_width = 0;
    commit_type = commitManual;
    lds::hideWidget(ui->pushButton_up);
    lds::hideWidget(ui->pushButton_down);
    lds::hideWidget(ui->pushButton_cook);
    lds::hideWidget(ui->pushButton_service);
    lds::hideWidget(ui->pushButton_take);
    lds::hideWidget(ui->pushButton_back);

    ui->pushButton_cook->enableLongPress(true, this);

    connect(ui->pushButton_up, SIGNAL(clicked()), this, SLOT(topageup()));
    connect(ui->pushButton_back, SIGNAL(clicked()), this, SIGNAL(signalBack()));
    connect(ui->pushButton_down, SIGNAL(clicked()), this, SLOT(topagedown()));
    connect(ui->pushButton_cook, SIGNAL(clicked()), this, SLOT(toordercook()));
    connect(ui->pushButton_service, SIGNAL(clicked()), this, SLOT(toorderservice()));
    connect(ui->pushButton_take, SIGNAL(clicked()), this, SLOT(toordertake()));
    connect(ui->pushButton_cook, SIGNAL(timeout()), this, SLOT(toprintsettings()));
}

kds_main_info_brief::~kds_main_info_brief()
{
    delete ui;
}

QSet<int> kds_main_info_brief::modifiedFlow_ids(int state)
{
    return ui->frame->modifiedFlow_ids(state);
}

void kds_main_info_brief::setIndexJson(const QByteArray &index_json, const QString &which)
{
    this->which.setOther(which);
    if(save_width != this->width()) {
        save_width = this->width();
        for(int k = 0; k < btns_list.count(); k ++) {
            delete btns_list[k].service;
            delete btns_list[k].cook;
            delete btns_list[k].take;
            disconnect(kds::model, SIGNAL(signal_select_and_autoselect()), this, SLOT(tobuttonsRefresh()));
        }
        btns_list.clear();
        //
        connect(kds::model, SIGNAL(signal_select_and_autoselect()), this, SLOT(tobuttonsRefresh()));
        int cell_width = this->width() * 0.48;
        int button_width = this->width() * 0.1;
        int cell_left = (this->width() - cell_width) / 2 - button_width - 20;
        cell_left = qMin(70, cell_left);
        ui->frame->setGeometry(2 * button_width, 0, cell_width, this->height());
        ui->frame->setIndexJson(index_json);


        QList<QRectF> rect_list = kds::paintTableBrief(0, ui->frame->getIndexJson(), ui->frame->rect());
        for(int k = 0; k < rect_list.count(); k ++) {//include title row
            briefButtons pair;
            const QRectF &rect = rect_list[k];
            int button_height = rect.height();

            pair.cook = new lds_pushbutton(this);
            pair.cook->setFocusPolicy(Qt::NoFocus);
            pair.cook->setGeometry(ui->frame->x() - button_width, ui->frame->y() + rect.y(), button_width, rect.height());
            pair.cook->setIcon(QPixmap(":/kds/order_cooking.png"));//tr("开始制作"));
            pair.cook->setIconSize(QSize(button_height, button_height));
            pair.cook->setData("type", KDS::cookCooking);
            pair.cook->setStyleSheet("font-size:15pt;background-color:white;" + QString(";border-top-left-radius:%1px;border-bottom-left-radius:%1px;color:white;").arg(button_height / 2));
            connect(pair.cook, SIGNAL(clicked()), this, SLOT(tobuttoncheck()));
            pair.service = new lds_pushbutton(this);
            pair.service->setFocusPolicy(Qt::NoFocus);
            pair.service->setGeometry(ui->frame->x() + ui->frame->width() - 0, ui->frame->y() + rect.y(), button_width, rect.height());
            pair.service->setIcon(QPixmap(":/kds/order_service.png"));//tr("制作完成"));
            pair.service->setIconSize(QSize(button_height, button_height));
            pair.service->setData("type", KDS::cookFinished);
            pair.service->setStyleSheet("font-size:15pt;background-color:white;" + QString(";border-top-right-radius:%1px;border-bottom-right-radius:%1px;color:white;").arg(button_height / 2));
            connect(pair.service, SIGNAL(clicked()), this, SLOT(tobuttoncheck()));
            pair.take = new lds_pushbutton(this);
            pair.take->setFocusPolicy(Qt::NoFocus);
            pair.take->setGeometry(pair.service->geometry());
            pair.take->setIcon(QPixmap(":/kds/order_done.png"));
            pair.take->setIconSize(QSize(button_height, button_height));
            pair.take->setData("type", KDS::cookToken);
            pair.take->setStyleSheet("font-size:15pt;background-color:white;" + QString(";border-top-right-radius:%1px;border-bottom-right-radius:%1px;color:white;").arg(button_height / 2));
            connect(pair.take, SIGNAL(clicked()), this, SLOT(tobuttoncheck()));
            pair.cancel = new lds_pushbutton(this);
            pair.cancel->setFocusPolicy(Qt::NoFocus);
            pair.cancel->setGeometry(pair.service->geometry());
            pair.cancel->setIcon(QPixmap(":/kds/button_cancel.png"));
            pair.cancel->setIconSize(QSize(button_height, button_height));
            pair.cancel->setData("type", KDS::cookCanceled);
            pair.cancel->setStyleSheet("font-size:15pt;background-color:white;" + QString(";border-top-right-radius:%1px;border-bottom-right-radius:%1px;color:white;").arg(button_height / 2));
            connect(pair.cancel, SIGNAL(clicked()), this, SLOT(tobuttoncheck()));
            //
            btns_list << pair;
        }

        assignQuickButton();
    } else {
        ui->frame->setIndexJson(index_json);
    }
    btnslistMapToItemrow(0);
    buttonsRefresh();
}

QByteArray kds_main_info_brief::getIndexJson()
{
    return ui->frame->getIndexJson();
}

int kds_main_info_brief::getIndexJsonCount()
{
    return ui->frame->p.array_count("{items");
}

void kds_main_info_brief::setCommitType(kds_main_info_brief::commitType commit_type)
{
    this->commit_type = commit_type;
}

void kds_main_info_brief::topageup()
{
    if(ui->frame->p.isValid()) {
        int row_begin = qMax(0, ui->frame->p.path_value("{row_begin").toInt() - 1);
        ui->frame->replaceItemValue("", "row_begin", row_begin);
        btnslistMapToItemrow(row_begin);
        buttonsRefresh();
    }
}

void kds_main_info_brief::toallcooking()
{
    int state;
    QString type;
    QString flow_id;
    lds_query_db query;
    QString errstring;
    QStringList print_flow_id_list;
    bool via_cook = false;
    query.db.transaction();
    for(int k = 0; k < ui->frame->p.array_count("{items"); k ++) {
        type = ui->frame->p.array_value("{items[%1{type", k).toString();
        flow_id = ui->frame->p.array_value("{items[%1{flow_id", k).toString();
        state = kds::model->tablename_kvmap_value("cey_u_orderdish_print_kt_order", flow_id, "int_flowID", "ch_state").toInt();
        if(type == "item" && state < KDS::cookCooking) {
            if(!kds::update_print_kt_dish(flow_id, KDS::cookCooking, errstring, &via_cook)) {
                query.db.rollback();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
            if(via_cook)
                print_flow_id_list << flow_id;
        }
    }
    query.db.commit();
    kds::model->tablename_kvmap_delete_for_select_and_autoselect();
    emit signalBack();
    //
    foreach(const QString &flow_id, print_flow_id_list)
        kds::print_cook(flow_id);
    //
}

void kds_main_info_brief::toallservice()
{
    int state;
    QString type;
    QString flow_id;
    QString errstring;
    lds_query_db query;
    QString exception_finish_sql = ui->frame->p.path_value("{exception_finish_sql").toString();
    int item_count = 0;
    bool via_cook = false;
    QStringList print_flow_id_list;
    query.db.transaction();
    for(int k = 0; k < ui->frame->p.array_count("{items"); k ++) {
        type = ui->frame->p.array_value("{items[%1{type", k).toString();
        flow_id = ui->frame->p.array_value("{items[%1{flow_id", k).toString();
        state = kds::model->tablename_kvmap_value("cey_u_orderdish_print_kt_order", flow_id, "int_flowID", "ch_state").toInt();
        if(type == "item") {
            item_count ++;
            if(state < KDS::cookFinished) {
                if(!kds::update_print_kt_dish(flow_id, KDS::cookFinished, errstring, &via_cook)) {
                    query.db.rollback();
                    lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                    return;
                }
                if(via_cook)
                    print_flow_id_list << flow_id;
            }
        }
    }
    if(item_count == 0 && exception_finish_sql.count() > 0)
        query.execAlter(exception_finish_sql);
    query.db.commit();
    kds::model->tablename_kvmap_delete_for_select_and_autoselect();
    emit signalBack();
    //
    foreach(const QString &flow_id, print_flow_id_list)
        kds::print_cook(flow_id);
    //
}

void kds_main_info_brief::toreset()
{
    ui->frame->reset();
    btnslistMapToItemrow(0);
    buttonsRefresh();
}

void kds_main_info_brief::toordercook()
{
    toallcooking();
}

void kds_main_info_brief::toorderservice()
{
    toallservice();
}

void kds_main_info_brief::toordertake()
{
    int state;
    QString type;
    QString flow_id;
    QString errstring;
    lds_query_db query;
    query.db.transaction();
    for(int k = 0; k < ui->frame->p.array_count("{items"); k ++) {
        type = ui->frame->p.array_value("{items[%1{type", k).toString();
        flow_id = ui->frame->p.array_value("{items[%1{flow_id", k).toString();
        state = kds::model->tablename_kvmap_value("cey_u_orderdish_print_kt_order", flow_id, "int_flowID", "ch_state").toInt();
        if(type == "item" && KDS::cookFinished == state) {
            if(!kds::update_print_kt_dish(flow_id, KDS::cookToken, errstring)) {
                query.db.rollback();
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }
    }
    query.db.commit();
    kds::model->tablename_kvmap_delete_for_select_and_autoselect();
    emit signalRefresh();
}

void kds_main_info_brief::toprintsettings()
{
    kds_main_info_print dialog(this);
    dialog.setWindowTitle(QObject::tr("设置"));
    if(QDialog::Accepted == lds_roundeddialog_rect_align_600x400(&dialog).exec()) {
        pushButton_cook_style();
    }
}

void kds_main_info_brief::tobuttoncheck()
{
    lds_pushbutton *b = qobject_cast<lds_pushbutton *>(this->sender());
    if(0 == b)
        return;
    if(ui->frame->p.isNull()) {
        return;
    }
    int less_than_state = ui->frame->p.path_value("{less_than_state").toInt();

    int item_row = b->getData("item_row").toInt();
    if(item_row >= ui->frame->p.array_count("{items")) {
        return;
    }
    const QString flow_id = ui->frame->p.array_value("{items[%1{flow_id", item_row).toString();
    lds_query_db query;
    QString errstring;
    bool via_cook = false;
    bool have_update_state = false;
    query.db.transaction();
    if(KDS::cookCooking == b->getData("type").toInt()) {
        if(!kds::update_print_kt_dish(flow_id, KDS::cookCooking, errstring, &via_cook)) {
            query.db.rollback();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        have_update_state = true;
    }
    if(KDS::cookFinished == b->getData("type").toInt()) {
        if(!kds::update_print_kt_dish(flow_id, KDS::cookFinished, errstring, &via_cook)) {
            query.db.rollback();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        have_update_state = true;
    }
    if(KDS::cookToken == b->getData("type").toInt()) {
        if(!kds::update_print_kt_dish(flow_id, KDS::cookToken, errstring, &via_cook)) {
            query.db.rollback();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        have_update_state = true;
    }
    if(KDS::cookCanceled == b->getData("type").toInt()) {
        if(!kds::update_print_kt_dish(flow_id, KDS::cookCanceled, errstring, &via_cook)) {
            query.db.rollback();
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
            return;
        }
        have_update_state = true;
    }
    query.db.commit();

    if(have_update_state) {
        if(b->getData("type").toInt() >= less_than_state) {
            if(0 == ui->frame->deleteArrayItem("{items", item_row)) {

            }
        }
        emit signalRefreshSingle();
    }
end:
    kds::model->tablename_kvmap_delete_for_select_and_autoselect();
    //
    if(via_cook)
        kds::print_cook(flow_id);
    //
}

void kds_main_info_brief::tobuttonsRefresh()
{
    buttonsRefresh();
}

void kds_main_info_brief::topagedown()
{
    if(ui->frame->p.isValid()) {
        int row_begin = qMin(qMax(0, ui->frame->p.array_count("{items") - 1), ui->frame->p.path_value("{row_begin").toInt() + 1);
        ui->frame->replaceItemValue("", "row_begin", row_begin);
        btnslistMapToItemrow(row_begin);
        buttonsRefresh();
    }
}

void kds_main_info_brief::buttonsRefresh()
{
    for(int k = 0; k < btns_list.count(); k ++) {
        lds::hideWidget(btns_list[k].cook, true);
        lds::hideWidget(btns_list[k].service, true);
        lds::hideWidget(btns_list[k].take, true);
        lds::hideWidget(btns_list[k].cancel, true);
    }
    lds::hideWidget(ui->pushButton_up, false);
    lds::hideWidget(ui->pushButton_down, false);
    lds::hideWidget(ui->pushButton_cook, false);
    lds::hideWidget(ui->pushButton_service, false);
    lds::hideWidget(ui->pushButton_take, false);
    lds::hideWidget(ui->pushButton_back, false);

    if(ui->frame->p.isValid()) {
        int item_count = ui->frame->p.array_count("{items");
        int item_row = 0;
        int row_begin = ui->frame->p.path_value("{row_begin").toInt();
        //
        for(int k = 0; k < btns_list.count(); k ++) {
            item_row = btns_list[k].cook->getData("item_row").toInt();
            if(item_row >= 0 && item_row < item_count) {
                const QString type = ui->frame->p.array_value("{items[%1{type", item_row).toString();
                const QString flow_id = ui->frame->p.array_value("{items[%1{flow_id", item_row).toString();
                const int state = kds::model->tablename_kvmap_value("cey_u_orderdish_print_kt_order", flow_id, "int_flowID", "ch_state").toInt();
                if(type == "item") {
                    if(kds::cey_u_orderdish_num_dish(flow_id) <= 0) {
                        lds::hideWidget(btns_list[k].cancel, false);
                        continue;
                    }
                    if(state == KDS::cookPrepared) {
                        lds::hideWidget(btns_list[k].cook, false);
                        lds::hideWidget(btns_list[k].service, false);
                    }
                    if(state == KDS::cookCooking) {
                        lds::hideWidget(btns_list[k].service, false);
                    }
                    if(state == KDS::cookFinished) {
                        lds::hideWidget(btns_list[k].take, false);
                    }
                }
            }
        }
        //
        lds::hideWidget(ui->pushButton_down, true);
        lds::hideWidget(ui->pushButton_up, true);
        if(btns_list.count() > 0
                && row_begin + btns_list.count() < item_count) {
            lds::hideWidget(btns_list.last().cook);
            lds::hideWidget(btns_list.last().service);
            lds::hideWidget(btns_list.last().take);
            lds::hideWidget(ui->pushButton_down, false);
        }
        if(btns_list.count() > 0
                && row_begin > 0) {
            lds::hideWidget(btns_list.first().cook);
            lds::hideWidget(btns_list.first().service);
            lds::hideWidget(btns_list.first().take);
            lds::hideWidget(ui->pushButton_up, false);
        }
        //
        if(kds::is_fastfdfood_kitchen()) {
            lds::hideWidget(ui->pushButton_cook);
            lds::hideWidget(ui->pushButton_take);
            for(int k = 0; k < btns_list.count(); k ++) {
                lds::hideWidget(btns_list[k].cook);
                lds::hideWidget(btns_list[k].take);
            }
            assignQuickButton();
        }
        if(kds::is_restaurant_kitchen()) {
            lds::hideWidget(ui->pushButton_take);
            for(int k = 0; k < btns_list.count(); k ++) {
                lds::hideWidget(btns_list[k].take);
            }
            assignQuickButton();
        }
        if(kds::is_restaurant_service()) {
            lds::hideWidget(ui->pushButton_cook);
            lds::hideWidget(ui->pushButton_service);
            lds::hideWidget(ui->pushButton_back);
            for(int k = 0; k < btns_list.count(); k ++) {
                lds::hideWidget(btns_list[k].cook);
                lds::hideWidget(btns_list[k].service);
            }
            assignQuickButton();
        }
    }
    ui->frame->update();
}

void kds_main_info_brief::btnslistMapToItemrow(int row_begin)
{
    for(int k = 0; k < btns_list.count(); k ++) {
        btns_list[k].cook->setData("item_row", row_begin + k);
        btns_list[k].service->setData("item_row", row_begin + k);
        btns_list[k].take->setData("item_row", row_begin + k);
        btns_list[k].cancel->setData("item_row", row_begin + k);
    }
}

void kds_main_info_brief::assignQuickButton()
{
    if(btns_list.count() < 2) {
        return;
    }
    int cell_width = this->width() * 0.48;
    int button_width = this->width() * 0.1;
    int cell_left = (this->width() - cell_width) / 2 - button_width - 20;
    cell_left = qMin(70, cell_left);
    int button_height = btns_list.first().cook->height();
    QList<lds_pushbutton *> quickbtns;
    quickbtns << ui->pushButton_take;
    quickbtns << ui->pushButton_service;
    quickbtns << ui->pushButton_cook;
    quickbtns << ui->pushButton_back;
    //
    const QRect rect0 = QRect(0,
                              btns_list.first().cook->y() - ui->frame->y(),
                              btns_list.first().take->x() - btns_list.first().cook->x() - btns_list.first().cook->width(),
                              btns_list.first().cook->height());
    const QRect rect1 = QRect(0,
                              btns_list.last().cook->y() - ui->frame->y(),
                              btns_list.last().take->x() - btns_list.last().cook->x() - btns_list.last().cook->width(),
                              btns_list.last().cook->height());
    //
    for(int k = 0, sqr = 0; k < quickbtns.count(); k ++) {
        if(lds::widgetIsShow(quickbtns[k])) {
            quickbtns[k]->setGeometry(
                        this->width() - cell_left - 10, this->height() - cell_left - sqr * (cell_left + 4), cell_left, cell_left);
            quickbtns[k]->setIconSize(QSize(cell_left, cell_left) * 0.6);
            quickbtns[k]->setStyleSheet(QString("font-size:15pt;background-color:white;border-radius:%1px;").arg(quickbtns[k]->width() / 2));
            sqr ++;
        }
    }
    //
    ui->pushButton_up->setGeometry(rect0.translated(ui->frame->x(), ui->frame->y()).adjusted(0, 0, -kds::sawtoothRight(rect0), 0));
    ui->pushButton_up->setIconSize(QSize(button_height, button_height) * 0.6);
    ui->pushButton_up->setStyleSheet("background-color:transparent;border:0px;");
    //
    ui->pushButton_down->setGeometry(rect1.translated(ui->frame->x(), ui->frame->y()).adjusted(0, 0, -kds::sawtoothRight(rect1), 0));
    ui->pushButton_down->setIconSize(QSize(button_height, button_height) * 0.6);
    ui->pushButton_down->setStyleSheet("background-color:transparent;border:0px;");
    //
    pushButton_cook_style();
}

void kds_main_info_brief::pushButton_cook_style()
{
    QString border_style;
    if(lds::sysconf->value("kds_main_info_print/check", false).toBool())
        border_style = "border:5px solid " + kds::color_cooking;
    ui->pushButton_cook->setStyleSheet(QString("font-size:15pt;background-color:white;border-radius:%1px;").arg(ui->pushButton_cook->width() / 2) + border_style);

}

void kds_main_info_brief::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
}

void kds_main_info_brief::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
}
