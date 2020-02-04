#ifndef LDS_COMBOBOX_H
#define LDS_COMBOBOX_H

#include <QComboBox>
#include <QPixmap>


//combobox 的下拉高度，比寻常的高一些





class lds_ComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant comusrdata READ comusrdata WRITE setcomusrdata USER true)
    Q_PROPERTY(QVariant eitherUsrOrText READ eitherUsrOrText WRITE setEitherUsrOrText USER true)
public:
    struct PAIRLIST{
        inline PAIRLIST(const QString &_k, const QVariant &_u):
            key(_k),
            usrvalue(_u) {}
        QString key;
        QVariant usrvalue;
    };
    static  lds_ComboBox *createCom(const QString &key_value_sql);

public:
    explicit lds_ComboBox(QWidget *parent = 0);
    void resetTableView(int verticalHeight);
    void addItemsByMap(const  QMap<QString, QVariant>&texts);//顺序非是正序
    void addItemsBySql(const QString &key_value_sql);
    void addItemsByModel(const QAbstractItemModel *key_value_model);
    void addItemsByPairList(const QList< QPair<QString, QVariant> >& pair_list);

    void setcomtext(const QString &text);
    void setcomusrdata(const QVariant &usrtext);//这个model有usrdata和textdata的内容， 会根据usrdata来检索

    QVariant comusrdata(int comindex = -1);//返回usrdata
    QVariant curusrdata();//返回usrdata

    void setEitherUsrOrText(const QVariant &text);
    QVariant eitherUsrOrText();

    QString comText(const QVariant &d);

    void enableEmitColumn(int emitColumn);
    QVariant model_data(int row, int column, int role = Qt::DisplayRole);
    bool model_data_set(int row, int column, const QVariant &value, int role = Qt::DisplayRole);

    void copyItemsFrom(QComboBox *com);
    void copyItemsTo(QComboBox *com);
private slots:
    void toEmitColumn(int index);
    void toScrollDown();
    void toScrollUp();
signals:
    void signalEmitColumn(const QString text);
private:
    int emitColumn;
};

#endif // LDS_COMBOBOX_H
