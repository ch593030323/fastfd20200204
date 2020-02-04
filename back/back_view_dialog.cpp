#include "back_view_dialog.h"
#include "ui_back_view_dialog.h"
#include "ftableview_standmodel_sql_none.h"
#include "ftableview_delegate.h"
#include <QPainter>
#include <QtDebug>
#include "public_sql.h"
#include <QFile>
#include "w_bt_dish_base.h"
#include "w_bt_dish_table.h"
#include "w_bt_dish_suit.h"
#include "w_bt_dish_cookset.h"
#include "w_bt_dish_kitchen_dish.h"
#include "w_bt_dish_quit.h"
#include "w_m_member.h"
#include "w_m_member_cardic.h"
#include "w_m_member_list.h"
#include "w_m_member_paramset2.h"
#include "w_m_member_point.h"
#include "w_m_member_recharge.h"
#include "w_m_member_statemanage.h"
#include "w_m_member_type.h"
#include "w_rpt_report_daymonweekanalysis.h"
#include "w_rpt_report_frame_dishquit.h"
#include "w_rpt_report_categorysalesanalysis.h"
#include "w_rpt_report_daybusinessreprot.h"
#include "w_rpt_report_directsalegrossprofitreport.h"
#include "w_rpt_report_periodbusinessanalysis.h"
#include "w_rpt_report_frame_tax.h"
#include "w_rpt_report_promotionreport.h"
#include "w_rpt_report_servantpromotquery.h"
#include "w_rpt_report_servicefeelowpinquery.h"
#include "w_rpt_report_tablebusinessreport.h"
#include "w_sys_manage.h"
#include "w_sys_manage_basicdataexport.h"
#include "w_sys_manage_basicdataimport.h"
#include "w_sys_manage_companyinformation.h"
#include "w_sys_manage_mysql.h"
#include "w_sys_manage_operatorinformation.h"
#include "w_sys_manage_operatorright.h"
#include "w_sys_manage_sqlcopy.h"
#include "w_sys_manage_sysreboot.h"
#include "w_sys_manage_sysset.h"
#include "w_sys_manage_wirelessordermonitoring.h"
#include "w_ds_sale_anticheckout.h"
#include "w_ds_sale_businessserach.h"
#include "w_ds_sale_cashjournal.h"
#include "w_ds_sale_daybusiness.h"
#include "w_ds_sale_dishsaleout.h"
#include "w_ds_sale_qt.h"
#include "w_ds_sale_salejournal.h"
#include "w_bt_dish_suit.h"
#include "w_i_inventory_check.h"
#include "w_i_inventory_invoicing.h"
#include "w_i_inventory_query.h"
#include "w_i_inventory_summaryquery.h"
#include "backheader.h"
#include "business_documents_query.h"
#include "back_anti_checkout.h"
#include "lds_messagebox.h"
#include <QtDebug>
#include "public_sql.h"
#include "lds_messagebox.h"
#include "public_logindialog.h"
#include "w_sys_manage_outer_settings.h"
#include "w_sys_manage_outer_pay_set.h"
#include "w_sys_manage_cloudsync_with_time.h"
#include "w_bt_dish_paymode.h"
//#include <QSvgRenderer>
#include "fastfd_update.h"
#include "qprocess.h"
#include "w_rpt_report_frame_member.h"
#include "w_rpt_report_frame_dishsale.h"
#include "w_rpt_report_frame_saleflow.h"
#include "w_rpt_report_frame_cashflow.h"
#include "w_rpt_report_frame_cash.h"
#include "w_rpt_report_frame_shift.h"
#include "material_purchase_apply.h"
#include "material_purchase_in_storage.h"
#include "material_loss_apply.h"
#include "w_sys_manage_promotion_setting.h"
#include "w_rpt_report_frame_material_saleflow.h"
#include "w_rpt_report_frame_material_saleflow2.h"
#include "w_rpt_report_frame_dishsalerank.h"
#include "w_sys_manage_cloudsync_cloud2.h"
#ifdef Q_OS_WIN
#include "TCHAR.H"
#endif
#include "w_inventory_input.h"
#include "w_inventory_draw.h"
#include "lds_dialog_input.h"
#include "ui_lds_dialog_input_dialog.h"
#include "w_rpt_report_frame_servantpromotquery.h"
#include "w_sys_manage_sqlcopy_soft_limit2.h"
#include "w_i_inventory_provider.h"

