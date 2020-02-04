#include "colordialog.h"
#include "ui_colordialog.h"
#include <QStandardItem>
#include <QtDebug>
#include <QMouseEvent>
#include <QPainter>

static bool initrgb = false;
static QRgb stdrgb[6*8];
static QRgb cusrgb[2*8];
static bool customSet = false;


static void initRGB()
{
    if (initrgb)
        return;
    initrgb = true;
    int i = 0;
    for (int g = 0; g < 4; g++)
        for (int r = 0;  r < 4; r++)
            for (int b = 0; b < 3; b++)
                stdrgb[i++] = qRgb(r * 255 / 3, g * 255 / 3, b * 255 / 2);

    for (i = 0; i < 2*8; i++)
        cusrgb[i] = 0xffffffff;
}

static int pWidth = 220;
static int pHeight = 200;

QPoint QColorPicker::colPt()
{
    QRect r = contentsRect();
    return QPoint((360 - hue) * (r.width() - 1) / 360, (255 - sat) * (r.height() - 1) / 255);
}

int QColorPicker::huePt(const QPoint &pt)
{
    QRect r = contentsRect();
    return 360 - pt.x() * 360 / (r.width() - 1);
}

int QColorPicker::satPt(const QPoint &pt)
{
    QRect r = contentsRect();
    return 255 - pt.y() * 255 / (r.height() - 1);
}

void QColorPicker::setCol(const QPoint &pt)
{
    setCol(huePt(pt), satPt(pt));
}

QColorPicker::QColorPicker(QWidget* parent)
    : QFrame(parent)
{
    hue = 0; sat = 0;
    setCol(150, 255);

    setAttribute(Qt::WA_NoSystemBackground);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed) );
}

QColorPicker::~QColorPicker()
{
}


QSize QColorPicker::sizeHint() const
{
    return QSize(pWidth + 2*frameWidth(), pHeight + 2*frameWidth());
}

void QColorPicker::setCol(int h, int s)
{
    int nhue = qMin(qMax(0,h), 359);
    int nsat = qMin(qMax(0,s), 255);
    if (nhue == hue && nsat == sat)
        return;

    hue = nhue; sat = nsat;
    /*QRect r(colPt(), QSize(20,20));
    r = r.united(QRect(colPt(), QSize(20,20)));
    r.translate(contentsRect().x()-9, contentsRect().y()-9);*/
    //    update(r);
    //    repaint(r);


//    p.fillRect(0, pt.y(), r.width(),  1, Qt::white);
//    p.fillRect(pt.x(), 0, 1, r.height(), Qt::white);

//    repaint(QRect(0, pt.y(), r.width(),  1));
//    repaint(QRect(pt.x(), 0, 1, r.height()));
    update();
}

void QColorPicker::mouseMoveEvent(QMouseEvent *m)
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol(p);
    emit newCol(hue, sat);
}

void QColorPicker::mousePressEvent(QMouseEvent *m)
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol(p);
    setFocus();
    emit newCol(hue, sat);
}

void QColorPicker::paintEvent(QPaintEvent* )
{
    QPainter p(this);
    drawFrame(&p);
    QRect r = contentsRect();

    p.drawPixmap(r.topLeft(), pix);
    QPoint pt = colPt() + r.topLeft();
    p.setPen(Qt::black);

    //    p.setPen(Qt::white);
    p.fillRect(0, pt.y(), r.width(),  1, QColor("#ff546b"));
    p.fillRect(pt.x(), 0, 1, r.height(), QColor("#ff546b"));
    p.fillRect(pt.x()-2, pt.y()-2, 5, 5, QColor("#ff546b"));
    /*
    p.drawLine(QPointF(0,pt.y()), QPoint(r.width(), pt.y()));
    p.drawLine(QPointF(pt.x(),0), QPoint(pt.x(), r.height()));
    p.fillRect(pt.x()-9, pt.y(), 20, 2, Qt::black);
    p.fillRect(pt.x(), pt.y()-9, 2, 20, Qt::black);*/

}

void QColorPicker::resizeEvent(QResizeEvent *ev)
{
    QFrame::resizeEvent(ev);

    int w = width() - frameWidth() * 2;
    int h = height() - frameWidth() * 2;
    QImage img(w, h, QImage::Format_RGB32);
    img.fill(Qt::black);
    int x, y;
    uint *pixel = (uint *) img.scanLine(0);
    for (y = 0; y < h; y++) {
        const uint *end = pixel + w;
        x = 0;
        while (pixel < end) {
            QPoint p(x, y);
            QColor c;
            c.setHsv(huePt(p), satPt(p), 200);
            *pixel = c.rgb();
            ++pixel;
            ++x;
        }
    }
    pix = QPixmap::fromImage(img);
}
///////////////


