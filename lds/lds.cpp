#include "lds.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QStyle>
#include <QSettings>
#include <QFontDatabase>
#include <QDateTime>
#include <QPainter>
#include <QTimer>
#include <QHostInfo>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <QApplication>
#ifdef Q_OS_UNIX
#include <sys/ioctl.h>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#endif
#include <QFileInfo>
#include <QUdpSocket>
#include "math.h"
#include <QTextCodec>
#include <QDir>
#include <QProcess>
#ifdef Q_OS_WIN
#include "windows.h"
#endif
#include <QThread>

static QVariantMap systemKeyValueMap;
static QMap<QString, QString> reportModelMap;

lds_const_var<QString> lds::terminalCode;

lds_const_int lds::QT_FIXED_HEIGHT_BOTTOM;
lds_const_int lds::QT_FIXED_BETWEEN;
lds_const_int lds::QT_FIXED_EDIT_HEIGHT;
lds_const_int lds::BACK_LOGO_HEIGHT;
lds_const_int lds::BORDER_DEFAULT_HEIGHT;
lds_const_int lds::PAY_LEFT_SPACE;

lds_const_var<QPoint> lds::MAIN_WINDOW_TOPLEFT;
lds_const_var<QSize> lds::MAIN_WINDOW_SIZE;//主窗口的大小，底部高40px
lds_const_var<QSize> lds::FULL_WINDOW_SIZE;//前台,后台,中餐点菜,中餐前台
lds_const_var<QString> lds::FONT_NUM_Family;//外部数字字体
lds_const_var<QString> lds::FONT_SONGTI_Family;//外部宋体字体
lds_const_var<QString> lds::FONT_DEFAULT_Family;
lds_const_var<QString> lds::FONT_DIGIT_Family;

lds_const_var<QString> lds::LOCAL_TERMINAL_INDEX_FF;

lds_const_var<QString> lds::gs_operid;
lds_const_var<QString> lds::gs_opername;
lds_const_var<QString> lds::gs_password;

QVariantMap lds::skin_name_map;
lds_const_var<QString> lds::localdataPath = QString( "userdata/fastfd_localdata");
lds_const_var<QString> lds::arg_d_value;

QSettings * lds::sysconf = 0;

const Qt::HANDLE lds::MAIN_THREAD_ID = QThread::currentThreadId();

int (*lds::func_dec_price)() = NULL;

int (*lds::func_dec_num)() = NULL;

int (*lds::func_dec_amt)() = NULL;

static lds::resultCode  getTerminalCode(QString &terminalCode, QString *errstring = 0);

static bool ps_ef_grep(QStringList &pids, const QString &psFilter, const QString &exactFilter) {
    //cmds
    QString cmd;
    cmd = "ps -ef|grep " + psFilter;
    //readall
    QString readall;
    QByteArray d;
    d.resize(2014);
    FILE *fd = popen(cmd.toLocal8Bit(),"r");
    if(fd == NULL) {
        qDebug() << "open error";
        return false;
    }
    fread(d.data(), d.size() - 1, 1,  fd);
    pclose(fd);
    readall = d;
    //~readall
    QStringList readalllist = readall.split("\n", QString::SkipEmptyParts);
    foreach(const QString &readline, readalllist) {
        if(readline.lastIndexOf(exactFilter) != -1) {
            //前两个空格中的字符串
            QString b2str;
            int b0 = 0;
            b0 = readline.indexOf(" ", b0);
            while(readline.mid(b0, 1) == " ") b0 ++;

            int b1 = readline.indexOf(" ", b0);
            b2str = readline.mid(b0, b1 - b0);
            //~前两个空格中的字符串
            pids << b2str;
            continue;
        }
    }
    //~cmds
    return true;
}

lds::resultCode lds::LDS_INIT(int (*func_dec_price)(), int (*func_dec_num)(), int (*func_dec_amt)(), QString *errstring)
{
    resultCode retflag = resultOk;
    lds::func_dec_price = func_dec_price;
    lds::func_dec_num = func_dec_num;
    lds::func_dec_amt = func_dec_amt;

    lds::QT_FIXED_HEIGHT_BOTTOM.setOther(45);
    lds::QT_FIXED_BETWEEN.setOther(120);
    lds::QT_FIXED_EDIT_HEIGHT.setOther(30);
    lds::BACK_LOGO_HEIGHT.setOther(30);
    lds::BORDER_DEFAULT_HEIGHT.setOther(2);
    lds::PAY_LEFT_SPACE.setOther(2);

    //resetWINDOW_SIZE
    resetWINDOW_SIZE();
    //~
    //
    int FONT_ID = QFontDatabase::addApplicationFont("userdata/settings/font/HYQINGTING-55J.TTF");
    if(-1 != FONT_ID) {
        lds::FONT_NUM_Family.setOther(QFontDatabase::applicationFontFamilies(FONT_ID).first());
    }
    FONT_ID = QFontDatabase::addApplicationFont("userdata/settings/font/xinsongti.TTF");
    if(-1 != FONT_ID) {
        lds::FONT_SONGTI_Family.setOther(QFontDatabase::applicationFontFamilies(FONT_ID).first());
    }
    FONT_ID = QFontDatabase::addApplicationFont("userdata/settings/font/LiquidCrystal-Normal.otf");
    if(-1 != FONT_ID) {
        lds::FONT_DIGIT_Family.setOther(QFontDatabase::applicationFontFamilies(FONT_ID).first());
    }

    lds::FONT_DEFAULT_Family.setOther(qApp->font().family());

    skin_name_map_reset();

    ///获取机器号
    QString hardID;
    retflag = getTerminalCode(hardID, errstring);
    lds::terminalCode.setOther(hardID);
    ///~获取机器号
    return retflag;
}

void lds::resetWINDOW_SIZE()
{
    QSize available_size;
    QString screen_primary_size = lds::sysconf->value("system_setting/screen_primary_size", QObject::tr("自动获取")).toString();
    if(screen_primary_size.contains("x")) {
        available_size = (QSize(screen_primary_size.split("x").value(0).toInt(), screen_primary_size.split("x").value(1).toInt()));
    } else {
        available_size = (qApp->desktop()->availableGeometry().size());
#ifdef LINPOS_WIN
        lds::FULL_WINDOW_SIZE.setOther(qApp->desktop()->size());
#endif
    }
    resetWINDOW_SIZE(available_size);
}

