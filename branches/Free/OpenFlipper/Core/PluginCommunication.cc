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




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION of Comunication with plugins
//
//=============================================================================


//== INCLUDES =================================================================

#include "Core.hh"

#include <QToolBox>

#include "OpenFlipper/BasePlugin/BaseInterface.hh"
#include "OpenFlipper/BasePlugin/ToolboxInterface.hh"
#include "OpenFlipper/BasePlugin/TextureInterface.hh"

#include "OpenFlipper/BasePlugin/PluginFunctionsCore.hh"

#include <OpenFlipper/common/RendererInfo.hh>

//== IMPLEMENTATION ==========================================================

//========================================================================================
// ===             Object List Communication                       =======================
//========================================================================================

/** This function is called by a plugin if it changed something in the object list (source,target,...). The information is passed to all plugins.
 * @param _identifier Id of the updated object
 * @param _type       What part of the object has been updated
 */
void Core::slotObjectUpdated(int _identifier, const UpdateType& _type) {
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"updatedObject( " + QString::number(_identifier) + ", " + updateTypeName(_type)
                 + tr(" ) called by ") + QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,"updatedObject( " + QString::number(_identifier) + ", " + updateTypeName(_type) + tr(" ) called by Core") );
    }
  }

  // Disable redraws as everything here has to update the object only once
  OpenFlipper::Options::redrawDisabled(true);

  // If we are called for a special object, we update it ourself so the Plugins dont need to do that.
  BaseObject* object = 0;
  if ( _identifier != -1 ) {
    if ( !PluginFunctions::getObject(_identifier,object) ) {
      emit log(LOGERR,tr("updated_objects called for non existing object with id : ") + QString::number(_identifier) );
      return;
    }
  }


  // If the identifier is -1 we force an update of all objects in the scene (Bad idea for scenes with many objects)
  if ( _identifier == -1 ) {

    std::cerr << "Warning, updated object called with -1" << std::endl;

    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_ALL ) ;  o_it != PluginFunctions::objectsEnd(); ++o_it) {

      // just inform the plugins as we don't do anything else (Do deprecated and new updatedObjects here)
      emit signalObjectUpdated(o_it->id());
      emit signalObjectUpdated(o_it->id(), _type);

      // Call the objects update function
      o_it->update();

    }
  } else {
    // just inform the plugins as we don't do anything else (Do deprecated and new updatedObjects here)
    emit signalObjectUpdated(_identifier);
    emit signalObjectUpdated(_identifier, _type);
  }


  // If we have an single object, call it's update function
  if ( object != 0 )
    object->update(_type);

  // Reenable redraws
  OpenFlipper::Options::redrawDisabled(false);


  // Reset scenegraph but keep scene center!
  resetScenegraph(false);

  updateView();
}

void Core::slotVisibilityChanged( int _id ) {

  // tell plugins
  emit visibilityChanged( _id );

  // Reset scenegraph but keep scene center!
  resetScenegraph(false);
  
  updateView();
}

/** This function is called if the active object has changed. The information is passed to all plugins.
*/
void Core::slotObjectSelectionChanged( int _id )
{
  // just inform the plugins as we don't do anything else
  emit objectSelectionChanged(_id);

  // Send via second interface
  emit signalObjectUpdated(_id,UPDATE_STATE);

  updateView();
}

void Core::slotObjectPropertiesChanged( int _id )
{
  emit objectPropertiesChanged(_id);
}

//========================================================================================
// ===             Texture Communication                       ===========================
//========================================================================================


