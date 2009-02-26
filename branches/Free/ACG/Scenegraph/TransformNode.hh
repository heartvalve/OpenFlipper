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
//  CLASS TransformNode
//
//=============================================================================

#ifndef ACG_TRANSFORM_NODE_HH
#define ACG_TRANSFORM_NODE_HH


//== INCLUDES =================================================================


#include "BaseNode.hh"
#include "../Math/GLMatrixT.hh"
#include "../Math/VectorT.hh"
#include "../Math/QuaternionT.hh"


//== NAMESPACES  ==============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class TransformNode TransformNode.hh <ACG/Scenegraph/TransformNode.hh>

    TransformNode - general 3D geometric transformation node.
    Provides functionality to translate, scale or rotate around a
    specified center. This is the base class for e.g. the
    TrackballNode and the ManipulatorNode, that only add a GUI for
    generating the transformations.
*/
  
class ACGDLLEXPORT TransformNode : public BaseNode
{
public:
  
  
  /// Constructor
  TransformNode( BaseNode* _parent=0,
		 const std::string& _name="<TransformNode>" );

  /// Destructor.
  virtual ~TransformNode(){}

  /// set name 
  ACG_CLASSNAME(TransformNode);

    
  /// set current GL-color and GL-material
  void enter(GLState& _state, unsigned int _drawmode);
  /// restores original GL-color and GL-material
  void leave(GLState& _state, unsigned int _drawmode);



  /// set center
  void set_center(const Vec3d& _c) { center_ = _c; }
  /// get center
  const Vec3d& center() const { return center_; }

  
  /** Reset transformation to identity, i.e. reset rotation, translating and 
      scaling factor. */
  void loadIdentity();
  /** set the current transformation to the identity transformation,
      i.e. change the center() to the transfomed center and loadIdentity().
  */
  virtual void setIdentity();



  //--- set values ---


  /// Add a translation to the current Transformation.
  void translate(const Vec3d& _v);

  /** Add a rotation to the current Transformation.
      Assume angle in degree and axis normalized */
  void rotate(double _angle, const Vec3d& _axis);

  /// Add scaling to the current Transformation.
  void scale(double _s) { scale(Vec3d(_s, _s, _s)); }

  /// Add scaling to the current Transformation.
  void scale(const Vec3d& _s);


  //--- get values ---


  /// Returns a const reference to the current transformation matrix
  const GLMatrixd& matrix() const { return matrix_; }
  /// return inverse matrix
  const GLMatrixd& inverse_matrix() const { return inverse_matrix_; }

  /// return rotation axis & angle
  void rotation(Vec3d& _axis, double& _angle) const { 
    quaternion_.axis_angle(_axis, _angle);
    _angle *= 180.0/M_PI;
  }
  /// return rotation matrix
  const GLMatrixd& rotation() const { 
    return rotation_matrix_; 
  }
  /// return inverse rotation matrix
  const GLMatrixd& inverse_rotation() const { 
    return inverse_rotation_matrix_; 
  }


  /// returns ref. to translation vector
  const Vec3d& translation() const { return translation_; }

  /// returns scaling factor
  const Vec3d& scaling() const { return scaling_; }

  bool apply_transformation() { return applyTransformation_; }

  void apply_transformation(bool _applyTransformation) { applyTransformation_ = _applyTransformation; }

  
private:

  /// update matrix
  void updateMatrix();
    

  // ELEMENTS
  GLMatrixd         matrix_, inverse_matrix_;
  GLMatrixd         rotation_matrix_, inverse_rotation_matrix_;
  Vec3d             center_;
  Vec3d             translation_;
  Vec3d             scaling_;
  Quaterniond       quaternion_;


  bool applyTransformation_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TRANSFORM_NODE_HH
//=============================================================================
