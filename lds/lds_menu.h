#ifndef LDS_MENU_H
#define LDS_MENU_H

#include <QMenu>
#include <QWidgetAction>
#include <QDialog>

class lds_menu : public QMenu
{
    Q_OBJECT
public:
    explicit lds_menu(QWidget *parent = 0);
    ~lds_menu();
    void setPopWidget(QWidget *widget);
private slots:
    void addPopWidget();
private:
    QWidget *widget;
};

#endif // LDS_MENU_H
