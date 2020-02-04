#ifndef lds_toolbar_H
#define lds_toolbar_H

#include <QWidget>
#include <QToolButton>
#include <QMap>
#include <QLineEdit>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QBoxLayout>
#include "lds_toolbar_button.h"



//多个toolbutton的布局




class QPushButton;
class lds_toolbar : public QFrame
{
    Q_OBJECT
public:
    static const QString stretch;
public:
    explicit lds_toolbar(QWidget *parent = 0);
    void setup(const QStringList &names, Qt::Alignment flag=Qt::AlignLeft);
    lds_toolbar_button *index_widget(const QString &name);
    lds_toolbar_button *index_widget(int index);

    void setDefaultFalseAutoDefaultFalse(const QString &name);

private:
    void layoutaddwidget(QBoxLayout *layout, const QStringList &names, Qt::Alignment flag);
private:
    QMap<QString,lds_toolbar_button *> nameTool;
    QList<lds_toolbar_button *> nameList;
};

#endif // lds_toolbar_H
