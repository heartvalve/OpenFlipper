//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================





#include <QtGui>

#include "TextureControl.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <math.h>

#define TEXTUREDATA "TextureData"



void TextureControlPlugin::slotTextureAdded( QString _textureName , QString _filename , uint _dimension , int _id)
{
  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
    return;
  }

  // Get Texture data for this object or create one if it does not exist
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    texData = new TextureData();
    obj->setObjectData(TEXTUREDATA, texData);
  }

  if ( texData->textureExists(_textureName) ) {
    emit log(LOGERR,"Trying to add already existing texture " + _textureName + " for object " + QString::number(_id) );
    return;
  }

  // ================================================================================
  // Get the image file
  // ================================================================================

  QImage textureImage;
  getImage(_filename,textureImage);

  // ================================================================================
  // Add the texture to the texture node and get the corresponding id
  // ================================================================================
  GLuint glName = 0;

  //inform textureNode about the new texture
  if( obj->dataType( DATA_TRIANGLE_MESH ) )
    glName = PluginFunctions::triMeshObject(obj)->textureNode()->add_texture(textureImage);

  if ( obj->dataType( DATA_POLY_MESH ) )
    glName = PluginFunctions::polyMeshObject(obj)->textureNode()->add_texture(textureImage);

  // ================================================================================
  // Store texture information in objects metadata
  // ================================================================================

  if (glName == 0) {
    emit log(LOGERR,"Unable to bind texture!");
    return;
  }

  texData->addTexture(_textureName,_filename,_dimension,glName);
  texData->texture(_textureName).disable();
}

void TextureControlPlugin::slotTextureAdded( QString _textureName , QString _filename , uint _dimension)
{
  // Add this texture to the list of global textures
  if ( ! globalTextures_.textureExists(_textureName) ) {
    globalTextures_.addTexture(_textureName,_filename,_dimension,0);
    globalTextures_.texture(_textureName).disable();

    QImage textureImage;
    getImage(_filename,textureImage);
    globalTextures_.texture(_textureName).textureImage = textureImage;

  } else {
    emit log(LOGERR,"Trying to add already existing global texture " + _textureName );
    return;
  }

  // Add a new entry to the global Texture menu
  QAction* new_texture = new QAction(_textureName, this);
  new_texture->setStatusTip(tr("Switch all objects to this Texture ( if available )"));
  new_texture->setCheckable(true);
  actionGroup_->addAction(new_texture);
  textureMenu_->addAction(new_texture);
  new_texture->setChecked(true);
  textureActions_.push_back(new_texture);

}

void TextureControlPlugin::slotMultiTextureAdded( QString _textureGroup , QString _name , QString _filename , int _id , int& _textureId ) {
   // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
  }

  // Check if we support this kind of data
  if ( !obj->dataType(DATA_TRIANGLE_MESH) && !obj->dataType(DATA_POLY_MESH) ) {
      emit log(LOGERR,"Trying to add textures to object failed because of unsupported object type");
      return;
  }

  // Get Texture data for this object or create one if it does not exist
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    texData = new TextureData();
    obj->setObjectData(TEXTUREDATA, texData);
  }

  if ( !texData->textureExists( _textureGroup ) )
    texData->addMultiTexture( _textureGroup );

  // Add the texture
  slotTextureAdded( _name , _filename , 2 , _id);

  // Get the id of the new texture
  _textureId = -1;
  _textureId = texData->texture(_name).id();

  //hide the texture (its accessible through the multiTexture)
  texData->texture(_name).hidden( true );

  QImage textureImage;
  getImage(_filename,textureImage);
  texData->texture(_name).textureImage = textureImage;

  // Store the new texture in the list of this textureGroup
  if ( _textureId != -1 ) {
    texData->texture(_textureGroup).multiTextureList << _name ;
  } else {
    emit log(LOGERR,"Error when getting internal id of new multitexture!");
  }

}

void TextureControlPlugin::getImage( QString _fileName, QImage& _image ) {
  QString loadFilename;

  if ( _fileName.startsWith("/") || _fileName.startsWith(".") )
    loadFilename = _fileName;
  else
    loadFilename = OpenFlipper::Options::textureDirStr() + QDir::separator() + _fileName;

  if ( !_image.load( loadFilename ) ){
        emit log(LOGERR, "Cannot load texture " + _fileName + " at : " + loadFilename);
        _image.load(OpenFlipper::Options::textureDirStr() + QDir::separator() + "unknown.png");
  }

}

void TextureControlPlugin::addedEmptyObject( int _id ) {
  fileOpened(_id);
}

