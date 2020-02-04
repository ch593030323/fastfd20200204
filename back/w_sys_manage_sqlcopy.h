#ifndef W_SYS_MANAGE_SQLCOPY_H
#define W_SYS_MANAGE_SQLCOPY_H

#include <QDialog>

class back_view_dialog1;
class Ui_w_sys_manage_sqlcopy_Dialog;
class w_sys_manage_sqlcopy : public QDialog
{
    Q_OBJECT
public:
    explicit w_sys_manage_sqlcopy(QWidget *parent = 0);
    static void static_tobackup(QWidget *parent);//备份
    static QStringList static_tobackup_infos;

    static bool enkeep();
    static bool cnkeep();
    static void unmount_u_pan(QWidget *parent);
public slots:
    void check_backuppath();
private slots:
    void tobackup_strategy();//数据库备份策略
    void torestore();//恢复
    void tobackup();//备份

    void init_sql_VOID();//数据库初始化
    void init_sql_CN();//数据库中文演示
    void init_sql_EN();//数据库英文演示

    void tosetinit();//配置初始化
    void filedelete();//文件删除
    void toexit();

    void getfilepath();//磁盘
    void getfilepath_2();//sd
    void getfilepath_3();//usb

    void toumount_upan();
    void to_update_btn();//若radio多选时，备份，删除不可用

    void to_admin_set();//关于软件logo，使用时间的设置

    void to_zipbackup(bool f);

    void to_sql_upload_ftp();
private:
    QString getcomtext();//获取备份文件的文件名
    QString getlabeltext();//获取备份文件的路径
    void updatecomrtext();//更新com的文件列表信息
    bool check_restore();
private:
    Ui_w_sys_manage_sqlcopy_Dialog *ui;
};

//备份策略
class Ui_w_sys_manage_sqlcopy_backup_strategy_Dialog;
#include <QDateTime>
class w_sys_manage_sqlcopy_backup_strategy:public QDialog{
    Q_OBJECT
public:
    explicit w_sys_manage_sqlcopy_backup_strategy(QWidget *parent = 0);
private slots:
    void took();
    void toexit();
private:
    Ui_w_sys_manage_sqlcopy_backup_strategy_Dialog *ui;
};
/*
 * 1更改logo 80.90
 * 2软件使用限制 70.80
*/
//软件到期无法使用
#include <QDateTime>
class Ui_w_sys_manage_sqlcopy_soft_limit_Dialog;
class w_sys_manage_sqlcopy_soft_limit:public QDialog{
    Q_OBJECT
public:
    explicit w_sys_manage_sqlcopy_soft_limit(QWidget *parent = 0);
private:
    Ui_w_sys_manage_sqlcopy_soft_limit_Dialog *ui;
};
#endif // W_SYS_MANAGE_SQLCOPY_H