int QColorLuminancePicker::y2val(int y)
{
    int d = height() - 2*coff - 1;
    return 255 - (y - coff)*255/d;
}

int QColorLuminancePicker::val2y(int v)
{
    int d = height() - 2*coff - 1;
    return coff + (255-v)*d/255;
}

QColorLuminancePicker::QColorLuminancePicker(QWidget* parent)
    :QWidget(parent)
{
    hue = 100; val = 100; sat = 100;
    pix = 0;
    //    setAttribute(WA_NoErase, true);
}

QColorLuminancePicker::~QColorLuminancePicker()
{
    delete pix;
}

int QColorLuminancePicker::value()
{
    return val;
}

void QColorLuminancePicker::mouseMoveEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
}
void QColorLuminancePicker::mousePressEvent(QMouseEvent *m)
{
    setVal(y2val(m->y()));
    setFocus();
}

void QColorLuminancePicker::setVal(int v)
{
    if (val == v)
        return;
    val = qMax(0, qMin(v,255));
    delete pix; pix=0;
    repaint();
    emit newHsv(hue, sat, val);
}

//receives from a hue,sat chooser and relays.
void QColorLuminancePicker::setCol(int h, int s)
{
    setCol(h, s, val);
    emit newHsv(h, s, val);
}

void QColorLuminancePicker::paintEvent(QPaintEvent *)
{
    int w = width() - 5;

    QRect r(0, foff, w, height() - 2*foff);
    int wi = r.width() - 2;
    int hi = r.height() - 2;
    if (!pix || pix->height() != hi || pix->width() != wi) {
        delete pix;
        QImage img(wi, hi, QImage::Format_RGB32);
        int y;
        uint *pixel = (uint *) img.scanLine(0);
        for (y = 0; y < hi; y++) {
            const uint *end = pixel + wi;
            while (pixel < end) {
                QColor c;
                c.setHsv(hue, sat, y2val(y+coff));
                *pixel = c.rgb();
                ++pixel;
            }
        }
        pix = new QPixmap(QPixmap::fromImage(img));
    }
    QPainter p(this);
    p.drawPixmap(1, coff, *pix);
//    const QPalette &g = palette();
//    qDrawShadePanel(&p, r, g, true);
//    p.setPen(g.foreground().color());
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#ff546b"));//g.foreground());
    QPolygon a;
    int y = val2y(val);
    a.setPoints(3, w, y, w+5, y+5, w+5, y-5);
//    p.eraseRect(w, 0, 5, height());
    p.drawPolygon(a);
    p.fillRect(0, y, wi, 1, QColor("#ff546b"));
}

void QColorLuminancePicker::setCol(int h, int s,  int v)
{
    val = v;
    hue = h;
    sat = s;
    delete pix; pix=0;
    repaint();
}

colordialog::colordialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::colordialog)
{
    ui->setupUi(this);


    QStandardItemModel *model = new QStandardItemModel;
    ui->tableView->setModel(model);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);//setDefaultSectionSize(50);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//setDefaultSectionSize(50);
#else
    ui->tableView->verticalHeader()->setResizeMode(QHeaderView::Stretch);//setDefaultSectionSize(50);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);//setDefaultSectionSize(50);
#endif
    ui->tableView->setSelectionMode(QTableView::SingleSelection);
    ui->tableView->setSelectionBehavior(QTableView::SelectItems);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setStyleSheet("QTableView{border-top:4px solid transparent;background:transparent;}");
    ui->tableView->setEditTriggers(QTableView::NoEditTriggers);
    ui->tableView->setItemDelegate(new itemDelegate(this));
    ui->tableView->horizontalHeader()->setMinimumSectionSize(0);
    ui->tableView->verticalHeader()->setMinimumSectionSize(0);

    initRGB();
    for(int column = 0; column < 8; column ++) {
        QList<QStandardItem*> items;
        for(int row = 0; row < 6; row ++) {
            items << new QStandardItem;
            items.last()->setData(QColor(stdrgb[column*6 + row]), Qt::BackgroundRole);
        }
        model->appendColumn(items);
    }



    com_add_item(ui->comboBox_html, "aqua");
    com_add_item(ui->comboBox_html, "fuchsia");
    com_add_item(ui->comboBox_html, "hotpink");
    com_add_item(ui->comboBox_html, "lawngreen");
    com_add_item(ui->comboBox_html, "palegreen");
    com_add_item(ui->comboBox_html, "steelblue");
    com_add_item(ui->comboBox_html, "powderblue");
    com_add_item(ui->comboBox_html, "tan");
    com_add_item(ui->comboBox_html, "orchid");
    com_add_item(ui->comboBox_html, "teal");
    com_add_item(ui->comboBox_html, "thistle");
    com_add_item(ui->comboBox_html, "tomato");
    com_add_item(ui->comboBox_html, "turquoise");
    com_add_item(ui->comboBox_html, "violet");


    picker = new QColorPicker(this);
    mpicker = new QColorLuminancePicker(this);
    connect(picker, SIGNAL(newCol(int,int)), mpicker, SLOT(setCol(int,int)));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setMargin(0);
    hlayout->addWidget(picker);
    hlayout->addWidget(mpicker);
    ui->frame_picker->setLayout(hlayout);

    ui->frame_picker->setFixedSize(400, 200);
    picker->setFixedSize(360, 192/*coff = 4*/);
    mpicker->setFixedSize(40 - 6, 200);
    ui->frame->setFixedWidth(400);
    ui->frame->layout()->setMargin(0);
    ui->label_show->setFixedWidth(200);
    ui->label_show->clear();
    ///////////
    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(tocancel()));
    connect(ui->tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(toupdate(QModelIndex,QModelIndex)));

    connect(ui->comboBox_html, SIGNAL(currentIndexChanged(QString)), this,SLOT(toupdateHtml(QString)));
    connect(ui->spinBox_blue, SIGNAL(valueChanged(int)), this,SLOT(toupdateBlue(int)));
    connect(ui->spinBox_green, SIGNAL(valueChanged(int)), this,SLOT(toupdateGreen(int)));
    connect(ui->spinBox_red, SIGNAL(valueChanged(int)), this,SLOT(toupdateRed(int)));
    connect(mpicker, SIGNAL(newHsv(int,int,int)), this, SLOT(toupdate(int,int,int)));
    connect(picker, SIGNAL(newCol(int,int)), this, SLOT(toupdate(int,int)));
}

