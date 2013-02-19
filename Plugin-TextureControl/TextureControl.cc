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





#include <QtGui>

#include "TextureControl.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh"
#include "OpenFlipper/common/GlobalOptions.hh"
#include "ImageStorage.hh"

#define TEXTUREDATA "TextureData"
 


TextureControlPlugin::TextureControlPlugin() :
settingsDialog_(0),
textureMenu_(0),
actionGroup_(0),
contextMenu_(0)
{

}

void TextureControlPlugin::slotTextureAdded( QString _textureName , QString _filename , uint _dimension , int _id)
{
  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotTextureAdded: Unable to get Object for id " + QString::number(_id) );
    return;
  }

  // Get Texture data for this object or create one if it does not exist
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    texData = new TextureData();
    obj->setObjectData(TEXTUREDATA, texData);
  }

  if ( texData->textureExists(_textureName) ) {
    emit log(LOGERR,"slotTextureAdded: Trying to add already existing texture " + _textureName + " for object " + QString::number(_id) );
    return;
  }

  // ================================================================================
  // Get the image file
  // ================================================================================

  // Add Image to the image store and set the index in the texture description
  int newId = imageStore().addImageFile(_filename);

  if ( newId == -1 ) {
    emit log(LOGERR,imageStore().error());
    return;
  }

  // ================================================================================
  // Add the texture to the texture node and get the corresponding id
  // ================================================================================
  GLuint glName = 0;

  //inform textureNode about the new texture
  if( obj->dataType( DATA_TRIANGLE_MESH ) )
    glName = PluginFunctions::triMeshObject(obj)->textureNode()->add_texture(imageStore().getImage(newId,0));

  if ( obj->dataType( DATA_POLY_MESH ) )
    glName = PluginFunctions::polyMeshObject(obj)->textureNode()->add_texture(imageStore().getImage(newId,0));

  // ================================================================================
  // Store texture information in objects metadata
  // ================================================================================

  if (glName == 0) {
    emit log(LOGERR,"slotTextureAdded: Unable to bind texture!");
    return;
  }

  texData->addTexture(_textureName,_filename,_dimension,glName);

  // Remember id in texture descriptor
  texData->setImage(_textureName,newId);

  texData->texture(_textureName).disable();
}

void TextureControlPlugin::slotTextureAdded( QString _textureName , QString _filename , uint _dimension)
{
  // Add this texture to the list of global textures
  if ( ! globalTextures_.textureExists(_textureName) ) {
    globalTextures_.addTexture(_textureName,_filename,_dimension,0);
    globalTextures_.texture(_textureName).disable();

    int newImageId = imageStore().addImageFile(_filename);

    if ( newImageId == -1 ) {
      emit log(LOGERR,imageStore().error());
      return;
    }

    globalTextures_.texture(_textureName).textureImageId(newImageId);

  } else {
    emit log(LOGERR,"slotTextureAdded: Trying to add already existing global texture " + _textureName );
    return;
  }

  // Add a new entry to the global Texture menu
  QAction* new_texture = new QAction(_textureName, this);
  new_texture->setStatusTip(tr("slotTextureAdded: Switch all objects to this Texture ( if available )"));
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
    emit log(LOGERR,"slotMultiTextureAdded: Unable to get Object for id " + QString::number(_id) );
  }

  // Check if we support this kind of data
  if ( !obj->dataType(DATA_TRIANGLE_MESH) && !obj->dataType(DATA_POLY_MESH) ) {
      emit log(LOGERR,"slotMultiTextureAdded: Trying to add textures to object failed because of unsupported object type");
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
  _textureId = texData->texture(_name).id();

  //hide the texture (its accessible through the multiTexture)
  texData->texture(_name).hidden( true );

  // Add to image store
  int newImageId = imageStore().addImageFile(_filename);

  if ( newImageId == -1 ) {
    emit log(LOGERR,imageStore().error());
    return;
  }

  // Add to texture description
  texData->texture(_name).textureImageId(newImageId);

  // Store the new texture in the list of this textureGroup
  if ( _textureId != -1 ) {
    texData->texture(_textureGroup).multiTextureList << _name ;
  } else {
    emit log(LOGERR,"slotMultiTextureAdded: Error when getting internal id of new multitexture!");
  }

}

void TextureControlPlugin::addedEmptyObject( int _id ) {
  
  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
//     emit log(LOGERR,"addedEmptyObject: Unable to get Object for id " + QString::number(_id) );
    return;
  }
  
  // Check if we support this kind of data
  if ( !obj->dataType(DATA_TRIANGLE_MESH) && !obj->dataType(DATA_POLY_MESH) ) {
    return;
  }
  
  // Get Texture data for this object or create one if it does not exist
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    texData = new TextureData();
    obj->setObjectData(TEXTUREDATA, texData);
  }
  
  // Iterate over all available global textures and add them to the object
  for ( uint i = 0 ; i < globalTextures_.textures().size() ; ++i) {

    // Add to image store
    int newImageId = imageStore().addImageFile(globalTextures_.textures()[i].filename());

    if ( newImageId == -1 ) {
      emit log(LOGERR,imageStore().error());
      continue;
    }

    // ================================================================================
    // Add the texture to the texture node and get the corresponding id
    // ================================================================================
    GLuint glName = 0;
    
    //inform textureNode about the new texture
    if( obj->dataType( DATA_TRIANGLE_MESH ) )
      glName = PluginFunctions::triMeshObject(obj)->textureNode()->add_texture(imageStore().getImage(newImageId,0));
    
    if ( obj->dataType( DATA_POLY_MESH ) )
      glName = PluginFunctions::polyMeshObject(obj)->textureNode()->add_texture(imageStore().getImage(newImageId,0));
    
    // ================================================================================
    // Store texture information in objects metadata
    // ================================================================================
    if (glName != 0) {
      texData->addTexture(globalTextures_.textures()[i], glName);

      // Add to texture description
      texData->setImage(globalTextures_.textures()[i].name(),newImageId);
    }
    else {
      imageStore().removeImage(newImageId);
      emit log(LOGERR,"addedEmptyObject: Unable to bind Texture");
      continue;
    }
    
    
    // ================================================================================
    // Update texture mapping in meshNode
    // ================================================================================
    if( obj->dataType( DATA_TRIANGLE_MESH ) ){
      PluginFunctions::triMeshObject(obj)->meshNode()->setTextureMap( 0 );
    }
    
    if ( obj->dataType( DATA_POLY_MESH ) ){
      PluginFunctions::polyMeshObject(obj)->meshNode()->setTextureMap( 0 );
    }
    
  }
}

