#include "w_sys_manage_operatorright_type_right.h"

w_sys_manage_operatorright_type_right::w_sys_manage_operatorright_type_right(QWidget *parent) :
    w_sys_manage_operatorright_right(ldsStringMap()
                                     .append("table", "cey_bt_dish_type")
                                     .append("table_no", "ch_dish_typeno")
                                     .append("table_name", "vch_dish_typename")
                                     .append("table_oper", "cey_bt_dish_type_oper"),
                                     parent)
{
}
