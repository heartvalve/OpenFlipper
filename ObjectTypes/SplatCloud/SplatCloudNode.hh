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

//================================================================
//
//  CLASS SplatCloudNode
//
//    SplatCloudNode renders splats by passing positions, normals, pointsizes and colors (and picking colors) to the GL.
//    These elements are internally stored in an array using an OpenGL vertex-buffer-object
//    including vertices, normals, texcoords and colors.
//
//================================================================


#ifndef ACG_SPLATCLOUDNODE_HH
#define ACG_SPLATCLOUDNODE_HH


//== INCLUDES ====================================================


#include "SplatCloud/SplatCloud.hh"

#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Scenegraph/BaseNode.hh>

#include <ACG/Scenegraph/DrawModes.hh>

#include <ACG/GL/gl.hh>


//== NAMESPACES ==================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION ============================================


/** \class SplatCloudNode SplatCloudNode.hh <ObjectTypes/SplatCloud/SplatCloudNode.hh>
 *
 * This class is able to render a SplatCloud as splats, dots or points
 */

class DLLEXPORT SplatCloudNode : public BaseNode
{
public:

  //-- TYPEDEFS ----------------------------------------------------

  typedef SplatCloud::Position  Position;
  typedef SplatCloud::Color     Color;
  typedef SplatCloud::Normal    Normal;
  typedef SplatCloud::Pointsize Pointsize;
  typedef SplatCloud::Index     Index;
  typedef SplatCloud::Viewlist  Viewlist;
  typedef SplatCloud::Selection Selection;

  //----------------------------------------------------------------

public:

  /// constructor
  SplatCloudNode( const SplatCloud &_splatCloud, BaseNode *_parent = 0, std::string _name = "<SplatCloudNode>" );

  /// destructor
  ~SplatCloudNode();

  ACG_CLASSNAME( SplatCloudNode );

  /// return available draw modes
  inline DrawModes::DrawMode availableDrawModes() const { return splatsDrawMode_ | dotsDrawMode_ | pointsDrawMode_; }

  /// update bounding box
  void boundingBox( ACG::Vec3d &_bbMin, ACG::Vec3d &_bbMax );

  /// draw the SplatCloud
  void draw( GLState &_state, const DrawModes::DrawMode &_drawMode );

  /// picking
  void pick( GLState &_state, PickTarget _target );

  // TODO: hack, because pick() doesn't get a drawmode
  inline void enterPick( GLState &_state, PickTarget _target, const DrawModes::DrawMode &_drawMode ) { pickDrawMode_ = _drawMode; }

  // ---- splat cloud ----

  inline const SplatCloud &splatCloud() const { return splatCloud_; }

  // ---- modification tags ----

  inline void modifiedPositions()  { positionsModified_  = true; }
  inline void modifiedColors()     { colorsModified_     = true; }
  inline void modifiedNormals()    { normalsModified_    = true; }
  inline void modifiedPointsizes() { pointsizesModified_ = true; }
  inline void modifiedSelections() { selectionsModified_ = true; }
  inline void modifiedPickColors() { pickColorsModified_ = true; }

  inline void modifiedAll()
  {
    modifiedPositions();
    modifiedColors();
    modifiedNormals();
    modifiedPointsizes();
    modifiedSelections();
    modifiedPickColors();
  }

  // ---- default values ----

  inline void setDefaultColor    ( const Color     &_color     ) { defaultColor_     = _color;     }
  inline void setDefaultNormal   ( const Normal    &_normal    ) { defaultNormal_    = _normal;    }
  inline void setDefaultPointsize( const Pointsize &_pointsize ) { defaultPointsize_ = _pointsize; }

  inline const Color     &defaultColor()     const { return defaultColor_;     }
  inline const Normal    &defaultNormal()    const { return defaultNormal_;    }
  inline const Pointsize &defaultPointsize() const { return defaultPointsize_; }

