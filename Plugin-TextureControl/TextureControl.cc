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

void TextureControlPlugin::slotTextureAdded( QString _textureName , QString _filename , uint _dimension)
{
  Texture tex;
  tex.name = _textureName;
  tex.filename = _filename;
  tex.dimension = _dimension;
  tex.scale = true;
  tex.clamp_min = FLT_MIN;
  tex.clamp_max = FLT_MAX;
  tex.clamp = false;
  tex.repeat = false;
  tex.center = false;
  tex.abs = false;
  tex.max_val = 1.0;
  tex.type = VERTEXBASED;
  textures_.push_back( tex );
   
  activeTexture_ = _textureName;
  
  QAction* new_texture = new QAction(_textureName, this);
  new_texture->setStatusTip(tr("Switch to this Texture"));
  new_texture->setCheckable(true);
  actionGroup_->addAction(new_texture);
  textureMenu_->addAction(new_texture);
  new_texture->setChecked(true);
  
  textureActions_.push_back(new_texture);
  
  ///@todo if a texture is added later, update or generate it for all objects
//   emit updateTexture(_textureName,-1);
}

void TextureControlPlugin::slotTextureUpdated( QString _textureName , int _identifier ) {
  
  BaseObjectData* object;
  if (! PluginFunctions::get_object(  _identifier , object ) ) {
    emit log(LOGERR,"Unable to get Object for id " + QString::number(_identifier) );
    return; 
  } 
  
  // Search the list of textures if we have the texture
  int textureid = -1;
  for ( int i = 0 ; i < (int)textures_.size() ; ++i ) {
    if ( textures_[i].name == _textureName ) {
      textureid = i;
      break;
    }
  }
   
  
  if ( textureid == -1 ) {
    emit log(LOGERR,"Unable to find texture with name " + _textureName );
    return;
  }
  
  // Only update if the active texture is the current texture
  if ( activeTexture_ != _textureName) 
    return;

  const bool repeat = textures_[textureid].repeat;

  if( object->dataType( DATA_TRIANGLE_MESH ) ) {
  
    TriMesh* mesh = PluginFunctions::triMesh(object);
    doUpdateTexture(textureid, *mesh);
    PluginFunctions::triMeshObject(object)->textureNode()->set_repeat(repeat);
    QString filename = OpenFlipper::Options::textureDir().absolutePath() + 
      OpenFlipper::Options::dirSeparator() + 
      textures_[textureid].filename;

    PluginFunctions::triMeshObject(object)->textureNode()->read(filename.toUtf8());
  } 

  if ( object->dataType( DATA_POLY_MESH ) ) {
    
    PolyMesh* mesh = PluginFunctions::polyMesh(object);
    doUpdateTexture(textureid, *mesh);

    PluginFunctions::polyMeshObject(object)->textureNode()->set_repeat(repeat);
    QString filename = OpenFlipper::Options::textureDir().absolutePath() + 
      OpenFlipper::Options::dirSeparator() + 
      textures_[textureid].filename;

    PluginFunctions::polyMeshObject(object)->textureNode()->read(filename.toUtf8());
  }
  emit update_view();

}

template< typename MeshT >
void TextureControlPlugin::doUpdateTexture ( int _textureid, MeshT& _mesh )
{
  if ( textures_[_textureid].type == HALFEDGEBASED ) {
    if (textures_[_textureid].dimension == 1) {

      OpenMesh::HPropHandleT< double > texture;
      if ( ! PluginFunctions::get_property_handle(&_mesh,textures_[_textureid].name,texture) ) {
        emit log(LOGERR,"Unable to get property " + textures_[_textureid].name );
        return;
      }

      copyTexture(_textureid, _mesh, texture);

    } else if ( textures_[_textureid].dimension == 2 ) {

      OpenMesh::HPropHandleT< OpenMesh::Vec2d > texture2D;
      if ( ! PluginFunctions::get_property_handle(&_mesh, textures_[_textureid].name, texture2D) ) {
        emit log(LOGERR,"Unable to get property " + textures_[_textureid].name);
        return;
      }

      copyTexture(_textureid, _mesh, texture2D);

    } else 
      emit log(LOGERR, "Unsupported Texture Dimension " + QString::number(textures_[_textureid].dimension) );
  } else if ( textures_[_textureid].type == VERTEXBASED ) {
    if ( textures_[_textureid].dimension == 1 ) {

      OpenMesh::VPropHandleT< double > texture;
      if ( ! PluginFunctions::get_property_handle(&_mesh,textures_[_textureid].name,texture) ) {
        emit log(LOGERR,"Unable to get property " + textures_[_textureid].name );
        return;  
      }

        copyTexture(_textureid, _mesh, texture);

      } else if ( textures_[_textureid].dimension == 2 ) {

        OpenMesh::VPropHandleT< OpenMesh::Vec2d >  texture2D;
        if ( ! PluginFunctions::get_property_handle(&_mesh,textures_[_textureid].name,texture2D) ) {
          emit log(LOGERR,"Unable to get property " + textures_[_textureid].name );
          return;  
        }

        copyTexture(_textureid, _mesh, texture2D);

      } /*else if ( textures_[_textureid].dimension == 3 ) {
        
        OpenMesh::VPropHandleT< OpenMesh::Vec3d >  scalarField3D;
        if ( ! PluginFunctions::get_property_handle(&_mesh,textures_[_textureid].name,scalarField3D) ) {
          emit log(LOGERR,"Unable to get property " + textures_[_textureid].name );
          return;  
        }
        
        copyTexture(_textureid, _mesh, scalarField3D);
        
      }*/ else
        emit log(LOGERR, "Unsupported Texture Dimension " + QString::number(textures_[_textureid].dimension) );
        
    } else
      emit log(LOGERR, "Unsupported Texture type");
      
}

