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

#include "mouse/CMouseEditTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/CGisWidget.h"
#include "canvas/CCanvas.h"

#include <QtWidgets>

CMouseEditTrk::CMouseEditTrk(const QPointF& point, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(point, gis, parent)
{
    cursor = cursor1 = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
}

CMouseEditTrk::CMouseEditTrk(CGisItemTrk &trk, CGisDraw * gis, CCanvas * parent)
    : IMouseEditLine(trk, gis, parent)
{
    cursor = cursor1 = QCursor(QPixmap(":/cursors/cursorMoveLine.png"),0,0);
    key    = trk.getKey();

    // reset any focus the track might have.
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseMove);
    trk.setMouseFocusByPoint(NOPOINT, CGisItemTrk::eFocusMouseClick);
    if(trk.hasUserFocus())
    {
        trk.gainUserFocus(false);
    }

    /*
        trigger complete update of GIS components to make sure all changes to
        the originating object are reflected on the canvas->
    */
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}

CMouseEditTrk::~CMouseEditTrk()
{

}

IGisLine * CMouseEditTrk::getGisLine()
{
    return dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
}

void CMouseEditTrk::slotCopyToNew()
{
    if(coords1.size() < 2)
    {
        return;
    }

    CGisProject * project = CGisWidget::self().selectProject();
    if(project == 0)
    {
        return;
    }

    /// @todo make this independent from track
    QString name;
    CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(CGisWidget::self().getItemByKey(key));
    if(trk != 0)
    {
        name = trk->getName();
    }

    name = QInputDialog::getText(0, QObject::tr("Edit name..."), QObject::tr("Enter new track name."), QLineEdit::Normal, name);

    if(name.isEmpty())
    {
        return;
    }

    new CGisItemTrk(coords1,name, project, -1);

    canvas->resetMouse();
    canvas->slotTriggerCompleteUpdate(CCanvas::eRedrawGis);
}