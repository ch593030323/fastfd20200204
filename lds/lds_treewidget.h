#ifndef LDS_TREEWIDGET_H
#define LDS_TREEWIDGET_H

#include <QTreeWidget>
#include <QStack>
//0 一级
    //00 二级
    //01
        //010 三级
        //011
            //。。。
    //02
#include <QTreeWidgetItem>
class lds_treeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit lds_treeWidget(QWidget *parent = 0);
    void restore();
    QTreeWidgetItem *rootitem;
    int deepofitem(QTreeWidgetItem *item);

    void set_0_Index(int index);//特指深度为0 的对应的index的位置即0index
    int get_0_Index();//特指深度为0 的对应的index的位置即0index

    void deleteAllItem();

    void select_beforerow();

    QString key();
    void selectItemBykey(const QString &key);

    void setItemSizeHint(int column = 0, QSize size = QSize(60,30));
signals:
    void signalSavedFinished();
private slots:
    void savedeep();
    void savedeepEmitFinished();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    void setcurdeep();
    void deleteItem(QTreeWidgetItem *item);
    void setItemSizeHint0(QTreeWidgetItem *item, int column, QSize size);
    QVector<int> curdeep() {return treedeepvect;}
    QVector<int> treedeepvect;
    QStack<QString> stack_key;

};

#endif // LDS_TREEWIDGET_H
