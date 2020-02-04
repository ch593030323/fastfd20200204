#ifndef GETCHINESESPELL_H
#define GETCHINESESPELL_H

#include <QObject>

class getChineseSpell : public QObject
{
    Q_OBJECT
public:
    explicit getChineseSpell(QObject *parent = 0);
    static QString ChineseSpell(QString src);
    
};

#endif // GETCHINESESPELL_H
