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

#include "gis/prj/IGisProject.h"
#include "gis/prj/CDetailsPrj.h"
#include "gis/IGisItem.h"
#include "gis/CGisListWks.h"
#include "gis/CGisDraw.h"
#include "CMainWindow.h"


#include <QtWidgets>


IGisProject::IGisProject(type_e type, const QString &filename, CGisListWks *parent)
    : QTreeWidgetItem(parent)
    , type(type)
    , filename(filename)
    , valid(false)
{

}

IGisProject::~IGisProject()
{
    delete dlgDetails;
}


void IGisProject::genKey()
{
    if(key.isEmpty())
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setVersion(QDataStream::Qt_5_2);

        *this >> stream;

        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(buffer);
        key = md5.result().toHex();
    }
}


void IGisProject::edit()
{
    if(dlgDetails.isNull())
    {
        dlgDetails = new CDetailsPrj(*this, 0);
        dlgDetails->setObjectName(getName());
    }

    CMainWindow::self().addWidgetToTab(dlgDetails);
}

void IGisProject::setName(const QString& str)
{
    metadata.name = str;
    setText(0, str);
    changed();
}

void IGisProject::setKeywords(const QString& str)
{
    metadata.keywords = str;
    changed();
}

void IGisProject::setDescription(const QString& str)
{
    metadata.desc = str;
    changed();
}

void IGisProject::setLinks(const QList<IGisItem::link_t>& links)
{
    metadata.links = links;
    changed();
}

void IGisProject::changed()
{
    setText(1,"*");
}


void IGisProject::setupName(const QString &defaultName)
{
    if(metadata.name.isEmpty())
    {
        setText(0, defaultName);
        metadata.name = defaultName;
    }
    else
    {
        setText(0,metadata.name);
    }

}

void IGisProject::markAsSaved()
{
    setText(1,"");
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }
        item->updateDecoration(IGisItem::eMarkNone, IGisItem::eMarkChanged);
    }
}

QString IGisProject::getInfo()
{
    QString str = metadata.name.isEmpty() ? text(0) : metadata.name;    
    str = "<div style='font-weight: bold;'>" + str + "</div>";

    if(metadata.time.isValid())
    {
        str += "<br/>\n";
        str += IUnit::datetime2string(metadata.time, false);
    }


    QString desc = IGisItem::removeHtml(metadata.desc).simplified();
    if(!desc.isEmpty())
    {
        str += "<br/>\n";

        if(desc.count() < 100)
        {
            str += desc;
        }
        else
        {
            str += desc.left(97) + "...";
        }
    }

    if(!filename.isEmpty())
    {
        str += QObject::tr("<br/>\nFilename: %1").arg(filename);
    }

    // count number of items by type
    int counter[IGisItem::eTypeMax] = {0};
    for(int i = 0; i < childCount(); i++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        counter[item->type()]++;
    }
    if(counter[IGisItem::eTypeWpt])
    {
        str += "<br/>\n" + QObject::tr("Waypoints: %1").arg(counter[IGisItem::eTypeWpt]);
    }
    if(counter[IGisItem::eTypeTrk])
    {
        str += "<br/>\n" + QObject::tr("Tracks: %1").arg(counter[IGisItem::eTypeTrk]);
    }
    if(counter[IGisItem::eTypeRte])
    {
        str += "<br/>\n" + QObject::tr("Routes: %1").arg(counter[IGisItem::eTypeRte]);
    }
    if(counter[IGisItem::eTypeOvl])
    {
        str += "<br/>\n" + QObject::tr("Areas: %1").arg(counter[IGisItem::eTypeOvl]);
    }

    return str;
}

IGisItem * IGisProject::getItemByKey(const IGisItem::key_t& key)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            return item;
        }
    }
    return 0;
}

void IGisProject::getItemByPos(const QPointF& pos, QList<IGisItem *> &items)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

        if(item->isCloseTo(pos))
        {
            items << item;
        }
    }
}


bool IGisProject::delItemByKey(const IGisItem::key_t& key, QMessageBox::StandardButtons& last)
{
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            if(last != QMessageBox::YesToAll)
            {
                QString msg = QObject::tr("Are you sure you want to delete '%1' from project '%2'?").arg(item->getName()).arg(text(0));
                last = QMessageBox::question(0, QObject::tr("Delete..."), msg, QMessageBox::YesToAll|QMessageBox::Cancel|QMessageBox::Ok|QMessageBox::No, QMessageBox::Ok);
                if((last == QMessageBox::No) || (last == QMessageBox::Cancel))
                {
                    // as each item in the project has to be unique, we can stop searching.
                    return false;
                }
            }
            delete item;

            /*
                Database projects are a bit different. Deleteing an item does not really
                mean the project is changed as the item is still stored in the database.
            */
            if(type != eTypeDb)
            {
                changed();
            }

            // as each item in the project has to be unique, we can stop searching.
            return true;
        }
    }
    return false;
}

void IGisProject::editItemByKey(const IGisItem::key_t& key)
{
    for(int i = childCount(); i > 0; i--)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(child(i-1));
        if(item == 0)
        {
            continue;
        }

        if(item->getKey() == key)
        {
            item->edit();
        }
    }
}

void IGisProject::drawItem(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, CGisDraw * gis)
{
    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

//        if(seenKeys.contains(item->getKey()))
//        {
//            continue;
//        }
//        seenKeys << item->getKey();

        item->drawItem(p, viewport, blockedAreas, gis);
    }

}

void IGisProject::drawItem(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    for(int i = 0; i < childCount(); i++)
    {

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }


        item->drawItem(p, viewport, gis);
    }

}

void IGisProject::drawLabel(QPainter& p, const QRectF& viewport, QList<QRectF>& blockedAreas, QSet<QString> &seenKeys, const QFontMetricsF& fm, CGisDraw * gis)
{

    for(int i = 0; i < childCount(); i++)
    {
        if(gis->needsRedraw())
        {
            break;
        }

        IGisItem * item = dynamic_cast<IGisItem*>(child(i));
        if(item == 0)
        {
            continue;
        }

//        if(seenKeys.contains(item->getKey()))
//        {
//            continue;
//        }
//        seenKeys << item->getKey();

        item->drawLabel(p, viewport, blockedAreas, fm, gis);
    }

}