QMap<QString, QImage> back_view_dialog::name_image_map;
back_view_dialog::back_view_dialog(QWidget *parent) :
    lds_roundeddialog_rect0(parent)
{
    ui = new Ui_back_view_dialog;
    ui->setupUi(this);
    lds::setwflagFrameless(this);
    resize(lds::MAIN_WINDOW_SIZE);

    ui->tableView_list->setProperty("outer_stylesheet", "tableView_type");

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    this->setLayout(vlayout);
    ui->widget_3->setTitleColor(&titleColor);
    vlayout->addWidget(ui->widget_3);
    vlayout->addWidget(ui->label_4);
    vlayout->addWidget(ui->widget_content);
    vlayout->addWidget(ui->widget_tail);
    vlayout->setContentsMargins(0, 0, 0, 3);
    vlayout->setSpacing(0);
    ui->frame->setFixedWidth(150);
    if(lds::FULL_WINDOW_SIZE.width() == 1920) {
        ui->frame->setFixedWidth(250);
    }
    dialog = 0;
    backlist_model = 0;

    QTimer::singleShot(0, this, SLOT(tofirstGoIn()));
    //3
}

back_view_dialog::~back_view_dialog()
{
    qDebug() << __FILE__ << __LINE__ << __FUNCTION__;
    delete ui;
}

void back_view_dialog::tableview_span_column(QTableView *t,int c)
{
    for(int r = 0, rcount = t->verticalHeader()->count();r<rcount;r++) {
        t->setSpan(r, c, 1, 2);
    }
}

QPixmap back_view_dialog::get_fastfdbacklogo()
{
    return lds::get_fastfdbacklogo();
}

QStringList back_view_dialog::get_fastfdbacktext()
{
    return lds::get_fastfdbacktext();
}

void back_view_dialog::retranslateView()
{
    //1
    backlist_model->model_data_set(0, 0, QObject::tr("基础资料"), Qt::UserRole);
    backlist_model->model_data_set(1, 0, QObject::tr("会员设置"), Qt::UserRole);
    backlist_model->model_data_set(2, 0, QObject::tr("库存管理"), Qt::UserRole);
    backlist_model->model_data_set(3, 0, QObject::tr("报表分析"), Qt::UserRole);
    backlist_model->model_data_set(4, 0, QObject::tr("系统设置"), Qt::UserRole);
    backlist_model->model_data_set(5, 0, QObject::tr("云端连锁"), Qt::UserRole);
    backlist_model->model_data_set(6, 0, QObject::tr("前台"), Qt::UserRole);
    backlist_model->model_data_set(7, 0, QObject::tr("退出"), Qt::UserRole);


    //2
    ui->label_version->setText(fastfd_update::gs_soft_version);
    ui->label->clear();
    ui->label->setPixmap(back_view_dialog::get_fastfdbacklogo());
    QStringList ret_list = back_view_dialog::get_fastfdbacktext();
    ui->label_2->setText(ret_list.value(0).trimmed());
    ui->label_5->setText(ret_list.value(1).trimmed());
    ui->label_6->setText(ret_list.value(2).trimmed());


    //3
    for(int k = 0; k < ui->stackedWidget->count(); k++) {
        QTableView *tableview = static_cast<QTableView *>(ui->stackedWidget->widget(k));
        if(tableview) {
            QStandardItemModel *model = static_cast<QStandardItemModel *>(tableview->model());
            model->removeRows(0, model->rowCount());
            delete model;

            ui->stackedWidget->removeWidget(tableview);
            delete tableview;
        }
    }
    index_key_map.clear();
    toupdateSection(ui->tableView_list->currentIndex());
}

QString back_view_dialog::dialogClassName()
{
#ifdef QT_DEBUG
    if(dialog) {
        qDebug() << "=============:" <<  dialog->metaObject()->className();
    }
#endif
    return "";
}