/** Called by a plugin if it created a texture. The information is passed to all plugins. If a texture control plugin is available it has to react on the signal.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotAddTexture( QString _textureName , QString _filename, uint _dimension, int _id) {
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"addTexture( " + _textureName + "," + _filename + ","  +  QString::number(_dimension) + ", " + QString::number(_id) +  tr(" ) called by ") +
        QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,"addTexture( " + _textureName + "," + _filename + "," +  QString::number(_dimension) + ", " + QString::number(_id) +  tr(" ) called by Core") );
    }
  }
  
  emit addTexture(_textureName , _filename,_dimension,_id);
}

/** Called by a plugin if it created a texture. The information is passed to all plugins. If a texture control plugin is available it has to react on the signal.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotAddTexture( QString _textureName , QString _filename, uint _dimension) {
  
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"slotAddTexture( " + _textureName + "," + _filename + ","  +  QString::number(_dimension)  +  tr(" ) called by ") +
        QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,"slotAddTexture( " + _textureName + "," + _filename + "," +", " +  QString::number(_dimension) + tr(" ) called by Core") );
    }
  }
  
  emit addTexture(_textureName , _filename,_dimension);
}

/** Called by a plugin if a texture has to be updated. The information is passed to all plugins. The Plugin providing the given Texture should react on this event.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotUpdateTexture( QString _name , int _identifier){

  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"slotUpdateTexture( "  + _name + " , " + QString::number(_identifier) + tr(" ) called by ") +
                 QString( sender()->metaObject()->className() ) );
      }
    }
  }

  emit updateTexture(_name, _identifier);
}


void Core::slotMultiTextureAdded( QString _textureGroup , QString _name , QString _filename , int _id , int& _textureId ) {
  
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"slotMultiTextureAdded( " + _textureGroup + ", " + _name + "," + _filename + ","  + QString::number(_id) + tr(" ) called by ") +
        QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,"slotMultiTextureAdded( " + _textureGroup + ", " + _name + "," + _filename + ","  + QString::number(_id) + tr(" ) called by Core") );
    }
  }
  
  emit addMultiTexture( _textureGroup , _name , _filename , _id , _textureId  );
}

/** Called by a plugin if all textures should be updated. The information is passed to all plugins. All plugins providing textures should react on this event.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotUpdateAllTextures( ){
  emit updateAllTextures();
}

/** Called by a plugin if the parameters of a texture should be changed. The information is passed to all plugins. A Texturecontrol plugin should react on this event.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotSetTextureMode(QString _textureName, QString _mode, int _id) {

  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"slotSetTextureMode( " + _textureName + " , " + _mode + " , " + QString::number(_id) + tr(" ) called by ") +
                 QString( sender()->metaObject()->className() ) );
      }
    }
  }

  emit setTextureMode(_textureName,_mode,_id);
}

/** Called by a plugin if the parameters of a texture should be changed. The information is passed to all plugins. A Texturecontrol plugin should react on this event.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotSetTextureMode(QString _textureName ,QString _mode) {

  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"slotSetTextureMode( " + _textureName + " , " + _mode + tr(" ) called by ") +
                 QString( sender()->metaObject()->className() ) );
      }
    }
  }

  emit setTextureMode(_textureName,_mode);
}

/** Called by a plugin if it updated a texture. The information is passed to all plugins. If a texture control plugin is available it has to react on the signal and update the visualization of the texture.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotTextureUpdated( QString _textureName , int _identifier ) {
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"slotTextureUpdated( " + _textureName + " , " + QString::number(_identifier) + tr(" ) called by ") +
                 QString( sender()->metaObject()->className() ) );
      }
    }
  }
  emit updatedTextures(_textureName,_identifier);
}

/** Called by plugins if texture mode should be switched
 */
void Core::slotSwitchTexture( QString _textureName, int _id ) {
  emit switchTexture(_textureName, _id);
}

/** Called by plugins if texture mode should be switched
 */
void Core::slotSwitchTexture( QString _textureName ) {
  emit switchTexture(_textureName);
}


/** Called by plugins if texture image should be changed
 */
void Core::slotTextureChangeImage( QString _textureName , QImage& _image ) {
  emit textureChangeImage( _textureName ,_image );
}

/** Called by plugins if texture image should be changed
 */
void Core::slotTextureChangeImage( QString _textureName , QImage& _image , int _id )  {
  emit textureChangeImage( _textureName , _image , _id );
}

/** Called by plugins if texture image should be fetched
 */
void Core::slotTextureGetImage( QString _textureName , QImage& _image ) {
  emit textureGetImage( _textureName ,_image );
}

