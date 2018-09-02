/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler oliver.eichler@gmx.de

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

#include "canvas/CCanvas.h"
#include "helpers/CSettings.h"
#include "mouse/CScrOptRuler.h"
#include "mouse/IMouse.h"

#include <functional>
#include <QtWidgets>

CScrOptRuler::CScrOptRuler(IMouse *mouse, CCanvas *canvas)
    : IScrOpt(mouse)
{
    setupUi(this);
    connect(pushClose, &QPushButton::clicked, this, &CScrOptRuler::slotClose);

    auto func = std::bind(&CCanvas::slotTriggerCompleteUpdate, canvas, CCanvas::eRedrawMouse);
    connect(toolShowTable, &QToolButton::toggled, this, func);
    connect(toolShowAscent, &QToolButton::toggled, this, func);
    connect(toolShowCourse, &QToolButton::toggled, this, func);
    connect(toolShowAngle, &QToolButton::toggled, this, func);
    connect(toolShowPrecision, &QToolButton::toggled, this, func);

    SETTINGS;
    cfg.beginGroup("Ruler");
    toolShowTable->setChecked(cfg.value("showTable", true).toBool());
    toolShowCourse->setChecked(cfg.value("showCourse", true).toBool());
    toolShowAscent->setChecked(cfg.value("showAscent", true).toBool());
    toolShowAngle->setChecked(cfg.value("showAngle", false).toBool());
    toolShowPrecision->setChecked(cfg.value("showPrecision", false).toBool());
    cfg.endGroup(); // Ruler

    move(0,0);
    adjustSize();
    show();
}

CScrOptRuler::~CScrOptRuler()
{
    SETTINGS;
    cfg.remove("Ruler");
    cfg.beginGroup("Ruler");
    cfg.setValue("showTable", toolShowTable->isChecked());
    cfg.setValue("showCourse", toolShowCourse->isChecked());
    cfg.setValue("showAscent", toolShowAscent->isChecked());
    cfg.setValue("showAngle", toolShowAngle->isChecked());
    cfg.setValue("showPrecision", toolShowPrecision->isChecked());
    cfg.endGroup(); // Ruler
}

void CScrOptRuler::slotClose()
{
    mouse->IMouse::rightButtonDown(QPoint(0,0));
}