void back_view_dialog::tofirstGoIn()
{
    //1
      backlist_model = new ftableview_standmodel_sql_none;
      backlist_model->set_row_column(8, 1);

      backlist_model->model_data_set(0, 0,"jczl", Qt::UserRole+1);
      backlist_model->model_data_set(1, 0,"hysz", Qt::UserRole+1);
      backlist_model->model_data_set(2, 0,"kcgl", Qt::UserRole+1);
      backlist_model->model_data_set(3, 0,"bbfx", Qt::UserRole+1);

      backlist_model->model_data_set(4, 0,"xtsz", Qt::UserRole+1);

      backlist_model->model_data_set(5, 0,"ydls", Qt::UserRole+1);

      backlist_model->model_data_set(6, 0,"qt", Qt::UserRole+1);
      backlist_model->model_data_set(7, 0,"tc", Qt::UserRole+1);

      ftableview_delegate *d = new ftableview_delegate(ui->tableView_list);
      d->keepConnect(ui->tableView_list);
      //    d->setMargin(1);
      d->indexisEmptyDrawGrad = true;

      ui->tableView_list->setModel(backlist_model);
      ui->tableView_list->setItemDelegate(d);
      ui->tableView_list->setHeaderResizeMode(Qt::Vertical, QHeaderView::Stretch);
      ui->tableView_list->setHeaderResizeMode(Qt::Horizontal, QHeaderView::Stretch);
      connect(ui->tableView_list,SIGNAL(clicked(QModelIndex)),this,SLOT(toupdateSection(QModelIndex)));

      ui->tableView_list->setCurrentIndex(backlist_model->index(0,0));
      retranslateView();
      public_sql::etimer.start();
  }

  void back_view_dialog::toupdateSection(const QModelIndex &index)
  {
      const QString key = index.data(Qt::UserRole + 1).toString();
      if(key == "jczl") {
          if(index_key_map.value(index.row(), -1) == -1) {
              QTableView *tableview = creat_tableview();
              QStandardItemModel *model = new QStandardItemModel(this);
              tableview->setModel(model);

  #ifdef QT_SCPOS_SUPPORT
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/c1", QObject::tr("商品资料"), "商品资料")
                                  << createItem("0", ":/image/backview/c4", QObject::tr("组合商品"), "组合商品")
                                  << createItem("0", ":/image/backview/c8", QObject::tr("退货原因"), "退货原因")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/f2", QObject::tr("基础资料导出"), "基础资料导出")
                                  << createItem("0", ":/image/backview/f1", QObject::tr("基础资料导入"), "基础资料导入")
                                  << new QStandardItem
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );

              tableview_span_column(tableview, 0);
              tableview_span_column(tableview, 2);
  #else
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/c1", QObject::tr("商品资料"), "商品资料")
                                  << createItem("0", ":/image/backview/c2", QObject::tr("做法设置"), "做法设置")
                                  << createItem("0", ":/image/backview/c4", QObject::tr("组合商品"), "组合商品")
                                  << createItem("0", ":/image/backview/c5", QObject::tr("餐桌信息"), "餐桌信息")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("1", ":/image/backview/f2", QObject::tr("基础资料导出"), "基础资料导出")
                                  << createItem("1", ":/image/backview/f1", QObject::tr("基础资料导入"), "基础资料导入")
                                  << createItem("1", ":/image/backview/c8", QObject::tr("退货原因"), "退货原因")
                                  << createItem("1", ":/image/backview/g4", QObject::tr("客户"), "客户")
                                  );

              tableview_span_column(tableview, 0);
  //            tableview->setSpan(2, 3, 2, 1);
  #endif
              index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
          }
      }
      if(key == "hysz") {
          if(index_key_map.value(index.row(), -1) == -1) {
              QTableView *tableview = creat_tableview();
              QStandardItemModel *model = new QStandardItemModel(this);
              tableview->setModel(model);

              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/b1", QObject::tr("会员参数设置"), "会员参数设置")
                                  << createItem("0", ":/image/backview/b2", QObject::tr("会员类型变更"), "会员类型变更")
                                  << createItem("0", ":/image/backview/b4", QObject::tr("会员资料维护"), "会员资料维护")
                                  << createItem("0", ":/image/backview/b3", QObject::tr("会员积分管理"), "会员积分管理")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/b5", QObject::tr("会员状态管理"), "会员状态管理")
                                  << createItem("0", ":/image/backview/b6", QObject::tr("充值"), "充值")
                                  << createItem("0", ":/image/backview/b8", QObject::tr("芯片清除"), "芯片清除")
                                  << createItem("0", ":/image/backview/b7", QObject::tr("会员报表"), "会员报表")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              tableview_span_column(tableview, 0);
              tableview_span_column(tableview, 2);

              index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
          }
      }
      if(key == "kcgl") {
          if(index_key_map.value(index.row(), -1) == -1) {
              QTableView *tableview = creat_tableview();
              QStandardItemModel *model = new QStandardItemModel(this);
              tableview->setModel(model);

              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/d9", QObject::tr("供应商"), "供应商")
                                  << createItem("0", ":/image/backview/d8", QObject::tr("采购入库"), "采购入库")
                                  << createItem("0", ":/image/backview/d5", QObject::tr("采购退货"), "采购退货")
                                  << createItem("0", ":/image/backview/d7", QObject::tr("商品报损"), "商品报损")
                                  << createItem("0", ":/image/backview/d9", QObject::tr("库存盘点"), "库存盘点")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/d4", QObject::tr("盘点查询"), "盘点查询")
                                  << createItem("0", ":/image/backview/d3", QObject::tr("进销存汇总"), "进销存汇总")
                                  << createItem("0", ":/image/backview/d2", QObject::tr("单据汇总查询"), "单据汇总查询")
                                  << createItem("0", ":/image/backview/d6", QObject::tr("库存查询"), "库存查询")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );

              tableview_span_column(tableview, 0);
              tableview_span_column(tableview, 2);

              index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
          }
      }
      if(key == "bbfx") {
          if(index_key_map.value(index.row(), -1) == -1) {
              QTableView *tableview = creat_tableview();
              QStandardItemModel *model = new QStandardItemModel(this);
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/a15", QObject::tr("交班报表"), "交班报表")
                                  << createItem("0", ":/image/backview/a8", QObject::tr("日营业情况统计"), "日营业情况统计")
                                  << createItem("0", ":/image/backview/a13", QObject::tr("收银流水"), "收银流水")
                                  << createItem("0", ":/image/backview/a2", QObject::tr("商品销售汇总"), "商品销售汇总")
                                  << createItem("0", ":/image/backview/e7", QObject::tr("销售流水"), "销售流水")
                                  << createItem("0", ":/image/backview/a3", QObject::tr("商品销售排行统计"), "商品销售排行统计")
                                  << createItem("0", ":/image/backview/e2", QObject::tr("营业数据按单查询"), "营业数据按单查询")

                                  << createItem("0", ":/image/backview/e1", QObject::tr("反结账查询"), "反结账查询")
                                  << createItem("0", ":/image/backview/a12", QObject::tr("退货情况报表"), "退货情况报表")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/a7", QObject::tr("品类销售营业分析"), "品类销售营业分析")
                                  << createItem("0", ":/image/backview/a1", QObject::tr("促销报表"), "促销报表")
                                  << createItem("0", ":/image/backview/a9", QObject::tr("日月周报表分析"), "日月周报表分析")
                                  << createItem("0", ":/image/backview/a6", QObject::tr("服务员提成查询"), "服务员提成查询")
                                  << createItem("0", ":/image/backview/f21", QObject::tr("原料销售报表"), "原料销售报表")
                      #ifdef QT_SCPOS_SUPPORT
                      #else
                                  << createItem("0", ":/image/backview/a4", QObject::tr("桌台营业统计"), "桌台营业统计")
                      #endif
                                  << createItem("0", ":/image/backview/a5", QObject::tr("服务费与低消查询"), "服务费与低消查询")
                                  << createItem("0", ":/image/backview/a10", QObject::tr("时段营业情况分析"), "时段营业情况分析")
                                  << createItem("0", ":/image/backview/a10", QObject::tr("税收报表"), "税收报表")
                                  );
              tableview->setModel(model);
              index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
          }
      }

      if(key == "xtsz") {
          if(index_key_map.value(index.row(), -1) == -1) {
              QTableView *tableview = creat_tableview();
              QStandardItemModel *model = new QStandardItemModel(this);
              tableview->setModel(model);

              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/f13", QObject::tr("外部设备"), "外部设备")
                                  << createItem("0", ":/image/backview/f14", QObject::tr("收银设置"), "收银设置")
                                  << createItem("0", ":/image/backview/f12", QObject::tr("收银方式"), "收银方式")
                                  << createItem("0", ":/image/backview/e3", QObject::tr("促销设置"), "促销设置")
                                  << createItem("0", ":/image/backview/f7", QObject::tr("公司信息"), "公司信息")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/f5", QObject::tr("操作员信息"), "操作员信息")
                                  << createItem("0", ":/image/backview/f6", QObject::tr("操作员权限"), "操作员权限")
                                  << createItem("0", ":/image/backview/f4", QObject::tr("数据库"), "数据库")
                                  << createItem("0", ":/image/backview/f15", QObject::tr("在线升级"), "在线升级")
                                  << createItem("0", ":/image/backview/g5", QObject::tr("注册"), "注册")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              tableview_span_column(tableview, 0);
              tableview_span_column(tableview, 2);

              index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
          }
      }
      if(key == "ydls") {
          if(index_key_map.value(index.row(), -1) == -1) {
              QTableView *tableview = creat_tableview();
              QStandardItemModel *model = new QStandardItemModel(this);
              model->appendColumn(QList<QStandardItem*>()
                                  << createItem("0", ":/image/backview/f9", QObject::tr("云设置"), "云设置")
                                  << createItem("0", ":/image/backview/g3", QObject::tr("采购申请"), "采购申请")
                                  << createItem("0", ":/image/backview/g2", QObject::tr("门店入库"), "门店入库")
                                  << createItem("0", ":/image/backview/g1", QObject::tr("门店出库"), "门店出库")
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              model->appendColumn(QList<QStandardItem*>()
                                  );
              tableview->setModel(model);
              tableview_span_column(tableview, 0);
              index_key_map.insert(index.row(), ui->stackedWidget->addWidget(tableview));
          }
      }
      if(key == "qt") {
          if(backheader::operate_right_check_code(this, "0241")) {//前台
              this->accept();
              return;
          }
          //        }
          ui->tableView_list->setCurrentIndex(tableview_list_pre_row);
          return;
      }
      if(key == "tc") {
          if(0 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("确认退出"), QObject::tr("确认"),QObject::tr("取消"))) {
              this->reject();
              return;
          }
          ui->tableView_list->setCurrentIndex(tableview_list_pre_row);
          return;
      }

      ui->stackedWidget->setCurrentIndex(index_key_map.value(index.row(), -1));
      tableview_list_pre_row = index;
}

