#ifndef LDS_LABEL_CELL_H
#define LDS_LABEL_CELL_H

#include <QWidget>

class lds_label_cell : public QWidget
{
    Q_OBJECT
public:
    lds_label_cell(QWidget *parent);

    //virtual
    virtual QString paintLabel() = 0;
    virtual QString paintText() = 0;
    //~virtual
signals:
    void paintTextChanged();
};

#endif // LDS_LABEL_CELL_H
