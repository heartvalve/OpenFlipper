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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



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

  
  struct Primitive
  {
    Vec3d position; // position
    Vec3d axis;    // direction / axis vector

    GlutPrimitiveType type;    // glut primitive type
    
    ACG::Vec3uc color; // color
    
    // glut primitive resolution
    double       size;
    double       innersize; // size of inner loop for torus, height for cone
    unsigned int slices, stacks;
    
    // Constructor
    Primitive() 
    {
      // set default resolution
      size = 1.0;  
      innersize = 0.5;
      slices = 20;
      stacks = 20;
    }

    Primitive(GlutPrimitiveType _t, Vec3d _p, Vec3d _v, ACG::Vec3uc _c)
    {
      type     = _t;
      position = _p;
      axis     = _v;
      color    = _c;
      
      // set default resolution
      size = 1.0;  
      innersize = 0.5;
      slices = 20;
      stacks = 20;
    }

    // Copy Constructor
    Primitive( const Primitive& _p)
    {
      // use defined = operator
      *this = _p;
    }

    // = operator
    Primitive& operator=(const Primitive& _p)
    {
      type      = _p.type;
      position  = _p.position;
      axis      = _p.axis;
      color     = _p.color;
      size      = _p.size;  
      innersize = _p.innersize;
      slices    = _p.slices;
      stacks    = _p.stacks;

      return *this;
    }
  };
  
  
  GlutPrimitiveNode( BaseNode*         _parent=0,
                     std::string       _name="<GlutPrimitive>" )
    : BaseNode(_parent, _name)
  {};


  GlutPrimitiveNode( GlutPrimitiveType _type,
                     BaseNode*         _parent=0,
                     std::string       _name="<GlutPrimitive>" )
    : BaseNode(_parent, _name)
  {
    // add a single primitive of the given type
    Primitive p;
    p.type = _type;
    primitives_.push_back(p);
  }


  /// destructor
  virtual ~GlutPrimitiveNode() {}

  void add_primitive(GlutPrimitiveType _type, Vec3d _pos, Vec3d _axis, ACG::Vec3uc _color);

  void clear(){primitives_.clear();};
  
  /// set position
  void set_position(const Vec3d& _p, int _idx = 0);
  /// get position
  const Vec3d get_position(int _idx = 0) const;

  /// get a primitive
  Primitive& get_primitive(int _idx){return primitives_[_idx];};
  
  /// set size
  void set_size(double _s, int _idx = 0);
  /// get size
  double get_size(int _idx = 0) const;
  
  ACG_CLASSNAME(GlutPrimitiveNode);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);
  
  /// drawing the primitive
  void draw(GLState& _state, DrawModes::DrawMode _drawMode);
  void draw_obj(int _idx) const;
  
  /// picking
  void pick(GLState& _state, PickTarget _target);

private:
  
  std::vector<Primitive> primitives_;
  
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_GLUT_PRIMITIVE_NODE_HH
//=============================================================================
