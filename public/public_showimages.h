#ifndef PUBLIC_SHOWIMAGES_H
#define PUBLIC_SHOWIMAGES_H

#include <QDialog>
#include <QStyledItemDelegate>
#include <QStandardItemModel>

namespace Ui {
class public_showImages;
}

class public_showImages : public QDialog
{
    Q_OBJECT

public:
    explicit public_showImages(const QString &imagedir, QWidget *parent = 0);
    ~public_showImages();

    QStringList selected_image_path_list();
private slots:
    void toloadimage();
    void loadtimestart();
    void tocancel();
    void took();
private:
    Ui::public_showImages *ui;
    QStringList image_path_list;
    int imageindex;
    QTimer *timer;
    class delegate : public QStyledItemDelegate
    {
    public:
        delegate(QObject *parent) : QStyledItemDelegate(parent) {}
        // painting
        void paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    };
    class ItemModel : public QStandardItemModel{
    public:
        ItemModel(QObject *parent = 0) : QStandardItemModel(parent) {}
        Qt::ItemFlags flags(const QModelIndex &index) const;
    } ;
    ItemModel *model;
};

#endif // PUBLIC_SHOWIMAGES_H