void lds::resetWINDOW_SIZE(const QSize &available_size)
{

    lds::FULL_WINDOW_SIZE.setOther(available_size);
    //ubuntu 默认
    int sys_desk_bottom_height = 0;
#ifdef LINPOS_WIN
    sys_desk_bottom_height = qApp->desktop()->size().height() - qApp->desktop()->availableGeometry().height();
#endif
#ifdef LINPOS_OS6
    sys_desk_bottom_height = 40;
#endif
#ifdef LINPOS_OS7
    sys_desk_bottom_height = 0;
#endif
    lds::QT_FIXED_HEIGHT_BOTTOM.setOther(45);
    if(lds::FULL_WINDOW_SIZE.width() == 1920
            || lds::FULL_WINDOW_SIZE.width() == 1366) {
        lds::QT_FIXED_HEIGHT_BOTTOM.setOther(60);//3的倍数
    }

    lds::BACK_LOGO_HEIGHT.setOther(30);
    if(lds::FULL_WINDOW_SIZE.width() == 800) {
        lds::BACK_LOGO_HEIGHT.setOther(25);
    }

    //如果修改本处，可能导致软件闪退，
    lds::MAIN_WINDOW_SIZE.setOther(QSize(lds::FULL_WINDOW_SIZE.width(), lds::FULL_WINDOW_SIZE.height() - sys_desk_bottom_height));
    lds::MAIN_WINDOW_TOPLEFT.setOther(QApplication::desktop()->availableGeometry().topLeft());
    //
    lds::PAY_LEFT_SPACE.setOther((qMin(1024, lds::MAIN_WINDOW_SIZE.width()) / 3) + 0);
    if(lds::FULL_WINDOW_SIZE.width() == 1920) {
        lds::PAY_LEFT_SPACE.setOther(lds::MAIN_WINDOW_SIZE.width()*1/4);
    }
}

void lds::systemTextCodec()
{
    //设置编号
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");//中文可用//包含头文件
    QTextCodec::setCodecForLocale(codec);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#else
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
}

/**
本来方式试用与widget的flag是frameless的窗口
*/
QPoint lds::getPopPoint(const QPoint &gpos,const QPoint &gpos2, const QSize &popsize, Qt::LayoutDirection dir)
{
    QPoint pos = gpos;
    QPoint pos2 =  gpos2;
    QRect screen = QRect(QApplication::desktop()->availableGeometry().topLeft(), lds::MAIN_WINDOW_SIZE);// QApplication::desktop()->availableGeometry(pos);
    if (dir == Qt::RightToLeft) {
        pos.setX(pos.x()-popsize.width());
        pos2.setX(pos2.x()-popsize.width());
        if (pos.x() < screen.left())
            pos.setX(qMax(pos.x(), screen.left()));
        else if (pos.x()+popsize.width() > screen.right())
            pos.setX(qMax(pos.x()-popsize.width(), screen.right()-popsize.width()));
    } else {
        if (pos.x()+popsize.width() > screen.right())
            pos.setX(screen.right()-popsize.width());
        pos.setX(qMax(pos.x(), screen.left()));
    }
    if (pos.y() + popsize.height() > screen.bottom())
        pos.setY(pos2.y() - popsize.height());
    else if (pos.y() < screen.top())
        pos.setY(screen.top());
    if (pos.y() < screen.top())
        pos.setY(screen.top());
    if (pos.y()+popsize.height() > screen.bottom())
        pos.setY(screen.bottom()-popsize.height());
    return pos;
}

QPoint lds::getPopPoint(QWidget *parent, QWidget *popwidget)
{
    return getPopPoint(parent, parent->rect(), popwidget);
}

QPoint lds::getPopPoint(QWidget *parent, const QRect &section, QWidget *popwidget)
{
    QPoint bottomLeft = section.bottomLeft();
    QPoint topLeft = section.topLeft();

    bottomLeft = parent->mapToGlobal(bottomLeft);
    topLeft = parent->mapToGlobal(topLeft);

    return lds::getPopPoint(bottomLeft, topLeft, popwidget->size(), popwidget->layoutDirection());
}

QPoint lds::getPopPoint_size(QWidget *parent, QWidget *popwidget)
{
    return lds::getPopPoint0(parent, popwidget, SIZE);
}

QPoint lds::getPopPoint0(QWidget *parent, QWidget *popwidget, SIZETYPE type)
{
    QPoint pos = (parent->layoutDirection() == Qt::RightToLeft) ? parent->rect().bottomRight() : parent->rect().bottomLeft();
    QPoint pos2 = (parent->layoutDirection() == Qt::RightToLeft) ? parent->rect().topRight() : parent->rect().topLeft();

    pos = parent->mapToGlobal(pos);
    pos2 = parent->mapToGlobal(pos2);

    QSize size;
    switch(type) {
    case SIZEHNIT:
        size = popwidget->sizeHint();
        break;
    case SIZE:
        size = popwidget->size();
        break;
    }

    return getPopPoint(pos, pos2, size, parent->layoutDirection());
}

/**
pop居中与父类的的左上角的坐标*/
QPoint lds::getGlobalCenterPopup(QWidget *parent, QWidget *pop) {
    QPoint p = parent->mapToGlobal(QPoint(0,0));
    return QStyle::alignedRect(parent->layoutDirection(), Qt::AlignCenter, pop->size(), QRect(p, QSize(parent->size()))).topLeft();
}

QPoint lds::getParentCenterPopup(QWidget *parent, QWidget *pop)
{
    QPoint p =QPoint(0,0);
    return QStyle::alignedRect(parent->layoutDirection(), Qt::AlignCenter, pop->size(), QRect(p, QSize(parent->size()))).topLeft();

}

QString lds::get_sys_language()
{
    QSettings desktop_setting("/usr/desk/desktop.ini", QSettings::IniFormat);
    QString tran=desktop_setting.value("desktop/language", "1").toString();
    QString SYS_LANGUAGE = _CN_;
    if(tran == "0")
        SYS_LANGUAGE = _EN_;
#ifdef QT_DEBUG
    return "fr";
#endif
    return SYS_LANGUAGE;
}

QString lds::get_soft_curent_language_first()
{
    QString language = lds::sysconf->value("system_setting/SYS_LANGUAGE").toString();
    if(language.isEmpty()) {//随系统
        language = lds::get_sys_language();
    }
    return language;
}

void lds::set_soft_curent_language(const QString &key)
{
    lds::sysconf->setValue("system_setting/SYS_LANGUAGE", key);
}

QString lds::get_soft_curent_language_second()
{
    QString language = lds::get_soft_curent_language_first();
    QString language2 = lds::sysconf->value("system_setting/SYS_LANGUAGE_SECOND").toString();
    if(language2.isEmpty()) {//随系统
        if(language == _CN_) return _EN_;
        if(language == _EN_) return _CN_;
        return _EN_;
    }
    return language2;
}

void lds::set_soft_curent_language_second(const QString &key)
{
    lds::sysconf->setValue("system_setting/SYS_LANGUAGE_SECOND", key);
}


//!约定的主线程的mysql数据库别名为：空
//!约定的子线程的mysql数据库别名为：线程号
//!约定的主线程的其他数据库别名为：非空、非defconnectionname
//!约定的子线程的其他数据库别名为：线程号+"-"+connectionname
//! 本函数值获取mysql数据库的别名

QVariant lds::dtifnull(const QString &dtstring)
{
    if(dtstring.isEmpty()) return QVariant(QVariant::DateTime);
    return dtstring;
}

void lds::setHeaderResizeMode(QHeaderView *h, QHeaderView::ResizeMode mode)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    h->setSectionResizeMode(mode);
#else
    h->setResizeMode(mode);
