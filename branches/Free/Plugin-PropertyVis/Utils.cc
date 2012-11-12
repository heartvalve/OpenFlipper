/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

#include "Utils.hh"

#include <QPushButton>

const char* PropertyInfo::entity2str(ENTITY_FILTER entity) {
    switch (entity) {
        case EF_EDGE:
            return "→";
        case EF_FACE:
            return "△";
        case EF_HALFEDGE:
            return "⇀";
        case EF_VERTEX:
            return "•";
        case EF_CELL:
            return "C";
        case EF_HALFFACE:
            return "HF";
        default:
            return "error";
    }
}


NewNameMessageBox::NewNameMessageBox(QString propName)
    :
      QMessageBox(),
      propName(propName),
      replace(false),
      rename(false),
      cancel(false)
{
    setWindowTitle("Name Selection");

    setText(tr("The name %1 is already in use.\nWould you like to choose a different name, replace the old property or cancel loading?").arg(propName));

    buttonRename = new QPushButton("Rename");
    buttonReplace = new QPushButton("Replace");
    buttonCancel = new QPushButton("Cancel");

    addButton(buttonRename, QMessageBox::AcceptRole);
    addButton(buttonCancel, QMessageBox::RejectRole);
    addButton(buttonReplace, QMessageBox::YesRole);

    connect(buttonRename,  SIGNAL(clicked()), this, SLOT(slotRename()));
    connect(buttonReplace, SIGNAL(clicked()), this, SLOT(slotReplace()));
    connect(buttonCancel,  SIGNAL(clicked()), this, SLOT(slotCancel()));

    setModal(true);
}