  /// if the data array exists, the entry with the given index is returned, otherwise the default value is returned
  inline const Position  &getPosition ( int _idx ) const { return splatCloud_.hasPositions()  ? splatCloud_.positions ( _idx ) : DEFAULT_POSITION;  }
  inline const Color     &getColor    ( int _idx ) const { return splatCloud_.hasColors()     ? splatCloud_.colors    ( _idx ) : defaultColor_;     }
  inline const Normal    &getNormal   ( int _idx ) const { return splatCloud_.hasNormals()    ? splatCloud_.normals   ( _idx ) : defaultNormal_;    }
  inline const Pointsize &getPointsize( int _idx ) const { return splatCloud_.hasPointsizes() ? splatCloud_.pointsizes( _idx ) : defaultPointsize_; }
  inline const Index     &getIndex    ( int _idx ) const { return splatCloud_.hasIndices()    ? splatCloud_.indices   ( _idx ) : DEFAULT_INDEX;     }
  inline const Viewlist  &getViewlist ( int _idx ) const { return splatCloud_.hasViewlists()  ? splatCloud_.viewlists ( _idx ) : DEFAULT_VIEWLIST;  }
  inline const Selection &getSelection( int _idx ) const { return splatCloud_.hasSelections() ? splatCloud_.selections( _idx ) : DEFAULT_SELECTION; }

  //----------------------------------------------------------------

private:

  // ---- splat cloud ----

  /// reference to class containing all the data
  const SplatCloud &splatCloud_;

  // ---- modification tags ----

  /// marks if parts of the data has been modified
  bool positionsModified_;
  bool colorsModified_;
  bool normalsModified_;
  bool pointsizesModified_;
  bool selectionsModified_;
  bool pickColorsModified_;

  /// return true iff any of the data values in the VBO has to be changed
  inline bool vboModified() const
  {
    return positionsModified_  ||
           colorsModified_     ||
           normalsModified_    ||
           pointsizesModified_ ||
           selectionsModified_ ||
           pickColorsModified_;
  }

  // ---- default values ----

  /// the default values will be used when the specific array is not present
  Color     defaultColor_;
  Normal    defaultNormal_;
  Pointsize defaultPointsize_;

  // ---- draw modes ----

  DrawModes::DrawMode splatsDrawMode_;
  DrawModes::DrawMode dotsDrawMode_;
  DrawModes::DrawMode pointsDrawMode_;

  // ---- picking ----

  unsigned int pickingBaseIndex_;

  // TODO: hack, see enterPick()
  DrawModes::DrawMode pickDrawMode_;

  // ---- vertex buffer object ----

  GLuint        vboGlId_;
  unsigned int  vboNumSplats_;
  unsigned char *vboData_;

  /// offsets relative to vboData_ or -1 if *not* present in VBO
  int vboPositionsOffset_;
  int vboColorsOffset_;
  int vboNormalsOffset_;
  int vboPointsizesOffset_;
  int vboSelectionsOffset_;
  int vboPickColorsOffset_;

  /// returns true iff the internal block structure of the VBO has to be changed
  inline bool vboStructureModified() const
  {
    return vboNumSplats_                != splatCloud_.numSplats()     ||
           (vboPositionsOffset_  != -1) != splatCloud_.hasPositions()  ||
           (vboColorsOffset_     != -1) != splatCloud_.hasColors()     ||
           (vboNormalsOffset_    != -1) != splatCloud_.hasNormals()    ||
           (vboPointsizesOffset_ != -1) != splatCloud_.hasPointsizes() ||
           (vboSelectionsOffset_ != -1) != splatCloud_.hasSelections();
  }

  void createVBO();
  void destroyVBO();
  void rebuildVBO( GLState &_state );

  void rebuildVBOPositions();
  void rebuildVBOColors();
  void rebuildVBONormals();
  void rebuildVBOPointsizes();
  void rebuildVBOSelections();
  void rebuildVBOPickColors( GLState &_state );

  static const Position  DEFAULT_POSITION;
  static const Index     DEFAULT_INDEX;
  static const Viewlist  DEFAULT_VIEWLIST;
  static const Selection DEFAULT_SELECTION;
};


//================================================================


} // namespace SceneGraph
} // namespace ACG


//================================================================


#endif // ACG_SPLATCLOUDNODE_HH