#endif
}

void lds::setHeaderResizeMode(QHeaderView *h, int logicalIndex, QHeaderView::ResizeMode mode)
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    h->setSectionResizeMode(logicalIndex, mode);
#else
    h->setResizeMode(logicalIndex, mode);
#endif
}

void lds::setwflagFrameless(QWidget *widget)
{
    widget->setWindowFlags(Qt::FramelessWindowHint|widget->windowFlags());
}

QColor lds::static_color_light(const QString &c, double light_0_3, int a)
{
    QColor color(c);
    return static_color_light(color, light_0_3, a);
}

QColor lds::static_color_light(const QColor &color, double light_0_3, int a)
{
    if(color.alpha() == 0) return color;

    int h = color.hslHue();
    int s = color.hslSaturation();
    int l = color.lightness();
    return QColor::fromHsl(h, s, l + light_0_3 * 22, a);
}

QString lds::appName()
{
    return QFileInfo(qApp->applicationFilePath()).fileName();
}

void lds::showWidget(QWidget *w) {
    if(w->isHidden())
        w->show();
}

void lds::hideWidget(QWidget *w) {
    //又是widget->isVisiabled() 会判断无效
    if(!w->isHidden())
        w->hide();
}

void lds::hideWidget(QWidget *w, bool hide)
{
    if(hide) {
        lds::hideWidget(w);
    } else {
        lds::showWidget(w);
    }
}

void lds::hideWidget(QWidget *w1, QWidget *w2, bool hide)
{
    hideWidget(w1, hide);
    hideWidget(w2, hide);
}

bool lds::widgetIsShow(QWidget *w)
{
    return !w->isHidden();
}

bool lds::widgetIsHide(QWidget *w)
{
    return w->isHidden();
}

QSize lds::dsize()
{
    QStringList bofangg_wh = lds::sysconf->value("system_setting/bofangg_rate").toString().split("x");
    if(bofangg_wh.count() == 2) {
        return  QSize(bofangg_wh[0].toInt(), bofangg_wh[1].toInt());
    }
    return lds::FULL_WINDOW_SIZE;
}

QString lds::groupSeparatorV2str(double value1, int decimal, bool clearDotZero)
{
    // 由于QString::number(-0.0003, 'f', 2) = "-0.00";的情况 所以这边
    value1 = QString::number(value1, 'f',decimal).toDouble();
    //
    QString f = value1 < 0 ? "-" : "";
    QString d = QString::number(qAbs(value1), 'f', decimal);
    QString p2 = d.split(".").value(1);
    QString p1 = d.split(".").value(0);
    for(int k = p1.count() - 3; k > 0; k = k - 3) {
        p1.insert(k, ",");
    }
    if(clearDotZero) {
        while(p2.endsWith("0"))
            p2.chop(1);
    }
    ///
    if(p2.length() == 0) {
        return f + p1;
    }
    return f + p1 + "." +p2;
}

double lds::groupSeparatorStr2v(const QString &text)
{
    QString r = text;
    return r.replace(",", "").toDouble();
}

QString lds::septNumber(int value, int decimal, bool clearDotZero)
{
    return lds::groupSeparatorV2str(value * 1.0, decimal, clearDotZero);
}

QString lds::septNumber(double value, int decimal, bool clearDotZero)
{
    return lds::groupSeparatorV2str(value, decimal, clearDotZero);
}

bool lds::isSeptNumber(const QString &value)
{
    QString v1 = value;
    bool ok = false;
    v1.replace(",", "").toDouble(&ok);
    return ok;
}

void lds::showWidget(QWidget *w, bool show)
{
    if(show) {
        lds::showWidget(w);
    } else {
        lds::hideWidget(w);
    }
}

void lds::showWidget(QWidget *w1, QWidget *w2, bool show)
{
    showWidget(w1, show);
    showWidget(w2, show);
}


int lds::size_g_of_u(const QString &in)
{
    int sum = 0;
    foreach(const QChar &c,in) {
        if(QByteArray().append(c).size() == 3)
            sum += 2;
        else
            sum += 1;
    }
    return sum;
}

QString lds::size_g_of_u_left_justified(const QString &in, int width, QChar fill, bool trunc)
{
    int w = size_g_of_u(in);
    QString trunc_str;
    int gbk_len;
    //fill
    if(width >= w) {

        return in + QString(width - w, fill);
    }
    //~trunc
    if(!trunc) {

        return in;
    }

    //trunc

    for(int k = 0; k < in.count() && (gbk_len = size_g_of_u(trunc_str)) < width; k++) {
        trunc_str += in.at(k);
    }
    if(gbk_len > width) { //a你 ->a
        trunc_str.chop(1);
    }
    return trunc_str;
}

QString lds::_fromlast_cutbetween(const QString &content, const QString &cmpvalue) {
    //s=1000100001001abc10000
    //_fromlast_cutbetween(s,"1")  //return abc
    //_fromlast_cutbetween(s,"c")  //return 1000100001001ab
    int endindex= content.lastIndexOf(cmpvalue);
    int beginindex= content.lastIndexOf(cmpvalue, endindex-1);

    return content.mid(beginindex+cmpvalue.count(), endindex-beginindex-cmpvalue.count());
}

QString lds::strcutBygbnum(const QString &data, int width, bool isfill, char fill, Qt::AlignmentFlag f)
{
    // 你a好 = 5 width = 3 result:你a
    // 你a好 = 5 width = 4 result:你a
    // 你a好 = 5 width = 5 result:你a好
    //1获取字段信息
    QList<int> unitsizelist;
    foreach(const QChar &c,data) {
        if(QByteArray().append(c).size() == 1)    unitsizelist.append(1);
        else   unitsizelist.append(2);
    }

    //开始拼接
    QString ret;
    int retsize = 0;
    for(int index = 0, curstep = 0, indexcount  = unitsizelist.count(); index < indexcount; index++) {
        curstep += unitsizelist[index];
        if(curstep <= width) {
            ret += data[index];
            retsize = curstep;
            continue;
        }
        break;
    }
    //填充判断空格
    if(isfill == true) {
        if(f == Qt::AlignLeft) {
            ret += QString(qMax(0, width-retsize), fill);
        }
        if(f == Qt::AlignRight) {
            ret =  QString(qMax(0, width-retsize), fill) + ret;
        }
    }

    return ret;
}

int lds::min_max_value_add1_loop(int min, int max, int value)
{
    //这里value是每次会随数据递增的
    int ret;

    int div = max-min+1;
    if(div  >  0) {
        ret = value%div+min;
    } else {
        ret = (value);
    }
    //v%(e-b+1)+b
    // b2 e5 v=0 r = 2
    // b2 e5 v=1 r = 3
    // b2 e5 v=2 r = 4
    // b2 e5 v=3 r = 5
    // b2 e5 v=4 r = 2
    return ret;
}

