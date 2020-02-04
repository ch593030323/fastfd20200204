#ifndef KDS_MAIN_INFO_BRIEF_H
#define KDS_MAIN_INFO_BRIEF_H

#include <QDialog>
#include <QFrame>
#include "lds_roundeddialog_rect.h"
#include "lds_query_variant.h"
#include "lds_query_db.h"
#include "lds_const_var.h"

namespace Ui {
class kds_main_info_brief;
}

class lds_pushbutton;
class kds_main_info_brief : public QFrame
{
    Q_OBJECT
public:
    struct briefButtons{
        lds_pushbutton *cook;
        lds_pushbutton *service;
        lds_pushbutton *take;
        lds_pushbutton *cancel;
    };

    enum commitType {
        commitManual = 0,
        commitAuto
    };

public:
    explicit kds_main_info_brief(QWidget *parent = 0);
    ~kds_main_info_brief();

    QSet<int>modifiedFlow_ids(int state);

    void setIndexJson(const QByteArray &index_json, const  QString&which);
    QByteArray getIndexJson();
    int getIndexJsonCount();

    void setCommitType(commitType commit_type);
    lds_const_var<QString> which;
signals:
    void signalRejected();
    void signalBack();
    void signalRefresh();
    void signalRefreshSingle();
private slots:
    void tobuttoncheck();
    void tobuttonsRefresh();
public slots:
    void topagedown();
    void topageup();

    void toallcooking();
    void toallservice();
    void toreset();
    void toordercook();
    void toorderservice();
    void toordertake();
    void toprintsettings();
private:
    void buttonsRefresh();
    void btnslistMapToItemrow(int row_begin);
    void assignQuickButton();
    void pushButton_cook_style();
protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);
private:
    Ui::kds_main_info_brief *ui;
    QList<briefButtons>btns_list;
    int save_width;
    commitType commit_type;
};

#endif // KDS_MAIN_INFO_BRIEF_H