template< typename MeshT >
void TextureControlPlugin::handleFileOpenTextures( MeshT*& _mesh , int _objectId ) {

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _objectId , obj ) ) {
    return;
  }

  if ( _mesh->has_vertex_texcoords2D() ){
    slotTextureAdded("Original Per Vertex Texture Coords","unknown.png",2,_objectId);
    slotSetTextureMode("Original Per Vertex Texture Coords","type=vertexbased",_objectId);
  }

  if ( _mesh->has_halfedge_texcoords2D() ){
    slotTextureAdded("Original Per Face Texture Coords","unknown.png",2,_objectId);
    slotSetTextureMode("Original Per Face Texture Coords","type=halfedgebased",_objectId);
  }

}

void TextureControlPlugin::fileOpened( int _id ) {
  // TODO:: Store original texture coords in a new property!

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"fileOpened: Unable to get Object for id " + QString::number(_id) );
    return;
  }

  // Check if we support this kind of data
  if ( !obj->dataType(DATA_TRIANGLE_MESH) && !obj->dataType(DATA_POLY_MESH) ) {
      return;
  }

  // Get Texture data for this object or create one if it does not exist
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    emit log(LOGERR,tr("fileOpened: Unable to get texture object data for id %1.").arg(_id) );
    return;
  }

  // Check if the file contains a texture map, store original textures and handle them before adding global textures
  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMesh* mesh = PluginFunctions::triMesh(obj);
    if ( mesh )
      handleFileOpenTextures(mesh,_id);
  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);
    if ( mesh )
      handleFileOpenTextures(mesh,_id);
  }

}

void TextureControlPlugin::slotTextureChangeImage( QString _textureName , QImage& _image , int _id ) {

  // ================================================================================
  // Get the new object
  // ================================================================================
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotTextureChangeImage: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for this object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );

  if ( texData == 0 || ( !texData->textureExists(_textureName))  ) {
    emit log(LOGERR,"slotTextureChangeImage: Texture does not exist: " + _textureName + " (objectid=" + QString::number(_id) + ")");
    return;
  }

  // ================================================================================
  // Update the image
  // ================================================================================
  Texture& texture = texData->texture(_textureName);

  // Add to image store
  int newImageId = imageStore().addImage(_image);

  // Add to texture description
  texture.textureImageId(newImageId);

  // ================================================================================
  // Flag dirty or update
  // ================================================================================

  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
    PluginFunctions::triMeshObject(obj)->textureNode()->set_texture( _image , texData->texture(_textureName).glName());
  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PluginFunctions::triMeshObject(obj)->textureNode()->set_texture( _image , texData->texture(_textureName).glName());
  }

  emit updateView();

}

void TextureControlPlugin::slotTextureChangeImage( QString _textureName , QImage& _image ) {

  // ================================================================================
  // Update texture Image for global textures
  // ================================================================================
  if ( ! globalTextures_.textureExists(_textureName) ) {
    emit log(LOGERR,"slotTextureChangeImage: Global texture does not exist: " + _textureName);
    return;
  }

  // ================================================================================
  // Update the image in the global texture
  // ================================================================================
  Texture& texture = globalTextures_.texture(_textureName);

  // Add to image store
  int newImageId = imageStore().addImage(_image);

  // Add to texture description
  texture.textureImageId(newImageId);

  // ================================================================================
  // check if the local textures need to be updated
  // ================================================================================
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it){

    TextureData* texData = dynamic_cast< TextureData* > ( o_it->objectData(TEXTUREDATA) );
    if (texData != 0)
      if ( texData->textureExists(_textureName) ){
        Texture& localTex = texData->texture(_textureName);
        localTex.textureImageId(newImageId);

          if( o_it->dataType( DATA_TRIANGLE_MESH ) ) {
            PluginFunctions::triMeshObject(o_it)->textureNode()->set_texture( _image , texData->texture(_textureName).glName());
          } else if ( o_it->dataType( DATA_POLY_MESH ) ) {
            PluginFunctions::triMeshObject(o_it)->textureNode()->set_texture( _image , texData->texture(_textureName).glName());
          }
      }
  }

  emit updateView();

}

void TextureControlPlugin::slotTextureGetImage( QString _textureName, QImage& _image, int _id ){
  
  // Get the object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotTextureGetImage: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    emit log(LOGERR, tr("slotTextureGetImage: Object has no texture data! Object: %1").arg(_id) );
    return;
  }

  // ================================================================================
  // Check for requested Texture
  // ================================================================================
  if ( !texData->textureExists(_textureName) ) {
    emit log(LOGERR, "slotTextureGetImage: Texture not available! " + _textureName );
    return;
  }

  if ( texData->texture(_textureName).type() == MULTITEXTURE )
    _image = QImage();
  else
    _image = imageStore().getImage(texData->texture(_textureName).textureImageId(),0 );
}