template< typename MeshT >
void TextureControlPlugin::handleFileOpenTextures( MeshT*& _mesh , int _objectId ) {
  // If this property is available we have a mapping between face_index_property and
  // available textures stored in the map
  OpenMesh::MPropHandleT< std::map< int, std::string > > property;
  if ( _mesh->get_property_handle(property,"TextureMapping") ) {

    // As internal texture indices might differ from the available ones,
    // We have to remap them after loading the textures!
    std::map< int,int > newMapping;

    // TODO : If only one Texture, use single Texturing mode
    if ( true ) {
      // Assume multiTexture Mode now and load the Textures
      for ( std::map< int, std::string >::iterator texture  = _mesh->property(property).begin();
                                                   texture != _mesh->property(property).end(); texture++ ) {
        int textureId = -1;
        slotMultiTextureAdded("OBJ Data",QString(texture->second.c_str()) , QString(texture->second.c_str()), _objectId, textureId );
        newMapping[texture->first] = textureId;
      }

      // Convert the indices stored in the mesh to the actual ones used for rendering
      OpenMesh::FPropHandleT< int > newIndexProperty;
      if (! _mesh->get_property_handle(newIndexProperty,"TextureControl: OriginalFileIndexMapping") ) {
        _mesh->add_property(newIndexProperty,"TextureControl: OriginalFileIndexMapping");
      }

      for ( TriMesh::FaceIter f_it = _mesh->faces_begin(); f_it != _mesh->faces_end(); ++f_it)
        _mesh->property(newIndexProperty, f_it ) =  newMapping[_mesh->texture_index( f_it )];

      // We use a different property for storing the IndexProperty to prevent overwriting them
      slotSetTextureMode("OBJ Data","indexProperty=TextureControl: OriginalFileIndexMapping", _objectId);
    }
  }

}

void TextureControlPlugin::fileOpened( int _id ) {
  // TODO:: Store original texture coords in a new property!

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
  }

  // Check if we support this kind of data
  if ( !obj->dataType(DATA_TRIANGLE_MESH) && !obj->dataType(DATA_POLY_MESH) ) {
      emit log(LOGERR,"Trying to add textures to object failed because of unsupported object type");
      return;
  }

  // Get Texture data for this object or create one if it does not exist
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    texData = new TextureData();
    obj->setObjectData(TEXTUREDATA, texData);
  }

  // Check if the file contains a texture map and handle it before adding global textures
  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
      TriMesh* mesh = PluginFunctions::triMesh(obj);
      if ( mesh )
        handleFileOpenTextures(mesh,_id);
  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);
    if ( mesh )
      handleFileOpenTextures(mesh,_id);
  }

  // Iterate over all available global textures and add them to the object
  for ( uint i = 0 ; i < globalTextures_.textures().size() ; ++i) {

    // ================================================================================
    // Get the image file
    // ================================================================================

    QImage textureImage;
    getImage(globalTextures_.textures()[i].filename(),textureImage);

    // ================================================================================
    // Add the texture to the texture node and get the corresponding id
    // ================================================================================
    GLuint glName = 0;

    //inform textureNode about the new texture
    if( obj->dataType( DATA_TRIANGLE_MESH ) )
      glName = PluginFunctions::triMeshObject(obj)->textureNode()->add_texture(textureImage);

    if ( obj->dataType( DATA_POLY_MESH ) )
      glName = PluginFunctions::polyMeshObject(obj)->textureNode()->add_texture(textureImage);

    // ================================================================================
    // Store texture information in objects metadata
    // ================================================================================

    if (glName != 0) {
      texData->addTexture(globalTextures_.textures()[i], glName);
      texData->setImage(globalTextures_.textures()[i].name(),textureImage);
    }
    else {
      emit log(LOGERR,"Unable to bind Texture");
      continue;
    }


    // ================================================================================
    // Update texture mapping in meshNode
    // ================================================================================
    if( obj->dataType( DATA_TRIANGLE_MESH ) ){
      PluginFunctions::triMeshObject(obj)->meshNode()->set_texture_map( 0 );
      PluginFunctions::triMeshObject(obj)->meshNode()->set_property_map( 0 );
    }

    if ( obj->dataType( DATA_POLY_MESH ) ){
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_texture_map( 0 );
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_property_map( 0 );
    }

  }
}

