
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

