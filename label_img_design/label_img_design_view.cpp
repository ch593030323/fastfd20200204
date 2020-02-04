#include "label_img_design_view.h"
#include "label_img_code128.h"
#include <QPrinter>

label_img_design_view::label_img_design_view(QWidget *parent) : QWidget(parent)
{
    label_queue_index = 0;
    printer = 0;
    off = 10;
}

label_img_design_view::~label_img_design_view()
{
    if(printer != 0)
        delete printer;
}

void label_img_design_view::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), Qt::white);
    for(const label_img_header::LabelLine &line : table.content) {
        QPainter painter(this);
        QFont f = painter.font();
        f.setWeight(QFont::Black);
        f.setPixelSize(line.font_pixel);
        painter.setFont(f);
        if(line.type == "text") {
            painter.drawText(QRectF(line.x, line.y, this->width() - line.x, this->height() - line.y), Qt::TextWrapAnywhere, line.desc + line.list.value(label_queue_index));
        }
        if(line.type == "bar"){
            painter.translate(line.x, line.y);
            label_img_code128::paintBar(&painter, line.list.value(label_queue_index));
        }
    }
}

int label_img_design_view::count()
{
    return table.content.value(0).list.count();
}

void label_img_design_view::setIndex(int k)
{
    label_queue_index = k;
}

void label_img_design_view::print()
{
    if(!label_img_header::printerNames().contains(table.printerName))
        return;
    if(table.content.count() == 0)
        return;
    QPixmap pix = QPixmap::grabWidget(this);
    {
        QPixmap copy(pix.height(), pix.width());
        QPainter paint(&copy);
        paint.begin(&copy);
        paint.translate(0, copy.height());
        paint.rotate(-90);
        paint.drawPixmap(0, 0, pix);
        paint.end();

        pix = copy;
    }
    QPainter painter;
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.begin(printer);
    painter.drawPixmap(0, 0, pix);
    painter.end();
}

void label_img_design_view::printFirst()
{
    setIndex(0);
    print();
}

void label_img_design_view::printAll()
{
    for(int k = 0; k < count(); k ++) {
        setIndex(k);
        print();
    }
    setIndex(0);
}

QPixmap label_img_design_view::pixmapFirst()
{
    setIndex(0);
    return QPixmap::grabWidget(this);
}

void label_img_design_view::setTable(const label_img_header::LabelTable &table)
{
    if(printer == 0
            || printer->printerName() != table.printerName
            ) {
        if(printer != 0) {
            qDebug() << "delete printer";
            delete printer;
        }
        printer = new QPrinter(QPrinter::HighResolution);
        printer->setOrientation(QPrinter::Landscape);
        printer->setPrinterName(table.printerName);
    }
    printer->setPaperSize(QSize(table.MMwidth, table.MMheight), QPrinter::Millimeter);
    this->resize(printer->paperRect().height(), printer->paperRect().width());
    this->table = table;
    this->update();
}

const label_img_header::LabelTable &label_img_design_view::curTable()
{
    return table;
}

void label_img_design_view::left(int id)
{
    if(id >= 0 && id < table.content.count()) {
        table.content[id].x += -1 * off;
        this->update();
    }
}

void label_img_design_view::right(int id)
{
    if(id >= 0 && id < table.content.count()) {
        table.content[id].x += 1 * off;
        this->update();
    }
}

void label_img_design_view::top(int id)
{
    if(id >= 0 && id < table.content.count()) {
        table.content[id].y += -1 * off;
        this->update();
    }
}

void label_img_design_view::bottom(int id)
{
    if(id >= 0 && id < table.content.count()) {
        table.content[id].y += 1 * off;
        this->update();
    }
}

void label_img_design_view::scale(int id, int op)
{
    if(id >= 0 && id < table.content.count()) {
        table.content[id].font_pixel += op * 1;
        this->update();
    }
}