QStringList lds::getUsbPathList()
{
#if defined(QT_DEBUG) && defined(Q_OS_UNIX)
    return QStringList()
            << "/home/cz/文档"
            << "/home/cz/图片";
#endif

    //获取usb路径list unix 和 新系統 和 发布版
#if defined(Q_OS_UNIX)
    QStringList ret_list;
    QDBusInterface dbusinterface("com.scpos.system", "/system/desktop", "com.scpos.desktop", QDBusConnection::systemBus(), 0);
    if(dbusinterface.isValid()) {
        QDBusReply<QStringList> reply = dbusinterface.call("getusbmounts");
        if(reply.isValid()) {
            ret_list = reply.value();
        }
    }
    return ret_list;
#endif

    //window
#if defined(Q_OS_WIN)
    return QStringList();
#endif
    return QStringList();
}

//解密
QString lds::of_str_decrypt(QString as_source)
{
    //Public function of_str_decrypt (string as_source) returns string
    //string as_source
    //    long i;
    int ll_len;
    QString ls_text;
    QString ls_result;
    QString ls_return;

    //Has been Shielded.
    ls_text = as_source.trimmed();
    ll_len = ls_text.length();
    ls_result = "";
    for(int i = 0; i < ll_len; i++) { // 123  -> 321
        //前插入
        ls_result.push_front(QChar((ls_text.at(i).unicode() -  (2 * (ll_len - i - 3)))));//char(asc(mid(ls_text,i,1)) - (2 * (ll_len - i - 2)));
    }
    return ls_result;
}

//加密
QString lds::of_str_encrypt(QString as_source)//空格会被替换吊
{
    //Public function of_str_encrypt (string as_source) returns string
    //string as_source
    //    long i;
    int ll_len;
    QString ls_text;
    QString ls_result;
    //Has been Shielded.
    ls_text = as_source.replace(" ", "").trimmed();
    ll_len = ls_text.length();
    ls_result = "";
    for(int i = 0; i < ll_len ; i++) {//321  -> 123
        ls_result.push_front(QChar((ls_text.at(i).unicode() +  (2 * (i - 2)))));
    }
    return ls_result;
}

/*
1232.56 1 1232.6
1232.52 1 1232.5
1232.569 2 1232.57
1232.561 2 1232.56
1232.561 1 1232.6
1232.561 0 1233
1232.561 -1 1230
1232.561 -2 1200
*/
double lds::doubleRound(double d, int prec)
{
    double p = pow(10, prec);
    double r = qRound(d * p) / p;
    return r;
}

QString lds::cur_machine_dt_str(const QString &displayFormat)
{
    return dt_str(QDateTime::currentDateTime(), displayFormat);
}

QString lds::dt_str(const QDateTime &dt, const QString &displayFormat)
{
    return dt.toString(displayFormat);
}

