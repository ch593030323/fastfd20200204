#include "btnlanguageswicth.h"
#include <QPainter>
#include <QPaintEvent>
#include "w_sys_manage_outer_pay_set.h"
#include "lds.h"
#include "public_dialog.h"
#include "lds_messagebox.h"
#include "saas_pay_detail.h"


btnlanguageswicth::btnlanguageswicth(QWidget *parent)
    : btnkeyswicth(parent) {
#ifdef QT_DEBUG
    initKeys(_CN_, _EN_);
#endif
}

void btnlanguageswicth::initKeys(const QString &first_key, const QString &second_key)
{
    init(first_key, second_key, first_key);
}

void btnlanguageswicth::initKeys_conf()
{
    initKeys(lds::get_soft_curent_language_first(), lds::get_soft_curent_language_second());
}

bool btnlanguageswicth::switchData()
{
    if(!saas_pay_detail::SaaSFuncCheck(this, "0512"/*多语言*/)) {
        return false;
    }
    //
    btnkeyswicth::switchData();
    //


    qDebug() << __FILE__ << __LINE__ << firstKey();
    qDebug() << __FILE__ << __LINE__ << secondKey();

    public_Dialog::retranslateSystem(firstKey());
    lds::set_soft_curent_language(firstKey());
    lds::set_soft_curent_language_second(secondKey());

    qDebug() << __FILE__ << __LINE__ << lds::get_soft_curent_language_first();
    qDebug() << __FILE__ << __LINE__ << lds::get_soft_curent_language_second();
    return true;
}