void TextureControlPlugin::slotTextureUpdated( QString _textureName , int _identifier ) {

  // ================================================================================
  // Get updated object
  // ================================================================================
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _identifier , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_identifier) );
    return;
  }

  // ================================================================================
  // Get objects texture data and verify that texture exists
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    std::cerr << "Texture data not found!" << std::endl;
    return;
  }

  // ================================================================================
  // Check if texture exists
  // ================================================================================
  if ( ! texData->textureExists(_textureName) ) {
    emit log(LOGERR,"Texture " + _textureName + " not found on object " + QString::number(_identifier) );
    return;
  }

  // ================================================================================
  // If texture is not enabled, mark it as dirty and defer update to visualization update
  // ================================================================================
  if ( ! texData->texture(_textureName).enabled() ) {
    texData->texture(_textureName).setDirty();
    return;
  }

  QImage textureImage;
  getImage( texData->texture(_textureName).filename(), textureImage);

  // ================================================================================
  // As the current texture is active, update it
  // ================================================================================
  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMesh* mesh = PluginFunctions::triMesh(obj);
    doUpdateTexture(texData->texture(_textureName), *mesh);
    PluginFunctions::triMeshObject(obj)->textureNode()->set_repeat(texData->texture(_textureName).parameters.repeat);
    PluginFunctions::triMeshObject(obj)->textureNode()->set_texture( textureImage );
  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);
    doUpdateTexture(texData->texture(_textureName), *mesh);
    PluginFunctions::polyMeshObject(obj)->textureNode()->set_repeat(texData->texture(_textureName).parameters.repeat);
    PluginFunctions::polyMeshObject(obj)->textureNode()->set_texture( textureImage );
  }

  // ================================================================================
  // Mark texture as not dirty
  // ================================================================================
  texData->texture(_textureName).clean();

  // ================================================================================
  // Enable the right draw mode and update
  // ================================================================================
  switchDrawMode( texData->texture( _textureName ).type() );
}

template< typename MeshT >
void TextureControlPlugin::doUpdateTexture ( Texture& _texture, MeshT& _mesh )
{

  if ( _texture.type() == HALFEDGEBASED ) {
    if (_texture.dimension() == 1) {

      OpenMesh::HPropHandleT< double > texture;
	  if ( ! _mesh.get_property_handle(texture, _texture.name().toStdString() ) ) {
        emit log(LOGERR,"Unable to get property " + _texture.name() );
        return;
      }

      copyTexture(_texture, _mesh, texture);

    } else if ( _texture.dimension() == 2 ) {

      OpenMesh::HPropHandleT< OpenMesh::Vec2d > texture2D;
	  if ( ! _mesh.get_property_handle( texture2D, _texture.name().toStdString() ) ) {
        emit log(LOGERR,"Unable to get property " + _texture.name() );
        return;
      }

      copyTexture( _texture, _mesh, texture2D);

    } else
      emit log(LOGERR, "Unsupported Texture Dimension " + QString::number(_texture.dimension() ) );
  } else if ( _texture.type() == VERTEXBASED ) {
    if ( _texture.dimension() == 1 ) {

      OpenMesh::VPropHandleT< double > texture;
	  if ( ! _mesh.get_property_handle(texture,_texture.name().toStdString() ) ) {
        emit log(LOGERR,"Unable to get property " + _texture.name() );
        return;
      }

        copyTexture(_texture, _mesh, texture);

      } else if ( _texture.dimension() == 2 ) {

        OpenMesh::VPropHandleT< OpenMesh::Vec2d >  texture2D;
		  if ( ! _mesh.get_property_handle(texture2D,_texture.name().toStdString() ) ) {
          emit log(LOGERR,"Unable to get property " + _texture.name() );
          return;
        }

        copyTexture( _texture, _mesh, texture2D);

      } /*else if ( textures_[_textureid].dimension == 3 ) {

        OpenMesh::VPropHandleT< OpenMesh::Vec3d >  scalarField3D;
        if ( ! _mesh.get_property_handle(scalarField3D,_texture.name) ) {
          emit log(LOGERR,"Unable to get property " + _texture.name );
          return;
        }

        copyTexture(_textureid, _mesh, scalarField3D);

      }*/ else
        emit log(LOGERR, "Unsupported Texture Dimension " + QString::number(_texture.dimension() ) );

    } else
      emit log(LOGERR, "Unsupported Texture type");

}

void TextureControlPlugin::computeValue(Texture& _texture, double _min, double _max, double& _value) {
   const bool clamp         = _texture.parameters.clamp ;
   const bool center        = _texture.parameters.center;
   const double max_val     = _texture.parameters.max_val;
   const bool abs           = _texture.parameters.abs;
   const double clamp_max   = _texture.parameters.clamp_max;
   const double clamp_min   = _texture.parameters.clamp_min;
   const bool scale         = _texture.parameters.scale;
   const double scaleFactor = fabs(_max) + fabs(_min);
   const bool repeat        = _texture.parameters.repeat;

   // Use absolute value as requested by plugin
   if ( abs )
      _value = fabs(_value);

   // Clamp if requested
   if ( clamp ) {
      if ( _value > clamp_max )
         _value = clamp_max;
      if (_value < clamp_min)
         _value = clamp_min;
   }

   // if the texture should not be repeated, scale to 0..1
   if ( ! repeat ) {
      if (! center ) {
        if ( scale) {
          _value /= scaleFactor;
          _value -= _min/scaleFactor;
        }
      } else {
         // the values above zero are mapped to 0.5..1 the negative ones to 0.5..0
         if (_value > 0.0) {
            _value /= ( _max * 2.0);
            _value += 0.5;
         } else {
            _value /= ( _min * 2.0);
            _value = 0.5 - _value;
         }
      }
   } else {
      _value -= _min;
      _value *= max_val / (_max - _min);
   }
}

