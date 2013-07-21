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
//  CLASS FastMesh
//
//=============================================================================


#ifndef ACG_OBJ_NODE_HH
#define ACG_OBJ_NODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "DrawModes.hh"
#include "../Math/VectorT.hh"
#include <string>
#include <vector>


//== FORWARD DECLARATION ======================================================


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT OBJNode : public BaseNode
{

public:

  /// Default constructor.
  OBJNode( BaseNode*         _parent=0,
	   std::string       _name="<OBJNode>" )
    : BaseNode(_parent, _name)
  {}


  /// destructor
  virtual ~OBJNode() {}


  ACG_CLASSNAME(OBJNode);


  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// drawing the primitive
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// picking
  void pick(GLState& _state, PickTarget _target);


  struct Face
  {
    Face(int _i0=-1, int _i1=-1, int _i2=-1,
	 int _t0=-1, int _t1=-1, int _t2=-1)
      : i0(_i0), i1(_i1), i2(_i2),
        t0(_t0), t1(_t1), t2(_t2) {}
    int i0, i1, i2, t0, t1, t2;
  };


  /// number of vertices
  size_t n_vertices() const { return vertices_.size(); }
  /// number of faces
  size_t n_faces()    const { return faces_.size(); }
  /// number of normals
  size_t n_normals()  const { return normals_.size(); }
  /// number of texcoords
  size_t n_texcoords()  const { return texCoords_.size(); }


  /// clear the node
  void clear()
  {
    vertices_.clear();
    faces_.clear();
    normals_.clear();
    texCoords_.clear();
  }


  /// add vertex
  size_t add_vertex(const Vec3f& _v)
  {
    vertices_.push_back(_v);
    return vertices_.size()-1;
  }


  /// add triangle
  size_t add_face(const Face& _f)
  {
    faces_.push_back(_f);
    return faces_.size()-1;
  }

  /// add triangle
  size_t add_face(unsigned int _i0,
			unsigned int _i1,
			unsigned int _i2)
  {
    faces_.push_back(Face(_i0, _i1, _i2));
    return faces_.size()-1;
  }


  /// get i'th vertex
  Vec3f& vertex(unsigned int _i)
  {
    assert(_i < n_vertices());
    return vertices_[_i];
  }
  /// get i'th vertex
  const Vec3f& vertex(unsigned int _i) const
  {
    assert(_i < n_vertices());
    return vertices_[_i];
  }


  /// get i'th face
  const Face&  face(unsigned int _i) const
  {
    assert(_i < n_faces());
    return faces_[_i];
  }
  /// get i'th face
  Face&  face(unsigned int _i)
  {
    assert(_i < n_faces());
    return faces_[_i];
  }


  /// get i'th normal
  const Vec3f& normal(unsigned int _i) const
  {
    assert(_i < n_normals());
    return normals_[_i];
  }
  /// get i'th normal
  Vec3f& normal(unsigned int _i)
  {
    assert(_i < n_normals());
    return normals_[_i];
  }


  /// Read from file. Implemented using OpenMesh loader and OBJNodeExporter.
  bool read(const std::string& _filename);


  /// Update face normals. Call when geometry changes
  void update_face_normals();


private:

  void draw_obj() const;
  void draw_obj_tex() const;

  std::vector<Vec3f> vertices_;
  std::vector<Vec3f> normals_;
  std::vector<Vec2f> texCoords_;
  std::vector<Face>  faces_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_OBJ_NODE_HH
//=============================================================================
