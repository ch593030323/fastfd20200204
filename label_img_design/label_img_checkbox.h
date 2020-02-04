#ifndef LABEL_IMG_CHECKBOX_H
#define LABEL_IMG_CHECKBOX_H

#include <QCheckBox>
#include "label_img_header.h"
#include "label_img_design_view.h"

class label_img_checkbox : public QCheckBox
{
    Q_OBJECT
public:
    explicit label_img_checkbox(
            int id,
            QWidget *parent = nullptr);
    int curID();
private:
    int id;
};

#endif // LABEL_IMG_CHECKBOX_H
