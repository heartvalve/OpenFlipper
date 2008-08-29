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

//== IMPLEMENTATION ========================================================== 

//========================================================================================
// ===             Object List Communication                       =======================
//========================================================================================   
   
/** This function is called by a plugin if it changed something in the object list (source,target,...). The information is passed to all plugins.
 * @param _identifier Id of the updated object
 */
 void Core::slotObjectsListUpdated(int _identifier) {
      // just inform the plugins as we dont do anything else
      emit ObjectListUpdated(_identifier);
 }
 
 /** This function is called if the active object has changed. The information is passed to all plugins.
 */
 void Core::slotActiveObjectChanged()
 {
    // just inform the plugins as we dont do anything else
    emit activeObjectChanged();
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
void Core::slotAddTexture( QString _textureName , QString _filename, uint _dimension) {
   emit addTexture(_textureName , _filename,_dimension);
}

/** Called by a plugin if a texture has to be updated. The information is passed to all plugins. The Plugin providing the given Texture should react on this event.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotUpdateTexture( QString _name , int _identifier){
   emit updateTexture(_name, _identifier);
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
void Core::slotSetTextureMode(QString _textureName ,QString _mode) {
  emit setTextureMode(_textureName,_mode);  
}

/** Called by a plugin if it updated a texture. The information is passed to all plugins. If a texture control plugin is available it has to react on the signal and update the visualization of the texture.\n
 * See in the documentation of the texture plugin interfaces for further detail.
*/
void Core::slotTextureUpdated( QString _textureName , int _identifier ) {
  emit updatedTextures(_textureName,_identifier);  
}

/** Called by plugins if texture mode should be switched
 */
void Core::slotSwitchTexture( QString _textureName ) {
  emit switchTexture(_textureName);
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