void TextureControlPlugin::slotTextureGetImage( QString _textureName, QImage& _image ){
  
  if ( ! globalTextures_.textureExists(_textureName) ) {
    emit log(LOGERR,"slotTextureGetImage: Global texture does not exist: " + _textureName);
    return;
  }

  if ( globalTextures_.texture(_textureName).type() == MULTITEXTURE )
    _image = QImage();
  else
    _image = imageStore().getImage(globalTextures_.texture(_textureName).textureImageId(),0);
}

void TextureControlPlugin::slotTextureIndex( QString _textureName, int _id, int& _index){
  
  // Get the object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotTextureIndex: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    emit log(LOGERR, tr("slotTextureIndex: Object has no texture data! Object: %1").arg(_id) );
    return;
  }

  // ================================================================================
  // Check for requested Texture
  // ================================================================================
  if ( !texData->textureExists(_textureName) ) {
    emit log(LOGERR, "slotTextureIndex: Texture not available! " + _textureName );
    return;
  }

  _index = texData->texture(_textureName).id();
}

void TextureControlPlugin::slotTextureIndexPropertyName(int _id, QString& _propertyName) {
    
    // Get the object
    BaseObjectData* obj;
    if (! PluginFunctions::getObject(  _id , obj ) ) {
        emit log(LOGERR,"slotTextureIndexPropertyName: Unable to get Object for id " + QString::number(_id) );
        return;
    }
    
    // Get texture index property name
    if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
        _propertyName = PluginFunctions::triMeshObject(obj)->meshNode()->indexPropertyName().c_str();
    } else if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
        _propertyName = PluginFunctions::polyMeshObject(obj)->meshNode()->indexPropertyName().c_str();
    } else {
        emit log(LOGERR,"slotTextureIndexPropertyName: Unable to access mesh for object with id " + QString::number(_id) );
    }
}

void TextureControlPlugin::slotTextureName( int _id, int _textureIndex, QString& _textureName){
  
  // Get the object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotTextureName: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    emit log(LOGERR, tr("slotTextureName: Object has no texture data! Object: %1").arg(_id) );
    return;
  }

  for (uint i=0; i < texData->textures().size(); i++ )
    if ( (texData->textures()[i]).id() == _textureIndex ){
      _textureName = (texData->textures()[i]).name();
      return;
    }

  emit log(LOGERR, "slotTextureName: TextureIndex not available! (" + QString::number(_textureIndex) + ")" );
  _textureName = "NOT_FOUND";
  return;
}

void TextureControlPlugin::slotTextureFilename( int _id, QString _textureName, QString& _textureFilename){
  
  // Get the object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotTextureFilename: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    emit log(LOGERR, tr("slotTextureFilename: Object has no texture data! Object: %1").arg(_id) );
    return;
  }

  // Search in local textures
  for (uint i=0; i < texData->textures().size(); i++ ) {
      for (int j=0; j < texData->textures()[i].multiTextureList.size(); j++ ) {
          if ( (texData->textures()[i]).name() == _textureName ){
              Texture& tex = texData->texture((texData->textures()[i]).name());
              _textureFilename = tex.filename();
              return;
          } else if ( (texData->textures()[i]).multiTextureList[j] == _textureName ){
              Texture& tex = texData->texture((texData->textures()[i]).multiTextureList[j]);
              _textureFilename = tex.filename();
              return;
          }
      }
  }
  
  _textureFilename = OpenFlipper::Options::textureDir().path() + 
      QDir::separator().toAscii() + (globalTextures_.texture(_textureName)).filename();
  
  QFile f(_textureFilename);
  if(!f.exists()) _textureFilename = "NOT_FOUND";
  
  return;
}

void TextureControlPlugin::slotGetCurrentTexture( int _id, QString& _textureName ){
  
  _textureName = "NONE";
  
  // Get the object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotGetCurrentTexture: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    #ifndef NDEBUG
    
      // Iterate over all per Object datas and output them
      QMap<QString, PerObjectData*>::const_iterator mapIter = obj->getPerObjectDataMap().begin();  
      while ( mapIter != obj->getPerObjectDataMap().end() ) {
        ++mapIter;
      }
    #endif
    
    return;
  }
  
  // Iterate over all available textures
  for ( uint i = 0 ; i < texData->textures().size() ; ++i) {
  
    if ( (texData->textures()[i]).enabled() ){
      _textureName = (texData->textures()[i]).name();
      
      if ( (texData->textures()[i]).type() == MULTITEXTURE ) {
        return;
      }
    }
  }
}

void TextureControlPlugin::slotGetSubTextures( int _id, QString _multiTextureName, QStringList& _subTextures ){
  // Get the object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"slotGetSubTextures: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    emit log(LOGERR, tr("slotGetSubTextures: Object has no texture data! Object: %1").arg(_id) );
    return;
  }

  // ================================================================================
  // Check for requested Texture
  // ================================================================================
  if ( !texData->textureExists(_multiTextureName) ) {
    emit log(LOGERR, "slotGetSubTextures: Texture not available! " + _multiTextureName );
    return;
  }
  
  if ( texData->texture(_multiTextureName).type() == MULTITEXTURE )
    _subTextures = texData->texture(_multiTextureName).multiTextureList;
  else
    _subTextures = QStringList();
}

