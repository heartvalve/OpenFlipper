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

#define TEXTUREDATA "TextureData"



void TextureControlPlugin::slotTextureAdded( QString _textureName , QString _filename , uint _dimension , int _id)
{
  std::cerr << "slotLocalTextureAdded " << _textureName.toStdString() << std::endl;

  // Get the new object
  BaseObjectData* obj;
  if (! PluginFunctions::getObject(  _id , obj ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_id) );
  }

  // Get Texture data for this object or create one if it does not exist
  TextureData* texData = dynamic_cast< TextureData* > ( obj->objectData(TEXTUREDATA) );
  if (texData == 0){
    texData = new TextureData();
    obj->setObjectData(TEXTUREDATA, texData);
  }


  // Add this texture to the list of global textures
  if ( ! texData->textureExists(_textureName) ) {
    texData->addTexture(_textureName,_filename,_dimension,0);
    texData->texture(_textureName).enabled = false;
  } else {
    emit log(LOGERR,"Trying to add already existing texture " + _textureName + " for object " + QString::number(_id) );
    return;
  }
}

void TextureControlPlugin::slotTextureAdded( QString _textureName , QString _filename , uint _dimension)
{
  std::cerr << "slotGlobalTextureAdded " << _textureName.toStdString() << std::endl;

  // Add this texture to the list of global textures
  if ( ! globalTextures_.textureExists(_textureName) ) {
    globalTextures_.addTexture(_textureName,_filename,_dimension,0);
    globalTextures_.texture(_textureName).enabled = false;
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

void TextureControlPlugin::fileOpened( int _id ) {
  // TODO:: Store original texture coords in a new property!

  std::cerr << "File opened : " << _id << std::endl;

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

  // Iterate over all available global textures and add them to the object
  for ( uint i = 0 ; i < globalTextures_.textures().size() ; ++i) {

    std::cerr << "Adding texture " << i << " " << globalTextures_.textures()[i].name.toStdString() << std::endl;

    // ================================================================================
    // Get the image file
    // ================================================================================
    // TODO: support arbitrary paths!
    QString filename = OpenFlipper::Options::textureDir().absolutePath() +
                       OpenFlipper::Options::dirSeparator() +
                       globalTextures_.textures()[i].filename;

    QImage textureImage;
    if ( !textureImage.load( filename ) ){
        emit log(LOGERR, "Cannot load texture " + filename );
        continue;
    }

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

    if (glName != 0)
      texData->addTexture(globalTextures_.textures()[i], glName);

    // ================================================================================
    // Update texture mapping in meshNode
    // ================================================================================
    if( obj->dataType( DATA_TRIANGLE_MESH ) ){
      PluginFunctions::triMeshObject(obj)->meshNode()->set_texture_map( texData->textureMap() );
      PluginFunctions::triMeshObject(obj)->meshNode()->set_property_map( texData->propertyMap() );
    }

    if ( obj->dataType( DATA_POLY_MESH ) ){
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_texture_map( texData->textureMap() );
      PluginFunctions::polyMeshObject(obj)->meshNode()->set_property_map( texData->propertyMap() );
    }

  }
}

void TextureControlPlugin::slotTextureUpdated( QString _textureName , int _identifier ) {

  std::cerr << "slotTextureUpdated called for texture " << _textureName.toStdString() << " "  << _identifier << std::endl;

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

  if ( ! texData->textureExists(_textureName) ) {
    std::cerr << "Texture not found on object" << std::endl;
    return;
  }

  if ( ! texData->texture(_textureName).enabled ) {
    texData->texture(_textureName).dirty = true;
    std::cerr << "Texture is not active on object" << std::endl;
    return;
  }

  // ================================================================================
  // As the current texture is active, update it
  // ================================================================================

  if( obj->dataType( DATA_TRIANGLE_MESH ) ) {
    TriMesh* mesh = PluginFunctions::triMesh(obj);
    doUpdateTexture(texData->texture(_textureName), *mesh);
    PluginFunctions::triMeshObject(obj)->textureNode()->set_repeat(texData->texture(_textureName).parameters.repeat);
  } else if ( obj->dataType( DATA_POLY_MESH ) ) {
    PolyMesh* mesh = PluginFunctions::polyMesh(obj);
    doUpdateTexture(texData->texture(_textureName), *mesh);
    PluginFunctions::polyMeshObject(obj)->textureNode()->set_repeat(texData->texture(_textureName).parameters.repeat);
  }

  // Mark the texture as not dirty
  texData->texture(_textureName).dirty = false;

  emit updateView();
}

template< typename MeshT >
void TextureControlPlugin::doUpdateTexture ( Texture& _texture, MeshT& _mesh )
{
  if ( _texture.type == HALFEDGEBASED ) {
    if (_texture.dimension == 1) {

      OpenMesh::HPropHandleT< double > texture;
	  if ( ! _mesh.get_property_handle(texture, _texture.name.toStdString() ) ) {
        emit log(LOGERR,"Unable to get property " + _texture.name );
        return;
      }

      copyTexture(_texture, _mesh, texture);

    } else if ( _texture.dimension == 2 ) {

      OpenMesh::HPropHandleT< OpenMesh::Vec2d > texture2D;
	  if ( ! _mesh.get_property_handle( texture2D, _texture.name.toStdString() ) ) {
        emit log(LOGERR,"Unable to get property " + _texture.name);
        return;
      }

      copyTexture( _texture, _mesh, texture2D);

    } else
      emit log(LOGERR, "Unsupported Texture Dimension " + QString::number(_texture.dimension) );
  } else if ( _texture.type == VERTEXBASED ) {
    if ( _texture.dimension == 1 ) {

      OpenMesh::VPropHandleT< double > texture;
	  if ( ! _mesh.get_property_handle(texture,_texture.name.toStdString() ) ) {
        emit log(LOGERR,"Unable to get property " + _texture.name );
        return;
      }

        copyTexture(_texture, _mesh, texture);

      } else if ( _texture.dimension == 2 ) {

        OpenMesh::VPropHandleT< OpenMesh::Vec2d >  texture2D;
		  if ( ! _mesh.get_property_handle(texture2D,_texture.name.toStdString() ) ) {
          emit log(LOGERR,"Unable to get property " + _texture.name );
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
        emit log(LOGERR, "Unsupported Texture Dimension " + QString::number(_texture.dimension) );

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
  std::cerr << "slotObjectUpdated called for : " << _identifier << std::endl;
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
    texData->textures()[i].dirty = true;

    bool update = false;
    for ( int j = 0 ; j < PluginFunctions::viewers() ; ++j ) {
       update |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED );
       update |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
    }

    if ( update && texData->textures()[i].enabled )
      emit updateTexture( texData->textures()[i].name , _identifier );
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

  std::cerr << "TextureControlPlugin::slotSetTextureMode called for : " << _textureName.toStdString() << std::endl;

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

  int i = 0;
  QString nextString = _mode.section(',',i,i);
  while ( nextString != "" ) {
    QString sectionName = nextString.section('=',0,0);
    QString value = nextString.section('=',1,1);

    if ( sectionName == "clamp" ) {
        if (value == "false") {
            texture.parameters.clamp = false;
        } else {
            texture.parameters.clamp = true;
        }
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
        if (value == "false") {
            texture.parameters.repeat = false;
        } else {
            texture.parameters.repeat = true;
        }
    } else
    if ( sectionName == "center" ) {
        if (value == "false") {
            texture.parameters.center = false;
        } else {
            texture.parameters.center = true;
        }
    } else
    if ( sectionName == "scale" ) {
        if (value == "false") {
            texture.parameters.scale = false;
        } else {
            texture.parameters.scale = true;
        }
    } else
    if ( sectionName == "type" ) {
        if (value == "halfedgebased") {
            texture.type = HALFEDGEBASED;
        } else {
            texture.type = VERTEXBASED;
        }
    } else
      emit log(LOGERR,"Unknown texture mode : " + sectionName);

    ++i;
    nextString = _mode.section(',',i,i);
  }

  // ================================================================================
  // Mark updated texture as dirty
  // ================================================================================
  texture.dirty = true;


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

        if ( _mode.contains("type") && (texture.type != localTex.type) ){
          localTex.type = texture.type;
          changed = true;
        }

        //only update if the texture is enabled
        if (changed){
          if ( texData->isEnabled(_textureName) )
            emit updateTexture( _textureName, o_it->id() );
          else
            localTex.dirty = true;
        }
      }
    }
  }
}

void TextureControlPlugin::slotSetTextureMode(QString _textureName, QString _mode, int _id) {
/*
  std::cerr << "TextureControlPlugin::slotSetTextureMode called for : " << _textureName.toStdString()
  std::cerr << "on object=" << _id <<  std::endl;

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

  int i = 0;
  QString nextString = _mode.section(',',i,i);
  while ( nextString != "" ) {
    QString sectionName = nextString.section('=',0,0);
    QString value = nextString.section('=',1,1);

    if ( sectionName == "clamp" ) {
        if (value == "false") {
            texture.parameters.clamp = false;
        } else {
            texture.parameters.clamp = true;
        }
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
        if (value == "false") {
            texture.parameters.repeat = false;
        } else {
            texture.parameters.repeat = true;
        }
    } else
    if ( sectionName == "center" ) {
        if (value == "false") {
            texture.parameters.center = false;
        } else {
            texture.parameters.center = true;
        }
    } else
    if ( sectionName == "scale" ) {
        if (value == "false") {
            texture.parameters.scale = false;
        } else {
            texture.parameters.scale = true;
        }
    } else
    if ( sectionName == "type" ) {
        if (value == "halfedgebased") {
            texture.type = HALFEDGEBASED;
        } else {
            texture.type = VERTEXBASED;
        }
    } else
      emit log(LOGERR,"Unknown texture mode : " + sectionName);

    ++i;
    nextString = _mode.section(',',i,i);
  }

  // ================================================================================
  // Mark updated texture as dirty
  // ================================================================================
  texture.dirty = true;


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

        if ( _mode.contains("type") && (texture.type != localTex.type) ){
          localTex.type = texture.type;
          changed = true;
        }

        //only update if the texture is enabled
        if (changed){
          if ( texData->isEnabled(_textureName) )
            emit updateTexture( _textureName, o_it->id() );
          else
            localTex.dirty = true;
        }
      }
    }
  }*/
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

  connect(settingsDialog_->okButton,SIGNAL(clicked()), this , SLOT (slotTexturePropertiesOk() ) );
  connect(settingsDialog_->applyButton,SIGNAL(clicked()), this , SLOT (slotTexturePropertiesApply() ) );
  connect(settingsDialog_->cancelButton,SIGNAL(clicked()), this , SLOT (slotTexturePropertiesCancel() ) );

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

void TextureControlPlugin::updateDialog() {
  std::cerr << "Not implemented : updateDialog" << std::endl;
  // TODO : Check
//   if ( textures_.size() == 0 )
//     return;
//
//   settingsDialog_->repeatBox->setChecked(textures_[textureid].repeat);
//   settingsDialog_->clampBox->setChecked(textures_[textureid].clamp);
//   settingsDialog_->centerBox->setChecked(textures_[textureid].center);
//   settingsDialog_->absBox->setChecked(textures_[textureid].abs);
//   QString tmp;
//   tmp.setNum(textures_[textureid].max_val);
//   settingsDialog_->max_val->setText( tmp );
//   tmp.setNum(textures_[textureid].clamp_min);
//   settingsDialog_->clamp_min->setText(tmp);
//   tmp.setNum(textures_[textureid].clamp_max);
//   settingsDialog_->clamp_max->setText(tmp);
//
//   // update plot only when dimension is 1
//   if ( textures_[textureid].dimension == 1) {
//     std::vector< double > x,y;
//
//     x.push_back(100.0);
//     y.push_back(100.0);
//
//     x.push_back(200.0);
//     y.push_back(150.0);
//
//     x.push_back(300.0);
//     y.push_back(10.0);
//
//     settingsDialog_->setOriginalData(x,y);
//
// //     getOriginalHistogram();
//
//   }
}

void TextureControlPlugin::slotSetTextureProperties() {
  std::cerr << "Not implemented : slotSetTextureProperties" << std::endl;
  // TODO : Check
//   updateDialog();
//   if ( textures_.size() == 0 )
//       return;
//
//   settingsDialog_->show();
}

void TextureControlPlugin::applyDialogSettings() {
  std::cerr << "Not implemented : applyDialogSettings" << std::endl;
  // TODO : Check
//   if ( textures_.size() == 0 )
//       return;
//
//   int textureid = -1;
//   for ( int i = 0 ; i < (int)textures_.size() ; ++i ) {
//         if ( textures_[i].name == activeTexture_ ) {
//               textureid = i;
//               break;
//         }
//   }
//
//   if (textureid == -1) {
//       emit log(LOGERR,"Unable to get active Texture");
//   }
//
//   textures_[textureid].repeat=settingsDialog_->repeatBox->isChecked();
//   textures_[textureid].clamp=settingsDialog_->clampBox->isChecked();
//   textures_[textureid].center=settingsDialog_->centerBox->isChecked();
//   textures_[textureid].abs=settingsDialog_->absBox->isChecked();
//   textures_[textureid].scale=settingsDialog_->scaleBox->isChecked();
//
//   QString tmp;
//   tmp = settingsDialog_->max_val->text();
//   textures_[textureid].max_val = tmp.toDouble();
//
//   tmp = settingsDialog_->clamp_min->text();
//   textures_[textureid].clamp_min = tmp.toDouble();
//
//   tmp = settingsDialog_->clamp_max->text();
//   textures_[textureid].clamp_max = tmp.toDouble();
//
//   // Update the corresponding meshes
//   for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it)
//       slotTextureUpdated(  activeTexture_ , o_it->id() );
//
//   emit updateView();
}

void TextureControlPlugin::slotTexturePropertiesOk() {
  // TODO : Check
//   applyDialogSettings();
//   settingsDialog_->hide();
}

void TextureControlPlugin::slotTexturePropertiesApply() {
  // TODO : Check
//     applyDialogSettings();
}

void TextureControlPlugin::slotTexturePropertiesCancel() {
  // TODO : Check
//     settingsDialog_->hide();
}


void TextureControlPlugin::slotTextureMenu(QAction* _action) {
  // call existing function to switch the texture
  slotSwitchTexture( _action->text() );
}

void TextureControlPlugin::doSwitchTexture( QString _textureName , int _id ) {

  std::cerr << "TextureControlPlugin::doSwitchTexture : " << _textureName.toStdString();
  std::cerr << " object=" << _id << std::endl;

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
  // Enable the given texture exclusively
  // ================================================================================
  if ( texData->enableTexture( _textureName , true ) ) {
    std::cerr << "Enabled Texture " << _textureName.toStdString() << std::endl;
  } else {
    std::cerr << "Failed to enabled Texture" << std::endl;
    return;
  }

  // ================================================================================
  // If texture is flagged dirty, update it ( this jumps to texture updated
  // which will update the visualization )
  // ================================================================================
  if ( texData->texture( _textureName).dirty ) {
    // TODO: maybe introduce lock to prevent extra redraws if updating all objects
    emit updateTexture( texData->texture( _textureName ).name , obj->id() );
    return;
  }

  // ================================================================================
  // Update texture map from meshNode and activate it
  // ================================================================================
  if( obj->dataType( DATA_TRIANGLE_MESH ) ){
    doUpdateTexture(texData->texture(_textureName), *PluginFunctions::triMeshObject(obj)->mesh());
    PluginFunctions::triMeshObject(obj)->textureNode()->activateTexture( texData->texture( _textureName ).glName );
    PluginFunctions::triMeshObject(obj)->meshNode()->set_texture_map( texData->textureMap() );
    PluginFunctions::triMeshObject(obj)->meshNode()->set_property_map( texData->propertyMap() );
  }

  if ( obj->dataType( DATA_POLY_MESH ) ){
    doUpdateTexture(texData->texture(_textureName), *PluginFunctions::polyMeshObject(obj)->mesh());
    PluginFunctions::polyMeshObject(obj)->textureNode()->activateTexture( texData->texture( _textureName ).glName );
    PluginFunctions::polyMeshObject(obj)->meshNode()->set_texture_map( texData->textureMap() );
    PluginFunctions::polyMeshObject(obj)->meshNode()->set_property_map( texData->propertyMap() );
  }

}

void TextureControlPlugin::slotSwitchTexture( QString _textureName , int _id ) {

  std::cerr << "TextureControlPlugin::slotSwitchTexture : " << _textureName.toStdString();
  std::cerr << "object=" << _id << std::endl;

  doSwitchTexture(_textureName, _id);

  // ================================================================================
  // Switch to a texture drawMode
  // ================================================================================
  bool textureMode = false;
  for ( int j = 0 ; j < PluginFunctions::viewers() ; ++j ) {
      textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED );
      textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
  }

  if ( !textureMode )
    PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );

  emit updateView();

  // TODO: Update menu
//     QList<QAction *> menuEntries = actionGroup_->actions();
//
//     for ( int i = 0 ; i < menuEntries.size(); ++i ) {
//       if ( menuEntries[i]->text() == _textureName )
//         menuEntries[i]->setChecked(true);
//     }

//     updateDialog();
}

