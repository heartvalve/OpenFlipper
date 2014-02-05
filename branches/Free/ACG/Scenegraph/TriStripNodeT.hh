/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS TriStripNodeT
//
//=============================================================================


#ifndef ACG_TRISTRIPNODET_HH
#define ACG_TRISTRIPNODET_HH


//== INCLUDES =================================================================


#include "MeshNodeT.hh"

#include <OpenMesh/Tools/Utils/StripifierT.hh>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================



/** \class TriStripNodeT TriStripNodeT.hh <ACG/Scenegraph/TriStripNodeT.hh>

    This node draws a mesh using triangle strips.
*/

template <class Mesh>
class TriStripNodeT : public MeshNodeT<Mesh>
{
public:

  typedef OpenMesh::StripifierT<Mesh>  MyStripifier;
  typedef MeshNodeT<Mesh>              Base;
  typedef typename Base::FaceMode      FaceMode;


  /// Default constructor
  TriStripNodeT(Mesh&        _mesh,
		BaseNode*    _parent=0,
		std::string  _name="<TriStripNodeT>" )
    : Base(_mesh, _parent, _name),
      strips_(_mesh)
  {}


  /// Destructor
  ~TriStripNodeT() {}


  ACG_CLASSNAME(TriStripNodeT);


  /// build triangle strips, delete face indices
  void update_strips()
  {
    std::vector<unsigned int>().swap(Base::indices_);
    strips_.stripify();
  }


  /// build face indices, delete strips
  virtual void update_topology()
  {
    strips_.clear();
    Base::update_topology();
  }



private:

  virtual void draw_faces(FaceMode _mode)
  {
    if (Base::mesh_.is_trimesh()  &&
	     _mode == Base::PER_VERTEX &&
	     strips_.is_valid())
    {
      typename MyStripifier::StripsIterator strip_it   = strips_.begin();
      typename MyStripifier::StripsIterator strip_last = strips_.end();

      for (; strip_it!=strip_last; ++strip_it)
	          glDrawElements(GL_TRIANGLE_STRIP,
		       strip_it->size(),
		       GL_UNSIGNED_INT,
		       &(*strip_it)[0] );
    }
    else Base::draw_faces(_mode);
  }


  MyStripifier  strips_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TRISTRIPNODET_HH defined
//=============================================================================

