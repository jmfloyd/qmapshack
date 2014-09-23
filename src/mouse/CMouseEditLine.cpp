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

#include "mouse/CMouseEditLine.h"
#include "mouse/CScrOptPoint.h"
#include "gis/trk/CGisItemTrk.h"
#include "canvas/CCanvas.h"
#include "gis/CGisDraw.h"

#include <QtWidgets>

CMouseEditLine::CMouseEditLine(CGisItemTrk &trk, CGisDraw *gis, CCanvas *parent)
    : IMouse(gis, parent)
    , state(eStateIdle)
{
    cursor  = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
    key     = trk.getKey();

    const CGisItemTrk::trk_t& data = trk.getTrackData();
    foreach (const CGisItemTrk::trkseg_t& seg, data.segs)
    {
        foreach(const CGisItemTrk::trkpt_t& pt, seg.pts)
        {
            if(pt.flags & CGisItemTrk::trkpt_t::eDeleted)
            {
                continue;
            }
            coords << QPointF(pt.lon * DEG_TO_RAD, pt.lat * DEG_TO_RAD);
        }
    }

    line = coords;
    gis->convertRad2Px(line);
}

CMouseEditLine::~CMouseEditLine()
{
    delete scrOptPoint;
}

void CMouseEditLine::draw(QPainter& p, bool needsRedraw, const QRect &rect)
{
    if(needsRedraw)
    {
        line = coords;
        gis->convertRad2Px(line);
    }

    p.setPen(QPen(Qt::magenta, 5));
    p.drawPolyline(line);

    QPixmap bullet("://icons/8x8/bullet_magenta.png");
    foreach(const QPointF& pt, line)
    {
        p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
    }

    if(idxOfFocus >= 0)
    {
        const QPointF& pt = line[idxOfFocus];

        p.setPen(Qt::darkBlue);
        p.setBrush(QColor(255,255,255,200));
        p.drawEllipse(pt, 6, 6);
        p.drawPixmap(pt.x() - 3, pt.y() - 3, bullet);
    }

    if(!scrOptPoint.isNull())
    {
        scrOptPoint->draw(p);
    }
}

void CMouseEditLine::mousePressEvent(QMouseEvent * e)
{
    point  = e->pos();
    if(e->button() == Qt::RightButton)
    {
        canvas->resetMouse();
        canvas->update();
    }
    else if(e->button() == Qt::LeftButton)
    {
        switch(state)
        {
            case eStateIdle:
            {
                if(idxOfFocus >= 0)
                {
                    scrOptPoint = new CScrOptPoint(line[idxOfFocus], canvas);
                    scrOptPoint->show();
                    canvas->update();

                    state = eStatePointSelected;
                }
                break;
            }
            case eStatePointSelected:
            {
                delete scrOptPoint;                
                canvas->update();
                idxOfFocus = -1;
                state = eStateIdle;
            }
            default:;
        }
    }

}

void CMouseEditLine::mouseMoveEvent(QMouseEvent * e)
{
    point  = e->pos();    

    switch(state)
    {
        case eStateIdle:
        {
            panCanvas(point);

            int old = idxOfFocus;
            idxOfFocus = getPointCloseBy(point);
            if(old != idxOfFocus)
            {
                canvas->update();
            }
            break;
        }
        default:;
    }
}

void CMouseEditLine::mouseReleaseEvent(QMouseEvent *e)
{

}

void CMouseEditLine::wheelEvent(QWheelEvent * e)
{
    canvas->update();
}


int CMouseEditLine::getPointCloseBy(const QPoint& screenPos)
{
    qint32 i    = 0;
    qint32 idx  = -1;
    qint32  d   = NOINT;
    foreach(const QPointF& point, line)
    {
        int tmp = (screenPos - point).manhattanLength();
        if(tmp < d)
        {
            idx = i;
            d   = tmp;
        }
        i++;
    }

    if(d > 40) idx = -1;

    return idx;
}