void TextureControlPlugin::slotObjectUpdated(int _identifier)
{
  // ================================================================================
  // Get updated object
  // ================================================================================
  if ( _identifier == -1 )
    return;

  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _identifier , obj ) ) {
    emit log(LOGERR,"slotObjectUpdated: Unable to get Object for id " + QString::number(_identifier) );
    return;
  }

  // ================================================================================
  // Get objects texture data and verify that texture exists
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    std::cerr << "slotObjectUpdated: Texture data not found!" << std::endl;
    return;
  }

  // ================================================================================
  // Set all textures to dirty
  // ================================================================================
  // TODO : if not in the texture rendering mode, do not emit update
  // Involves adding a interface part to react on draw mode changes
  // basic check implemented
  for ( uint i = 0; i < texData->textures().size(); ++i ) {
    texData->textures()[i].setDirty();

    bool update = false;
    for ( int j = 0 ; j < PluginFunctions::viewers() ; ++j ) {
       update |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED );
       update |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
    }

    if ( update && texData->textures()[i].enabled() )
      emit updateTexture( texData->textures()[i].name() , _identifier );
  }

}

void TextureControlPlugin::slotUpdateAllTextures( ) {
  std::cerr << "slotUpdateAllTextures: not implemented yet ... might be removed" << std::endl;
  // TODO : Check
//    // Force an update of all textures which are available for the updated object
//    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
//       for ( uint i = 0 ; i < textures_.size() ; ++i )
//          emit updateTexture( textures_[i].name , o_it->id() );
}

void TextureControlPlugin::slotSetTextureMode(QString _textureName ,QString _mode) {

  // ================================================================================
  // Update texture settings for global textures
  // ================================================================================
  if ( ! globalTextures_.textureExists(_textureName) ) {
    emit log(LOGERR,"Global texture does not exist: " + _textureName);
    return;
  }

  // ================================================================================
  // Parse parameters and update them in the texture data
  // ================================================================================
  Texture& texture = globalTextures_.texture(_textureName);

  // Cleanup representation
  _mode = _mode.toLower();

  int i = 0;
  QString nextString = _mode.section(',',i,i);
  while ( nextString != "" ) {
    QString sectionName = nextString.section('=',0,0);
    QString value = nextString.section('=',1,1);


    // Cleanup representation
    value       = value.trimmed();
    sectionName = sectionName.trimmed();

    if ( sectionName == "clamp" ) {
      texture.parameters.clamp = StringToBool(value);
    } else
    if ( sectionName == "clamp_max" ) {
      texture.parameters.clamp_max = value.toDouble();
    } else
    if ( sectionName == "clamp_min" ) {
      texture.parameters.clamp_min = value.toDouble();
    } else
    if ( sectionName == "max_val" ) {
      texture.parameters.max_val = value.toDouble();
    } else
    if ( sectionName == "repeat" ) {
      texture.parameters.repeat = StringToBool(value);
    } else
    if ( sectionName == "center" ) {
      texture.parameters.center = StringToBool(value);
    } else
    if ( sectionName == "scale" ) {
      texture.parameters.scale = StringToBool(value);
    } else
    if ( sectionName == "type" ) {
        if (value == "halfedgebased") {
            texture.type( HALFEDGEBASED );
        } else if ( value == "vertexbased")
        {
            texture.type( VERTEXBASED );
        } else  {
          emit log(LOGERR,"Unknown texture type : " + value + " for texture: " + _textureName);
        }
    } else
      emit log(LOGERR,"Unknown texture mode : " + sectionName);

    ++i;
    nextString = _mode.section(',',i,i);
  }

  // ================================================================================
  // Mark updated texture as dirty
  // ================================================================================
  texture.setDirty();


  // check if the local textures need to be updated
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it){

    TextureData* texData = dynamic_cast< TextureData* > ( o_it->objectData(TEXTUREDATA) );

    if (texData != 0){

      if ( texData->textureExists(_textureName) ){

        Texture& localTex = texData->texture(_textureName);

        //check if something changed
        bool changed = false;

        if ( _mode.contains("clamp") && (texture.parameters.clamp != localTex.parameters.clamp) ){
          localTex.parameters.clamp = texture.parameters.clamp;
          changed = true;
        }

        if ( _mode.contains("clamp_max") && (texture.parameters.clamp_max != localTex.parameters.clamp_max) ){
          localTex.parameters.clamp_max = texture.parameters.clamp_max;
          changed = true;
        }

        if ( _mode.contains("clamp_min") && (texture.parameters.clamp_min != localTex.parameters.clamp_min) ){
          localTex.parameters.clamp_min = texture.parameters.clamp_min;
          changed = true;
        }

        if ( _mode.contains("max_val") && (texture.parameters.max_val != localTex.parameters.max_val) ){
          localTex.parameters.max_val = texture.parameters.max_val;
          changed = true;
        }

        if ( _mode.contains("repeat") && (texture.parameters.repeat != localTex.parameters.repeat) ){
          localTex.parameters.repeat = texture.parameters.repeat;
          changed = true;
        }

        if ( _mode.contains("center") && (texture.parameters.center != localTex.parameters.center) ){
          localTex.parameters.center = texture.parameters.center;
          changed = true;
        }

        if ( _mode.contains("scale") && (texture.parameters.scale != localTex.parameters.scale) ){
          localTex.parameters.scale = texture.parameters.scale;
          changed = true;
        }

        if ( _mode.contains("type") && (texture.type() != localTex.type() ) ){
          localTex.type( texture.type() );
          changed = true;
        }

        //only update if the texture is enabled
        if (changed){
          if ( texData->isEnabled(_textureName) )
            emit updateTexture( _textureName, o_it->id() );
          else
            localTex.setDirty();
        }
      }
    }
  }
}