QString lds::reportModelMapValue(const QVariant &dd, const QString &defaultValue)
{
    if(reportModelMap.isEmpty()) {
        ///流水
        reportModelMap.insert("int_flowid", QObject::tr("序号"));
        reportModelMap.insert("int_id", QObject::tr("序号"));
        reportModelMap.insert("num_dish", QObject::tr("数量"));
        reportModelMap.insert("num_num", QObject::tr("点单数量"));
        reportModelMap.insert("num_back", QObject::tr("退货数量"));
        reportModelMap.insert("num_material", QObject::tr("原料数量"));
        reportModelMap.insert("num_num_change", QObject::tr("替换数量"));

        reportModelMap.insert("ch_dishno", QObject::tr("品码"));
        reportModelMap.insert("vch_dishname", QObject::tr("商品"));
        reportModelMap.insert("ch_materialno", QObject::tr("原料商品"));
        reportModelMap.insert("ch_discount", QObject::tr("折扣"));
        reportModelMap.insert("ch_curprice", QObject::tr("时价商品"));
        reportModelMap.insert("ch_groupno", QObject::tr("组号"));
        reportModelMap.insert("num_price", QObject::tr("单价"));
        reportModelMap.insert("num_sale_price", QObject::tr("商品进价"));
        reportModelMap.insert("num_present", QObject::tr("赠送"));
        reportModelMap.insert("int_unit_rate", QObject::tr("大小包装比率"));
        reportModelMap.insert("item_flag", QObject::tr("库存管理"));
        reportModelMap.insert("ch_stopflag", QObject::tr("停用"));
        reportModelMap.insert("num_m_price", QObject::tr("会员价"));
        reportModelMap.insert("vch_spell", QObject::tr("拼音简码"));
        reportModelMap.insert("ch_serviceflag", QObject::tr("收服务费"));

        reportModelMap.insert("ch_no", QObject::tr("编号"));
        reportModelMap.insert("vch_cp_memo", QObject::tr("特殊做法"));
        reportModelMap.insert("num_add_price", QObject::tr("加价"));//做法设置的加价


        reportModelMap.insert("int_divno", QObject::tr("分单号"));
        reportModelMap.insert("num_discount", QObject::tr("折扣"));
        reportModelMap.insert("vch_print_memo", QObject::tr("做法"));
        reportModelMap.insert("num_price_add", QObject::tr("加价"));//点菜流水的 加价

        reportModelMap.insert("num_total", QObject::tr("小计"));
        reportModelMap.insert("num_total2", QObject::tr("总计"));
        reportModelMap.insert("num_total3", QObject::tr("总计(加价)"));
        reportModelMap.insert("num_profit", QObject::tr("利润"));

        reportModelMap.insert("ch_payno", QObject::tr("付款号"));
        reportModelMap.insert("ch_unitno", QObject::tr("单位"));
        reportModelMap.insert("vch_unitname", QObject::tr("单位"));
        reportModelMap.insert("vch_pur_unitno", QObject::tr("入库单位"));
        reportModelMap.insert("ch_dish_typeno", QObject::tr("编码"));
        reportModelMap.insert("vch_dish_typename", QObject::tr("大类"));
        reportModelMap.insert("ch_sub_typeno", QObject::tr("小类"));
        reportModelMap.insert("vch_sub_typename", QObject::tr("小类"));
        reportModelMap.insert("ch_tableno", QObject::tr("餐桌"));
        reportModelMap.insert("ch_areano", QObject::tr("编码"));
        reportModelMap.insert("vch_areaname", QObject::tr("区域"));
        reportModelMap.insert("num_table", QObject::tr("餐桌数量"));
        reportModelMap.insert("num_kaitai", QObject::tr("开台"));
        reportModelMap.insert("num_kongxian", QObject::tr("空闲"));
        reportModelMap.insert("vch_tablename", QObject::tr("餐桌名称"));
        reportModelMap.insert("ch_typeno", QObject::tr("类型"));
        reportModelMap.insert("vch_typename", QObject::tr("类型名称"));

        reportModelMap.insert("ch_paymodeno", QObject::tr("收银代码"));
        reportModelMap.insert("vch_paymodename", QObject::tr("收银方式"));
        reportModelMap.insert("num_paymode_count", QObject::tr("收银次数"));
        reportModelMap.insert("num_table_count", QObject::tr("台数"));
        reportModelMap.insert("num_realamount", QObject::tr("实收金额"));
        reportModelMap.insert("num_change", QObject::tr("找零"));
        reportModelMap.insert("num_payamount", QObject::tr("付款金额"));
        reportModelMap.insert("vch_voucherno", QObject::tr("付款凭证"));
        reportModelMap.insert("int_deduct", QObject::tr("提成比率"));
        reportModelMap.insert("num_deduct", QObject::tr("提成定额"));

        reportModelMap.insert("num_realamount_income", QObject::tr("收入"));
        reportModelMap.insert("num_realamount_unincome", QObject::tr("非收入"));
        reportModelMap.insert("num_blotout", QObject::tr("抹零"));
        reportModelMap.insert("num_get", QObject::tr("收取"));
        reportModelMap.insert("num_free", QObject::tr("优惠"));
        reportModelMap.insert("num_service", QObject::tr("服务费"));
        reportModelMap.insert("num_lowcost", QObject::tr("低消"));
        reportModelMap.insert("vch_back_operid", QObject::tr("退货人"));
        reportModelMap.insert("dt_back_operdate", QObject::tr("退货时间"));
        reportModelMap.insert("vch_reason", QObject::tr("原因"));
        reportModelMap.insert("int_discount", QObject::tr("折扣"));
        reportModelMap.insert("vch_dis_operid", QObject::tr("折扣人"));
        reportModelMap.insert("dt_dis_operdate", QObject::tr("折扣时间"));
        reportModelMap.insert("ch_presentflag", QObject::tr("是否赠送"));
        reportModelMap.insert("vch_pre_operid", QObject::tr("赠送人"));
        reportModelMap.insert("dt_pre_operdate", QObject::tr("赠送时间"));
        reportModelMap.insert("ch_suitflag", QObject::tr("组合商品"));
        reportModelMap.insert("ch_suitno", QObject::tr("主商品"));
        reportModelMap.insert("ch_specialflag", QObject::tr("是否特价"));
        reportModelMap.insert("ch_billno", QObject::tr("单号"));

        reportModelMap.insert("ch_sheetno", QObject::tr("单号"));
        reportModelMap.insert("ch_sheettype", QObject::tr("类型"));
        reportModelMap.insert("cost_price", QObject::tr("金额"));
        reportModelMap.insert("num_already", QObject::tr("已还金额"));

        reportModelMap.insert("int_person", QObject::tr("人数"));
        reportModelMap.insert("num_cost", QObject::tr("消费"));
        reportModelMap.insert("num_rate", QObject::tr("税收"));
        reportModelMap.insert("int_rate", QObject::tr("税收"));
        reportModelMap.insert("num_cash", QObject::tr("应收"));
        reportModelMap.insert("num_fdeduct", QObject::tr("定额提成"));
        reportModelMap.insert("num_rdeduct", QObject::tr("比例提成"));

        ///会员
        reportModelMap.insert("vch_member", QObject::tr("会员名称"));
        reportModelMap.insert("vch_memberno", QObject::tr("会员编号"));
        reportModelMap.insert("m_typename", QObject::tr("卡类型"));
        reportModelMap.insert("ch_attribute", QObject::tr("卡种类"));
        reportModelMap.insert("vch_handtel", QObject::tr("手机号"));
        reportModelMap.insert("num_deposit", QObject::tr("充值金额"));
        reportModelMap.insert("ch_type", QObject::tr("类型"));
        reportModelMap.insert("ch_pay_mode", QObject::tr("支付方式"));

        reportModelMap.insert("ch_state", QObject::tr("状态"));
        reportModelMap.insert("ch_cardflag", QObject::tr("卡状态"));
        reportModelMap.insert("int_basedata", QObject::tr("保本金额"));
        reportModelMap.insert("m_cardcount", QObject::tr("卡总量"));
        reportModelMap.insert("m_unsendcardcount", QObject::tr("正常未发卡量"));

        reportModelMap.insert("m_sendcardcount", QObject::tr("正常已发卡量"));
        reportModelMap.insert("m_stopcardcount", QObject::tr("止付卡量"));
        reportModelMap.insert("m_hangcardcount", QObject::tr("挂失卡量"));
        reportModelMap.insert("m_badcardcount", QObject::tr("作废卡量"));
        reportModelMap.insert("num_remain", QObject::tr("剩余金额"));
        reportModelMap.insert("num_remain_pay", QObject::tr("未还款金额"));


        reportModelMap.insert("num_pay", QObject::tr("付款金额"));
        reportModelMap.insert("vch_operid", QObject::tr("操作员"));
        reportModelMap.insert("vch_operator", QObject::tr("名称"));
        reportModelMap.insert("vch_waiter", QObject::tr("服务员"));
        reportModelMap.insert("dt_operdate", QObject::tr("时间"));
        reportModelMap.insert("date", QObject::tr("日期"));
        reportModelMap.insert("vch_memo", QObject::tr("备注"));
        reportModelMap.insert("ch_balanceno", QObject::tr("交易单号"));

        reportModelMap.insert("m_deposit", QObject::tr("充值金额"));
        reportModelMap.insert("m_deposit_real", QObject::tr("充值实收"));
        reportModelMap.insert("m_deposit_discount", QObject::tr("充值优惠"));
        reportModelMap.insert("num_point", QObject::tr("积分"));

        ///库存
        reportModelMap.insert("num_store", QObject::tr("库存数量"));
        reportModelMap.insert("num_store_total", QObject::tr("库存金额"));
        reportModelMap.insert("num_store_price", QObject::tr("商品进价"));
        reportModelMap.insert("num_item_warn", QObject::tr("预警下限"));
        reportModelMap.insert("num_sys", QObject::tr("系统库存"));
        reportModelMap.insert("num_check", QObject::tr("盘点数量"));
        reportModelMap.insert("num_pro_loss", QObject::tr("盈亏数量"));
        reportModelMap.insert("vch_audit", QObject::tr("审核人"));
        reportModelMap.insert("dt_audit", QObject::tr("审核日期"));
        reportModelMap.insert("vch_handle", QObject::tr("经手人"));
        reportModelMap.insert("chg_flag", QObject::tr("调整库存"));
        reportModelMap.insert("ch_warehouseno", QObject::tr("仓库"));
        //
        reportModelMap.insert("do1", QObject::tr("操作"));
        reportModelMap.insert("do2", QObject::tr("操作"));
        reportModelMap.insert("do3", QObject::tr("操作"));
        //
        reportModelMap.insert("tx_cost", QObject::tr("商品金额"));
        reportModelMap.insert("tx_tax", QObject::tr("税额"));
        reportModelMap.insert("tx_cash", QObject::tr("含税金额"));
        reportModelMap.insert("vch_tax_number", QObject::tr("税号"));
        reportModelMap.insert("vch_providername", QObject::tr("供应商"));
        reportModelMap.insert("ch_providerno", QObject::tr("供应商编号"));

        reportModelMap.insert("sale_tx_cost", QObject::tr("商品金额(销售)"));
        reportModelMap.insert("sale_tx_tax", QObject::tr("税额(销售)"));
        reportModelMap.insert("invt_tx_cost", QObject::tr("商品金额(采购)"));
        reportModelMap.insert("invt_tx_tax", QObject::tr("税额(采购)"));
        reportModelMap.insert("invt_tx_net", QObject::tr("应纳税额"));
    }

    QString hkey =  dd.toString();
    QString hvalue = reportModelMap.value(hkey.toLower(), defaultValue);
    return hvalue;
}

