#include "w_sys_manage_operatorright_area_right.h"

w_sys_manage_operatorright_area_right::w_sys_manage_operatorright_area_right(QWidget *parent) :
    w_sys_manage_operatorright_right(ldsStringMap()
                                     .append("table", "cey_bt_table_area")
                                     .append("table_no", "ch_areano")
                                     .append("table_name", "vch_areaname")
                                     .append("table_oper", "cey_bt_table_area_oper"),
                                     parent)
{
}
