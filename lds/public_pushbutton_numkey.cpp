#include "public_pushbutton_numkey.h"
#include "lds.h"
#include <QFontDatabase>
#include <QtDebug>

public_pushbutton_numkey::public_pushbutton_numkey(QWidget *parent) : lds_pushbutton_wrap(parent)
{
    set_num_font = false;
}

void public_pushbutton_numkey::setText(const QString &text)
{
    if(false == set_num_font) {
        set_num_font = true;

        QPushButton::setText(text);
        QFont font = this->font();
        font.setFamily(lds::FONT_NUM_Family);
        font.setPointSize(17);
        font.setBold(true);
        if(text == "."
                || text == "C") {
            this->setFont(font);
        } else {
            bool ok;
            text.toInt(&ok);
            if(ok) {
                this->setFont(font);
            }
        }
    } else {
//        qDebug() << __FILE__ << __LINE__ << " setText only is conducted one time";
    }
}