void lds::reportModelMapClear()
{
    reportModelMap.clear();
}


QByteArray lds::fileOpenReadAll(const QString &filepath, bool *ok, QString *errstring)
{
    if(ok) *ok = true;
    QFile file(filepath);
    if(false == file.open(QFile::ReadOnly)) {
        if(ok) *ok = false;
        if(errstring) *errstring = file.errorString();
        qDebug() << file.errorString();
        return "";
    }
    return file.readAll();
}

bool lds::fileWriteByte(const QString &filepath, const QByteArray &byte, QString *errstring)
{
    QFile file(filepath);
    if(false == file.open(QFile::WriteOnly)) {
        if(errstring) *errstring = file.errorString();
        return false;
    }
    file.write(byte);
    return true;
}

bool lds::fileWriteString(const QString &filepath, const QString &string, QString *errstring)
{
    return fileWriteByte(filepath,string.toLocal8Bit(), errstring);
}

bool lds::fileAppendByte(const QString &filepath, const QByteArray &byte, QString *errstring)
{
    QFile file(filepath);
    if(false == file.open(QFile::WriteOnly | QFile::Append)) {
        if(errstring) *errstring = file.errorString();
        return false;
    }
    file.write(byte);
    return true;
}

QPixmap lds::pixmapIfNull(const QString &filepath, const QString &defaultFileValue)
{
    QPixmap p(filepath);
    if(p.isNull()) {
        return QPixmap(defaultFileValue);
    }
    return p;
}

void lds::paintPolygon(QPainter *painter, const QRect &rect, const QColor &color)
{
    painter->fillRect(rect, color);
    {
        QPainterPath path;
        path.moveTo(rect.width(), 0);
        path.lineTo(rect.width() - 70, 0);
        path.lineTo(rect.width() - 30, rect.height());
        path.closeSubpath();
        painter->fillPath(path, color.light(90));
    }

    {
        QPainterPath path;
        path.moveTo(rect.width(), 0);
        path.lineTo(rect.width() - 30, rect.height());
        path.lineTo(rect.width(), rect.height());
        path.closeSubpath();
        painter->fillPath(path, color.dark(90));
    }
}

QStringList lds::removeStrList(QStringList origin, const QStringList &take)
{
    foreach(const QString &t, take) {
        origin.removeAll(t);
    }
    return origin;
}

QPixmap lds::get_fastfdbacklogo()
{
    QString errstring;
    QPixmap fastfdbacklogo;
    if(!resCopyFile(":/image/backview/backlogo.png", "pos/fastfdbacklogo.png", errstring)) {
        qDebug() << errstring;
    }
    fastfdbacklogo.load("pos/fastfdbacklogo.png");
    if(!fastfdbacklogo.isNull()) {
        fastfdbacklogo = fastfdbacklogo.scaledToHeight(lds::BACK_LOGO_HEIGHT);
    }
    return fastfdbacklogo;
}

QStringList lds::get_fastfdbacktext()
{
    QFile fastfdbacktext("pos/fastfdbacktext.txt");
    if(!fastfdbacktext.exists()) {
        QDir().mkdir(QFileInfo(fastfdbacktext).path());
        fastfdbacktext.open(QFile::WriteOnly);
        fastfdbacktext.write(QT_TRANSLATE_NOOP("QObject", "摇钱数软件"));
        fastfdbacktext.write("\n");
        fastfdbacktext.write("Nanjing Restaurant Software Co,Ltd.");
        fastfdbacktext.write("\n");
        fastfdbacktext.write("www.yqspos.com");
        fastfdbacktext.close();
    }
    if(fastfdbacktext.open(QFile::ReadOnly)) {
        QString rets = fastfdbacktext.readAll();
        QStringList retlist = rets.split("\n");
        if(retlist.count() > 0) {
            qDebug() << QApplication::translate("QObject", retlist[0].toLocal8Bit());
            if(retlist[0] == QT_TRANSLATE_NOOP("QObject", "摇钱数软件")) {
                retlist[0] = QApplication::translate("QObject", retlist[0].toLocal8Bit());
            }
        }
        return retlist;
    }
    return QStringList();
}

bool lds::resCopyFile(const QString &resFile, const QString &toFile, QString &errstring)
{
    QFile file(toFile);
    if(file.exists())
        return true;
    //    ReadOwner = 0x4000, WriteOwner = 0x2000, ExeOwner = 0x1000,
    //    ReadUser  = 0x0400, WriteUser  = 0x0200, ExeUser  = 0x0100,
    //    ReadGroup = 0x0040, WriteGroup = 0x0020, ExeGroup = 0x0010,
    //    ReadOther = 0x0004, WriteOther = 0x0002, ExeOther = 0x0001
    QDir().mkdir(QFileInfo(toFile).path());
    file.setFileName(resFile);
    if(file.copy(toFile)) {
        QFile::setPermissions(toFile, QFile::Permission(0x777));
        return true;
    }

    errstring =  file.errorString();
    return false;
}

QSize lds::pushButtonSizeHint(const QPushButton *b, const QSize &sizeHint)
{
    int menu_width = 0;
    if(b->menu()) {
        menu_width = 20;
    }
    QSize size = sizeHint;
    //height
    if(lds::MAIN_WINDOW_SIZE.width() == 1366
            || lds::MAIN_WINDOW_SIZE.width() == 1920) {
        size.setHeight(qMax(size.height(), 36));
    } else {
        size.setHeight(qMax(size.height(), 30));
    }

    //width
    if(b->text().count() == 0) {
        if(b->iconSize().width() > size.width()) {
            size.setWidth(b->iconSize().width() + 2 + 2);
        } else {
            size.setWidth(size.height());
        }
    } else {
        size.setWidth(qMax(menu_width + 60, size.width()) + 30);
    }

    return size;
}

QByteArray lds::u2local(const QString &str)
{
    return u2local(str, lds::get_soft_curent_language_first());
}

QByteArray lds::u2local(const QString &str, const QString &languageKey)
{
    QString filepath = "userdata/settings/language/"+languageKey + "_code.ini";

    if(QFile::exists(filepath)) {
        QByteArray b;
        QSettings conf(filepath, QSettings::IniFormat);
        if(str == "ﻁ") {
            qDebug() << str;
        }

        for(int k = 0; k < str.count(); k++) {
            QVariant v = conf.value("unicode_int/"+QString::number(str.mid(k, 1).at(0).unicode()));
            if(v.isValid()) {
                b += uchar(v.toInt());
            } else {
                b += str.mid(k, 1);
            }
        }
        return b;
    }

    //default
    return QTextCodec::codecForName("GBK")->fromUnicode(str);
}