void TextureControlPlugin::slotTextureUpdated( QString _textureName , int _identifier ){

  // ================================================================================
  // Get updated object
  // ================================================================================
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _identifier , obj ) ) {
    emit log(LOGERR,"slotTextureUpdated: Unable to get Object for id " + QString::number(_identifier) );
    return;
  }

  //skip object if its not a mesh
  if( !obj->dataType( DATA_TRIANGLE_MESH ) && !obj->dataType( DATA_POLY_MESH ) )
    return;

  // ================================================================================
  // Get objects texture data and verify that texture exists
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    emit log(LOGERR,tr("slotTextureUpdated: Texture data not found: Object %1" ).arg(_identifier) );
    return;
  }

  // ================================================================================
  // Check if texture exists
  // ================================================================================
  if ( ! texData->textureExists(_textureName) ) {
    emit log(LOGERR,"slotTextureUpdated: Texture " + _textureName + " not found on object " + QString::number(_identifier) );
    return;
  }

  // ================================================================================
  // If texture is not enabled, mark it as dirty and defer update to visualization update
  // ================================================================================
  if ( ! texData->texture(_textureName).enabled() ) {
    texData->texture(_textureName).setDirty();
    return;
  }

  // ================================================================================
  // Enable the texture in texture node
  // ================================================================================
  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMesh* mesh = PluginFunctions::triMesh(obj);
    doUpdateTexture(texData->texture(_textureName), *mesh);
    // Texture has been bound to that object by slotAddTexture.. directly or by fileOpened from global texture
    // Just activate it
    PluginFunctions::triMeshObject(obj)->textureNode()->activateTexture(texData->texture(_textureName).glName() );
    PluginFunctions::triMeshObject(obj)->textureNode()->set_repeat(texData->texture(_textureName).parameters.repeat);
  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);
    doUpdateTexture(texData->texture(_textureName), *mesh);
    // Texture has been bound to that object by slotAddTexture.. directly or by fileOpened from global texture
    // Just activate it
    PluginFunctions::polyMeshObject(obj)->textureNode()->activateTexture(texData->texture(_textureName).glName() );
    PluginFunctions::polyMeshObject(obj)->textureNode()->set_repeat(texData->texture(_textureName).parameters.repeat);
  }

  // ================================================================================
  // Mark texture as not dirty
  // ================================================================================
  texData->texture(_textureName).clean();

  // ================================================================================
  // Tell plugins to update texture
  // ================================================================================
  emit updatedObject(obj->id(),UPDATE_TEXTURE);

}

void TextureControlPlugin::slotUpdateTexture( QString _textureName , int _identifier) {
  if ( _textureName == "Reflection Lines" )
    slotTextureUpdated( _textureName , _identifier );

}

template< typename MeshT >
void TextureControlPlugin::doUpdateTexture ( Texture& _texture, MeshT& _mesh )
{

  if ( _texture.type() == HALFEDGEBASED ) {
    if (_texture.dimension() == 1) {

      OpenMesh::HPropHandleT< double > texture;
      if ( ! _mesh.get_property_handle(texture, _texture.name().toStdString() ) ) {
        emit log(LOGERR,tr("doUpdateTexture: HALFEDGEBASED dimension 1: Unable to get property %1").arg(_texture.name()) );
        return;
      }

      copyTexture(_texture, _mesh, texture);

    } else if ( _texture.dimension() == 2 ) {

      OpenMesh::HPropHandleT< OpenMesh::Vec2d > texture2D;
      if ( ! _mesh.get_property_handle( texture2D, _texture.name().toStdString() ) ) {
        emit log(LOGERR,tr("doUpdateTexture: HALFEDGEBASED dimension 2: Unable to get property %1").arg(_texture.name()) );
        return;
      }

      copyTexture( _texture, _mesh, texture2D);

    } else
      emit log(LOGERR, "doUpdateTexture: Unsupported Texture Dimension " + QString::number(_texture.dimension() ) );
  } else if ( _texture.type() == VERTEXBASED ) {
    if ( _texture.dimension() == 1 ) {

      OpenMesh::VPropHandleT< double > texture;
      if ( ! _mesh.get_property_handle(texture,_texture.name().toStdString() ) ) {
        emit log(LOGERR,tr("doUpdateTexture: VERTEXBASED dimension 1: Unable to get property %1").arg(_texture.name()) );
        return;
      }

        copyTexture(_texture, _mesh, texture);

      } else if ( _texture.dimension() == 2 ) {

        OpenMesh::VPropHandleT< OpenMesh::Vec2d >  texture2D;
        if ( ! _mesh.get_property_handle(texture2D,_texture.name().toStdString() ) ) {
          emit log(LOGERR,tr("doUpdateTexture: VERTEXBASED dimension 2: Unable to get property %1").arg(_texture.name()) );
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
        emit log(LOGERR, "doUpdateTexture: Unsupported Texture Dimension " + QString::number(_texture.dimension() ) );

    } else
      emit log(LOGERR, "doUpdateTexture: Unsupported Texture type");

}

void TextureControlPlugin::slotDrawModeChanged(int _viewerId ) {

  // Only update if we have a relevant draw mode
  if (! ( ( PluginFunctions::drawMode(_viewerId) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED ) ||
          ( PluginFunctions::drawMode(_viewerId) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED) || 
          ( PluginFunctions::drawMode(_viewerId) == ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE) ||
          ( PluginFunctions::drawMode(_viewerId) == ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE_SHADED) )) {
    return;
  }

  // Iterate over all Objects
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                         o_it != PluginFunctions::objectsEnd();
                                         ++o_it) {

    // Get the corresponding texture data
    TextureData* texData = dynamic_cast< TextureData* > ( o_it->objectData(TEXTUREDATA) );
    if (texData == 0){
      continue;
    }

    // Go over all textures and if one of them is enabled and dirty, update it here
    for ( uint i = 0; i < texData->textures().size(); ++i ) {
      if ( texData->textures()[i].enabled() && texData->textures()[i].dirty() ) {
        emit updateTexture( texData->textures()[i].name() , o_it->id() );
      }
    }


  }
  
  emit updateView();

}

void TextureControlPlugin::slotObjectUpdated(int _identifier, const UpdateType& _type)
{  
    if( !_type.contains(UPDATE_ALL) && !_type.contains(UPDATE_GEOMETRY) && !_type.contains(UPDATE_TOPOLOGY) )
        return;
    
  // ================================================================================
  // Get updated object
  // ================================================================================
  if ( _identifier == -1 )
    return;

  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _identifier , obj ) )
    return;

  //skip object if its not a mesh
  if( !obj->dataType( DATA_TRIANGLE_MESH ) && !obj->dataType( DATA_POLY_MESH ) )
    return;

  // ================================================================================
  // Get objects texture data and verify that texture exists
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
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
       update |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE );
       update |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE_SHADED );
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

