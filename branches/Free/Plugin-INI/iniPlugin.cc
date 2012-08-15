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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

// Includes
#include "iniPlugin.hh"

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/Light/Light.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

/// Constructor
INIPlugin::INIPlugin() {
}


template <>
void INIPlugin::parseIniFileT(INIFile& _ini, LightObject* _object) {
  
  if ( _object ) {
    
    std::vector< QString > draw_modes;
    if( _ini.get_entry(draw_modes, _object->name(), "MaterialDrawMode") )
      _object->materialNode()->drawMode( listToDrawMode(draw_modes) );
    
  }
}


template <>
void INIPlugin::saveIniFileT(INIFile& _ini, LightObject* _object) {
  
  if ( _object ) {
    
    _ini.add_entry( _object->name() ,
                    "MaterialDrawMode" ,
                    drawModeToList( _object->materialNode()->drawMode()) );    
                    
  }
}

void INIPlugin::parseIniFile(INIFile& _ini, BaseObjectData* _object) {
  
  if ( _object ) {
    std::vector< QString > draw_modes;
    if( _ini.get_entry(draw_modes, _object->name(), "MaterialDrawMode") )
      _object->materialNode()->drawMode( listToDrawMode(draw_modes) );
    
    ACG::Vec4f col(0.0,0.0,0.0,0.0);
    
    if ( _ini.get_entryVecf( col, _object->name() , "BaseColor" ) )
      _object->materialNode()->set_base_color(col);
    
    if ( _ini.get_entryVecf( col, _object->name() , "AmbientColor" ) )
      _object->materialNode()->set_ambient_color(col);
    
    if ( _ini.get_entryVecf( col, _object->name() , "DiffuseColor" ) )
      _object->materialNode()->set_diffuse_color(col);
    
    if ( _ini.get_entryVecf( col, _object->name() , "SpecularColor" ) )
      _object->materialNode()->set_specular_color(col);
    
    double shininess;
    if ( _ini.get_entry( shininess, _object->name() , "Shininess" ) )
      _object->materialNode()->set_shininess(shininess);
    
    double reflectance;
    if ( _ini.get_entry( reflectance, _object->name() , "Reflectance" ) ) {
      _object->materialNode()->set_reflectance(reflectance);        
    }
    
    bool visible;
    if ( _ini.get_entry( visible, _object->name() , "Visible" ) ) {
      _object->visible(visible);        
    }
    
    int size = 1;
    if ( _ini.get_entry( size, _object->name() , "PointSize" ) )
      _object->materialNode()->set_point_size(size);
    if ( _ini.get_entry( size, _object->name() , "LineWidth" ) )
      _object->materialNode()->set_line_width(size);
  }
}


void INIPlugin::saveIniFile(INIFile& _ini, BaseObjectData* _object) {
  
  if ( _object ) {
        _ini.add_entry( _object->name() ,
            "MaterialDrawMode" ,
            drawModeToList( _object->materialNode()->drawMode()) );
        _ini.add_entryVec( _object->name() ,
            "BaseColor" ,
            _object->materialNode()->base_color()) ;
        _ini.add_entryVec( _object->name() ,
            "AmbientColor" ,
            _object->materialNode()->ambient_color()) ;
        _ini.add_entryVec( _object->name() ,
            "DiffuseColor" ,
            _object->materialNode()->diffuse_color());
        _ini.add_entryVec( _object->name() ,
            "SpecularColor" ,
            _object->materialNode()->specular_color());
        _ini.add_entry( _object->name() ,
            "Shininess" ,
            _object->materialNode()->shininess());
        _ini.add_entry( _object->name() ,
            "Reflectance" ,
            _object->materialNode()->reflectance());       
        _ini.add_entry( _object->name() ,
            "Visible" ,
            _object->visible() );     
        _ini.add_entry( _object->name() ,
            "PointSize" ,
            _object->materialNode()->point_size());
        _ini.add_entry( _object->name() ,
            "LineWidth" ,
            _object->materialNode()->line_width());
    }
    
}

void INIPlugin::loadIniFile( INIFile& _ini, int _id ) {
    
    BaseObjectData* baseObject;
    if ( !PluginFunctions::getObject(_id,baseObject) ) {
        emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
        return;
    }
    
    PolyMeshObject* polyObject(0);
    TriMeshObject*  triObject(0);
    LightObject*    lightObject(0);
    
    // Load all data from baseobjectdata part
    parseIniFile(_ini,baseObject);
    
    if(baseObject->dataType() == DATA_POLY_MESH) {
        polyObject = PluginFunctions::polyMeshObject(baseObject);
        parseIniFileT(_ini, polyObject);
    } else if ( baseObject->dataType() == DATA_TRIANGLE_MESH) {
        triObject = PluginFunctions::triMeshObject(baseObject);
        parseIniFileT(_ini, triObject);
    }  else if ( baseObject->dataType() == DATA_LIGHT ) {
      lightObject = PluginFunctions::lightObject(baseObject);
      parseIniFileT(_ini, lightObject);
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
    TriMeshObject*  triObject(0);
    LightObject*    lightObject(0);
    
    // Save all data from baseobjectdata part
    saveIniFile(_ini,baseObject);
    
    if(baseObject->dataType() == DATA_POLY_MESH) {
        polyObject = PluginFunctions::polyMeshObject(baseObject);
        saveIniFileT(_ini, polyObject);
    } else if ( baseObject->dataType() == DATA_TRIANGLE_MESH) {
        triObject = PluginFunctions::triMeshObject(baseObject);
        saveIniFileT(_ini, triObject);
    } else if ( baseObject->dataType() == DATA_LIGHT ) {
        lightObject = PluginFunctions::lightObject(baseObject);
        saveIniFileT(_ini, lightObject);
    } else {
      // Unhandled data type
      emit log(LOGERR, tr("The specified data type is not supported, yet. Aborting!"));
    }
}



Q_EXPORT_PLUGIN2( iniplugin , INIPlugin );
