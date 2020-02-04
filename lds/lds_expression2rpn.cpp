#include "lds_expression2rpn.h"

lds_expression2RPN::lds_expression2RPN()
{
}

QVector<QString> lds_expression2RPN::get_rpn_by(const QString &str)
{
    return convertToRPN(get_expression_by(str));
}

QVector<QString> lds_expression2RPN::get_expression_by(const QString &str)
{
    //过滤空格
    QString ep = str;
    ep = ep.replace(" ", "");
    //开始获取单元
    QVector<QString> split_vector;
    bool ok = false;
    ep.toDouble(&ok);
    if(ok) {
        split_vector << ep;
        goto end;
    }
    for(int index = 0; index < ep.count();) {
        QString p0;
        p0 = ep.mid(index, 1);
        split_vector.append(p0);
        index ++;
        if(!containOpFlag(split_vector[split_vector.count() - 1])) {
            do{
                if(index >= ep.count()) break;
                p0 = ep.mid(index, 1);//(1-int_discount*0.01)
                if(!containOpFlag(p0)) {//.contains(factor)) {
                    split_vector[split_vector.count() - 1] += p0;
                    index++;
                } else {
                    break;
                }
            } while(1);
        }
    }
end:
    return split_vector;
}

QVector<QString> lds_expression2RPN::convertToRPN(const QVector<QString> &expression)
{

    // write your code here
    QVector<QString>op;//符号栈
    QVector<QString>num;//表达式结果栈
    for(int i=0;i<expression.size();i++)//一遍扫描
    {
        if(expression[i] == "+" || expression[i] == "-")//处理加号、减号
        {
            if(op.size()==0)
                op.push_back(expression[i]);
            else
            {
                while(op.size()!=0 && (op[op.size()-1] == "*" || op[op.size()-1] == "/" ||op[op.size()-1] == "+" || op[op.size()-1] == "-"))
                {
                    QString s=op.back();
                    op.pop_back();
                    num.push_back(s);

                } op.push_back(expression[i]);
            }
            if(op[op.size()-1] == "(")
            {
                op.push_back(expression[i]);
            }
        }
        else if(expression[i] == "*" || expression[i] == "/")//处理乘号、除号
        {
            if(op.size()==0)
                op.push_back(expression[i]);
            else if(op[op.size()-1] == "*" || op[op.size()-1] == "/" )
            {
                QString s=op.back();
                op.pop_back();
                num.push_back(s);
                op.push_back(expression[i]);
            }
            else if(op[op.size()-1] == "+" || op[op.size()-1] == "-")
            {
                op.push_back(expression[i]);
            }
            else if(op[op.size()-1] == "(")
            {
                op.push_back(expression[i]);
            }
        }
        else if(expression[i] == "(")//处理左括号
        {
            op.push_back(expression[i]);
        }
        else if(expression[i] == ")")//处理右括号
        {
            while(op.back()!="(")
            {
                QString s=op.back();
                op.pop_back();
                num.push_back(s);
            }
            op.pop_back();
        }
        else//运算数直接压入表达式结果栈
        {
            num.push_back(expression[i]);
        }
    }
    while(op.size()!=0)//符号栈仍有符号时，将其压入表达式结果栈
    {
        QString s=op.back();
        op.pop_back();
        num.push_back(s);
    }
    return num;
}

bool lds_expression2RPN::containOpFlag(const QString &express)
{
//    ("[()+\\-*/]");
    return express.contains("(")
            || express.contains(")")
            || express.contains("+")
            || express.contains("-")
            || express.contains("*")
            || express.contains("/")
            ;

}