void TextureControlPlugin::slotSwitchTexture( QString _textureName ) {

  std::cerr << "TextureControlPlugin::slotSwitchTexture : " << _textureName.toStdString() << std::endl;

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd();
                                        ++o_it)

    doSwitchTexture(_textureName, o_it->id() );

  // ================================================================================
  // Switch to a texture drawMode
  // ================================================================================
  bool textureMode = false;
  for ( int j = 0 ; j < PluginFunctions::viewers() ; ++j ) {
      textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED );
      textureMode |= ( PluginFunctions::drawMode(j) == ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
  }

  if ( !textureMode )
    PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );

  emit updateView();

  // TODO: Update menu
//     QList<QAction *> menuEntries = actionGroup_->actions();
//
//     for ( int i = 0 ; i < menuEntries.size(); ++i ) {
//       if ( menuEntries[i]->text() == _textureName )
//         menuEntries[i]->setChecked(true);
//     }

//     updateDialog();
}


void TextureControlPlugin::slotUpdateContextMenu( int _objectId ) {

  std::cerr << "TextureControlPlugin::slotUpdateContextMenu " << _objectId << std::endl;

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

  for ( uint i = 0 ; i < texData->textures().size() ; ++i ) {

    QAction* action = actionGroup->addAction( texData->textures()[i].name );

    action->setCheckable(true);

    if ( texData->textures()[i].enabled )
      action->setChecked(true);


  }

  contextMenu_->addActions(actionGroup->actions());

}


void TextureControlPlugin::slotTextureContextMenu( QAction * _action ) {

  // id of object for which the context menu is created, is stored in the action
  QVariant idVariant = _action->data( );
  int id = idVariant.toInt();

  std::cerr << "TextureControlPlugin::slotTextureContextMenu : " << id << std::endl;

  slotSwitchTexture( _action->text() , id );

}

Q_EXPORT_PLUGIN2( texturecontrolplugin , TextureControlPlugin );

