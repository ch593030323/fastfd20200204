#ifndef LDS_LINEEDIT_H
#define LDS_LINEEDIT_H

#include <QLineEdit>
#include <QLabel>

class lds_lineedit:public QLineEdit
{
    Q_OBJECT
public:
    lds_lineedit(QWidget *parent = 0);
    virtual ~lds_lineedit();

    void disableSelectAllWhenGetFocus();
    void setLittleText(const QString &little_text, const QColor &little_text_color);

protected:
    void focusOutEvent(QFocusEvent *e);
    void focusInEvent(QFocusEvent *e);
    void paintEvent(QPaintEvent *e);
signals:
    void getfocus();
    void outfocus();
private:
    QString little_text;
    QColor little_text_color;

public:
    void setcomMapBySql(const QString &key_value_sql);
    void setcomKey(const QString &key);
    QString comKey();
private:
    QMap<QString, QString> com_map;
    QString com_key;
};

#endif // LDS_LINEEDIT_H
