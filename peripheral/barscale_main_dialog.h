#ifndef BARSCALE_MAIN_DIALOG_H
#define BARSCALE_MAIN_DIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QItemDelegate>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QTcpSocket>
#include <QUdpSocket>

namespace Ui {
class barscale_main_dialog;
}
class barscale_downdialog;
class  barscale_main_dialog : public QDialog
{
    Q_OBJECT
public:
    struct BARSCAEL_ITEMS{
        BARSCAEL_ITEMS() {}
        BARSCAEL_ITEMS(
                const QString &_pluno,//plu号
                const QString &_dishname,//品名
                const QString &_dishno,//品码
                const QString &_price,//价格
                const QString &_validdate//有效期
                ):
            pluno(_pluno),
            dishname(_dishname),
            dishno(_dishno),
            price(_price),
            validdate(_validdate)
        {}
        QString pluno;//PLU编号
        QString dishname;//商品名
        QString dishno;//商品代码
        QString price;//单价(/kg)
        QString validdate;//有效期

    };
    enum{
        BARSCALE_PLU = 0,
        BARSCALE_NAME,
        BARSCALE_CODE,
        BARSCALE_PRICE,
        BARSCALE_WEIGHTTYPE,

        BARSCALE_VALIDDATE,
        BARSCALE_FLAG,
    };
    class barscaleModel:public QSqlTableModel{
    public:
        barscaleModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase::database())
            :QSqlTableModel(parent, db), data_is_editing(false) {}
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant  data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
        bool data_is_editing;
    };

public:
    explicit barscale_main_dialog(QWidget *parent = 0);
    ~barscale_main_dialog();
    void set_shop_name(const QString &shop_name);//设置商店名称
    QString shop_name();
    void keep(QSqlQuery &queryouter/*外部数据库的sql语句*/,
              const QString sqlwherebarcode/*sql语句需要提供barcode(条码) = '%1'类似的字段：例如-》select vch_name,num_price,dt_validdate  from x where barcode = '%1'
          */
              );

private:
    void update_downmodel();
    void creatItem(QList<QStandardItem *> &items, const QStringList &list);
    bool brefore_getscaletypes();
protected:
    void goodsadd(const QList<BARSCAEL_ITEMS> items);
protected slots:
    virtual void togoodadd();//添加

private slots:
    void on_pushButton_clicked();//下载商品信息

    void on_pushButton_2_clicked();//清除PLU

    void on_pushButton_3_clicked();//设置标签(内置)

    void on_pushButton_4_clicked();//网络管理

    void on_pushButton_5_clicked();//退出

    void downdialog_exit();

    void update_barcodetype(const QString scaletype);

    void save_barscale();//保存
\
    void update_scaleflag(const QString &flag);//前导码

    void on_pushButton_export_clicked();//导出

    void on_pushButton_import_clicked();

    void on_pushButton_remove_clicked();//移除

    void on_pushButton_allcontrol_clicked();//批处理

    void on_pushButton_clearsection_clicked();

    void on_pushButton_switch_mode_clicked();

    void toshowother();

    void to_plu_keep_code();
private:
    Ui::barscale_main_dialog *ui;
    barscaleModel *downmodel0;
    QStandardItemModel *downmodel;
    barscale_downdialog *downdialog;
    bool down_isbreak;
    QList<BARSCAEL_ITEMS> items_list;

    int switch_mode;
};

class barscale_main_delegate: public QItemDelegate{
public:
    barscale_main_delegate(barscale_main_dialog *parent, QObject *parent1=0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QMap<int, QString> modelinfos;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    barscale_main_dialog *_parent;
};

class barscale_main_SOCKET{

public:
    enum TYPE{
        TCP,
        UDP
    };
    barscale_main_SOCKET();
    virtual ~barscale_main_SOCKET();

    qint64 writedirect(const QByteArray & byte);

    QAbstractSocket *tcpSocket;
    void resetSocket(TYPE _type);

    bool tryconnect(const QString &hostaddress, quint16 port, int msecs = 5000, bool *manual_break = 0);

    bool closeconnect(int msecs = 5000, bool *manual_break = 0);

    bool waitForReadyRead(int msecs = 5000, bool *manual_break = 0);
};

#endif // BARSCALE_MAIN_DIALOG_H
