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
    
    Note that in order for the transformations to apply in
    each traversal of the scenegraph, one has to call
    apply_transformation(true) once.
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
  void enter(GLState& _state, const DrawModes::DrawMode& _drawmode);
  /// restores original GL-color and GL-material
  void leave(GLState& _state, const DrawModes::DrawMode& _drawmode);



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

  /// Add scaling to the current Transformation.
  void scale(const GLMatrixd& _m);


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

  /// return scale matrix
  const GLMatrixd& scale() const { 
    return scale_matrix_; 
  }
  /// return inverse scale matrix
  const GLMatrixd& inverse_scale() const { 
    return inverse_scale_matrix_; 
  }

  bool apply_transformation() { return applyTransformation_; }

  void apply_transformation(bool _applyTransformation) { applyTransformation_ = _applyTransformation; }


  // ortho 2d mode
  bool is2D(){return is2DObject_;};
  void set2D(bool _2d){is2DObject_ = _2d;};
  
  bool isPerSkeletonObject(){return isPerSkeletonObject_;};
  void setPerSkeletonObject(bool _is){isPerSkeletonObject_ = _is;};
  void setPerSkeletonModelView(GLMatrixd _is){perSkeletonModelView_ = _is;};
  
  void ortho2DMode(GLState& _state);
  void perSkeletonMode(GLState& _state);
  void update2DOffset(ACG::Vec2d _offset){offset_ += _offset;};
  void scale2D(double _scale){scaleFactor2D_ = _scale;};
  void setImageDimensions(ACG::Vec2i _dim){imageDimensions_ = _dim;};

private:

  /// update matrix
  void updateMatrix();


  // ELEMENTS
  GLMatrixd         matrix_, inverse_matrix_;
  GLMatrixd         rotation_matrix_, inverse_rotation_matrix_, scale_matrix_, inverse_scale_matrix_;
  Vec3d             center_;
  Vec3d             translation_;
  Quaterniond       quaternion_;


  bool applyTransformation_;

public:
  // ortho 2d mode
  bool is2DObject_;
  bool isPerSkeletonObject_;
  GLMatrixd perSkeletonModelView_;
  double scaleFactor2D_;
  ACG::Vec2i imageDimensions_;
  ACG::Vec2d offset_;

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TRANSFORM_NODE_HH
//=============================================================================
