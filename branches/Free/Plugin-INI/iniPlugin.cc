/*===========================================================================*\
*                                                                           *
*                              OpenFlipper                                  *
*      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
*                           www.openflipper.org                             *
*                                                                           *
*---------------------------------------------------------------------------*
*  This file is part of OpenFlipper.                                        *
*                                                                           *
*  OpenFlipper is free software: you can redistribute it and/or modify      *
*  it under the terms of the GNU Lesser General Public License as           *
*  published by the Free Software Foundation, either version 3 of           *
*  the License, or (at your option) any later version with the              *
*  following exceptions:                                                    *
*                                                                           *
*  If other files instantiate templates or use macros                       *
*  or inline functions from this file, or you compile this file and         *
*  link it with other files to produce an executable, this file does        *
*  not by itself cause the resulting executable to be covered by the        *
*  GNU Lesser General Public License. This exception does not however       *
*  invalidate any other reasons why the executable file might be            *
*  covered by the GNU Lesser General Public License.                        *
*                                                                           *
*  OpenFlipper is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU LesserGeneral Public          *
*  License along with OpenFlipper. If not,                                  *
*  see <http://www.gnu.org/licenses/>.                                      *
*                                                                           *
\*===========================================================================*/

/*===========================================================================*\
*                                                                           *
*   $Revision: 7810 $                                                       *
*   $Author: kremer $                                                       *
*   $Date: 2009-12-08 16:23:42 +0100 (Tue, 08 Dec 2009) $                   *
*                                                                           *
\*===========================================================================*/

// Includes
#include "iniPlugin.hh"

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

/// Constructor
INIPlugin::INIPlugin() {
}

void INIPlugin::loadIniFile( INIFile& _ini, int _id ) {
    
    BaseObjectData* baseObject;
    if ( !PluginFunctions::getObject(_id,baseObject) ) {
        emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
        return;
    }
    
    PolyMeshObject* polyObject(0);
    TriMeshObject* triObject(0);
    
    if(baseObject->dataType() == DATA_POLY_MESH) {
        polyObject = PluginFunctions::polyMeshObject(baseObject);
        parseIniFileT(_ini, polyObject);
    } else if ( baseObject->dataType() == DATA_TRIANGLE_MESH) {
        triObject = PluginFunctions::triMeshObject(baseObject);
        parseIniFileT(_ini, triObject);
    } else {
        // Unhandled data type
        emit log(LOGERR, tr("The specified data type is not supported, yet. Aborting!"));
    }
}

void INIPlugin::saveIniFile( INIFile& _ini, int _id) {
    
    BaseObjectData* baseObject;
    if ( !PluginFunctions::getObject(_id,baseObject) ) {
        emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
        return;
    }
    
    PolyMeshObject* polyObject(0);
    TriMeshObject* triObject(0);
    
    if(baseObject->dataType() == DATA_POLY_MESH) {
        polyObject = PluginFunctions::polyMeshObject(baseObject);
        saveIniFileT(_ini, polyObject);
    } else if ( baseObject->dataType() == DATA_TRIANGLE_MESH) {
        triObject = PluginFunctions::triMeshObject(baseObject);
        saveIniFileT(_ini, triObject);
    } else {
        // Unhandled data type
        emit log(LOGERR, tr("The specified data type is not supported, yet. Aborting!"));
    }
}