bool TextureControlPlugin::StringToBool(QString _value){
  if (_value == "false")
    return false;
  else
    return true;
}

void TextureControlPlugin::slotSetTextureMode(QString _textureName, QString _mode, int _id) {

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
  }

  // Get Texture data for this object
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );

  if ( texData == 0 || ( !texData->textureExists(_textureName))  ) {
    emit log(LOGERR,"Texture does not exist: " + _textureName + " (object=" + QString::number(_id) + ")");
    return;
  }

  // ================================================================================
  // Parse parameters and update them in the texture data
  // ================================================================================
  Texture& texture = texData->texture(_textureName);

  _mode = _mode.toLower();

  bool changed = false;

  int i = 0;
  QString nextString = _mode.section(',',i,i);
  while ( nextString != "" ) {
    QString sectionName = nextString.section('=',0,0);
    QString value = nextString.section('=',1,1);

    // Cleanup representation
    value       = value.trimmed();
    sectionName = sectionName.trimmed();

    if ( sectionName == "clamp" ) {
      if ( StringToBool(value) != texture.parameters.clamp ) {
        texture.parameters.clamp = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "clamp_max" ) {
      if (value.toDouble() != texture.parameters.clamp_max){
        texture.parameters.clamp_max = value.toDouble();
        changed = true;
      }
    } else if ( sectionName == "clamp_min" ) {
      if (value.toDouble() != texture.parameters.clamp_min){
        texture.parameters.clamp_min = value.toDouble();
        changed = true;
      }
    } else if ( sectionName == "max_val" ) {
      if (value.toDouble() != texture.parameters.max_val){
        texture.parameters.max_val = value.toDouble();
        changed = true;
      }
    } else if ( sectionName == "repeat" ) {
      if ( StringToBool(value) != texture.parameters.repeat ) {
        texture.parameters.repeat = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "center" ) {
      if ( StringToBool(value) != texture.parameters.center ) {
        texture.parameters.center = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "scale" ) {
      if ( StringToBool(value) != texture.parameters.scale ) {
        texture.parameters.scale = StringToBool(value);
        changed = true;
      }
    }else if ( sectionName == "indexProperty" ) {
      if ( value != texture.indexMappingProperty() ) {
        texture.indexMappingProperty( value );
        changed = true;
      }
    } else if ( sectionName == "type" ) {
        if ( ( value == "halfedgebased" ) && ( texture.type() != HALFEDGEBASED ) ) {
          texture.type( HALFEDGEBASED );
          changed = true;
        } else if ( (value == "vertexbased") && (texture.type() != HALFEDGEBASED)  ) {
          texture.type( VERTEXBASED );
          changed = true;
        } else {
          emit log(LOGERR,"Unknown texture type : " + value + " for texture: " + _textureName);
        }
    } else
      emit log(LOGERR,"Unknown texture mode : " + sectionName);

    ++i;
    nextString = _mode.section(',',i,i);
  }

  //only update if the texture is enabled
  if (changed){
    if ( texData->isEnabled(_textureName) )
      emit updateTexture( _textureName, _id );
    else
      texture.setDirty();
  }
}

void TextureControlPlugin::pluginsInitialized() {
  // ================================================================================
  // Create global texture menu
  // ================================================================================
  textureMenu_ = new QMenu(tr("&Texture Control"));
  textureMenu_->setTearOffEnabled(true);
  emit addMenubarAction(textureMenu_->menuAction(), VIEWMENU );

  // ================================================================================
  // Create Settings dialog
  // ================================================================================
  // TODO : Settings dialog updates required to change global/local textures,...
  settingsDialog_ = new texturePropertiesWidget(0);
  connect( settingsDialog_, SIGNAL( applyProperties(TextureData*,QString,int) ),
           this,              SLOT( applyDialogSettings(TextureData*,QString,int) ));

  connect( settingsDialog_, SIGNAL( getCoordinates1D(QString,int,std::vector< double >&)),
           this,              SLOT( getCoordinates1D(QString,int,std::vector< double >&)));

  settingsDialog_->installEventFilter( this );

  // ================================================================================
  // Create action group and menu for global textures
  // ================================================================================
  actionGroup_ = new QActionGroup( 0 );
  actionGroup_->setExclusive( true );
  connect( actionGroup_, SIGNAL( triggered( QAction * ) ),
          this, SLOT( slotTextureMenu( QAction * ) ) );

  QAction* AC_Texture_Settings = new QAction(tr("&Texture Settings"), this);
  AC_Texture_Settings->setStatusTip(tr("Set the texture visualization properties"));
  connect(AC_Texture_Settings, SIGNAL(triggered()), this, SLOT(slotSetTextureProperties()));
  textureMenu_->addAction(AC_Texture_Settings);

  textureMenu_->addSeparator();
  textureMenu_->addActions(actionGroup_->actions());

  // ================================================================================
  // Create basic per object context menu
  // ================================================================================
  contextMenu_ = new QMenu(0);
  contextMenu_->setTitle("Textures");
  emit addContextMenuItem(contextMenu_->menuAction() ,DATA_TRIANGLE_MESH , CONTEXTOBJECTMENU );
  emit addContextMenuItem(contextMenu_->menuAction() ,DATA_POLY_MESH     , CONTEXTOBJECTMENU );

}

void TextureControlPlugin::slotSetTextureProperties() {

  settingsDialog_->show( &globalTextures_, -1);
}

void TextureControlPlugin::applyDialogSettings(TextureData* _texData, QString _textureName, int _id) {

  if (_id != -1){
    //local texture

    // Get the object
    BaseObjectData* obj;
    if (! PluginFunctions::getObject(  _id , obj ) ) {
      emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
    }

    if ( !_texData->textureExists(_textureName) ) {
      emit log(LOGERR,"Texture does not exist in applyDialogSettings " + _textureName );
    }

    Texture& texture = _texData->texture(_textureName );

    if( obj->dataType( DATA_TRIANGLE_MESH ) ){
      PluginFunctions::triMeshObject(obj)->textureNode()->set_texture(texture.textureImage , texture.glName() );
    } else  if ( obj->dataType( DATA_POLY_MESH ) ) {
      PluginFunctions::polyMeshObject(obj)->textureNode()->set_texture(texture.textureImage , texture.glName() );
    }

    // Always mark texture as dirty
    _texData->texture( _textureName ).setDirty();

    emit updateView();

  } else {
    // global texture

    _texData->texture( _textureName ).setDirty();

    Texture& globalTexture = _texData->texture(_textureName);

    // check if the local textures need to be updated
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it){

      TextureData* texData = dynamic_cast< TextureData* > ( o_it->objectData(TEXTUREDATA) );

      if ( texData != 0 && texData->textureExists(_textureName) ){

        //overwrite local parameters
        Texture& localTexture = texData->texture(_textureName);

        bool changed = false;

        if (localTexture.parameters.clamp != globalTexture.parameters.clamp){
          localTexture.parameters.clamp = globalTexture.parameters.clamp;
          changed = true;
        }
        if (localTexture.parameters.clamp_max != globalTexture.parameters.clamp_max){
          localTexture.parameters.clamp_max = globalTexture.parameters.clamp_max;
          changed = true;
        }
        if (localTexture.parameters.clamp_min != globalTexture.parameters.clamp_min){
          localTexture.parameters.clamp_min = globalTexture.parameters.clamp_min;
          changed = true;
        }
        if (localTexture.parameters.max_val != globalTexture.parameters.max_val){
          localTexture.parameters.max_val = globalTexture.parameters.max_val;
          changed = true;
        }
        if (localTexture.parameters.repeat != globalTexture.parameters.repeat){
          localTexture.parameters.repeat = globalTexture.parameters.repeat;
          changed = true;
        }
        if (localTexture.parameters.center != globalTexture.parameters.center){
          localTexture.parameters.center = globalTexture.parameters.center;
          changed = true;
        }
        if (localTexture.parameters.scale != globalTexture.parameters.scale){
          localTexture.parameters.scale = globalTexture.parameters.scale;
          changed = true;
        }

        // update if something has changed
        if ( changed ){
          if ( texData->isEnabled(_textureName) )
            slotTextureUpdated( _textureName  , o_it->id() );
          else
            texData->texture( _textureName ).setDirty();
        }
      }
    }
  }
}

void TextureControlPlugin::slotTextureMenu(QAction* _action) {
  // call existing function to switch the texture
  slotSwitchTexture( _action->text() );
}

void TextureControlPlugin::doSwitchTexture( QString _textureName , int _id ) {

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    std::cerr << "Object has no texture data" << std::endl;
    return;
  }

  // ================================================================================
  // Check for requested Texture
  // ================================================================================
  if ( !texData->textureExists(_textureName) ) {
    emit log(LOGERR, "Texture not available! " + _textureName );
    return;
  }

  // ================================================================================
  // Enable the given texture exclusively or use multitexture setting
  // ================================================================================
  bool multiTextureMode = ( texData->texture(_textureName).type() == MULTITEXTURE );
  if ( !multiTextureMode ) {
    if ( ! texData->enableTexture( _textureName , true ) ) {
      emit log(LOGERR, "Failed to enabled Texture " + _textureName );
      return;
    }
  } else {
    // get the list of textures for this mode
    QStringList textureList = texData->texture(_textureName).multiTextureList;

    for ( uint i = 0 ; i < texData->textures().size() ; ++i ) {
      if ( textureList.contains( texData->textures()[i].name() ) )
        texData->enableTexture( texData->textures()[i].name() , false );
       else
        texData->disableTexture( texData->textures()[i].name() );

    }

    std::cerr << "Trying to enable multitexture" << std::endl;
  }

  // ================================================================================
  // If texture is flagged dirty, update it ( this jumps to texture updated
  // which will update the visualization )
  // ================================================================================

  if ( !multiTextureMode && texData->texture( _textureName).dirty() ) {

    // TODO: maybe introduce lock to prevent extra redraws if updating all objects
    emit updateTexture( texData->texture( _textureName ).name() , obj->id() );
    return;
  }

  // ================================================================================
  // Update texture map from meshNode and activate it
  // ================================================================================
  if( obj->dataType( DATA_TRIANGLE_MESH ) ){
    if (!multiTextureMode) {
      doUpdateTexture(texData->texture(_textureName), *PluginFunctions::triMeshObject(obj)->mesh());
      PluginFunctions::triMeshObject(obj)->textureNode()->activateTexture( texData->texture( _textureName ).glName() );
      PluginFunctions::triMeshObject(obj)->meshNode()->set_index_property_name("No Texture Index");
      PluginFunctions::triMeshObject(obj)->meshNode()->set_texture_map( 0 );
      PluginFunctions::triMeshObject(obj)->meshNode()->set_property_map( 0 );
    } else {
      PluginFunctions::triMeshObject(obj)->meshNode()->set_index_property_name( texData->texture( _textureName ).indexMappingProperty().toStdString() );
      PluginFunctions::triMeshObject(obj)->meshNode()->set_texture_map( texData->textureMap() );
      PluginFunctions::triMeshObject(obj)->meshNode()->set_property_map( 0 );
    }

  }

  if ( obj->dataType( DATA_POLY_MESH ) ){
    if (!multiTextureMode) {
      doUpdateTexture(texData->texture(_textureName), *PluginFunctions::polyMeshObject(obj)->mesh());
      PluginFunctions::polyMeshObject(obj)->textureNode()->activateTexture( texData->texture( _textureName ).glName() );
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_index_property_name("No Texture Index");
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_texture_map( 0 );
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_property_map( 0 );
    } else {
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_index_property_name( texData->texture( _textureName ).indexMappingProperty().toStdString() );
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_texture_map( texData->textureMap() );
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_property_map( 0 );
    }
  }

  // ================================================================================
  // Switch to a texture drawMode
  // ================================================================================
  switchDrawMode(texData->texture( _textureName ).type());

}

