#ifndef PUBLIC_SCALE_LED_H
#define PUBLIC_SCALE_LED_H

#include <QWidget>
#include <QMap>

class public_scale_led : public QWidget
{
    Q_OBJECT
public:
    struct cellData{
        QString key;
        QString title;
        QString number;
        QString suffix;
    };
public:
    explicit public_scale_led(QWidget *parent = 0);

    void appendCell(const QString &key, const QString &title, const QString &suffix);

    void updateCellNumber(const QString &key, const QString &number);
    void updateCellNumber(int index, const QString &number);
    QString cellNumber(const QString &key);

    void updateCellTitle(const QString &key, const QString &title);

    QColor greenColor();

    void setFONT_DIGIT_Family(const QString &FONT_DIGIT_Family);


    const QList<cellData> &get_cell_list_ref();
protected:
    virtual void paintEvent(QPaintEvent *e);
private:
    QList<cellData> cell_list;
    QString FONT_DIGIT_Family;

};

#endif // PUBLIC_SCALE_LED_H
