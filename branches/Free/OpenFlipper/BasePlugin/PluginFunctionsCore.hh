/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
//  Standard Functions
//
//=============================================================================

/**
 * \file PluginFunctions.hh
 * This file contains functions to setup the internal structures for PluginFunctions.
 * Dont Use these Functions in your Plugins!
 */

//
#ifndef PLUGINFUNCTIONSCORE_HH
#define PLUGINFUNCTIONSCORE_HH

#ifndef OPENFLIPPERCORE
  #ifndef OPENFLIPPERPLUGINLIB
    #ifdef WIN32
      #pragma message("PluginFunctionsCore is only allowed to be used from within OpenFlippers Core application!")
      
    #else
      #warning PluginFunctionsCore is only allowed to be used from within OpenFlippers Core application!
      #error Bla
    #endif
  #endif
#endif

#include <OpenFlipper/common/GlobalDefines.hh>

#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <OpenFlipper/common/ViewerProperties.hh>

#include <ACG/QtWidgets/QtSceneGraphWidget.hh>

namespace PluginFunctions {

//=======================================
// Set pointers for global handling in PluginFunctions without exporting them to the Plugins
    /** @name Setup Functions
    * @{ */
//=======================================
/// Set the internal Viewer pointer ( DO NOT USE!! )
DLLEXPORT
void setViewers( std::vector< glViewer* > _viewerWidgets );

/// Get a Viewer
DLLEXPORT
glViewer* viewer(int  _viewerId );

/// Set the internal viewerProperties pointer ( DO NOT USE!! )
DLLEXPORT
void setViewerProperties( std::vector< Viewer::ViewerProperties* > _viewerProperties );

/** Set the internal scenegraph root node pointer. This is the topmost
* node of the whole scenegraph ( DO NOT USE!! )
*/
DLLEXPORT
void setSceneGraphRootNode( SeparatorNode* _root_node );

/** Set the internal scenegraph root node pointer for global nodes 
*
* This node is below the global status nodes and takes global nodes
* at one level. ( DO NOT USE!! )
*/
DLLEXPORT
void setSceneGraphRootNodeGlobal( SeparatorNode* _root_node );

/// Set the internal data root node pointers ( DO NOT USE!! )
DLLEXPORT
void setDataSeparatorNodes( SeparatorNode* _dataRootNode );

/** @} */

//=======================================
// Handle object count internally
/** @name Internal Counters
* @{ */
//=======================================
/// Decrease the number of current Object
DLLEXPORT
void increaseObjectCount();

/// Increase the number of current Object
DLLEXPORT
void decreaseObjectCount();

/// Decrease the number of current Object
DLLEXPORT
void increaseTargetCount();

/// Increase the number of current Object
DLLEXPORT
void decreaseTargetCount();

/** @} */

//=======================================
// Object container functions
/** @name Internal object handle container
* @{ */
//=======================================

/// Add object to internal object map
DLLEXPORT
void addObjectToMap(int _objectId, BaseObject* _object);

/// Remove object from internal object map
DLLEXPORT
void removeObjectFromMap(int _objectId);

/** @} */

//=======================================
// SceneGraph Generator Map
/** @name Internal container for scenegraph widget generators
* @{ */
//=======================================

/// Add a scenegraph generator ( the handled type will be extracted from the generator)
DLLEXPORT 
void addSceneGraphGenerator(ACG::QtWidgets::SceneGraphWidgetGenerator* _generator);

DLLEXPORT
QMap< std::string ,ACG::QtWidgets::SceneGraphWidgetGenerator* > getSceneGraphGeneratorList();

/** @} */

}

#endif //PLUGINFUNCTIONSCORE_HH