template< typename MeshT >
void TextureControlPlugin::copyTexture ( int _textureid, MeshT& _mesh, OpenMesh::VPropHandleT< double > _texProp )
{
  double max,min;
  computeMinMaxScalar(_textureid, _mesh, _texProp, min, max);

  for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it) {
    double value = _mesh.property(_texProp, v_it);
    computeValue(_textureid, min, max, value);
    _mesh.set_texcoord2D( v_it, ACG::Vec2f(float(value), float(value) ) );
  }
}

template< typename MeshT >
void TextureControlPlugin::copyTexture ( int _textureid, MeshT& _mesh, OpenMesh::VPropHandleT< OpenMesh::Vec2d > _texProp )
{
  for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it) {
    OpenMesh::Vec2d value = _mesh.property(_texProp, v_it);
    _mesh.set_texcoord2D( v_it, ACG::Vec2f(float(value[0]), float(value[1]) ) );
  }
}
    
template< typename MeshT >
void TextureControlPlugin::copyTexture ( int _textureid, MeshT& _mesh, OpenMesh::HPropHandleT< double > _texProp )
{
  double max,min;
  computeMinMaxScalar(_textureid, _mesh, _texProp, min, max);

  for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin(); h_it != _mesh.halfedges_end(); ++h_it) {
    double value = _mesh.property(_texProp, h_it);
    computeValue(_textureid, min, max, value);
    _mesh.set_texcoord2D( h_it, ACG::Vec2f(float(value), float(value) ) );
  }
}

template< typename MeshT >
void TextureControlPlugin::copyTexture ( int _textureid, MeshT& _mesh, OpenMesh::HPropHandleT< OpenMesh::Vec2d > _texProp )
{
  for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin(); h_it != _mesh.halfedges_end(); ++h_it) {
    OpenMesh::Vec2d value = _mesh.property(_texProp, h_it);
    _mesh.set_texcoord2D( h_it, ACG::Vec2f(float(value[0]), float(value[1]) ) );
  }
}

template< typename MeshT >
void TextureControlPlugin::computeMinMaxScalar(int _textureid, MeshT& _mesh,OpenMesh::VPropHandleT< double > _texture,
                                                               double& _min , double& _max) {
   const bool   abs = textures_[_textureid].abs;
   const bool   clamp = textures_[_textureid].clamp ;
   const double clamp_max = textures_[_textureid].clamp_max;
   const double clamp_min = textures_[_textureid].clamp_min;
   
   _max = FLT_MIN;
   _min = FLT_MAX;
   
   for ( typename MeshT::VertexIter v_it = _mesh.vertices_begin() ; v_it != _mesh.vertices_end(); ++v_it) {
      if ( abs ) {
         _max = std::max( fabs(_mesh.property(_texture,v_it)) , _max);
         _min = std::min( fabs(_mesh.property(_texture,v_it)) , _min);
      } else {
         _max = std::max( _mesh.property(_texture,v_it) , _max);
         _min = std::min( _mesh.property(_texture,v_it) , _min);
      }
   } 
   
   if ( clamp ) {
      if ( _max > clamp_max )
         _max = clamp_max;
      if (_min < clamp_min)
         _min = clamp_min;
   }
}

