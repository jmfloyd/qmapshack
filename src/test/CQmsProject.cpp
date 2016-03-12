/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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

#include <QtCore>

#include "test/TestHelper.h"
#include "test/test_QMapShack.h"

#include "gis/gpx/CGpxProject.h"
#include "gis/qms/CQmsProject.h"

void test_QMapShack::_readQmsFile_1_6_0()
{
    IGisProject *proj1 = readProjFile("V1.6.0_file1.qms");
    verify("V1.6.0_file1.qms", *proj1);
    delete proj1;

    IGisProject *proj2 = readProjFile("V1.6.0_file2.qms");
    verify("V1.6.0_file2.qms", *proj2);
    delete proj1;
}

void test_QMapShack::_writeReadQmsFile()
{
    for(const QString &file : inputFiles)
    {
        IGisProject *proj = readProjFile(file);

        QString tmpFile = TestHelper::getTempFileName("qms");
        CQmsProject::saveAs(tmpFile, *proj);

        delete proj;

        proj = readQmsFile(tmpFile);
        verify(file, *proj);

        delete proj;

        QFile(tmpFile).remove();
    }
}

CQmsProject* test_QMapShack::readQmsFile(const QString &file, bool)
{
    CQmsProject *proj = new CQmsProject(fileToPath(file), (CGisListWks*) nullptr);

    SUBVERIFY(IGisProject::eTypeQms == proj->getType(), "Project has invalid type");

    tryVerify(file, *proj);

    return proj;
}
