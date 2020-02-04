#ifndef LDS_SVG_RENDER_H
#define LDS_SVG_RENDER_H

#include <QRectF>
#include <QSvgRenderer>
#include <QPainter>

class lds_svg_render
{
public:
    lds_svg_render();
    static bool svg_render_by_rect(QSvgRenderer *render, QPainter *p, const QRectF &r);
    static bool svg_render_by_rect(QSvgRenderer *render, QPainter *p, const QRectF &r, const QRectF &rectp);

};

#endif // LDS_SVG_RENDER_H
