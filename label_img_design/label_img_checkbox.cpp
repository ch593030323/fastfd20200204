#include "label_img_checkbox.h"

label_img_checkbox::label_img_checkbox(int id, QWidget *parent)
    : QCheckBox(parent)
    , id(id)
{

}

int label_img_checkbox::curID()
{
    return id;
}
