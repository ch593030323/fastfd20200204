#ifndef LABEL_IMG_DESIGN_VIEW_H
#define LABEL_IMG_DESIGN_VIEW_H

#include <QWidget>
#include "label_img_header.h"
class QPrinter;

class label_img_design_view : public QWidget
{
    Q_OBJECT
public:
    explicit label_img_design_view(QWidget *parent = nullptr);
    ~label_img_design_view();

    void printFirst();
    void printAll();

    QPixmap pixmapFirst();
    void setTable(const label_img_header::LabelTable &table);
    const label_img_header::LabelTable &curTable();

    void left(int id);
    void right(int id);
    void top(int id);
    void bottom(int id);
    void scale(int id, int op);
protected:
    void paintEvent(QPaintEvent */*event*/);
private:
    int count();
    void setIndex(int k);
    void print();
private:
    int label_queue_index;
    label_img_header::LabelTable table;
    int off;
    QPrinter *printer;
};

#endif // LABEL_IMG_DESIGN_VIEW_H