void lds::skin_name_map_reset()
{
    skin_name_map.clear();

    skin_name_map.insert("obsidian", QObject::tr("曜石黑"));
#if defined(QT_SCPOS_SUPPORT)
    skin_name_map.insert("green", QObject::tr("清新绿"));
#else
    skin_name_map.insert("skyblue", QObject::tr("天空蓝"));
#endif

#ifdef QT_DEBUG
    skin_name_map.clear();
    skin_name_map.insert("obsidian", QObject::tr("曜石黑"));
    skin_name_map.insert("green", QObject::tr("清新绿"));
    skin_name_map.insert("skyblue", QObject::tr("天空蓝"));
#endif
}

void lds::arg_forbid_debug(int argc, char *argv[])
{
    //登录界面,从参数中获取usr和passwd
    ///./fastfd -u1111 -p1111 -d1 用户名 密码 debug模式
    if(argc>1) {
        //1
        QStringList ps;
        for(int index=1;index < argc;index++) {
            ps.append(argv[index]);
        }
        //2
        QMap<QString, QString> pre_value;
        foreach(QString p, ps) {
            pre_value.insert(p.mid(0, 2), p.mid(2));
        }
        lds::arg_d_value.setOther(pre_value.value("-d"));
    }

#if (defined QT_NO_DEBUG)
    if(arg_d_value.toInt() == 0) {
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        qInstallMsgHandler(arg_message_handler);
#else
        qInstallMessageHandler(arg_message_handler);
#endif
    }
#endif
}
//回调函数实现debug信息到文件
//有做法自动弹出，可以设置
//预订单，落单，付款，结束后返回
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
void lds::arg_message_handler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    //调试信息提示
    case QtDebugMsg:
        break;
        //一般的warning提示
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
        //严重错误提示
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
        //致命错误提示
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }
}
#else
void lds::arg_message_handler(QtMsgType type, const QMessageLogContext &msg, const QString &text)
{
    QString txt;
    switch (type) {
    //调试信息提示
    case QtDebugMsg:
        break;
        //一般的warning提示
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(text);
        break;
        //严重错误提示
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(text);
        break;
        //致命错误提示
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(text);
        break;
    default:
        break;
    }
}
#endif

void lds::cmd_ps_pid(QStringList &pids, const QString &psFilter, const QString &exactFilter)
{
#ifdef LINPOS_DEBUG
    ps_ef_grep(pids, psFilter, exactFilter);
#endif
#ifdef LINPOS_OS6
    ps_ef_grep(pids, psFilter, exactFilter);
#endif
#ifdef LINPOS_OS7
    ps_ef_grep(pids, psFilter, exactFilter);
#endif
#ifdef LINPOS_WIN
    return;
#endif
}


bool lds_record_do_double_clicked::press_event_do_double_clicked(const QString &index)
{
    int mescOff = QTime::currentTime().msecsTo(QTime(0, 0, 0));
    bool is_double_clicked = false;

    if(press_event_do_press_index == index) {
        if(press_event_do_press_mesc_off - mescOff  < 200) {
            is_double_clicked = true;
        }
    }
    press_event_do_press_index = index;
    press_event_do_press_mesc_off = mescOff;
    return is_double_clicked;
}

bool lds_record_do_double_clicked::press_event_do_double_clicked(const QModelIndex &index)
{
    QString pos;
    if(index.isValid()) {
        pos  = QString("%1,%2").arg(index.row()).arg(index.column());
    }

    return press_event_do_double_clicked(pos);
}


void lds::setSystemValue(const QString &key, const QVariant &value)
{
    systemKeyValueMap.insert(key, value);
}


QVariant lds::systemValue(const QString &key, const QVariant &defValue)
{
    return systemKeyValueMap.value(key, defValue);
}


bool lds::dogLoad(QString &dogID, QString *errstring)
{
    ERROR_CODE code = conductlock(dogID);
    if(errstring)
        *errstring = error_name(code);
    return code == ERROR_CODE_NOERR;
}


QString lds::error_name(lds::ERROR_CODE code)
{
    if(ERROR_CODE_WRONGLIB == code) {
        return QObject::tr("加密狗错误，导入库错误");
    }
    if(ERROR_CODE_NODOG == code) {
        return QObject::tr("加密狗错误，没有找到加密狗");
    }
    if(ERROR_CODE_WRONGDOG == code) {
        return QObject::tr("加密狗错误，导入库错误");
    }
    if(ERROR_CODE_NOLIB == code) {
        return QObject::tr("加密狗错误，没有找到加密狗库文件");
    }
    if(ERROR_CODE_NOERR == code) {
        return QObject::tr("操作成功");
    }
    return "UNKONW ERROR";
}


lds::ERROR_CODE lds::conductlock(QString &dogID)
{
    //1
#ifdef Q_OS_WIN
    //windows下加密狗
    long nAppID = 1650534242; //应用程序ID, 4字节,通过设号工具设置  注意保密
    long Keys[8]= {0};
    long nKeyNum = 0;

    //QLibrary mylib("NoxApp64.dll");   //声明所用到的dll文件 我的是64位编译器 所以使用64 位dll
    QLibrary mylib(QCoreApplication::applicationDirPath() + "/NoxApp.dll");   //声明所用到的dll文件

    if(mylib.load())
    {
        NoxFind my_NoxFind=(NoxFind)mylib.resolve("NoxFind");
        NoxOpen my_NoxOpen=(NoxOpen)mylib.resolve("NoxOpen");
        NoxGetLastError my_NoxGetLastError=(NoxGetLastError)mylib.resolve("NoxGetLastError");
        NoxReadStorage  my_NoxReadStorage=(NoxReadStorage)mylib.resolve("NoxReadStorage");
        NoxClose my_NoxClose=(NoxClose)mylib.resolve("NoxClose");

        if(!my_NoxFind || !my_NoxOpen || !my_NoxGetLastError || !my_NoxReadStorage || !my_NoxClose)   //保证每个函数导入成功
        {
            return ERROR_CODE_WRONGLIB;
        }

        if(0 !=my_NoxFind(nAppID,Keys,&nKeyNum))
        {
            //            errsrring=QObject::tr("加密狗错误，没有找到加密狗");
            return ERROR_CODE_NODOG;
        }
        //qDebug() <<"There are" <<nKeyNum<<" NOX keys.\n";

        //打开第一只加密锁
        char * userPin = "8c751b20059acb5a"; //用户密码，通过设号工具设置 注意保密
        if( 0 != my_NoxOpen(Keys[0],userPin))
        {
            //            errsrring=QObject::tr("加密狗错误，导入库错误");
            return ERROR_CODE_WRONGDOG;
        }
        //qDebug() <<"Open Key Successfuflly!\n";

        //存储区读(16字节)
        unsigned char readStorage[16] = {0};
        if( 0 != my_NoxReadStorage(Keys[0],readStorage))
        {
            //            errsrring=QObject::tr("加密狗错误，导入库错误");
            return ERROR_CODE_WRONGDOG;
        }

        QByteArray p = (char*)readStorage;
        dogID = p;

        //关闭加密锁
        my_NoxClose(Keys[0]);
    }
    else
    {
        //        errsrring=QObject::tr("加密狗错误，没有找到加密狗库文件");
        return ERROR_CODE_NOLIB;
    }
#endif
    return ERROR_CODE_NOERR;
    //2
}


