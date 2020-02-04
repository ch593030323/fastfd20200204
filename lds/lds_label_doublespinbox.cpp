#include "lds_label_doublespinbox.h"
#include <QPainter>
#include "lds.h"
#include <QtDebug>
#include "lds_label_text.h"
#include "lds_messagebox.h"

lds_label_doublespinbox::lds_label_doublespinbox(QWidget *parent)
    : lds_label_cell(parent)
{
    bgLight = 0;
    left = lds_label_text::borderPix;
    top = lds_label_text::borderPix;
    right = lds_label_text::borderPix;
    bottom = lds_label_text::borderPix;


    const QString skin_name  = lds::sysconf->value("system_setting/skin_name", "obsidian").toString();
    if(skin_name == "obsidian")
    {
        bgColor = QColor("#2b2b2b");
    }
    if(skin_name == "green")
    {
        bgColor = QColor("#3a8200");
    }
    if(skin_name == "skyblue")
    {
        bgColor = QColor("#2291a3");
    }
}

QString lds_label_doublespinbox::paintLabel()
{
    return p_data._labelText;
}

QString lds_label_doublespinbox::paintText()
{
    return text();
}

void lds_label_doublespinbox::setDecimals(int decimals)
{
    p_data._decimals = decimals;
}

int lds_label_doublespinbox::decimals()
{
    return p_data._decimals;
}

void lds_label_doublespinbox::setValue(double value)
{
    p_data._edit_value.clear();
    p_data._value = value;
    selectAll();
    update();

    emit valueChanged(p_data.value());
    emit paintTextChanged();
}

void lds_label_doublespinbox::setValue(const QString &v)
{

    QString save_text = p_data._edit_value;
    if(isselectAll()) {
        p_data._edit_value.clear();
        selectCLear();
    }

    p_data._edit_value += v;
    if(p_data._edit_value.count(".") > 2) {
        p_data._edit_value = save_text;
    }
    if(p_data._edit_value.contains(".")
            && p_data._edit_value.length() - p_data._edit_value.indexOf(".") > (p_data._decimals + 1)/*00.*/) {
        p_data._edit_value = save_text;
    }
    p_data._value = p_data._edit_value.toDouble();
    update();

    emit valueChanged(p_data.value());
    emit paintTextChanged();
}

void lds_label_doublespinbox::setLabelText(const QString &labelText)
{
    p_data._labelText = labelText;
}

void lds_label_doublespinbox::setBgColorLight(double bgLight)
{
    this->bgLight = bgLight;
}

double lds_label_doublespinbox::value()
{
    return p_data.value();
}

double lds_label_doublespinbox::value_if_prefix(double else_value)
{
    if(prefix().length() > 0) {
        return else_value;
    }
    return value();
}

QString lds_label_doublespinbox::text()
{
    int f = 1;
    if(bShowNegativeValue) f = -1;
    QString retv;
    if(p_data._edit_value.isEmpty())
        retv = p_data._prefix+lds::septNumber(f * p_data._value, p_data._decimals);
    else
        retv = p_data._prefix+p_data._edit_value;

    return retv;
}

void lds_label_doublespinbox::enableSelectAll()
{
    p_data._enableSelectAll = true;
}

void lds_label_doublespinbox::selectAll()
{
    if(p_data._enableSelectAll)
        p_data._isselectAll = true;
}

void lds_label_doublespinbox::selectCLear()
{
    if(p_data._enableSelectAll)
        p_data._isselectAll = false;
}

bool lds_label_doublespinbox::isselectAll()
{
    return p_data._isselectAll;
}

void lds_label_doublespinbox::enableClickable()
{
    isclickable = true;
}

void lds_label_doublespinbox::enableShowNegativeValue()
{
    bShowNegativeValue = true;
}

QString lds_label_doublespinbox::falseText()
{
    return text();
}

void lds_label_doublespinbox::setBorder(int left, int top, int right, int bottom)
{
    this->left = left;
    this->top = top;
    this->right = right;
    this->bottom = bottom;
}

void lds_label_doublespinbox::setMessage(const QString &text)
{
    isclickable = true;
    message = text;
    bShowMessage = true;
}

void lds_label_doublespinbox::setPrefix(const QString &prefix)
{
    p_data._prefix = prefix;
}

QString lds_label_doublespinbox::prefix()
{
    return p_data._prefix;
}

void lds_label_doublespinbox::clear()
{
    p_data._value = 0;
    p_data._prefix.clear();
    p_data._edit_value.clear();
    update();
}

void lds_label_doublespinbox::setBgColor(const QColor &color)
{
    bgColor = color;
}

void lds_label_doublespinbox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QRect rect0 = this->rect().adjusted(left, 0, -right, -0);
    QLinearGradient linerGrad(QPointF(0, 0), QPointF(0, this->height()));
    if(isclickable) {
        linerGrad.setColorAt(0, lds::static_color_light(bgColor, bgLight + 1));
        linerGrad.setColorAt(1, lds::static_color_light(bgColor, bgLight));
    } else {
        linerGrad.setColorAt(0, lds::static_color_light(bgColor, bgLight));
        linerGrad.setColorAt(1, lds::static_color_light(bgColor, bgLight));
    }
    if(ispressed) {
        linerGrad.setColorAt(0, QColor("#ff546b"));
        linerGrad.setColorAt(1, QColor("#ff546b"));
    }
    if(false == isEnabled()) {
        linerGrad.setColorAt(0, lds::static_color_light(bgColor, bgLight + 1));
        linerGrad.setColorAt(1, lds::static_color_light(bgColor, bgLight + 1));
    }
    painter.fillRect(this->rect(), linerGrad);
    painter.setPen(QPen(QColor("white")));

    QFont f = painter.font();
    painter.drawText(rect0, Qt::AlignLeft | Qt::AlignVCenter, paintLabel());

    if(isselectAll()) {
        f.setUnderline(true);
        painter.setFont(f);
    }
    painter.drawText(rect0, Qt::AlignRight | Qt::AlignVCenter, paintText());
}

void lds_label_doublespinbox::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    //
    if(isclickable) {
        ispressed = true;
        update();
    }
}

void lds_label_doublespinbox::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
    //
    if(isclickable) {
        ispressed = false;
        update();
        emit signalClicked();
    }
    if(bShowMessage) {
        lds_messagebox::information(this, MESSAGE_TITLE_VOID, message);
    }
}


lds_label_doublespinbox_data::lds_label_doublespinbox_data()
{
    _value = 0;
    _decimals = 2;
    _isselectAll = false;
    _enableSelectAll = false;
}

double lds_label_doublespinbox_data::value()
{
    return QString::number(_value, 'f', _decimals).toDouble();
}

QString lds_label_doublespinbox_data::text()
{
    return QString::number(_value, 'f', _decimals);
}