bool TextureControlPlugin::parseMode( QString _mode, Texture& _texture ) {

  bool changed = false;

  int i = 0;
  QString nextString = _mode.section(',',i,i);
  while ( nextString != "" ) {
    QString sectionName = nextString.section('=',0,0);
    QString value = nextString.section('=',1,1);

    // Cleanup representation
    value       = value.trimmed();
    sectionName = sectionName.trimmed();
    sectionName = sectionName.toLower();

    if ( sectionName == "clamp" ) {
      if ( StringToBool(value) != _texture.parameters.clamp ) {
        _texture.parameters.clamp = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "clamp_max" ) {
      if (value.toDouble() != _texture.parameters.clampMax){
        _texture.parameters.clampMax = value.toDouble();
        changed = true;
      }
    } else if ( sectionName == "clamp_min" ) {
      if (value.toDouble() != _texture.parameters.clampMin){
        _texture.parameters.clampMin = value.toDouble();
        changed = true;
      }
    } else if ( sectionName == "max_val" ) {
      if (value.toDouble() != _texture.parameters.repeatMax){
        _texture.parameters.repeatMax = value.toDouble();
        changed = true;
      }
    } else if ( sectionName == "min_val" ) {
      if (value.toDouble() != _texture.parameters.repeatMin){
        _texture.parameters.repeatMin = value.toDouble();
        changed = true;
      }
    } else if ( sectionName == "repeat" ) {
      if ( StringToBool(value) != _texture.parameters.repeat ) {
        _texture.parameters.repeat = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "center" ) {
      if ( StringToBool(value) != _texture.parameters.center ) {
        _texture.parameters.center = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "scale" ) {
      if ( StringToBool(value) != _texture.parameters.scale ) {
        _texture.parameters.scale = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "abs" ) {
      if ( StringToBool(value) != _texture.parameters.abs ) {
        _texture.parameters.abs = StringToBool(value);
        changed = true;
      }
    } else if ( sectionName == "indexproperty" ) {
      if ( value != _texture.indexMappingProperty() ) {
        _texture.indexMappingProperty( value );
        changed = true;
      }
    } else if ( sectionName == "visiblename" ) {
      if ( value != _texture.visibleName() ) {
        _texture.visibleName( value );
        changed = true;
      }
    } else if ( sectionName == "type" ) {
        if ( ( value == "halfedgebased" ) && ( _texture.type() != HALFEDGEBASED ) ) {
          _texture.type( HALFEDGEBASED );
          changed = true;
        } else if ( (value == "vertexbased") && (_texture.type() != HALFEDGEBASED)  ) {
          _texture.type( VERTEXBASED );
          changed = true;
        }  else if ( (value == "environmentmap") && (_texture.type() != ENVIRONMENT)  ) {
          _texture.type( ENVIRONMENT );
          changed = true;
        } else {
          emit log(LOGERR,"parseMode: Unknown texture type : " + value + " for texture: " + _texture.name() );
        }
    } else
      emit log(LOGERR,"parseMode: Unknown texture mode : " + sectionName);

    ++i;
    nextString = _mode.section(',',i,i);
  }

  return changed;
}

void TextureControlPlugin::slotSetTextureMode(QString _textureName ,QString _mode) {

  // ================================================================================
  // Update texture settings for global textures
  // ================================================================================
  if ( ! globalTextures_.textureExists(_textureName) ) {
    emit log(LOGERR,"slotSetTextureMode: Global texture does not exist: " + _textureName);
    return;
  }

  // ================================================================================
  // Parse parameters and update them in the texture data
  // ================================================================================
  Texture& texture = globalTextures_.texture(_textureName);

  // Parse the mode settings
  parseMode(_mode,texture);
  _mode = _mode.toLower();

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

        if ( _mode.contains("clamp_max") && (texture.parameters.clampMax != localTex.parameters.clampMax) ){
          localTex.parameters.clampMax = texture.parameters.clampMax;
          changed = true;
        }

        if ( _mode.contains("clamp_min") && (texture.parameters.clampMin != localTex.parameters.clampMin) ){
          localTex.parameters.clampMin = texture.parameters.clampMin;
          changed = true;
        }

        if ( _mode.contains("max_val") && (texture.parameters.repeatMax != localTex.parameters.repeatMax) ){
          localTex.parameters.repeatMax = texture.parameters.repeatMax;
          changed = true;
        }
        
        if ( _mode.contains("min_val") && (texture.parameters.repeatMin != localTex.parameters.repeatMin) ){
          localTex.parameters.repeatMin = texture.parameters.repeatMin;
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

        if ( _mode.contains("visiblename") && (texture.visibleName() != localTex.visibleName() ) ){
          localTex.visibleName( texture.visibleName() );
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
    emit log(LOGERR,"slotSetTextureMode: Unable to get Object for id " + QString::number(_id) );
  }

  // Get Texture data for this object
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );

  if ( texData == 0 || ( !texData->textureExists(_textureName))  ) {
    emit log(LOGERR,"slotSetTextureMode: Texture does not exist: " + _textureName + " (object=" + QString::number(_id) + ")");
    return;
  }

  // ================================================================================
  // Parse parameters and update them in the texture data
  // ================================================================================
  Texture& texture = texData->texture(_textureName);


  bool changed = parseMode(_mode,texture);

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


  slotTextureAdded("Reflection Lines","reflection_map.png",2);
  slotSetTextureMode("Reflection Lines","type=environmentmap");
}

void TextureControlPlugin::slotSetTextureProperties() {

  settingsDialog_->show( &globalTextures_, -1);
}

void TextureControlPlugin::applyDialogSettings(TextureData* _texData, QString _textureName, int _id) {

  if (_id != -1)
  {
    //local texture

    // Get the object
    BaseObjectData* obj;
    if (! PluginFunctions::getObject(  _id , obj ) ) {
      emit log(LOGERR,"applyDialogSettings: Unable to get Object for id " + QString::number(_id) );
    }

    if ( !_texData->textureExists(_textureName) ) {
      emit log(LOGERR,"applyDialogSettings: Texture does not exist in applyDialogSettings " + _textureName );
    }

    Texture& texture = _texData->texture(_textureName );

    if( obj->dataType( DATA_TRIANGLE_MESH ) ){
      PluginFunctions::triMeshObject(obj)->textureNode()->set_texture(imageStore().getImage(texture.textureImageId(),0) , texture.glName() );
    } else  if ( obj->dataType( DATA_POLY_MESH ) ) {
      PluginFunctions::polyMeshObject(obj)->textureNode()->set_texture(imageStore().getImage(texture.textureImageId(),0) , texture.glName() );
    }

    // Always mark texture as dirty
    _texData->texture( _textureName ).setDirty();


    //switch back to the active texture to force rebind
    for ( uint i=0; i < _texData->textures().size(); i++ )
      if ( _texData->textures()[i].enabled() ){
        doSwitchTexture( _texData->textures()[i].name(), _id);
        break;
      }

    emit updateView();

  } 
  else 
  {
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
        if (localTexture.parameters.clampMax != globalTexture.parameters.clampMax){
          localTexture.parameters.clampMax = globalTexture.parameters.clampMax;
          changed = true;
        }
        if (localTexture.parameters.clampMin != globalTexture.parameters.clampMin){
          localTexture.parameters.clampMin = globalTexture.parameters.clampMin;
          changed = true;
        }
        if (localTexture.parameters.repeatMax != globalTexture.parameters.repeatMax){
          localTexture.parameters.repeatMax = globalTexture.parameters.repeatMax;
          changed = true;
        }
        if (localTexture.parameters.repeatMin != globalTexture.parameters.repeatMin){
          localTexture.parameters.repeatMin = globalTexture.parameters.repeatMin;
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

  // Switch to the corresponding draw mode
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd();
                                        ++o_it) {

    TextureData* texData = dynamic_cast< TextureData* > ( o_it->objectData(TEXTUREDATA) );

    if (texData != 0) {
      switchDrawMode(texData->texture(_action->text()).type());
    }

  }
}

void TextureControlPlugin::doSwitchTexture( QString _textureName , int _id ) {

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"doSwitchTexture: Unable to get Object for id " + QString::number(_id) );
  }
  
  // Ignore light sources: ( TODO: Maybe use better detetion if textures are usefull )
  if ( obj->dataType(typeId("Light")) )
   return;

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    emit log(LOGERR, tr("doSwitchTexture: Object has no texture data! Object: %1").arg(_id) );
    return;
  }

  // ================================================================================
  // Check for requested Texture
  // ================================================================================
  if ( !texData->textureExists(_textureName) ) {
    emit log(LOGERR, "doSwitchTexture: Texture not available! " + _textureName );
    return;
  }

  // ================================================================================
  // Check on texture types
  // ================================================================================
  QStringList textureList;
  switch (texData->texture(_textureName).type()) {
    // Handle MultiTextures first
    // Enable all textures of a multitexture block and disable all others
    case MULTITEXTURE:
      // get the list of textures for this mode
      textureList = texData->texture(_textureName).multiTextureList;

      texData->enableTexture(_textureName, true);

      for ( uint i = 0 ; i < texData->textures().size() ; ++i ) {
        if ( textureList.contains( texData->textures()[i].name() ) || (texData->textures()[i].name() == _textureName) )
          texData->enableTexture( texData->textures()[i].name() , false );
        else
          texData->disableTexture( texData->textures()[i].name() );
      }
      break;
    // These textures are working on mesh properties and need to be copied on access.
    // The actual drawing is performed if the plugin belonging to the texture updated it.
    // otherwise it will be directly enabled by this function
    case HALFEDGEBASED:
    case VERTEXBASED:
      // Enable the texture
      if ( ! texData->enableTexture( _textureName , true ) )
        emit log(LOGERR, "doSwitchTexture: Failed to enabled VERTEXBASED or HALFEDGEBASED Texture " + _textureName );

      // Check if dirty. If dirty, force plugin to update the texture otherwise we will copy it to our buffers and render it
      if ( texData->texture( _textureName).dirty() ) {
        // TODO: maybe introduce lock to prevent extra redraws if updating all objects
        emit updateTexture( texData->texture( _textureName ).name() , obj->id() );
      } else {
        // Copy the texture data to the global one
        if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
          doUpdateTexture(texData->texture(_textureName), *PluginFunctions::triMeshObject(obj)->mesh());
        } else if( obj->dataType( DATA_POLY_MESH ) ) {
          doUpdateTexture(texData->texture(_textureName), *PluginFunctions::polyMeshObject(obj)->mesh());
        } else {
          emit log(LOGERR, "doSwitchTexture: HALFEDGEBASED or VERTEXBASED type require poly or trimesh to work! Texture: " + _textureName );
        }
      }

      break;
    // Environment textures are static for now so directly enable it without an update
    case ENVIRONMENT:
      if ( ! texData->enableTexture( _textureName , true ) ) {
        emit log(LOGERR, "doSwitchTexture: Failed to enabled ENVIRONMENT Texture " + _textureName );
        return;
      }
      break;
    case UNSET:
      emit log(LOGERR, "doSwitchTexture. Texture Type is unset! This should never happen! " + _textureName );
      break;
  }

  // ================================================================================
  // Update texture mappings and activate the textures
  // ================================================================================
  if ( texData->texture(_textureName).type() == MULTITEXTURE ) {
    if( obj->dataType( DATA_TRIANGLE_MESH ) ){
      // Set the property map for mapping between faces and textures
      PluginFunctions::triMeshObject(obj)->meshNode()->setIndexPropertyName( texData->texture( _textureName).indexMappingProperty().toStdString() );
      // Unbind all textures ( textures will be bound by textureNode later on
      PluginFunctions::triMeshObject(obj)->textureNode()->activateTexture(0);
      // Set the mapping between texture ids in the index property and their gl Names
      PluginFunctions::triMeshObject(obj)->meshNode()->setTextureMap( texData->textureMap() );
      
      if (PluginFunctions::triMeshObject(obj)->mesh()->has_halfedge_texcoords2D())
        PluginFunctions::triMeshObject(obj)->meshNode()->setHalfedgeTextcoordPropertyName("h:texcoords2D");
        
      
    } else if( obj->dataType( DATA_POLY_MESH ) ){
      // Set the property map for mapping between faces and textures
      PluginFunctions::polyMeshObject(obj)->meshNode()->setIndexPropertyName( texData->texture( _textureName).indexMappingProperty().toStdString() );
      // Unbind all textures ( textures will be bound by textureNode later on
      PluginFunctions::polyMeshObject(obj)->textureNode()->activateTexture(0);
      // Set the mapping between texture ids in the index property and their gl Names
      PluginFunctions::polyMeshObject(obj)->meshNode()->setTextureMap( texData->textureMap() );
      
      if (PluginFunctions::polyMeshObject(obj)->mesh()->has_halfedge_texcoords2D())
        PluginFunctions::polyMeshObject(obj)->meshNode()->setHalfedgeTextcoordPropertyName("h:texcoords2D");      
      
    } else {
      emit log(LOGERR, "doSwitchTexture: MultiTexture Error: Only supported on Tri or Poly Mesh for Texture: " + _textureName );
    }
  } else {
    if( obj->dataType( DATA_TRIANGLE_MESH ) ){
      // Activate the requested texture in texture node
      PluginFunctions::triMeshObject(obj)->textureNode()->activateTexture( texData->texture( _textureName ).glName() );
      // Disable the mapping properties ( only for multi texture mode )
      PluginFunctions::triMeshObject(obj)->meshNode()->setIndexPropertyName("No Texture Index");
      PluginFunctions::triMeshObject(obj)->meshNode()->setTextureMap( 0 );
      
      if ( texData->texture(_textureName).type() == HALFEDGEBASED ) {
        // We set it to the standard name here, as we copy user texture coordinates to the global representation
        PluginFunctions::triMeshObject(obj)->meshNode()->setHalfedgeTextcoordPropertyName("h:texcoords2D");      
      } else {
        PluginFunctions::triMeshObject(obj)->meshNode()->setHalfedgeTextcoordPropertyName("No Texture");      
      }
       
    } else if ( obj->dataType( DATA_POLY_MESH ) ){
      // Activate the requested texture in texture node
      PluginFunctions::polyMeshObject(obj)->textureNode()->activateTexture( texData->texture( _textureName ).glName() );
      // Disable the mapping properties ( only for multi texture mode )
      PluginFunctions::polyMeshObject(obj)->meshNode()->setIndexPropertyName("No Texture Index");
      PluginFunctions::polyMeshObject(obj)->meshNode()->setTextureMap( 0 );
      
      
      if ( texData->texture(_textureName).type() == HALFEDGEBASED ) {
        // We set it to the standard name here, as we copy user texture coordinates to the global representation
        PluginFunctions::polyMeshObject(obj)->meshNode()->setHalfedgeTextcoordPropertyName("h:texcoords2D");      
      } else {
        PluginFunctions::polyMeshObject(obj)->meshNode()->setHalfedgeTextcoordPropertyName("No Texture");      
      }
      
    } else {
      emit log(LOGERR, "doSwitchTexture: Texture Error ( mesh required) for Texture: " + _textureName );
    }
  }

  emit updatedObject(obj->id(),UPDATE_TEXTURE);

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
      case ENVIRONMENT:
        textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_ENV_MAPPED );
        break;
      case UNSET:
        emit log(LOGERR,"doSwitchTexture: Switching drawmode for unknown Texture Type!");
        break;
    }
  }


  if ( !textureMode ) {
    switch (_type) {
      case MULTITEXTURE:
      case HALFEDGEBASED:
        PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE_SHADED, PluginFunctions::ALL_VIEWERS );
        break;
      case VERTEXBASED:
        PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED , PluginFunctions::ALL_VIEWERS);
        break;
      case ENVIRONMENT:
        PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_ENV_MAPPED , PluginFunctions::ALL_VIEWERS);
        break;
      case UNSET:
        emit log(LOGERR,"doSwitchTexture: Switching drawmode for unknonw Texture Type!");
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

  //skip object if its not a mesh
  if( !obj->dataType( DATA_TRIANGLE_MESH ) && !obj->dataType( DATA_POLY_MESH ) ) {
    contextMenu_->clear();
    return;
  }

  // ================================================================================
  // Get objects texture data and verify that texture exists
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    emit log(LOGERR,tr("slotUpdateContextMenu: Texture data not found! Object %1 ").arg(_objectId) );
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

    if ( !texData->textures()[i].visibleName().isEmpty() )
      action = actionGroup->addAction( texData->textures()[i].visibleName() );
    else
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

  // Get the object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  id , obj ) ) {
    emit log(LOGERR,"slotTextureContextMenu: Unable to get Object for id " + QString::number(id) );
    return;
  }

  // Get the corresponding texture data
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );

  if (_action->text() == "Texture Settings"){

    if (texData == 0){

      QMessageBox msgBox;
      msgBox.setText("Cannot show Properties. No Textures available!");
      msgBox.exec();
      return;

    } else {
      settingsDialog_->show( texData, id, obj->name() );
    }

  } else {

    if ( texData != 0) {
      slotSwitchTexture( _action->text() , id );

      // Switch to a texture drawMode
      switchDrawMode(texData->texture( _action->text() ).type());
    }
  }

}

