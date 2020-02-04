#include "public_pushbuton_left_right.h"
#include <QStylePainter>
#include <QPainter>
#include <QStyleOptionButton>
#include <QtDebug>
#include "lds.h"
#include "lds_checkbox.h"

static const QString CHECK_ON = "CHECK_ON";
static const QString CHECK_OFF = "CHECK_OFF";
public_pushbuton_left_right::public_pushbuton_left_right(QWidget *parent) :
    QPushButton(parent)
{
    xp1 = 5;
    yp1 = 5;
    xp2 = -5;
    yp2 = -5;

    this->setFocusPolicy(Qt::NoFocus);
    clickabled = true;
    this->setMinimumHeight(30);
}

void public_pushbuton_left_right::setTitleKey(const QString &title, const QVariant &value)
{
    check_title = title;
    check_value = value;
    if(key_value_list.count() > 0) {
        setText(title+ (is_n?"\n" : ""));
    } else {
        setText(title+ (is_n?"\n" : ""));
    }
    repaint();
}

void public_pushbuton_left_right::disableClicked()
{
    clickabled = false;
}

void public_pushbuton_left_right::setCheckedByIni(const QString &config_path, const QVariant &config_default_value, bool is_n)
{
    this->config_path = config_path;
    this->is_n = is_n;
    enableConnectCheck(this->text(), lds::sysconf->value(config_path, config_default_value));
}

void public_pushbuton_left_right::setCheckedByIniN(const QString &config_path, const QVariant &config_default_value)
{
    setCheckedByIni(config_path, config_default_value, true);
}

QVariant public_pushbuton_left_right::checkedValue()
{
    return check_value;
}

void public_pushbuton_left_right::enableConnectCheck(const QString &check_title, const QVariant &check_key_def)
{
    QList< QPair<QVariant, QString> > key_value_list;
    key_value_list.append(QPair<QVariant, QString>(true, CHECK_ON));
    key_value_list.append(QPair<QVariant, QString>(false, CHECK_OFF));

    enableConnectCheck(check_title, check_key_def, key_value_list);
}

void public_pushbuton_left_right::enableConnectCheck(const QString &check_title, const QVariant &check_key_def, const QList< QPair<QVariant, QString> > &key_value_list)
{
    this->key_value_list = key_value_list;
    setTitleKey(check_title, check_key_def);
    connect(this, SIGNAL(clicked()),this,SLOT(toSwitchChecked()));
}

void public_pushbuton_left_right::toSwitchChecked()
{
    int key_value_list_index = key_value_list_index_get(check_value);
    key_value_list_index++;
    if(key_value_list.count() > 0)
        key_value_list_index = key_value_list_index%key_value_list.count();
    else
        key_value_list_index = 0;

    QVariant value = key_value_list.value(key_value_list_index).first;
    if(config_path.length() > 0) {
        lds::sysconf->setValue(config_path, value);
    }
    setTitleKey(check_title, value);

    emit signal_switch(value);
}

QString public_pushbuton_left_right::key_value_list_value_get(const QVariant &key, const QString &defvalue)
{
    for(int k = 0; k < key_value_list.count(); k++) {
        if(key == key_value_list[k].first) {
            return key_value_list[k].second;
        }
    }
    return defvalue;
}

int public_pushbuton_left_right::key_value_list_index_get(const QVariant &key)
{
    for(int k = 0; k < key_value_list.count(); k++) {
        if(key == key_value_list[k].first) {
            return k;
        }
    }
    return -1;
}

void public_pushbuton_left_right::paintEvent(QPaintEvent *e)
{
    QStylePainter p(this);
    QStyleOptionButton o;
    QRect rect0 = this->rect().adjusted(xp1, yp1, xp2, yp2);
    initStyleOption(&o);
    QString t = o.text;
    o.text = "";
    p.drawControl(QStyle::CE_PushButton, o);

    //
    p.drawText(rect0,
               (is_n ? Qt::AlignLeft | Qt::AlignTop : Qt::AlignLeft | Qt::AlignVCenter),
                check_title);

    //
    QString value = key_value_list_value_get(check_value, check_value.toString());
    p.setPen(QColor("#ff546b"));

    p.setRenderHint(QPainter::Antialiasing);
    if(value == CHECK_OFF || value == CHECK_ON) {
        QPixmap pixmap;
        if(value == CHECK_ON)
            pixmap = QPixmap(lds_CheckBox::png_path_appcheckon());
        if(value == CHECK_OFF)
            pixmap = QPixmap(lds_CheckBox::png_path_appcheckoff());

        pixmap = pixmap.scaledToHeight((is_n ? rect0.height() / 1.8 : rect0.height()));
        p.drawPixmap(QStyle::alignedRect(this->layoutDirection(),
                                         (is_n ? Qt::AlignRight | Qt::AlignBottom : Qt::AlignRight | Qt::AlignVCenter),
                                         pixmap.size(), rect0), pixmap);
    } else {
        if(clickabled) {
            QFont font = p.font();
            font.setUnderline(true);
            p.setFont(font);
        }
        p.drawText(rect0, Qt::AlignRight | Qt::AlignBottom, value);
    }
}

void public_pushbuton_left_right::mouseReleaseEvent(QMouseEvent *e)
{
    if(clickabled) {
        QPushButton::mouseReleaseEvent(e);
    } else {
        this->setDown(false);
    }
}

bool public_pushbuton_left_right::isChecked() const
{
    return QPushButton::isChecked();
}