template< typename MeshT >
void TextureControlPlugin::computeMinMaxScalar(int _textureid, MeshT& _mesh,OpenMesh::HPropHandleT< double > _texture,
                                                               double& _min , double& _max) {
   const bool   abs = textures_[_textureid].abs;
   const bool   clamp = textures_[_textureid].clamp ;
   const double clamp_max = textures_[_textureid].clamp_max;
   const double clamp_min = textures_[_textureid].clamp_min;
   
   _max = FLT_MIN;
   _min = FLT_MAX;
   
   for ( typename MeshT::HalfedgeIter h_it = _mesh.halfedges_begin() ; h_it != _mesh.halfedges_end(); ++h_it) {
      if ( abs ) {
         _max = std::max( fabs(_mesh.property(_texture,h_it)) , _max);
         _min = std::min( fabs(_mesh.property(_texture,h_it)) , _min);
      } else {
         _max = std::max( _mesh.property(_texture,h_it) , _max);
         _min = std::min( _mesh.property(_texture,h_it) , _min);
      }
   } 
   
   if ( clamp ) {
      if ( _max > clamp_max )
         _max = clamp_max;
      if (_min < clamp_min)
         _min = clamp_min;
   }
} 
 
void TextureControlPlugin::computeValue(int _textureid, double _min, double _max, double& _value) {
   const bool clamp = textures_[_textureid].clamp ;
   const bool center = textures_[_textureid].center;
   const double max_val = textures_[_textureid].max_val;
   const bool abs = textures_[_textureid].abs;
   const double clamp_max = textures_[_textureid].clamp_max;
   const double clamp_min = textures_[_textureid].clamp_min;
   const double scale = fabs(_max) + fabs(_min);
   const bool repeat = textures_[_textureid].repeat;
   
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
        if ( textures_[_textureid].scale) {
          _value /= scale;
          _value -= _min/scale;
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
   // Object erased, so do nothing
   if ( _identifier == -1 ) {
      return;
   }
   
   // Force an update of all textures which are available for the updated object
   for ( uint i = 0 ; i < textures_.size() ; ++i ) {
      emit updateTexture( textures_[i].name , _identifier );
   }
   
   emit update_view();
}

void TextureControlPlugin::slotUpdateAllTextures( ) {
   // Force an update of all textures which are available for the updated object   
   for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it)   
      for ( uint i = 0 ; i < textures_.size() ; ++i ) 
         emit updateTexture( textures_[i].name , o_it->id() );
}

void TextureControlPlugin::slotSetTextureMode(QString _textureName ,QString _mode) {
   int textureid = -1;
   for ( int j = 0 ; j < (int)textures_.size() ; ++j ) {
        if ( textures_[j].name == _textureName ) {
             textureid = j;
             break;
        }
   }
   
   if ( textureid == -1 ) {
     emit log(LOGERR,"Texture Mode setting requested for " + _textureName + " but texture not found" );
     return;
   }
   
   int i = 0;
   QString nextString = _mode.section(',',i,i);
   while ( nextString != "" ) {
      QString sectionName = nextString.section('=',0,0);
      QString value = nextString.section('=',1,1);
      
      if ( sectionName == "clamp" ) {
         if (value == "false") {
              textures_[textureid].clamp = false;
         } else {
              textures_[textureid].clamp = true;
         }
      } else
      if ( sectionName == "clamp_max" ) {
              textures_[textureid].clamp_max = value.toDouble();
      } else
      if ( sectionName == "clamp_min" ) {
              textures_[textureid].clamp_min = value.toDouble();
      } else
      if ( sectionName == "max_val" ) {
              textures_[textureid].max_val = value.toDouble();
      } else
      if ( sectionName == "repeat" ) {
         if (value == "false") {
              textures_[textureid].repeat = false;
         } else {
              textures_[textureid].repeat = true;
         } 
      } else
      if ( sectionName == "center" ) {
         if (value == "false") {
              textures_[textureid].center = false;
         } else {
              textures_[textureid].center = true;
         } 
      } else
      if ( sectionName == "scale" ) {
         if (value == "false") {
              textures_[textureid].scale = false;
         } else {
              textures_[textureid].scale = true;
         }
      } else
      if ( sectionName == "type" ) {
         if (value == "halfedgebased") {
              textures_[textureid].type = HALFEDGEBASED;
         } else {
              textures_[textureid].type = VERTEXBASED;
         }
      } else
        emit log(LOGERR,"Unknown texture mode : " + sectionName);
      
      ++i;
      nextString = _mode.section(',',i,i);
   }
   
   if ( activeTexture_ == _textureName ) {
      
      // Force an update of all objects 
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it)   
         updateTexture(  _textureName , o_it->id() );
      
      emit update_view();
   }
}

