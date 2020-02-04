#ifndef KDS_QUIT_H
#define KDS_QUIT_H

#include <QDialog>
#include "lds_query_db.h"

namespace Ui {
class kds_quit;
}

class kds_quit : public QDialog
{
    Q_OBJECT
public:
    explicit kds_quit(QWidget *parent = 0);
    ~kds_quit();

private slots:
    void took();
    void tobuttoncheck();
    void torefresh();
    void totimecheck();
private:
    void refresh();
    void setRowContent(int k, int ch_state, QMap<QString, qrtVariantPairList> &backqueue);
    void updateStateTransaction(int row, int ch_state, const QString &vch_memo = "");
private:
    Ui::kds_quit *ui;
};

#endif // KDS_QUIT_H
