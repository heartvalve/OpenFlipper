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

#include <OpenFlipper/common/GlobalDefines.hh>

#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <OpenFlipper/common/ViewerProperties.hh>

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

/// Set the internal scenegraph root node pointer ( DO NOT USE!! )
DLLEXPORT
void setSceneGraphRootNode( SeparatorNode* _root_node );

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

}

#endif //PLUGINFUNCTIONSCORE_HH
