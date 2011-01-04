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
    typedef ACG::Matrix4x4d Matrix4x4;
  
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