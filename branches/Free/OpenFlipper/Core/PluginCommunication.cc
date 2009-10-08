/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
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

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

//== IMPLEMENTATION ==========================================================

//========================================================================================
// ===             Object List Communication                       =======================
//========================================================================================

/** This function is called by a plugin if it changed something in the object list (source,target,...). The information is passed to all plugins.
 * @param _identifier Id of the updated object
 */
void Core::slotObjectUpdated(int _identifier) {
  if ( OpenFlipper::Options::doSlotDebugging() ) {
    if ( sender() != 0 ) {
      if ( sender()->metaObject() != 0 ) {
        emit log(LOGINFO,"updatedObject( " + QString::number(_identifier) + tr(" ) called by ") +
                 QString( sender()->metaObject()->className() ) );
      }
    } else {
      emit log(LOGINFO,"updatedObject( " + QString::number(_identifier) + tr(" ) called by Core") );
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

  // just inform the plugins as we dont do anything else
  emit signalObjectUpdated(_identifier);

  if ( object != 0 )
    object->update();

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
  // just inform the plugins as we dont do anything else
  emit objectSelectionChanged(_id);

//   std::cerr << "objectSelection changed triggers updatedObjects for now" << std::endl;
//   emit updatedObject(_id);
}

void Core::slotObjectPropertiesChanged( int _id )
{
  emit objectPropertiesChanged(_id);
}

//====================================================================================
// ===             Toolbox handling                                   =================
//=====================================================================================

 /** If the Toolbox is switched the toolboxActivated function of the each plugin that has a toolbox is called by this function.
  */
// void
// Core::slotToolboxSwitched(int _index)
// {
//    // Check all plugins
//    for ( uint i = 0 ; i < plugins.size(); ++i) {
//       if (plugins[i].toolboxindex == -1)
//          continue;
//
//       ToolboxInterface* toolboxPlugin = qobject_cast< ToolboxInterface * >(plugins[i].plugin);
//       if (! toolboxPlugin)
//          continue;
//
//       // Call the corresponding function of the plugins
//       if ( _index == plugins[i].toolboxindex ) {
//            toolboxPlugin->toolboxActivated(true);
//       } else {
//            toolboxPlugin->toolboxActivated(false);
//       }
//    }
// }

//========================================================================================
// ===             Texture Communication                       ===========================
//========================================================================================


/** Called by a plugin if it created a texture. The information is passed to all plugins. If a texture control plugin is available it has to react on the signal.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotAddTexture( QString _textureName , QString _filename, uint _dimension, int _id) {
  emit addTexture(_textureName , _filename,_dimension,_id);
}

/** Called by a plugin if it created a texture. The information is passed to all plugins. If a texture control plugin is available it has to react on the signal.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotAddTexture( QString _textureName , QString _filename, uint _dimension) {
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

//========================================================================================
// ===             Backup Communication                       ============================
//========================================================================================

/// Called if a backup is requested by the plugins
void Core::backupRequest( int _id , QString _name ) {
  emit createBackup(  _id , _name , nextBackupId_);
  ++nextBackupId_;
}





//=============================================================================