//compute histogram for the given texture property
void TextureControlPlugin::getCoordinates1D(QString _textureName, int _id, std::vector< double >& _x ){

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"getCoordinates1D: Unable to get Object for id " + QString::number(_id) );
  }

  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    emit log(LOGERR,tr("getCoordinates1D: Object  %1 has no texture data ").arg(_id) );
    return;
  }

  // ================================================================================
  // Check for requested Texture
  // ================================================================================
  if ( !texData->textureExists(_textureName) ) {
    emit log(LOGERR, "getCoordinates1D: Texture not available! " + _textureName );
    return;
  }

  if ( texData->texture( _textureName ).dirty() )
    emit updateTexture( _textureName , _id );


  // collect the 1d texture coords from vertex or halfedge porperty, depending on the texture type
  _x.clear();

  if( obj->dataType( DATA_TRIANGLE_MESH ) ) 
  {
    TriMesh* mesh = PluginFunctions::triMesh(obj);
    
    if ( texData->texture(_textureName).type() == VERTEXBASED )
    {
      OpenMesh::VPropHandleT< double > coordProp;
      
      if ( !mesh->get_property_handle(coordProp, _textureName.toStdString() ) )
      {
        emit log(LOGERR,tr("getCoordinates1D: Texture Property not found: Object %1 , TextureName %2").arg(_id).arg(_textureName) );
        return;
      }
      
      for ( TriMesh::VertexIter v_it = mesh->vertices_begin() ; v_it != mesh->vertices_end(); ++v_it)
        _x.push_back( mesh->property(coordProp,v_it) );
      
    } // end of if vertex based for tri meshes
    else if ( texData->texture(_textureName).type() == HALFEDGEBASED ) 
    {
      OpenMesh::HPropHandleT< double > coordProp;
      
      if ( !mesh->get_property_handle(coordProp, _textureName.toStdString() ) )
      {
        emit log(LOGERR,tr("getCoordinates1D: Texture Property not found: Object %1 , TextureName %2").arg(_id).arg(_textureName) );
        return;
      }
      
      for ( TriMesh::HalfedgeIter h_it = mesh->halfedges_begin() ; h_it != mesh->halfedges_end(); ++h_it)
        _x.push_back( mesh->property(coordProp,h_it) );
    } // end of if halfedge based for tri meshes
    
  } // end of if tri mesh
  else if ( obj->dataType( DATA_POLY_MESH ) ) 
  {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);

    if ( texData->texture(_textureName).type() == VERTEXBASED )
    {
      OpenMesh::VPropHandleT< double > coordProp;
    
      if ( !mesh->get_property_handle(coordProp, _textureName.toStdString() ) )
      {
        emit log(LOGERR,tr("getCoordinates1D: Texture Property not found: Object %1 , TextureName %2").arg(_id).arg(_textureName) );
        return;
      }

      for ( PolyMesh::VertexIter v_it = mesh->vertices_begin() ; v_it != mesh->vertices_end(); ++v_it)
        _x.push_back( mesh->property(coordProp,v_it) );
    } // end of if vertex based for poly meshes
    else if ( texData->texture(_textureName).type() == HALFEDGEBASED ) 
    {
      OpenMesh::HPropHandleT< double > coordProp;
    
      if ( !mesh->get_property_handle(coordProp, _textureName.toStdString() ) )
      {
        emit log(LOGERR,tr("getCoordinates1D: Texture Property not found: Object %1 , TextureName %2").arg(_id).arg(_textureName) );
        return;
      }

      for ( PolyMesh::HalfedgeIter h_it = mesh->halfedges_begin() ; h_it != mesh->halfedges_end(); ++h_it)
        _x.push_back( mesh->property(coordProp,h_it) );
    } // end of if halfedge based for poly meshes
  }// end of if poly mesh
  
}