lds::resultCode getTerminalCode(QString &terminalCode, QString *errstring)
{
    ///debug测试版本
#ifdef LINPOS_DEBUG
    terminalCode =  "AA80606110524183C";
    terminalCode =  "AA80310092516736C";
    terminalCode =  "AA80524163822983D";
    terminalCode =  "AA80703090245838C";
    terminalCode =  "AA80625142201290C";
    terminalCode =  "AA80713110718860C";
    //    terminalCode =  "AA80625141553624C";
    terminalCode =  "AA80703090245838C";
    terminalCode =  "AA80417154652901D";

    //    terminalCode =  "AA80927145850999C";
    //    terminalCode =  "AA81230100007969C";
    //    terminalCode =  "AA80713110718860C";
    //    terminalCode = "FA1001";
    //    terminalCode =  "A80504213136693C";
    //    terminalCode =  "AA80402134645527D";
    //    terminalCode =  "AA80801131630036C";
    terminalCode =  "AA81221090445135C";
    terminalCode =  "AA81214063225521C";
    terminalCode =  "AA80117152800729D";
    terminalCode =  "AA80614174714685D";
//    terminalCode =  "AA80922154052045D";
//        terminalCode =  "AA80320090042424D";

    return lds::resultOk;
#endif

#ifdef LINPOS_OS6
    QDBusInterface dbusinterface("com.scpos.system", "/system/desktop", "com.scpos.desktop", QDBusConnection::systemBus(), 0);
    if(dbusinterface.isValid()) {
        QDBusReply<QString> reply = dbusinterface.call("getsysid");
        if(reply.isValid()) {
            terminalCode = reply.value();
        }
    }
#endif

#ifdef LINPOS_OS7
    //获取
    char buf_1[255] = {0};
    FILE *fd = popen("cat /proc/cpuinfo | grep Serial","r");
    if(fd != NULL) {
        fread(buf_1, sizeof(buf_1) - 1, 1,  fd);
        buf_1[sizeof(buf_1)] = '\0';
        pclose(fd);

        terminalCode = QString(QByteArray(buf_1, sizeof(buf_1))).split(":").value(1).trimmed();
    }
#endif

#ifdef LINPOS_WIN
    //硬盘号
    QString lpRootPathName = "C:\\";
    LPTSTR lpVolumeNameBuffer=new TCHAR[12];//磁盘卷标
    DWORD nVolumeNameSize=12;// 卷标的字符串长度
    DWORD VolumeSerialNumber;//硬盘序列号
    DWORD MaximumComponentLength;// 最大的文件长度
    LPTSTR lpFileSystemNameBuffer=new TCHAR[10];// 存储所在盘符的分区类型的长指针变量
    DWORD nFileSystemNameSize=10;// 分区类型的长指针变量所指向的字符串长度
    DWORD FileSystemFlags;// 文件系统的一此标志

    GetVolumeInformation((LPTSTR)lpRootPathName.utf16(),
                         lpVolumeNameBuffer, nVolumeNameSize,
                         &VolumeSerialNumber, &MaximumComponentLength,
                         &FileSystemFlags,
                         lpFileSystemNameBuffer, nFileSystemNameSize);

    //    qDebug() << VolumeSerialNumber;
    terminalCode = "WIN" + QString::number(VolumeSerialNumber);
#endif

    if(terminalCode.length() == 0) {
        if(errstring)
            *errstring = "terminalCode is empty";
        return lds::resultError;
    }

    if(terminalCode.startsWith("FA")) {
    } else if(terminalCode.startsWith("AA")) {
    } else {
        terminalCode =  "AA"   +terminalCode;
    }
    return lds::resultOk;
}


bool lds::isNumber(const QVariant &d)
{
    return d.type() == QVariant::Double
            || d.type() == QVariant::Int
            || d.type() == QVariant::UInt
            || d.type() == QVariant::LongLong
            || d.type() == QVariant::ULongLong
            ;
}


void lds::paintDense7Pattern(QPainter *painter)
{
    QRect rect(0, 0, painter->device()->width(), painter->device()->height());

    painter->setPen(Qt::NoPen);
    if(lds::sysconf->value("system_setting/skin_name", "obsidian").toString() == "obsidian") {
        painter->fillRect(rect, QColor("#383838"));
        painter->setBrush(QBrush(QColor("#484848"), Qt::Dense7Pattern));
    } else {
        painter->fillRect(rect, QColor("white"));
        painter->setBrush(QBrush(QColor("lightgray"), Qt::Dense7Pattern));
    }
    painter->drawRect(rect);
}

QPixmap lds::baseNamePixmap(const QString &filePath, const QString &baseName)
{
    QStringList list;
    //    .jpg .jpeg .gif .png .bmp
    list << ".jpg" << ".jpeg" << ".gif"<< ".png" << ".bmp";
    foreach(const QString& suffix, list) {
        if(false == QPixmap(filePath + "/" + baseName + suffix).isNull()) {
            return QPixmap(filePath + "/" + baseName + suffix);
        }
    }

    return QPixmap();
}

int lds::func_dec_price_def()
{
    return 2;
}

int lds::func_dec_num_def()
{
    return 2;
}

int lds::func_dec_amt_def()
{
    return 2;
}

bool lds::qAppContaniWidget(const QString &obj_name)
{
    foreach(QWidget *w, qApp->allWidgets()) {
        if(w->objectName() == obj_name) {
            return true;
        }
    }
    return false;
}

bool lds::isqAppActiveWindow(const QString &obj_name)
{
    QObject *o = qApp->activeWindow();
    if(o == 0)
        return false;
    return o->findChild<QWidget*>(obj_name) != 0
            || o->objectName() == obj_name;
}

QRect lds::indexRect(const QRect &option_rect, const QModelIndex &index, int border_width)
{
    return indexRect(option_rect, index, border_width, border_width);
}

QRectF lds::indexRect(const QRectF &option_rect, const QModelIndex &index, qreal border_width)
{
    return indexRect(option_rect, index, border_width, border_width);
}

QRectF lds::indexRect(const QRectF &option_rect, const QModelIndex &index, qreal border_inner, qreal border_outer)
{
    return ldsIndexRect<QRectF, qreal>(option_rect, index, border_inner, border_outer);
}

QRect lds::indexRect(const QRect &option_rect, const QModelIndex &index, int border_inner, int border_outer)
{
    return ldsIndexRect<QRect, int>(option_rect, index, border_inner, border_outer);
}