void TextureControlPlugin::switchDrawMode( TextureType _type ) {

  bool textureMode = false;
  for ( int j = 0 ; j < PluginFunctions::viewers() ; ++j ) {
    switch (_type) {
      case MULTITEXTURE:
      case HALFEDGEBASED:
        textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE );
        textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE_SHADED );
        break;
      case VERTEXBASED:
        textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED );
        textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
        break;
      case UNSET:
        emit log(LOGERR,"Switching drawmode for unknonw Texture Type!");
        break;
    }
  }

  if ( !textureMode ) {
    switch (_type) {
      case MULTITEXTURE:
      case HALFEDGEBASED:
        PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE_SHADED );
        break;
      case VERTEXBASED:
        PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
        break;
      case UNSET:
        emit log(LOGERR,"Switching drawmode for unknonw Texture Type!");
        break;
    }
  }

  emit updateView();
}

void TextureControlPlugin::slotSwitchTexture( QString _textureName , int _id ) {

  doSwitchTexture(_textureName, _id);
}

void TextureControlPlugin::slotSwitchTexture( QString _textureName ) {

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd();
                                        ++o_it)

    doSwitchTexture(_textureName, o_it->id() );

}


void TextureControlPlugin::slotUpdateContextMenu( int _objectId ) {

  // ================================================================================
  // Get picking object object
  // ================================================================================
  if ( _objectId == -1 )
    return;

  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _objectId , obj ) ) {
    emit log(LOGERR,"slotUpdateContextMenu: Unable to get Object for id " + QString::number(_objectId) );
    return;
  }

  // ================================================================================
  // Get objects texture data and verify that texture exists
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    std::cerr << "TextureControlPlugin::slotUpdateContextMenu: Texture data not found!" << std::endl;
    return;
  }

  // ================================================================================
  // Prepare Texture menu
  // ================================================================================
  contextMenu_->clear();
  QActionGroup* actionGroup = new QActionGroup(0);
  actionGroup->setExclusive( true );
  connect( actionGroup, SIGNAL( triggered( QAction * ) ),
          this, SLOT( slotTextureContextMenu( QAction * ) ) );

  QAction* action = actionGroup->addAction( "Texture Settings" );

  contextMenu_->addAction( action );
  contextMenu_->addSeparator();

  for ( uint i = 0 ; i < texData->textures().size() ; ++i ) {

    if ( texData->textures()[i].hidden() )
      continue;

    action = actionGroup->addAction( texData->textures()[i].name() );

    action->setCheckable(true);

    if ( texData->textures()[i].enabled() )
      action->setChecked(true);

    contextMenu_->addAction( action );
  }
}