void back_view_dialog::topop(const QModelIndex &index)
{
    QStringList texts = index.data().toString().split("\t", QString::SkipEmptyParts);
    QString text = texts.value(2);
    QString code_item = index.data(Qt::UserRole + 1).toString();
    if(!backheader::operate_right_check_zh(this, code_item)) {
        return;
    }

    if(code_item == "商品资料") {//
        dialog = static_cast<QDialog *>(new w_bt_dish_base(this));
    }
    if(code_item == "做法设置") {//
        dialog = static_cast<QDialog *>(new w_bt_dish_cookset(this));
    }
    if(code_item == "组合商品") {//
        dialog = static_cast<QDialog *>(new w_bt_dish_suit(this));
    }
    if(code_item == "餐桌信息") {//
        dialog = static_cast<QDialog *>(new w_bt_dish_table(this));
    }

    if(code_item == "基础资料导出") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_basicdataexport(this));
    }
    if(code_item == "基础资料导入") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_basicdataimport(this));
    }
    if(code_item == "退货原因") {//
        dialog = static_cast<QDialog *>(new w_bt_dish_quit(this));
    }
    if(code_item == "客户") {//
        dialog = static_cast<QDialog *>(new w_i_inventory_provider_costumer(this));
    }

    if(code_item == "会员参数设置") {//
        dialog = static_cast<QDialog *>(new w_m_member_paramset2(this));
    }
    if(code_item == "会员类型变更") {//
        dialog = static_cast<QDialog *>(new w_m_member_type(this));
    }
    if(code_item == "会员资料维护") {//
        dialog = static_cast<QDialog *>(new w_m_member_list(this));
    }
    if(code_item == "会员积分管理") {//
        dialog = static_cast<QDialog *>(new w_m_member_point(this));
    }


    if(code_item == "会员状态管理") {//
        dialog = static_cast<QDialog *>(new w_m_member_statemanage(this));
    }
    if(code_item == "充值") {//
        w_m_member_list::statictorecharge(this, 0, 0, "");
    }
    if(code_item == "芯片清除") {//
        dialog = static_cast<QDialog *>(new w_m_member_cardic(this));
    }
    if(code_item == "会员报表") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_member(this));
    }

    if(code_item == "采购入库") {//
        dialog = static_cast<QDialog *>(new w_inventory_input("RK",this));
    }
    if(code_item == "采购退货") {//
        dialog = static_cast<QDialog *>(new w_inventory_input("TH",this));
    }
    if(code_item == "商品报损") {//
        dialog = static_cast<QDialog *>(new w_inventory_input("CK",this));
    }
    if(code_item == "库存盘点") {//
        dialog = static_cast<QDialog *>(new w_inventory_draw(this));
    }
    if(code_item == "供应商") {//
        dialog = static_cast<QDialog *>(new w_i_inventory_provider(this));
    }

    if(code_item == "盘点查询") {//
        dialog = static_cast<QDialog *>(new w_i_inventory_check(this));
    }
    if(code_item == "进销存汇总") {//
        dialog = static_cast<QDialog *>(new w_i_inventory_Invoicing(this));
    }
    if(code_item == "单据汇总查询") {//
        dialog = static_cast<QDialog *>(new w_i_inventory_summaryquery(this));
    }
    if(code_item == "库存查询") {//
        dialog = static_cast<QDialog *>(new w_i_inventory_query(this));
    }

    if(code_item == "商品销售汇总") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_dishsale(this));
    }
    if(code_item == "销售流水") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_saleflow(this));
    }
    if(code_item == "收银流水") {//
//#ifdef QT_DEBUG
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_cash(this));
//        dialog = static_cast<QDialog *>(new w_rpt_report_frame_cashflow(this));
    }
    if(code_item == "商品销售排行统计") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_dishsalerank(this));
    }
    if(code_item == "营业数据按单查询") {//
        dialog = static_cast<QDialog *>(new business_documents_query(this));
    }
    if(code_item == "反结账查询") {//
        dialog = static_cast<QDialog *>(new back_anti_checkout(this));
    }
    if(code_item == "退货情况报表") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_dishquit(this));
    }
    if(code_item == "交班报表") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_shift(this));
    }
    if(code_item == "促销报表") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_promotionreport(this));
    }

    if(code_item == "日营业情况统计") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_daybusinessreprot(this));
    }
    if(code_item == "日月周报表分析") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_daymonweekanalysis(this));
    }
    if(code_item == "原料销售报表") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_material_saleflow(this));
    }
    if(code_item == "服务员提成查询") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_servantpromotquery(this));
    }

    if(code_item == "桌台营业统计") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_tablebusinessreport(this));
    }
    if(code_item == "服务费与低消查询") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_servicefeelowpinquery(this));
    }
    if(code_item == "时段营业情况分析") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_periodbusinessanalysis(this));
    }
    if(code_item == "税收报表") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_frame_tax(this));
    }
    if(code_item == "品类销售营业分析") {//
        dialog = static_cast<QDialog *>(new w_rpt_report_categorysalesanalysis(this));
    }

    if(code_item == "外部设备") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_outer_settings(this));
    }
    if(code_item == "收银设置") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_outer_pay_set(this));
    }
    if(code_item == "收银方式") {//
        dialog = static_cast<QDialog *>(new w_bt_dish_paymode(this));
    }
    if(code_item == "促销设置") {//
        dialog=qobject_cast<QDialog *>(new w_sys_manage_promotion_setting(this));
    }
    if(code_item == "公司信息") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_companyinformation(this));
    }

    if(code_item == "操作员信息") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_operatorinformation(this));
    }
    if(code_item == "操作员权限") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_operatorright(this));
    }
    if(code_item == "数据库") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_sqlcopy(this));
    }
    if(code_item == "在线升级") {//
        {
            QString errstring;
            //!等待
            lds_messagebox_loading_show loading(this, "", "WAITING...");
            loading.show_delay();
            //!~等待
            if(false == fastfd_update(this).check_need_update(&errstring)) {
                lds_messagebox::warning(this, MESSAGE_TITLE_VOID, errstring);
                return;
            }
        }
        if(1 == lds_messagebox::question(this, MESSAGE_TITLE_VOID, QObject::tr("有新版本更新"), QObject::tr("确认"), QObject::tr("取消"))) {
            return;
        }

        lds_dialog_input inputdialog(this);
        inputdialog.ui->label->setText(QObject::tr("密码") + ":");
        inputdialog.ui->lineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
        inputdialog.ui->lineEdit->setText(n_func::f_get_godpassword());
#endif
        inputdialog.setWindowTitle(QObject::tr("有新版本更新"));
        if(QDialog::Accepted != transparentCenterDialog(&inputdialog).exec()) {
            return;
        }
        if(inputdialog.ui->lineEdit->text()!=n_func::f_get_godpassword()) {
            lds_messagebox::warning(this, MESSAGE_TITLE_VOID, QObject::tr("密码错误"));
            return;
        }
        public_sql::softUpdate();
        this->reject();
    }
    if(code_item == "注册") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_sqlcopy_soft_limit2_for_customer(this));
    }
    if(code_item == "云设置") {//
        dialog = static_cast<QDialog *>(new w_sys_manage_cloudsync_cloud2(this));
    }
    if(code_item == "采购申请") {//
        dialog= static_cast<QDialog *>(new material_purchase_apply(this));
    }
    if(code_item == "门店入库") {//
        dialog= static_cast<QDialog *>(new material_purchase_in_storage(this));
    }
    if(code_item == "门店出库") {//
        dialog= static_cast<QDialog *>(new material_loss_apply(this));
    }
    if(dialog) {
        int ret = -1;
        dialog->setWindowTitle(text + dialogClassName());
        if(dialog->size() == lds::MAIN_WINDOW_SIZE) {
            ret =  transparentCenterDialog_void(dialog).exec();
        } else {
            ret = transparentCenterDialog(dialog).exec();
        }
        delete dialog;
        dialog=0;
        if(public_sql::Reboot == ret) {
            this->reject();
            public_sql::save_login->hideall_and_back_login();
        }
    }
}

