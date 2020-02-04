#include "w_bt_dish_flag.h"

w_bt_dish_flag::w_bt_dish_flag()
{
}


QString KDS::quitStateName(int state)
{
    switch(state) {
    case quit_state_wait: return QObject::tr("等待退货");
    case quit_state_processing:return QObject::tr("正在退货");
    case quit_state_finish:return QObject::tr("退货完成");
    case quit_state_error:return QObject::tr("退货报错");
    case quit_state_rejected:return QObject::tr("退货拒绝");
    case quit_state_unexisted:return QObject::tr("不存在");
    case quit_state_expired:return QObject::tr("退货超时");
    }
    return "unknow";
}


QString KDS::cookStateName(int state)
{
    switch(state) {
    case cookPrepared:  return QObject::tr("等待制作");
    case cookCooking:   return QObject::tr("正在制作");
    case cookFinished:   return QObject::tr("已经完成");
    case cookToken:   return QObject::tr("已经取走");
    case cookCanceled:   return QObject::tr("已经作废");
    }
    return "unknow state";
}
