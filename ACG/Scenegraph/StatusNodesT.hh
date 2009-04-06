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
//  Status Nodes
//
//=============================================================================


#ifndef ACG_STATUS_NODES_HH
#define ACG_STATUS_NODES_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/Mesh/Attributes.hh>

#include "MaterialNode.hh"
#include "DrawModes.hh"

#include <vector>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class StatusNodeT StatusNodesT.hh <ACG/Scenegraph/StatusNodesT.hh>
 *
 *     Renders Status flags of Mesh Vertices/Faces/Edges
 *
 *             **/
template <class Mesh, class Mod>
class StatusNodeT : public MaterialNode
{
public:

  /// constructor
  StatusNodeT( const Mesh&         _mesh,
               BaseNode*           _parent = 0,
               const std::string&  _name   = "<StatusNode>" );

  /// destructor
  ~StatusNodeT() {}

  ACG_CLASSNAME(StatusNode);


  /** build/update cache of active vertices/edges/faces. This function
      automatically enables caching.
  */
  void update_cache();


  unsigned int availableDrawModes() const;
  void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);
  void draw(GLState& _state, unsigned int _drawMode);
  void pick(GLState& /* _state */ , PickTarget /* _target */ ) {}


private:

  typedef typename Mesh::Face        Face;
  typedef typename Mesh::Vertex      Vertex;
  typedef typename Mesh::Halfedge    Halfedge;
  typedef typename Mesh::Edge        Edge;
  typedef typename Mesh::FaceHandle  FaceHandle;

  void draw_points();
  void draw_edges();
  void draw_faces(bool _per_vertex);


private:

  const Mesh&                mesh_;
  std::vector<unsigned int>  v_cache_, e_cache_, f_cache_;
  std::vector<FaceHandle>    fh_cache_;

  // bounding box
  Vec3f bbMin_;
  Vec3f bbMax_;
};



//== CLASS DEFINITION =========================================================


template <class Mesh, unsigned int Bit>
struct StatusModT
{
  static bool is_vertex_selected(const Mesh& _mesh,
                                 typename Mesh::VertexHandle _vh)
  {
    return _mesh.status(_vh).is_bit_set(Bit);
  }

  static bool is_edge_selected(const Mesh& _mesh,
                               typename Mesh::EdgeHandle _eh)
  {
    return _mesh.status(_eh).is_bit_set(Bit);
  }

  static bool is_face_selected(const Mesh& _mesh,
                               typename Mesh::FaceHandle _fh)
  {
    return _mesh.status(_fh).is_bit_set(Bit);
  }
};



//== CLASS DEFINITION =========================================================


template <class Mesh>
struct SelectionModT
  : public StatusModT<Mesh, OpenMesh::Attributes::SELECTED>
{};



/** \class SelectionNodeT StatusNodesT.hh <ACG/Scenegraph/StatusNodesT.hh>
 *
 *     Renders the Selection status of Mesh Vertices/Faces/Edges
 *
 *             **/
template <class Mesh>
class SelectionNodeT
  : virtual public StatusNodeT<Mesh, SelectionModT<Mesh> >
{
public:

  /** \brief Constructor
   * @param _mesh reference to mesh with status property
   * @param _parent Parent node in the scenegraph
   * @param _name Name of the Node
   */
  SelectionNodeT( const Mesh&         _mesh,
                  BaseNode*           _parent = 0,
                  const std::string&  _name   = "<SelectionNode>" )
    : StatusNodeT<Mesh, SelectionModT<Mesh> > (_mesh, _parent, _name)
  {}
};


//== CLASS DEFINITION =========================================================


template <class Mesh>
struct LockModT
  : public StatusModT<Mesh, OpenMesh::Attributes::LOCKED>
{};


template <class Mesh>
class LockNodeT : public StatusNodeT<Mesh, LockModT<Mesh> >
{
public:

  LockNodeT( const Mesh&         _mesh,
             BaseNode*           _parent = 0,
             const std::string&  _name   = "<LockNode>" )
    : StatusNodeT<Mesh, LockModT<Mesh> > (_mesh, _parent, _name)
  {}
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_STATUS_NODES_C)
#define ACG_STATUS_NODES_TEMPLATES
#include "StatusNodesT.cc"
#endif
//=============================================================================
#endif // ACG_STATUS_NODES_HH defined
//=============================================================================