void back_view_dialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Return) {
        this->done(public_sql::Accepted);
    }
    QDialog::keyPressEvent(e);
}

QTableView *back_view_dialog::creat_tableview()
{
    back_view_tableview *tableview = new back_view_tableview;
    itemDelegate *d = new itemDelegate;
    d->keepConnect(tableview);
    tableview->setItemDelegate(d);
    tableview->setStyleSheet("border:0;background:transparent;selection-background-color: transparent;");
    tableview->horizontalHeader()->hide();
    tableview->verticalHeader()->hide();
    lds::setHeaderResizeMode(tableview->verticalHeader(), QHeaderView::Stretch);
    lds::setHeaderResizeMode(tableview->horizontalHeader(), QHeaderView::Stretch);
    tableview->setSelectionMode(QTableView::SingleSelection);
    tableview->setEditTriggers(QTableView::NoEditTriggers);
    tableview->setShowGrid(false);
    connect(tableview, SIGNAL(clicked(QModelIndex)),this,SLOT(topop(QModelIndex)));
    return tableview;
}

back_view_dialog::itemDelegate::~itemDelegate()
{

}

void back_view_dialog::itemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int pwidth = lds::MAIN_WINDOW_SIZE.width()/15;

    int border = 3;
    QStringList texts = index.data().toString().split("\t", QString::SkipEmptyParts);
    QString text = texts.value(2);
    int dir = texts.value(0).toInt();

    QString imagepath = texts.value(1);
    const QRect rect = option.rect.adjusted(border,border,-border,-border);
    QFont f = painter->font();
    QRect rect_text;
    QRect pixmap_rect;
    QSize pixmap_size;
    if(dir == -1 || texts.isEmpty()) {
        return;
    }
    //1
    if(option.state & QStyle::State_Selected) {
        imagepath += "p";
        painter->fillRect(rect, QColor("#ff546b"));
    } else {
        painter->setPen(*borderColor);
        painter->drawRect(rect);
        painter->setPen(Qt::NoPen);
        painter->fillRect(rect, *bgColor);
    }
    //2
    painter->setPen(*textColor);
    f.setPointSize(12);
    painter->setFont(f);

    imagepath += ".png";
    //垂直
    if(dir == 1) {
        //        QSvgRenderer svg_render;
        //        svg_render.load(imagepath+".png");
        rect_text = QRect(rect.x(),  rect.y() + rect.height()*2/3, rect.width(), rect.height()/3);
        //若是文本都绘制不完整的话，则不会绘制图片
        //        if(false == rect_text.contains(painter->boundingRect(rect_text, (Qt::AlignCenter|Qt::TextWordWrap), text))) {
        //            painter->drawText(rect, (Qt::AlignCenter|Qt::TextWordWrap), text);
        //            return;
        //        }

        pixmap_rect = QRect(rect.x(),  rect.y() + rect.height() /6, rect.width(), rect.height()/3);
        pixmap_size = QSize(pwidth, pwidth);
        pixmap_rect = QStyle::alignedRect(option.direction, Qt::AlignCenter, pixmap_size, pixmap_rect).intersected(rect.adjusted(5, 5, -5, -5));

        if(name_image_map.value(imagepath).isNull())
            name_image_map.insert(imagepath, QImage(imagepath));

        painter->drawImage(pixmap_rect, name_image_map.value(imagepath));
        //        }
        painter->drawText(rect_text, (Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap), text);
    }

    if(dir == 0) {
        //        QSvgRenderer svg_render;
        //        svg_render.load(imagepath+".png");
        rect_text = QRect(rect.x() +rect.height(),  rect.y(), rect.width()-rect.height() - 10, rect.height());
        //若是文本都绘制不完整的话，则不会绘制图片
        //        if(false == rect_text.contains(painter->boundingRect(rect_text, (Qt::AlignCenter|Qt::TextWordWrap), text))) {
        //            painter->drawText(rect, (Qt::AlignCenter|Qt::TextWordWrap), text);
        //            return;
        //        }

        pixmap_rect = QRect(rect.x(),  rect.y(), rect.height(), rect.height());
        pixmap_size = QSize(pwidth, pwidth);
        pixmap_rect = QStyle::alignedRect(option.direction, Qt::AlignCenter, pixmap_size, pixmap_rect).intersected(QRect(rect.x(), rect.y(), rect.height(), rect.height()).adjusted(10, 10, -10, -10));
        //        if(!lds_svg_render::svg_render_by_rect(&svg_render, painter, pixmap_rect, rect)) {
        //            if(!QPixmap(imagepath).isNull())
        if(name_image_map.value(imagepath).isNull())
            name_image_map.insert(imagepath, QImage(imagepath));
        painter->drawImage(pixmap_rect, name_image_map.value(imagepath));
        //        QPixmap(imagepath).scaled(pixmap_size);
        //        painter->drawPixmap(pixmap_rect.topLeft(), QPixmap(imagepath).scaled(pixmap_size));

        painter->drawText(rect_text, (Qt::AlignVCenter|Qt::AlignLeft|Qt::TextWordWrap), text);
    }
}

