/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef SKELETONTRANSFORM_HH
#define SKELETONTRANSFORM_HH

#include "../Skeleton.hh"
#include <ACG/Math/Matrix4x4T.hh>
#include <OpenFlipper/common/GlobalDefines.hh>

/** \brief Skeleton transformation class
 *
 * This class should be used to manipulate skeletons since
 * it makes sure that all necessary transformations are applied
 * and that the transformations do not mess up the skeleton
 *
 **/
class DLLEXPORT SkeletonTransform {

  public:
    SkeletonTransform(Skeleton& _skeleton);
    ~SkeletonTransform() {}

  public:
    /// scale all bones of the skeleton by the given factor
    void scaleSkeleton(double _factor, Skeleton::Pose* _pose = 0);

    /// translate the skeleton
    void translateSkeleton(ACG::Vec3d _translation, Skeleton::Pose* _pose = 0);

    /// transform the skeleton
    void transformSkeleton(Matrix4x4 _transformation, Skeleton::Pose* _pose = 0);

    /// apply a translation to a joint in the refPose
    void translateJoint(Skeleton::Joint* _joint, ACG::Vec3d _translation, bool _keepChildPositions = true);

    /// apply a transformation to a joint in the refPose
    void transformJoint(Skeleton::Joint* _joint, Matrix4x4 _matrix, bool _keepChildPositions = true);

    /// rotate a joint in an arbitrary Pose
    void rotateJoint(Skeleton::Joint* _joint, Skeleton::Pose* _pose, Matrix4x4 _rotation, bool _applyToWholeAnimation = true);

    /// compute determinant to check if matrix is rotation matrix
    static double determinant(Matrix4x4& _m);
    
  private:
    Skeleton& skeleton_;
    Skeleton::Pose* refPose_;
};

#endif //SKELETONTRANSFORM_HH
