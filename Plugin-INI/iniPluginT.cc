#include "iniPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

template <class MeshObject>
void INIPlugin::parseIniFileT(INIFile& _ini, MeshObject* _object) {
    
    if ( _object ) {
        std::vector< QString > draw_modes;
        if( _ini.get_entry(draw_modes, _object->name(), "MaterialDrawMode") )
            _object->materialNode()->drawMode( ListToDrawMode(draw_modes) );
        
        if( _ini.get_entry(draw_modes, _object->name(), "MeshDrawMode") )
            _object->meshNode()->drawMode( ListToDrawMode(draw_modes) );
        
        ACG::Vec4f col(0.0,0.0,0.0,0.0);
        
        if ( _ini.get_entryVecf( col, _object->name() , "BaseColor" ) )
            _object->materialNode()->set_base_color(col);
        
        if ( _ini.get_entryVecf( col, _object->name() , "AmbientColor" ) )
            _object->materialNode()->set_ambient_color(col);
        
        if ( _ini.get_entryVecf( col, _object->name() , "DiffuseColor" ) )
            _object->materialNode()->set_diffuse_color(col);
        
        if ( _ini.get_entryVecf( col, _object->name() , "SpecularColor" ) )
            _object->materialNode()->set_specular_color(col);
        
        int size = 1;
        if ( _ini.get_entry( size, _object->name() , "PointSize" ) )
            _object->materialNode()->set_point_size(size);
        if ( _ini.get_entry( size, _object->name() , "LineWidth" ) )
            _object->materialNode()->set_line_width(size);
    }
}

template <class MeshObject>
void INIPlugin::saveIniFileT(INIFile& _ini, MeshObject* _object) {

    if ( _object ) {
        _ini.add_entry( _object->name() ,
            "MaterialDrawMode" ,
            drawModeToList( _object->materialNode()->drawMode()) );
        _ini.add_entry( _object->name() ,
            "MeshDrawMode" ,
            drawModeToList( _object->meshNode()->drawMode()) );
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
            "PointSize" ,
            _object->materialNode()->point_size());
        _ini.add_entry( _object->name() ,
            "LineWidth" ,
            _object->materialNode()->line_width());
    }
}
