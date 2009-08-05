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
//  CLASS DrawModes
//
//=============================================================================

#ifndef ACG_DRAWMODES_HH
#define ACG_DRAWMODES_HH


//== FORWARD DECLARATIONS =====================================================


namespace ACG 
{
  namespace SceneGraph 
  {
    class BaseNode;
  }
}


//== INCLUDES =================================================================

#include <string>
#include <vector>
#include "../Config/ACGDefines.hh"

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {

/** This namespace consists of the definition of all available draw modes,  
    function to get their name (description()) and to include them in a
    QPopupMenu.
*/

namespace DrawModes {



  /** Lists all basic draw modes. The function BaseNode::availableDrawModes() 
      returns OR'ed items of this enum */
  enum DrawModeIDs
  {
    /// not a valid draw mode
    NONE                         = 0, 
    /// use the default (global) draw mode and not the node's own.
    DEFAULT                      = 1,

    /// draw unlighted points using the default base color
    POINTS                       = 1<<1,
    /// draw colored, but not lighted points (requires point colors)
    POINTS_COLORED               = 1<<2,
    /// draw shaded points (requires point normals)
    POINTS_SHADED                = 1<<3,

    /// draw wireframe
    WIREFRAME                    = 1<<4,
    /// draw hidden line (2 rendering passes needed)
    HIDDENLINE                   = 1<<5,

    /// draw flat shaded faces (requires face normals)
    SOLID_FLAT_SHADED            = 1<<6,
    /// draw smooth shaded (Gouraud shaded) faces (requires halfedge normals)
    SOLID_SMOOTH_SHADED          = 1<<7,
    /// draw phong shaded faces
    SOLID_PHONG_SHADED           = 1<<8,

    /// draw colored, but not lighted faces using face colors
    SOLID_FACES_COLORED          = 1<<9,
    /// draw faces, but use Gouraud shading to interpolate vertex colors
    SOLID_POINTS_COLORED         = 1<<10,

    /// draw environment mapped
    SOLID_ENV_MAPPED             = 1<<11,

    /// draw textured faces
    SOLID_TEXTURED               = 1<<12,
    /// draw smooth shaded textured faces
    SOLID_TEXTURED_SHADED        = 1<<13,

    /// draw textured faces
    SOLID_1DTEXTURED             = 1<<14,
    /// draw smooth shaded textured faces
    SOLID_1DTEXTURED_SHADED      = 1<<15,

    /// draw textured faces
    SOLID_3DTEXTURED             = 1<<16,
    /// draw smooth shaded textured faces
    SOLID_3DTEXTURED_SHADED      = 1<<17,


    /// draw flat shaded and colored faces (requires face normals and colors)
    SOLID_FACES_COLORED_FLAT_SHADED    = 1<<18,

    /// draw per halfedge textured faces
    SOLID_2DTEXTURED_FACE        = 1<<19,

    /// draw per halfedge textured faces
    SOLID_2DTEXTURED_FACE_SHADED = 1<<20,
    
    /// drawing is controlled by shaders
    /// ( Use shadernodes to set them before the actual object to render)
    SOLID_SHADER                 = 1 <<21,

    /// marks the last used ID
    UNUSED                       = 1 <<22
  };




  /** Get a description string for \c _drawMode.
      An empty string is returned if \c _drawMode is not a
      valid draw mode.
   */
  ACGDLLEXPORT 
  std::string description(unsigned int _drawMode);

  
  /** Initialize the default modes.
      This function has to be called at least once at application startup.
  */
  ACGDLLEXPORT 
  void initializeDefaultDrawModes( void );


  /** Add a custom DrawMode.
      The new ID is returned in the variable \c _newId. A false return value
      indicates that the addition failed because all available IDs 
      are occupied.
  */
  ACGDLLEXPORT 
  bool addDrawMode( const std::string & _name, unsigned int & _newId );


  /** Get the Id of a DrawMode.
      The Id of the DrawMode with name \c _name is returned in variable \c _Id.
      Returns false if the DrawMode could not been found.
  */
  ACGDLLEXPORT 
  bool getDrawModeId( const std::string & _name, unsigned int & _Id  );


  /** Get all draw mode IDs that are encoded in _drawMode
  */
  ACGDLLEXPORT 
  std::vector< unsigned int > getDrawModeIDs( unsigned int _drawMode );


  /** Check whether an ID is encoded in _drawMode
  */
  ACGDLLEXPORT 
  bool containsId( unsigned int _drawMode, unsigned int _id );

//=============================================================================
} // namespace DrawModes
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_DRAWMODES_HH defined
//=============================================================================

