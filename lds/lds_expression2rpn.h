#ifndef LDS_EXPRESSION2RPN_H
#define LDS_EXPRESSION2RPN_H
#include <QVector>
#include <QString>
#include <QRegExp>

class lds_expression2RPN
{
public:
    lds_expression2RPN();
    static QRegExp factor;
    static QVector<QString> get_rpn_by(const QString &str);//convertToRPN(get_expression_by(str));
    static QVector<QString> get_expression_by(const QString &str);
    static QVector<QString> convertToRPN(const QVector<QString> &expression);
    static bool containOpFlag(const QString &express);
};

#endif // LDS_EXPRESSION2RPN_H
