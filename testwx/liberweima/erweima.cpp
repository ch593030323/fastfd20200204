#include "erweima.h"

erweima::erweima(QWidget *parent) :
    QWidget(parent)
{
    this->setMinimumWidth(100);
    this->setMinimumHeight(100);
    str1 ="www.scpos.com";
}

void erweima::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRcode *qr = QRcode_encodeString(str1,3, QR_ECLEVEL_L, QR_MODE_8, 1);
    if(0!=qr) {
        QColor fg("black");
        QColor bg("white");
        painter.setBrush(bg);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0,0,100,100);
        painter.setBrush(fg);
        const int s=qr->width>0?qr->width:1;
        const double w=100;
        const double h=100;
        const double aspect=w/h;
        const double scale=((aspect>1.0)?h:w)/s;
        for(int y=0;y<s;y++) {
            const int yy=y*s;
            for(int x=0;x<s;x++) {
                const int xx=yy+x;
                const unsigned char b=qr->data[xx];
                if(b &0x01) {
                    const double rx1=x*scale, ry1=y*scale;
                    QRectF r(rx1, ry1, scale, scale);
                    painter.drawRects(&r,1);
                }
            }
        }
        QRcode_free(qr);
    }
    else{
        QColor error("red");
        painter.setBrush(error);
        // painter.drawRect(0,0,50,20);
        //
    }
}

void erweima::setstr(const char *pcStr)
{
    str1=pcStr;
    update();
}
