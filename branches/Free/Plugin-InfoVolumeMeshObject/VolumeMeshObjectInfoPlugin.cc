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

/*===========================================================================*\
*                                                                            *
*   $Revision: 16192 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2013-01-25 08:55:31 +0100 (Fri, 25 Jan 2013) $                     *
*                                                                            *
\*===========================================================================*/

//=============================================================================
//
//  CLASS InfoVolumeMeshObjectPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "VolumeMeshObjectInfoPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/BaseObject.hh>

#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>

//== IMPLEMENTATION ==========================================================


InfoVolumeMeshObjectPlugin::InfoVolumeMeshObjectPlugin()
{
}

InfoVolumeMeshObjectPlugin::~InfoVolumeMeshObjectPlugin()
{
}


void InfoVolumeMeshObjectPlugin::initializePlugin()
{
}

/// initialize the plugin
void InfoVolumeMeshObjectPlugin::pluginsInitialized() {

  //set the slot descriptions
  setDescriptions();
  
}

//-----------------------------------------------------------------------------

//DataType InfoVolumeMeshObjectPlugin::supportedDataTypes() {
//    return DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH);
//}

//----------------------------------------------------------------------------------------------

void
InfoVolumeMeshObjectPlugin::
  slotInformationRequested(const QPoint _clickedPoint, DataType _type) {

    // not implemented yet

}

//------------------------------------------------------------------------------

void InfoVolumeMeshObjectPlugin::slotObjectUpdated( int _identifier , const UpdateType& _type)
{
}

//------------------------------------------------------------------------------

void InfoVolumeMeshObjectPlugin::slotObjectSelectionChanged( int _identifier )
{
  slotObjectUpdated( _identifier , UPDATE_ALL );
}

//------------------------------------------------------------------------------

void InfoVolumeMeshObjectPlugin::slotAllCleared()
{
}

Q_EXPORT_PLUGIN2( InfoVolumeMeshObjectPlugin , InfoVolumeMeshObjectPlugin );


