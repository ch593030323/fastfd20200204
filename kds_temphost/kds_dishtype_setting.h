#ifndef KDS_DISHTYPE_SETTING_H
#define KDS_DISHTYPE_SETTING_H

#include <QDialog>
#include <QStyledItemDelegate>

class QTableView;

namespace Ui {
class kds_dishtype_setting;
}

class kds_dishtype_setting : public QDialog
{
    Q_OBJECT

public:
    explicit kds_dishtype_setting(QWidget *parent = 0);
    ~kds_dishtype_setting();

private slots:
    void toadd();
    void toremove();
    void took();
private:
    Ui::kds_dishtype_setting *ui;



    class itemDelegate : public QStyledItemDelegate{
    public:
        itemDelegate(QObject*parent = 0) : QStyledItemDelegate(parent){}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
};

#endif // KDS_DISHTYPE_SETTING_H