void back_view_dialog::itemDelegate::keepConnect(back_view_tableview *tableview)
{
    bgColor = &tableview->bgColor;
    textColor = &tableview->textColor;
    borderColor = &tableview->borderColor;
}



void back_view_tableview::setbgColor(const QColor &color)
{
    bgColor = color;
}

QColor back_view_tableview::getbgColor() const
{
    return bgColor;
}

void back_view_tableview::settextColor(const QColor &color)
{
    textColor = color;
}

QColor back_view_tableview::gettextColor() const
{
    return textColor;
}

void back_view_tableview::setborderColor(const QColor &color)
{
    borderColor = color;
}

QColor back_view_tableview::getborderColor() const
{
    return borderColor;
}

back_view_tableview::back_view_tableview(QWidget *parent):QTableView(parent)
{
    this->setProperty("outer_stylesheet", "back_view_tableview");
}
QStandardItem *back_view_dialog::createItem(const QString &direct, const QString &image_path, const QString &text, const QString &usr)
{
#ifdef QT_DEBUG
    if(qAppTr(usr) != text) {
        qDebug() << __FILE__ << __LINE__ << "text != usr" << text << usr;
        QObject *o = 0;
        o->children();
    }

    if(w_sys_manage_operatorright::index_zh(usr) == -1) {
        qDebug() << __FILE__ << __LINE__ << "usr not existed" << text << usr ;
        QObject *o = 0;
        o->children();
    }
#endif
    QStandardItem *item = new QStandardItem;
    item->setData(direct + "\t" +  image_path + "\t" + text, Qt::DisplayRole);
    item->setData(usr, Qt::UserRole + 1);
    return item;
}