/** Called by plugins if texture image should be fetched
 */
void Core::slotTextureGetImage( QString _textureName , QImage& _image , int _id )  {
  emit textureGetImage( _textureName , _image , _id );
}

/** Called by plugins if texture index should be fetched
 */
void Core::slotTextureIndex( QString _textureName, int _id, int& _index){
  emit textureIndex( _textureName, _id, _index);
}

/** Called by plugins if texture index property name should be fetched
 */
void Core::slotTextureIndexPropertyName( int _id, QString& _propertyName){
  emit textureIndexPropertyName( _id, _propertyName);
}

/** Called by plugins if texture name should be fetched
 */
void Core::slotTextureName( int _id, int _textureIndex, QString& _textureName){
  emit textureName( _id, _textureIndex, _textureName);
}

/** Called by plugins if texture filename should be fetched
 */
void Core::slotTextureFilename( int _id, QString _textureName, QString& _textureFilename){
  emit textureFilename( _id, _textureName, _textureFilename);
}

/** Called by plugins if current texture name should be returned
 */
void Core::slotGetCurrentTexture( int _id, QString& _textureName ){
  emit getCurrentTexture( _id, _textureName );
}

/** Called by plugins if texture image should be fetched
 */
void Core::slotGetSubTextures( int _id, QString _multiTextureName, QStringList& _subTextures ){
  emit getSubTextures( _id, _multiTextureName, _subTextures );
}

//========================================================================================
// ===            Object Manager                              ============================
//========================================================================================

/// This slot is called by the object manager when a new object is created
void Core::newObject(int _objectId) {
  
   BaseObject* baseObject = 0;
   PluginFunctions::getObject(_objectId,baseObject);
   
   if ( baseObject ) {
    connect( baseObject, SIGNAL(visibilityChanged(int)),       this, SLOT(slotVisibilityChanged(int)),       Qt::DirectConnection) ;
    connect( baseObject, SIGNAL(objectSelectionChanged(int)),  this, SLOT(slotObjectSelectionChanged(int)),  Qt::DirectConnection );
    connect( baseObject, SIGNAL(objectPropertiesChanged(int)), this, SLOT(slotObjectPropertiesChanged(int)), Qt::DirectConnection );
   } else {
     emit log(LOGERR,tr("newObject received from objectManager with invalid id! This should not happen. The new Object will not work correctly!"));
   }
   
}

/// This slot is called by the object manager when a object is deleted
void Core::deletedObject(int /*_objectId*/) {
}


//========================================================================================
// ===            Cross Plugin connections                    ============================
//========================================================================================
void Core::slotCrossPluginConnect( QString _pluginName1, const char* _signal, QString _pluginName2, const char* _slot) {
  QObject* plugin1 = 0;
  QObject* plugin2 = 0;
  
  for ( int i = 0 ; i < (int)plugins_.size(); ++i ) {
    if ( plugins_[i].rpcName == _pluginName1 ) {
      plugin1 = plugins_[i].plugin;
    }
    if ( plugins_[i].rpcName == _pluginName2 ) {
      plugin2 = plugins_[i].plugin;
    }
  }

  if (  plugin1 == 0 ) {
    emit log(LOGERR,tr("Cross Plugin Interconnection failed because plugin %1 was not found!").arg(_pluginName1)); 
    return;
  }

  if (  plugin2 == 0 ) {
    emit log(LOGERR,tr("Cross Plugin Interconnection failed because plugin %1 was not found!").arg(_pluginName2)); 
    return;
  }
  
  // now connect them
  connect(plugin1,_signal,plugin2,_slot);
}

//========================================================================================
// ===            Renderer Settings                    ============================
//========================================================================================
void Core::slotSetRenderer(unsigned int _viewer, QString _rendererName) {
  renderManager().setActive(_rendererName,_viewer);
}

void Core::slotGetCurrentRenderer(unsigned int _viewer, QString& _rendererName) {
  _rendererName = renderManager().active(_viewer)->name;
}



//=============================================================================