void TextureControlPlugin::slotTextureContextMenu( QAction * _action ) {

  // id of object for which the context menu is created, is stored in the action
  QVariant idVariant = _action->data( );
  int id = idVariant.toInt();

  if (_action->text() == "Texture Settings"){

    BaseObjectData* obj;
    if (! PluginFunctions::getObject(  id , obj ) ) {
      emit log(LOGERR,"Unable to get Object for id " + QString::number(id) );
      return;
    }

    TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );

    if (texData == 0){

      QMessageBox msgBox;
      msgBox.setText("Cannot show Properties. No Textures available!");
      msgBox.exec();
      return;

    } else {
      settingsDialog_->show( texData, id, obj->name() );
    }

  } else {

    slotSwitchTexture( _action->text() , id );
  }

}

//compute histogram for the given texture property
void TextureControlPlugin::getCoordinates1D(QString _textureName, int _id, std::vector< double >& _x ){

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    std::cerr << "Object has no texture data" << std::endl;
    return;
  }

  // ================================================================================
  // Check for requested Texture
  // ================================================================================
  if ( !texData->textureExists(_textureName) ) {
    emit log(LOGERR, "Texture not available! " + _textureName );
    return;
  }

  OpenMesh::VPropHandleT< double > coordProp;

  _x.clear();

  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMesh* mesh = PluginFunctions::triMesh(obj);

    if ( !mesh->get_property_handle(coordProp, _textureName.toStdString() ) ){
      std::cerr << "Texture Property not found: " << _textureName.toStdString() << std::endl;
      return;
    }

    for ( TriMesh::VertexIter v_it = mesh->vertices_begin() ; v_it != mesh->vertices_end(); ++v_it)
      _x.push_back( mesh->property(coordProp,v_it) );

  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);

    if ( !mesh->get_property_handle(coordProp, _textureName.toStdString() ) ){
      std::cerr << "Texture Property not found: " << _textureName.toStdString() << std::endl;
      return;
    }

    for ( PolyMesh::VertexIter v_it = mesh->vertices_begin() ; v_it != mesh->vertices_end(); ++v_it)
      _x.push_back( mesh->property(coordProp,v_it) );
  }
}

Q_EXPORT_PLUGIN2( texturecontrolplugin , TextureControlPlugin );

