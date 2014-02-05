/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "iniPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <ObjectTypes/MeshObject/MeshObjectT.hh>
#include <OpenFlipper/common/BaseObjectData.hh>

template <class MeshObject>
void INIPlugin::parseIniFileT(INIFile& _ini, MeshObject* _object) {
    
    if ( _object ) {
      
      std::vector< QString > draw_modes;
      
      if( _ini.get_entry(draw_modes, _object->name(), "MeshDrawMode") )
          _object->meshNode()->drawMode( listToDrawMode(draw_modes) );

    }
}

template <class MeshObject>
void INIPlugin::saveIniFileT(INIFile& _ini, MeshObject* _object) {

    if ( _object ) {
        _ini.add_entry( _object->name() ,
            "MeshDrawMode" ,
            drawModeToList( _object->meshNode()->drawMode()) );
    }
}

