/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#ifndef CSELECTDOUBLELISTWIDGET_H
#define CSELECTDOUBLELISTWIDGET_H

#include "ui_ISelectDoubleListWidget.h"

class CSelectDoubleListWidget : public QWidget, private Ui::ISelectDoubleListWidget
{
    Q_OBJECT
public:
    CSelectDoubleListWidget(QWidget *parent);
    virtual ~CSelectDoubleListWidget();

    void setAvailable(const QList<QListWidgetItem *> &available);
    void setSelected(const QList<QListWidgetItem *> &selected);
    void setLabelAvailable(const QString & label);
    void setLabelSelected(const QString & label);
    const QList<QListWidgetItem *> selected();

private slots:
    void slotSelectedClicked(const QModelIndex & index);
    void slotAvailableClicked(const QModelIndex & index);
    void slotAdd();
    void slotRemove();
    void slotUp();
    void slotDown();

private:
    void filterListAvailable();
    void updateButtons();

    QList<QListWidgetItem *> available;
};
#endif //CSELECTDOUBLELISTWIDGET_H
