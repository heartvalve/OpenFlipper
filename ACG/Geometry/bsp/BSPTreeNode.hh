/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Wed, 26 Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  CLASS TreeNode
//
//=============================================================================

#ifndef MB_BSPTREENODE_HH
#define MB_BSPTREENODE_HH

//== INCLUDES =================================================================

#include <ACG/Geometry/Types/PlaneT.hh>
#include <ACG/Geometry/Algorithms.hh>
#include <ObjectTypes/PolyMesh/PolyMeshTypes.hh>
#include <ostream>

//== CLASS DEFINITION =========================================================

// Node of the tree: contains parent, children and splitting plane
template <class Mesh>
struct TreeNode
{
    typedef typename Mesh::FaceHandle      Handle;
    typedef typename Mesh::Point      	   Point;
    typedef typename Mesh::VertexHandle    VertexHandle;
    typedef std::vector<Handle>            Handles;
    typedef typename Handles::iterator     HandleIter;
    typedef typename Point::value_type     Scalar;
    typedef ACG::Geometry::PlaneT<Scalar>  Plane;

    TreeNode(const Handles& _handles, TreeNode* _parent)
            : handles_(_handles),
            parent_(_parent), left_child_(0), right_child_(0) {}
    ~TreeNode()
    {
        delete left_child_;
        delete right_child_;

        if (parent_)
        {
            if (this == parent_->left_child_)
                parent_->left_child_ = 0;
            else
                parent_->right_child_ = 0;
        }
    }

    HandleIter begin() {
        return handles_.begin();
    }
    HandleIter end()   {
        return handles_.end();
    }

    size_t size() const {
        return handles_.size();
    }

    Handles     handles_;
    TreeNode    *parent_, *left_child_, *right_child_;
    Plane       plane_;
    Point	bb_min, bb_max;
    
    /// This visualizes the bounding boxes
    void visualizeTree(PolyMesh *_object, int _max_depth)
    {
        if (_max_depth > 0 && (left_child_ || right_child_) )
        {
            if (left_child_)
                left_child_->visualizeTree(_object, _max_depth-1);
            if (right_child_)
                right_child_->visualizeTree(_object, _max_depth-1);
        }
        else
        {
            Point size_ = bb_max - bb_min;

            std::vector<VertexHandle> vhandle(8);
            vhandle[0] = _object->add_vertex(bb_min+Point(0.0,0.0,size_[2]));
            vhandle[1] = _object->add_vertex(bb_min+Point(size_[0],0.0,size_[2]));
            vhandle[2] = _object->add_vertex(bb_min+Point(size_[0],size_[1],size_[2]));
            vhandle[3] = _object->add_vertex(bb_min+Point(0.0,size_[1],size_[2]));
            vhandle[4] = _object->add_vertex(bb_min+Point(0.0,0.0,0.0));
            vhandle[5] = _object->add_vertex(bb_min+Point(size_[0],0.0,0.0));
            vhandle[6] = _object->add_vertex(bb_min+Point(size_[0],size_[1],0.0));
            vhandle[7] = _object->add_vertex(bb_min+Point(0.0,size_[1],0.0));


            // generate (quadrilateral) faces
            std::vector<VertexHandle>  face_vhandles;

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[0]);
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[2]);
            face_vhandles.push_back(vhandle[3]);
            _object->add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[7]);
            face_vhandles.push_back(vhandle[6]);
            face_vhandles.push_back(vhandle[5]);
            face_vhandles.push_back(vhandle[4]);
            _object->add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[0]);
            face_vhandles.push_back(vhandle[4]);
            face_vhandles.push_back(vhandle[5]);
            _object->add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[2]);
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[5]);
            face_vhandles.push_back(vhandle[6]);
            _object->add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[3]);
            face_vhandles.push_back(vhandle[2]);
            face_vhandles.push_back(vhandle[6]);
            face_vhandles.push_back(vhandle[7]);
            _object->add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[0]);
            face_vhandles.push_back(vhandle[3]);
            face_vhandles.push_back(vhandle[7]);
            face_vhandles.push_back(vhandle[4]);
            _object->add_face(face_vhandles);
        }
    }

    private:
    /*
     * Noncopyable because of root_.
     */
    TreeNode(const TreeNode &rhs);
    TreeNode &operator=(const TreeNode &rhs);

};

template<class Mesh>
std::ostream &operator<< (std::ostream &stream, const TreeNode<Mesh> &node) {
    stream << "[TreeNode instance. Handles: ";
    for (typename std::vector<typename TreeNode<Mesh>::Handle>::const_iterator it = node.handles_.begin(), it_end = node.handles_.end();
            it != it_end; ++it) {
        stream << it->idx();
        if (it < it_end-1) stream << ", ";
    }
    stream << ", parent: " << node.parent_ << ", left_child_: " << node.left_child_
            << ", right_child_: " << node.right_child_ << ", plane_: <not implemented>, bb_min: "
            << node.bb_min << ", bb_max: " << node.bb_max << ", size(): " << node.size() << "]";
    return stream;
}

//=============================================================================
#endif // MB_BSPTREENODE_HH defined
//=============================================================================
