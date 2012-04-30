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
//  CLASS PointNode
//
//=============================================================================


#ifndef ACG_COORDSYSNODE_HH
#define ACG_COORDSYSNODE_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/GLPrimitives.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================


/** \class CoordinateSystemNode
 * \brief Node for displaying coordinate systems

 CoordinateSystemNode renders a coordinate system.

**/

class DLLEXPORT CoordinateSystemNode : public BaseNode
{

public:
  
  /** default constructor
   * @param _parent Define the parent Node this node gets attached to
   * @param _name Name of this Node
   */
  CoordinateSystemNode( BaseNode* _parent=0, std::string  _name="<TextNode>");

  /// destructor
  ~CoordinateSystemNode();

  /// static name of this class
  ACG_CLASSNAME(CoordinateSystemNode);

  /// return available draw modes
  ACG::SceneGraph::DrawModes::DrawMode  availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw Coordsys
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);
  
  /// draw Coordsys for object picking
  void pick(GLState& _state, PickTarget _target);

  /// set position of the coordsys
  void position(const Vec3d& _pos);

  /// Get current position of the coordinate system;
  Vec3d position();

  /// Get current rotation of the coordinate system;
  Matrix4x4d rotation();

  /// Set the rotation of the coordinate system;
  void rotation(const Matrix4x4d & _rotation);

  /// set size of the coordsys ( Size is length of one of the axis )
  void size(const double _size);

  /// Get current size
  double size();

  private:

    void drawCoordsys(GLState&  _state);
    void drawCoordsysPick(GLState&  _state);

    /// 3d position of the coordsys origin
    Vec3d  position_;
    
    /// Orientation of coordsys
    Matrix4x4d rotation_;

    /// Size of the coordsys
    double coordsysSize_;

    int slices_;
    int stacks_;
    int loops_;

    GLSphere* sphere_;
    GLCone* cone_;
    GLCylinder* cylinder_;
    GLDisk* disk_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_COORDSYSNODE_HH defined
//=============================================================================