void TextureControlPlugin::slotAboutToRestore( int _objectid ) {

  // ================================================================================
  // Get  current object
  // ================================================================================
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _objectid , obj ) ) {
    emit log(LOGERR,"slotAboutToRestore: Unable to get Object for id " + QString::number(_objectid) );
  }
  
  // ================================================================================
  // Get Texture data for current object
  // ================================================================================
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0) {
    // Nothing to do
    return;
  }
  
  // ================================================================================
  // Disable the Texture mapping in the current objects Meshnode
  // This will prevent the renderer to crash if the map is wrong after the restore
  // ================================================================================
  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
    PluginFunctions::triMeshObject(obj)->meshNode()->setTextureMap(0);
  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PluginFunctions::polyMeshObject(obj)->meshNode()->setTextureMap(0);
  }

}

void TextureControlPlugin::slotRestored( int _objectid ) {
  // ================================================================================
  // Get the last active texture in the restored perObjectData
  // ================================================================================
  QString currentTexture;
  slotGetCurrentTexture(_objectid,currentTexture);
  
  // ================================================================================
  // Enable the last active texture 
  // ================================================================================
  slotSwitchTexture(currentTexture,_objectid);
}

Q_EXPORT_PLUGIN2( texturecontrolplugin , TextureControlPlugin );

