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
 * This file contains functions which can be used by plugins to access data in the framework.
 */

//
#ifndef PLUGINFUNCTIONS_VIEWCONTROL_HH
#define PLUGINFUNCTIONS_VIEWCONTROL_HH

#include <OpenFlipper/common/Types.hh>

#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/GLState.hh>
#include <OpenFlipper/common/ViewerProperties.hh>

/** The Namespace PluginFunctions contains functions for all plugins. These functions should be used to get the
 *  objects to work on or to set modes in the examiner widget. */
namespace PluginFunctions {

const int ALL_VIEWERS   = -2;
const int ACTIVE_VIEWER = -1;

const int VIEW_FREE   = 0;
const int VIEW_TOP    = 1;
const int VIEW_BOTTOM = 2;
const int VIEW_LEFT   = 3;
const int VIEW_RIGHT  = 4;
const int VIEW_FRONT  = 5;
const int VIEW_BACK   = 6;

//=======================================
// Get/set status of viewers
    /** @name Viewer Status and properties
    * @{ */
//=======================================

/** \brief Get the viewer properties
 *  Use this functions to get basic viewer properties such as backgroundcolor or rendering options.
 *
 * @param _id Which viewer should be used? ACTIVE_VIEWER will get the properties of the active Viewer which is the default
 */
DLLEXPORT
Viewer::ViewerProperties& viewerProperties(int _id = ACTIVE_VIEWER );

/** @} */


//=======================================
// View settings
    /** @name View settings
    * @{ */
//=======================================

/** Lock scene rotation via mouse
 *
 * @param _mode allow or disallow rotation
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void allowRotation(bool _mode, int _viewer = ACTIVE_VIEWER);

/** is scene rotation locked
 *
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
bool allowRotation(int _viewer = ACTIVE_VIEWER);

/** \brief Set current GL Context to main context
 */
DLLEXPORT
void setMainGLContext();


/** \brief Set the viewing direction
 * @param _dir direction
 * @param _up up vector
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void viewingDirection(const ACG::Vec3d &_dir, const ACG::Vec3d &_up  , int _viewer = ACTIVE_VIEWER);

/** \brief Set the look at transformation directly
* @param _eye eye point
* @param _center center point
* @param _up up vector
* @param _viewer Id of the viewer to use.
*                ALL_VIEWERS    will set all viewers (Default)
*                ACTIVE_VIEWER active viewer
*                0..3 Choose viewer explicitly
*/
DLLEXPORT
void lookAt(const ACG::Vec3d& _eye, const ACG::Vec3d& _center, const ACG::Vec3d& _up, int _viewer = ACTIVE_VIEWER);

/** \brief Set the Scene position
 * @param _center Center of the current scene
 * @param _radius Radius of the scene ( Use scene_radius to get the current radius )
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void setScenePos(const ACG::Vec3d& _center, const double _radius  , int _viewer = ALL_VIEWERS);

/** \brief Set the trackball Center
*
* The scene is rotated around the trackball center when using the mouse 
*
* @param _center Center of the trackball
* @param _viewer Id of the viewer to use.
*                ALL_VIEWERS    will set all viewers (Default)
*                ACTIVE_VIEWER active viewer
*                0..3 Choose viewer explicitly
*/
DLLEXPORT
void setTrackBallCenter(const ACG::Vec3d& _center, int _viewer );


/** \brief Get the trackball Center
*
* The scene is rotated around the trackball center when using the mouse 
*
* @param _viewer Id of the viewer to use.
*                ALL_VIEWERS    will set all viewers (Default)
*                ACTIVE_VIEWER active viewer
*                0..3 Choose viewer explicitly
*/
DLLEXPORT
const ACG::Vec3d trackBallCenter( int _viewer );

/** \brief Set the scene position
 *
 * Same as  setScenePos(const ACG::Vec3d& _center, double _radius) but reuses the current radius
 *
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 * @param _center Center of the current scene
 */
DLLEXPORT
void setScenePos(const ACG::Vec3d& _center  , int _viewer = ALL_VIEWERS);

/** \brief Get the current scene center
 *
 */
DLLEXPORT
const ACG::Vec3d sceneCenter( int _viewer = ALL_VIEWERS );

/** \brief Returns the current scene radius from the active examiner widget
 *
 * Returns the Radius of the scene
 */
DLLEXPORT
double sceneRadius();

DLLEXPORT
/** \brief Returns the current scene radius from a given examiner viewer
*
* Returns the Radius of the scene
* @param _viewer Give the viewer which should be asked for its current scene radius
*/
double sceneRadius( int _viewer );

DLLEXPORT
void setSceneRadius(double _radius , int _viewer = ALL_VIEWERS);

/** \brief Translate viewer pos by given vector
 *
 * Translates the scene by a given vector. ( This is only a view transformation and does not
 * effect the scene center. To really translate the scene, use setScenePos );
 * @param _vector translation
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void translate( const ACG::Vec3d &_vector  , int _viewer = ALL_VIEWERS);

/** \brief Rotate Scene around axis
 *
 * Rotates the current scene.
 * @param _axis   Rotation axis
 * @param _angle  Rotation Angle
 * @param _center Rotation Center
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void rotate(const ACG::Vec3d&  _axis,
            const double       _angle,
            const ACG::Vec3d&  _center,
            int                _viewer = ALL_VIEWERS);


/** \brief Go to home position
 *
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void viewHome(int _viewer = ALL_VIEWERS);

/** \brief View the whole scene
 *
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void viewAll(int _viewer = ALL_VIEWERS);

/** \brief Get the current viewing Direction
 *
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
ACG::Vec3d viewingDirection(int _viewer = ACTIVE_VIEWER);

/** \brief Check if the projection is orthographic
 * @param _viewer Id of the viewer to use.
 *                ALL_VIEWERS    will set all viewers (Default)
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
bool isProjectionOrthographic( int _viewer = ACTIVE_VIEWER );

/** \brief Get the current viewer position
 *
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
ACG::Vec3d eyePos(int _viewer = ACTIVE_VIEWER);

/** \brief Get the current up vector
 *
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
ACG::Vec3d upVector(int _viewer = ACTIVE_VIEWER);

/** \brief Switch to orthographic Projection
 *
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void orthographicProjection( int _viewer = ALL_VIEWERS );

/** \brief Set field of view angle
 *
 * @param _fovy   Field of view (in (1, 180) degrees)
 */
DLLEXPORT
void setFOVY( double _fovy );

/** \brief Get field of view angle
 */
DLLEXPORT
double fovy(int _viewer = ACTIVE_VIEWER);

/** \brief Switch to perspective Projection
 *
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void perspectiveProjection( int _viewer = ALL_VIEWERS );

/** \brief Set the draw Mode of a Viewer.\n
 *
 * The DrawModes are defined at ACG/Scenegraph/DrawModes.hh \n
 * They can be combined.
 *
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 *
 * @param _mode New drawmode of the given viewer
 */
DLLEXPORT
void setDrawMode( const ACG::SceneGraph::DrawModes::DrawMode _mode , int _viewer = ALL_VIEWERS);

/** \brief Get the current draw Mode of a Viewer
 *
 *
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
ACG::SceneGraph::DrawModes::DrawMode drawMode( int _viewer = ACTIVE_VIEWER );


/** \brief Set the background color of the examiner widget.
 *
 */
DLLEXPORT
void setBackColor( OpenMesh::Vec4f _color);

/** \brief Set a fixed View for a viewer
 *
 *
 * @param _mode id for the mode
 * @param _viewer Id of the viewer to use.
 *                ACTIVE_VIEWER active viewer
 *                0..3 Choose viewer explicitly
 */
DLLEXPORT
void setFixedView(int _mode, int _viewer = ACTIVE_VIEWER );


/** @} */


//=======================================
// Do animations in examiner viewer
    /** @name Animations
    * @{ */
//=======================================

/**  Fly to point and set new viewing direction (animated).
 * @param _position New viewer position ( the new eye point of the viewer )
 * @param _center   The new scene center ( the point we are looking at )
 * @param _time     Animation time in ms
 */
DLLEXPORT
void flyTo (const ACG::Vec3d &_position, const ACG::Vec3d &_center, double _time=1000.0);


/**  Fly to point and keep viewing direction (animated).
 * @param _center    The new scene center ( the point we are looking at )
 * @param _move_back Get closer if \c _move_back=\c true, get more distant else.
 * @param _time      Animation time in ms
 */
DLLEXPORT
void flyTo (const ACG::Vec3d &_center, bool _move_back = true, double _time=1000.0);


/** @} */

//=======================================
// Provide snapshot functions
    /** @name Snapshots
    * @{ */
//=======================================

/**  Take a snapshot of the current view
 * @param _viewer   Viewer id
 * @param _image    An image to store pixel data in
 * @param _width    Image width 
 * @param _height   Image height
 * @param _alpha    Make background transparent
 * @param _hideCoordsys   Hide coordsys node
 * @param _samples  Number of samples (multisampling)
 */
DLLEXPORT
void viewerSnapshot(int _viewer, QImage& _image, int _width = 0, int _height = 0,
                    bool _alpha = false, bool _hideCoordsys = false, int _samples = 1);

/** @} */

}

#endif //PLUGINFUNCTIONS_HH
