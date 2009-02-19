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
//   $Revision: 4482 $
//   $Author: moebius $
//   $Date: 2009-01-28 11:12:14 +0100 (Mi, 28. Jan 2009) $
//
//=============================================================================




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

/// Set the internal data root node pointer ( DO NOT USE!! )
DLLEXPORT
void setRootNode( SeparatorNode* _root_node );

/** @} */

}

#endif //PLUGINFUNCTIONSCORE_HH