void TextureControlPlugin::pluginsInitialized() {
  // Texture menu
  textureMenu_ = new QMenu(tr("&Texture Control"));
  
  emit addMenu( textureMenu_ , VIEWMENU ); 
  
  settingsDialog_ = new texturePropertiesWidget(0);
   
  connect(settingsDialog_->okButton,SIGNAL(clicked()), this , SLOT (slotTexturePropertiesOk() ) );
  connect(settingsDialog_->applyButton,SIGNAL(clicked()), this , SLOT (slotTexturePropertiesApply() ) );
  connect(settingsDialog_->cancelButton,SIGNAL(clicked()), this , SLOT (slotTexturePropertiesCancel() ) );
  
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
}
 
 void TextureControlPlugin::updateDialog() {
   if ( textures_.size() == 0 )
      return;
    
   int textureid = -1;
   for ( int i = 0 ; i < (int)textures_.size() ; ++i ) {
        if ( textures_[i].name == activeTexture_ ) {
             textureid = i;
             break;
        }
   }
   
  if ( textureid == -1 ) {
     emit log(LOGERR,"Active Texture not found");
     return;  
  }
   
   settingsDialog_->repeatBox->setChecked(textures_[textureid].repeat);
   settingsDialog_->clampBox->setChecked(textures_[textureid].clamp);
   settingsDialog_->centerBox->setChecked(textures_[textureid].center);
   settingsDialog_->absBox->setChecked(textures_[textureid].abs);
   QString tmp;
   tmp.setNum(textures_[textureid].max_val);
   settingsDialog_->max_val->setText( tmp );
   tmp.setNum(textures_[textureid].clamp_min);
   settingsDialog_->clamp_min->setText(tmp);
   tmp.setNum(textures_[textureid].clamp_max);
   settingsDialog_->clamp_max->setText(tmp);   
 }
 
 void TextureControlPlugin::slotSetTextureProperties() {
   updateDialog();
   if ( textures_.size() == 0 )
      return;
   
   settingsDialog_->show();
 }
 
void TextureControlPlugin::applyDialogSettings() {
   if ( textures_.size() == 0 )
      return;
   
   int textureid = -1;
   for ( int i = 0 ; i < (int)textures_.size() ; ++i ) {
         if ( textures_[i].name == activeTexture_ ) {
               textureid = i;
               break;
         }
   }
   
   if (textureid == -1) {
      emit log(LOGERR,"Unable to get active Texture");
   }
   
   textures_[textureid].repeat=settingsDialog_->repeatBox->isChecked();
   textures_[textureid].clamp=settingsDialog_->clampBox->isChecked();
   textures_[textureid].center=settingsDialog_->centerBox->isChecked();
   textures_[textureid].abs=settingsDialog_->absBox->isChecked();
   textures_[textureid].scale=settingsDialog_->scaleBox->isChecked();
   
   QString tmp;
   tmp = settingsDialog_->max_val->text();
   textures_[textureid].max_val = tmp.toDouble();
   
   tmp = settingsDialog_->clamp_min->text();
   textures_[textureid].clamp_min = tmp.toDouble();
   
   tmp = settingsDialog_->clamp_max->text();
   textures_[textureid].clamp_max = tmp.toDouble();
   
   // Update the corresponding meshes
   for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it)   
      slotTextureUpdated(  activeTexture_ , o_it->id() );
   
   emit update_view();
}
 
 void TextureControlPlugin::slotTexturePropertiesOk() {
   applyDialogSettings();
   settingsDialog_->hide();
}
 
  void TextureControlPlugin::slotTexturePropertiesApply() {
     applyDialogSettings();
 }
 
 void TextureControlPlugin::slotTexturePropertiesCancel() {
    settingsDialog_->hide();
 }

 
void TextureControlPlugin::slotTextureMenu(QAction* _action) {
  if ( activeTexture_ !=  _action->text() ) {
      activeTexture_ =  _action->text();
      
      // Force an update of all objects 
      for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it)   
         slotTextureUpdated(  _action->text() , o_it->id() );
      
      updateDialog();
      
      PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
  }
}

void TextureControlPlugin::slotSwitchTexture( QString _textureName ) {
  int textureid = -1;
  for ( int i = 0 ; i < (int)textures_.size() ; ++i ) {
        if ( textures_[i].name == _textureName ) {
              textureid = i;
              break;
        }
  }
  
  if (textureid == -1) {
    emit log(LOGERR,"Unable to switch to texture " + _textureName + " (not found)");
    return; 
  }
  
  PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_TEXTURED_SHADED );
   
  if ( activeTexture_ !=  _textureName ) {
    activeTexture_ =  _textureName;
    
    // Force an update of all objects 
    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objects_end(); ++o_it)   
        slotTextureUpdated(  _textureName , o_it->id() );
    
    updateDialog();
  }
}


Q_EXPORT_PLUGIN2( texturecontrolplugin , TextureControlPlugin );

