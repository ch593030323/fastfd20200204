#ifndef W_SYS_MANAGE_SQLCOPY_SOFT_LIMIT2_FOR_LANGUAGE_H
#define W_SYS_MANAGE_SQLCOPY_SOFT_LIMIT2_FOR_LANGUAGE_H

#include <QDialog>
#include <QDate>

namespace Ui {
class w_sys_manage_sqlcopy_soft_limit2_for_language;
}

class w_sys_manage_sqlcopy_soft_limit2_for_language : public QDialog
{
    Q_OBJECT

public:
    static bool isValid();
    static QDate deadDate();
    static QStringList freeLanuageList();
    static bool currentLanuageIsFree();
    static bool currentLanuageIsFree(const QString &first, const QString &second);

    static bool checkLimit(const QString &first, const QString &second, QWidget *parent);
    static const QString group_name;
public:
    explicit w_sys_manage_sqlcopy_soft_limit2_for_language(QWidget *parent = 0);
    ~w_sys_manage_sqlcopy_soft_limit2_for_language();

private slots:
    void took();
    void tocancel();
    void toTerminalCodeCopy();
    void topaste();
    void toread();
private:
    Ui::w_sys_manage_sqlcopy_soft_limit2_for_language *ui;
};

#endif // W_SYS_MANAGE_SQLCOPY_SOFT_LIMIT2_FOR_LANGUAGE_H
