#ifndef LDS_COLORLABEL_H
#define LDS_COLORLABEL_H

#include <QLabel>
#include <QMap>

class lds_colorlabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QString color READ color WRITE setcolor USER true)
public:
    explicit lds_colorlabel(QWidget *parent = 0);
    QMap<QString, QString> kcmap;
    QString color();
    void setcolor(const QString &c);
private:
    QString _c;

};

#endif // LDS_COLORLABEL_H