colordialog::~colordialog()
{
    delete ui;
}

QString colordialog::result_color_name()
{
    return color_name;
}

void colordialog::took()
{
    this->accept();
}

void colordialog::tocancel()
{
    this->reject();
}

void colordialog::toupdate(const QModelIndex &current, const QModelIndex &previous)
{
    if(this->focusWidget() != ui->tableView) return;

    QColor color = qvariant_cast<QColor>(ui->tableView->model()->data(current, Qt::BackgroundRole));

    toupdateColor(color);
}

void colordialog::toupdateHtml(const QString &html_color)
{
    if(this->focusWidget() != ui->comboBox_html) return;

    QColor color = QColor(html_color);

    toupdateColor(color);
}

void colordialog::toupdateRed(int html_color)
{
    if(this->focusWidget() != ui->spinBox_red) return;

    QColor color = QColor(ui->spinBox_red->value(), ui->spinBox_green->value(), ui->spinBox_blue->value());

    toupdateColor(color);
}

void colordialog::toupdateBlue(int html_color)
{
    if(this->focusWidget() != ui->spinBox_blue) return;

    QColor color = QColor(ui->spinBox_red->value(), ui->spinBox_green->value(), ui->spinBox_blue->value());

    toupdateColor(color);
}

void colordialog::toupdateGreen(int html_color)
{
    if(this->focusWidget() != ui->spinBox_green) return;

    QColor color = QColor(ui->spinBox_red->value(), ui->spinBox_green->value(), ui->spinBox_blue->value());

    toupdateColor(color);
}

void colordialog::toupdate(int h, int s, int v)
{
    if(this->focusWidget() != mpicker) return;

    QColor color = QColor::fromHsv(h, s, v);

    toupdateColor(color);

}

void colordialog::toupdate(int h, int s)
{
    if(this->focusWidget() != picker) return;

    QColor color = QColor::fromHsv(h, s, mpicker->value());

    toupdateColor(color);
}

void colordialog::toupdateColor(const QColor &color)
{
    QObject *focus = this->focusWidget();
    if(ui->comboBox_html != focus) {ui->comboBox_html->setCurrentIndex(-1);ui->comboBox_html->setEditText(color.name());}
    if(ui->spinBox_blue != focus)ui->spinBox_blue->setValue(color.blue());
    if(ui->spinBox_green != focus)ui->spinBox_green->setValue(color.green());
    if(ui->spinBox_red != focus)ui->spinBox_red->setValue(color.red());
    if(picker != focus) {picker->setCol(color.hue(), color.saturation());}
    if(mpicker != focus) {mpicker->setCol(color.hue(), color.saturation(), color.value());}

    ui->label_show->setStyleSheet("background-color:"+color.name());
    color_name = color.name();

}

QPixmap colordialog::pix_color(const QString &colorname)
{
    QPixmap p(50, 50);
    p.fill(QColor(colorname));
    return p;
}

void colordialog::com_add_item(QComboBox *com, const QString &colorname)
{
    com->addItem(pix_color(colorname), colorname);
}

void colordialog::itemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QColor color = qvariant_cast<QColor>(index.data(Qt::BackgroundRole));
    painter->fillRect(option.rect, color);
}
