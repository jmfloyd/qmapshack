/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "mouse/CScrOptPoint.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>

CScrOptPoint::CScrOptPoint(const QPointF &point, QWidget *parent)
    : IScrOpt(parent)
{
    setupUi(this);
    setOrigin(point.toPoint());

    move(point.toPoint() + QPoint(30,30));
    adjustSize();

}

CScrOptPoint::~CScrOptPoint()
{

}

void CScrOptPoint::draw(QPainter& p)
{
    QRectF r = rect();
    r.moveTopLeft(QPoint(x(), y()));
    QPainterPath path1;
    path1.addRoundedRect(r,5,5);

    qDebug() << origin << r;

    QPolygonF poly2;
    poly2 << origin << (r.topLeft() + QPointF(10,0)) << (r.topLeft() + QPointF(0,10)) << origin;
    QPainterPath path2;
    path2.addPolygon(poly2);

    path1 = path1.united(path2);

    p.setPen(CCanvas::penBorderGray);
    p.setBrush(CCanvas::brushBackWhite);
    p.drawPolygon(path1.toFillPolygon());
}