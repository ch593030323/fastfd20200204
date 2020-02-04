#ifndef W_BT_DISH_FLAG_H
#define W_BT_DISH_FLAG_H

#include <QObject>

class w_bt_dish_flag
{
public:
    w_bt_dish_flag();

    enum kitchenState {
        KITCHEN_MIN_DEF = 0x00,
        KITCHEN_FINISH,//厨打完成
        KITCHEN_VOID ,//置空不在使用
        KITCHEN_QUIT,//退货
        KITCHEN_CUICAI ,//催起

        //5
        KITCHEN_JIAOQI ,//叫起
        KITCHEN_GUAQI ,//挂起
        KITCHEN_ZHUANTAI ,//转台
        KITCHEN_FUZHI ,//商品复制
        KITCHEN_BINGTAI,//并台

        KITCHEN_JIACAI,//加菜单
        KITCHEN_WAIMAI,//加菜单
        KITCHEN_ZHUANTAI_CAIPIN,//商品转台
        KITCHEN_QUIT_KUAICAN,//快餐退货

        /*0x0b*/KITCHEN_MAX,//最大值
        ///最大为16个、还有2个空余
    };
    Q_DECLARE_FLAGS(kitchenStates, kitchenState)



    static void get_vch_plan_k3o3(QString &state, QString &id, const QString &ch_dishno, kitchenState s0 = KITCHEN_MIN_DEF);
    static char k2o3_state2hex_char(kitchenState state);
    static QString k2o3_stae2hex_str(kitchenState state);
    static QString k2o3_name(kitchenState state);
};

namespace KDS {
enum quitState{
    quit_state_wait = 0,//0收银软件
    quit_state_finish,//1收银软件
    quit_state_error,//2收银软件

    quit_state_processing,//3厨显
    quit_state_rejected,//4厨显
    quit_state_unexisted,//5厨显
    quit_state_expired//6厨显
};
QString quitStateName(int state);
//

enum cookState{
    cookPrepared = 0,
    cookCooking,//1
    cookFinished,//2
    cookToken,//3
    cookCanceled,//4
    cookMax
};

 QString  cookStateName(int state);
}

#endif // W_BT_DISH_FLAG_H
