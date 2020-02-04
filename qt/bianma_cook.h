#ifndef BIANMA_COOK_H
#define BIANMA_COOK_H

#include <QDialog>

namespace Ui {
class bianma_cook;
}

class bianma_cook : public QWidget
{
    Q_OBJECT
    
public:
    explicit bianma_cook(QWidget *parent = 0);
    ~bianma_cook();
signals:
    void edit_text(const QString &str);
public slots:
    void cleartext();
private slots:
    void tocenter();
protected:
    void showEvent(QShowEvent *e);
private:
    Ui::bianma_cook *ui;
};

#endif // BIANMA_COOK_H
