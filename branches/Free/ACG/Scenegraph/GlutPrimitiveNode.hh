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
//  CLASS GlutPrimitiveNode
//
//=============================================================================


#ifndef ACG_GLUT_PRIMITIVE_NODE_HH
#define ACG_GLUT_PRIMITIVE_NODE_HH


//== INCLUDES =================================================================


#include "BaseNode.hh"
#include "DrawModes.hh"
#include <string>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class GlutPrimitiveNode GlutPrimitiveNode.hh <ACG/Scenegraph/GlutPrimitiveNode.hh>

    This class is able to render all glut primitives (listed in
    GlutPrimitiveType).
**/

class ACGDLLEXPORT GlutPrimitiveNode : public BaseNode
{

public:

  /// Lists all available primivites
  enum GlutPrimitiveType
  {
    CONE=0, 
    CUBE, 
    DODECAHEDRON, 
    ICOSAHEDRON, 
    OCTAHEDRON, 
    SPHERE, 
    TEAPOT, 
    TETRAHEDRON, 
    TORUS
  }; 
  
  
  /// Default constructor.
  GlutPrimitiveNode( GlutPrimitiveType _type,
		     BaseNode*         _parent=0,
		     std::string       _name="<GlutPrimitive>" )
    : BaseNode(_parent, _name),
      type_(_type),
      size_(1.0),  
      innersize_(0.5),
      slices_(20), 
      stacks_(20),
      position_(0.0, 0.0, 0.0)
  {}
  

  /// destructor
  virtual ~GlutPrimitiveNode() {}

  /// set position
  void set_position(const Vec3f& _p) { position_ = _p; }
  /// get position
  const Vec3f& get_position() const { return position_; }

  /// set size
  void set_size(float _s) { size_ = _s; }
  /// get size
  float get_size() const { return size_; }

  
  ACG_CLASSNAME(GlutPrimitiveNode);


  /// return available draw modes
  unsigned int availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);
  
  /// drawing the primitive
  void draw(GLState& _state, unsigned int _drawMode);
  void draw_obj() const;
  /// picking
  void pick(GLState& _state, PickTarget _target);


private:
  
  GlutPrimitiveType  type_;
  float              size_, innersize_;
  unsigned int       slices_, stacks_;
  Vec3f              position_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_GLUT_PRIMITIVE_NODE_HH
//=============================================================================
