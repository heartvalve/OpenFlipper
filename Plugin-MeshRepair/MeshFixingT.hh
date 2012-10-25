/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         *
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
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


#ifndef OPENMESH_MESHFIXING_HH
#define OPENMESH_MESHFIXING_HH

//=============================================================================
//
//  CLASS MeshFixing
//
//=============================================================================

//== INCLUDES =================================================================

//== NAMESPACE ================================================================

//== CLASS DEFINITION =========================================================

/** \brief Compare two vectors
 *
 * If they are closer than a specified epsilon, we identify them as equal.
 *
 */
class CompareVectors
{
public:

  /// Constructor
  CompareVectors(double _eps = FLT_MIN);

  /// comparison operator
  bool operator()( const ACG::Vec3d& _v0, const ACG::Vec3d& _v1 ) const;

private:
  const double epsilon_;
};


/** \brief Fix a mesh
 *
 */
template<class MeshT>
class MeshFixing {

public:

  MeshFixing(MeshT& _mesh, double _epsilon );

  bool fix();


private:

  TriMesh& mesh_;


  /** \brief Add a face to the fixing data
   *
   * Degenerated faces will already be discarded in this step
   *
   * @param _p0 Vertex 1
   * @param _p1 Vertex 2
   * @param _p2 Vertex 3
   */
  void add_face(const ACG::Vec3d& _p0, const ACG::Vec3d& _p1, const ACG::Vec3d& _p2);

  void fix_topology();
  void fix_orientation();

  int neighbor(unsigned int _f, unsigned int _v0, unsigned int _v1);



  /// Internal vertex type
  struct Vertex
  {
    Vertex(const ACG::Vec3d& _p) : p(_p) { };
    ACG::Vec3d                    p;

    /// This vector will contain a list of all faces incident to the current vertex
    std::vector<unsigned int>     faces;
  };

  /// Internal face type
  struct Face
  {
    Face(int _i0, int _i1, int _i2) :
            component(-1)
    {
      v[0] = _i0;
      v[1] = _i1;
      v[2] = _i2;

    }

    unsigned int  v[3];
    int  component;
  };


  typedef std::map<ACG::Vec3d, int, CompareVectors>  VertexMap;

  typedef VertexMap::iterator MapIter;

  std::vector<Vertex>  vertices_;
  std::vector<Face>    faces_;


  /** Add a vertex to the map
   *
   * @param   _p New vertex
   * @return Index of the vertex
   */
  int add_vertex(const ACG::Vec3d& _p);

  VertexMap            vmap_;

};

#if defined(INCLUDE_TEMPLATES) && !defined(OPENMESH_MESHFIXING_CC)
#define OPENMESH_MESHFIXING_TEMPLATES
#include "MeshFixingT.cc"
#endif

#endif

