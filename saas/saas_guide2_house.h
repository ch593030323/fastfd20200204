#ifndef SAAS_GUIDE2_HOUSE_H
#define SAAS_GUIDE2_HOUSE_H

#include <QFrame>

class saas_guide2_house : public QFrame
{
    Q_OBJECT
public:
    explicit saas_guide2_house(QWidget *parent = 0);

    void addWidget(QWidget *w);
    QWidget *tabkeWidget();
    QWidget *curWidget();
    int countWidget();
    void printWidgets();
protected:
    void paintEvent(QPaintEvent *e);
private:
    void hideAllWidgets();

private:
    QList<QWidget *> w_list;

};

#endif // SAAS_GUIDE2_HOUSE_H
