#ifndef BAR_SCALE_DOWNDIALOG_H
#define BAR_SCALE_DOWNDIALOG_H

#include <QDialog>
#include <QItemDelegate>
#include <QApplication>
#include <QTimer>
#include <QPainter>
#include <QStandardItemModel>
#include "barscale_main_dialog.h"

class Ui_barscale_downdialog;

class barscale_downdialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit barscale_downdialog(barscale_main_dialog *parent);
    ~barscale_downdialog();
    QStandardItemModel *model;
    void showUpdate();
    QObject *finishButton();

    Ui_barscale_downdialog *ui;
private slots:
    void updateProgressbar(const QString &step/*1/6*/, int row/*2*/);//更新taleview里的进度条
    void updateBottomTime();
    void updateButtonText(const QString &text);
    void startTimer();
    void stopTimer();

private:
    class ItemDelegate : public QItemDelegate
    {
    public:
        explicit ItemDelegate(QObject *parent = 0) : QItemDelegate(parent) {}
        virtual void paint(QPainter * painter,const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
            int c = index.column();
            if(c == 5) {
                //12/456
                QStyleOptionProgressBar *bar = new QStyleOptionProgressBar;
                QRect bar_rect =
                        QRect(option.rect.x(), option.rect.y() + option.rect.height() /2 ,
                              option.rect.width(), option.rect.height() /2);
                QRect text_rect =
                        QRect(option.rect.x(), option.rect.y() ,
                              option.rect.width(), option.rect.height() /2);
                bar->rect = bar_rect;
                QString info = index.model()->data(index, Qt::DisplayRole).toString();
                QStringList infos = info.split("/");
                if(infos.size() == 2) {
                    bar->maximum = infos.value(1).toInt();
                    bar->progress = infos.value(0).toInt();
                }
                QApplication::style()->drawControl(QStyle::CE_ProgressBar,bar,painter);
                painter->drawText(text_rect, Qt::AlignCenter, info);

            } else {
                QItemDelegate::paint(painter,option,index);
            }
        }
    };

    QTimer *timer;
    int time_go;
    barscale_main_dialog *_parent;
};

#endif // BAR_SCALE_DOWNDIALOG_H
