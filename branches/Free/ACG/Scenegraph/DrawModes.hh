/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
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
#include <bitset>

// Avoid compiler warning in MSC
#if defined (_MSC_VER)
#   pragma warning (disable:4251)
#endif

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {

/** This namespace consists of the definition of all available draw modes,  
    function to get their name (description()) and to include them in a
    QPopupMenu.
*/

namespace DrawModes {

  typedef std::bitset<64> ModeFlagSet;

  class ACGDLLEXPORT DrawMode {
    public:
      
      DrawMode(); 
      
      /** \brief constructor for unsigned int. 
      *
      * This constructor creates a DrawMode with the given drawMode index.
      * Be CareFull! this constructor can only be used to construct atomic drawmodes!
      * The unsigned int will not be handled like an bitset but really as an index.
      * See the list of draw modes below to check for the right numbers.
      * You should use the predefined drawModes or create new ones using the other functions
      * and ignore this constructor!
      */
      DrawMode(unsigned int _index); 
      
      /** \brief constructor for ModeFlags. 
      *
      * This constructor creates a DrawMode from a bitset.
      * This makes it easier to predefine draw modes using a bitset.
      */      
      DrawMode( ModeFlagSet _flags );
      
      bool operator==(const DrawMode& _mode) const;
      
      DrawMode operator&(const DrawMode& _mode) const;
      
      DrawMode& operator++();
      
      DrawMode& operator|=( const DrawMode& _mode2  );
      
      DrawMode& operator&=( const DrawMode& _mode2  );
      
      bool operator!=( const DrawMode& _mode2  ) const;
      
      DrawMode operator|( const DrawMode& _mode2  ) const;
      
      DrawMode operator^( const DrawMode& _mode2  ) const;
      
      DrawMode operator~( ) const;
      
      /** \brief get an index of the current drawMode
      *
      * If this drawMode is a combination of different drawModes, the returned value will be 0.
      * Otherwise the internal flag index will be returned
      */
      unsigned int getIndex();
      
      /** Get a description string for this DrawMode
      * An empty string is returned if this is not a valid draw mode.
      * this list has the format DrawModeName+DrawMOdeName+...
      */
      std::string description();
      
      /** \brief Check if this is an atomic draw Mode
      *
      * This function checks, if this is a atomic drawmode ( no combination of multiple draw modes )
      */
      bool isAtomic() const;
      
      /** \brief Check whether an Atomic DrawMode is active in this draw Mode
      */ 
      bool containsAtomicDrawMode( DrawMode _atomicDrawMode ) const;
      
      /** \brief Seperates this drawMode into a list of all seperate atomic drawmodes
      *
      * A drawMOde can consist of several atomic drawmodes. This function returns a list of the seperated
      * atomic drawmodes.
      */
      std::vector< DrawMode > getAtomicDrawModes();
      
      /** \brief Get the number of maximum Modes which could be handled by the current implementation
      *
      */
      unsigned int maxModes();
      
      operator bool();
      
    private:
      ModeFlagSet modeFlags_;
  };
  
  
  /// not a valid draw mode
  extern ACGDLLEXPORT DrawMode NONE;
  /// use the default (global) draw mode and not the node's own.
  extern ACGDLLEXPORT DrawMode DEFAULT;
  
  //======================================================================
  //  Point Based Rendering Modes
  //======================================================================
  /// draw unlighted points using the default base color
  extern ACGDLLEXPORT DrawMode POINTS;

  /// draw colored, but not lighted points (requires point colors)
  extern ACGDLLEXPORT DrawMode POINTS_COLORED;
  /// draw shaded points (requires point normals)
  extern ACGDLLEXPORT DrawMode POINTS_SHADED;
  
  //======================================================================
  //  Edge Based Rendering Modes
  //======================================================================
  /// draw edges
  extern ACGDLLEXPORT DrawMode EDGES;

  /// draw edges with colors (without shading)
  extern ACGDLLEXPORT DrawMode EDGES_COLORED;
  /// draw wireframe
  extern ACGDLLEXPORT DrawMode WIREFRAME;
  
  //======================================================================
  //  Face Based Rendering Modes
  //======================================================================  
  // draw faces
  extern ACGDLLEXPORT DrawMode FACES;

  /// draw hidden line (2 rendering passes needed)
  extern ACGDLLEXPORT DrawMode HIDDENLINE;
  /// draw flat shaded faces (requires face normals)
  extern ACGDLLEXPORT DrawMode SOLID_FLAT_SHADED;
  /// draw smooth shaded (Gouraud shaded) faces (requires halfedge normals)  
  extern ACGDLLEXPORT DrawMode SOLID_SMOOTH_SHADED;
  /// draw phong shaded faces
  extern ACGDLLEXPORT DrawMode SOLID_PHONG_SHADED;
  /// draw colored, but not lighted faces using face colors
  extern ACGDLLEXPORT DrawMode SOLID_FACES_COLORED;
  /// draw faces, but use Gouraud shading to interpolate vertex colors
  extern ACGDLLEXPORT DrawMode SOLID_POINTS_COLORED;
  /// draw environment mapped  
  extern ACGDLLEXPORT DrawMode SOLID_ENV_MAPPED;
  /// draw textured faces
  extern ACGDLLEXPORT DrawMode SOLID_TEXTURED;
  /// draw smooth shaded textured faces
  extern ACGDLLEXPORT DrawMode SOLID_TEXTURED_SHADED;
  /// draw textured faces
  extern ACGDLLEXPORT DrawMode SOLID_1DTEXTURED;
  /// draw smooth shaded textured faces
  extern ACGDLLEXPORT DrawMode SOLID_1DTEXTURED_SHADED;
  /// draw textured faces
  extern ACGDLLEXPORT DrawMode SOLID_3DTEXTURED;
  /// draw smooth shaded textured faces
  extern ACGDLLEXPORT DrawMode SOLID_3DTEXTURED_SHADED;
  /// draw flat shaded and colored faces (requires face normals and colors)
  extern ACGDLLEXPORT DrawMode SOLID_FACES_COLORED_FLAT_SHADED;
  /// draw per halfedge textured faces
  extern ACGDLLEXPORT DrawMode SOLID_2DTEXTURED_FACE;
  /// draw per halfedge textured faces
  extern ACGDLLEXPORT DrawMode SOLID_2DTEXTURED_FACE_SHADED;
  /// drawing is controlled by shaders
  /// ( Use shadernodes to set them before the actual object to render)
  extern ACGDLLEXPORT DrawMode SOLID_SHADER;

  //======================================================================
  //  Face Based Rendering Modes
  //======================================================================  
  // draw cells
  extern ACGDLLEXPORT DrawMode CELLS;

  /// draw cells with colors (without shading)
  extern ACGDLLEXPORT DrawMode CELLS_COLORED;


  //======================================================================
  //  Halfedge Based Rendering Modes
  //======================================================================
  /// draw halfedges
  extern ACGDLLEXPORT DrawMode HALFEDGES;

  /// draw halfedges with colors (without shading)
  extern ACGDLLEXPORT DrawMode HALFEDGES_COLORED;


  /// marks the last used ID
  extern ACGDLLEXPORT DrawMode UNUSED;
  

  //=======================================================================
  // Non Member Functions
  //=======================================================================
  
  /** Initialize the default modes.
      This function has to be called at least once at application startup.
  */
  ACGDLLEXPORT 
  void initializeDefaultDrawModes( void );

  /** \brief Add a custom DrawMode.
   *
      The id of the new draw mode is returned. If it already exists, the id of the existing one
      is returned.

      @param _name Name of the drawmode to add
      @return Id of the new draw mode
  */
  ACGDLLEXPORT 
  const DrawMode& addDrawMode( const std::string & _name);


  /** \brief Get a custom DrawMode.
   *
      The id of the draw mode is returned or if it does not exist, DrawMode::NONE is returned.

      @param _name Name of the drawmode
      @return Id of the draw mode or DrawModes::NONE
  */
  ACGDLLEXPORT 
  const DrawMode& getDrawMode( const std::string & _name);

  
  /** \brief Check if the given draw mode exists
  *
  */
  ACGDLLEXPORT   
  bool drawModeExists(const std::string & _name);
  
  /** \brief given an index of an atomic draw mode, return the drawmode
  */
  ACGDLLEXPORT 
  DrawMode getDrawModeFromIndex( unsigned int _index );

//=============================================================================
} // namespace DrawModes
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_DRAWMODES_HH defined
//=============================================================================

