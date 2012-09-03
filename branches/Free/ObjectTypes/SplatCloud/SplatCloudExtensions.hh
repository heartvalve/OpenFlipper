#ifndef SPLATCLOUD_EXTENSIONS_HH
#define SPLATCLOUD_EXTENSIONS_HH


#include <ObjectTypes/SplatCloud/SplatCloud.hh>

#include <OpenFlipper/common/Types.hh>

#include <string>
#include <vector>
#include <bitset>


class SplatCloud_Projection
{
public:
  SplatCloud_Projection() : f_( 0.0 ), k1_( 0.0 ), k2_( 0.0 )
  {
    r_[0][0] = 1.0; r_[0][1] = 0.0; r_[0][2] = 0.0;
    r_[1][0] = 0.0; r_[1][1] = 1.0; r_[1][2] = 0.0;
    r_[2][0] = 0.0; r_[2][1] = 0.0; r_[2][2] = 1.0;
    t_   [0] = 0.0; t_   [1] = 0.0; t_   [2] = 0.0;
  }
public:
  double f_, k1_, k2_;
  double r_[3][3];
  double t_[3];
};


class SplatCloud_Camera
{
public:
  SplatCloud_Camera() : objectId_( -1 ), projection_(), imagePath_( "" ), imageWidth_( 0 ), imageHeight_( 0 ) { }
public:
  int                   objectId_;
  SplatCloud_Projection projection_;
  std::string           imagePath_;
  unsigned int          imageWidth_, imageHeight_;
};


class SplatCloud_Surface
{
public:
  SplatCloud_Surface() : firstSplatIdx_( -1 ), numSplats_( 0 ) { }
public:
  int          firstSplatIdx_;
  unsigned int numSplats_;
};


class SplatCloud_Quad
{
public:
  SplatCloud_Quad()
  {
    vertices_[0] = ACG::Vec3d( 0.0, 0.0, 0.0 );
    vertices_[1] = ACG::Vec3d( 0.0, 0.0, 0.0 );
    vertices_[2] = ACG::Vec3d( 0.0, 0.0, 0.0 );
    vertices_[3] = ACG::Vec3d( 0.0, 0.0, 0.0 );
  }
public:
  ACG::Vec3d vertices_[4];
};


class SplatCloud_Cluster
{
public:
  SplatCloud_Cluster() : surface_(), quad_() { }
public:
  SplatCloud_Surface surface_;
  SplatCloud_Quad    quad_;
};


typedef std::vector<SplatCloud_Camera>  SplatCloud_Cameras;
typedef std::vector<SplatCloud_Cluster> SplatCloud_Clusters;
typedef std::bitset<8>                  SplatCloud_Flags; 


class SplatCloud_CameraManager
{
public:
  SplatCloud_CameraManager() : cameras_() { }
public:
  SplatCloud_Cameras cameras_;
};


class SplatCloud_ClusterManager
{
public:
  SplatCloud_ClusterManager() : objectId_( -1 ), unclustered_(), clusters_() { }
public:
  int                 objectId_;
  SplatCloud_Surface  unclustered_;
  SplatCloud_Clusters clusters_;
};


class SplatCloud_GeneralManager
{
public:
  SplatCloud_GeneralManager() : flags_( 0 ) { }
public:
  SplatCloud_Flags flags_;
};


typedef SplatCloud::CloudPropertyT<SplatCloud_CameraManager>  SplatCloud_CameraManagerProperty;
typedef SplatCloud::CloudPropertyT<SplatCloud_ClusterManager> SplatCloud_ClusterManagerProperty;
typedef SplatCloud::CloudPropertyT<SplatCloud_GeneralManager> SplatCloud_GeneralManagerProperty;


static const SplatCloud::PropertyHandleT<SplatCloud_CameraManager>  SPLATCLOUD_CAMERAMANAGER_HANDLE ( "CameraManager"  );
static const SplatCloud::PropertyHandleT<SplatCloud_ClusterManager> SPLATCLOUD_CLUSTERMANAGER_HANDLE( "ClusterManager" );
static const SplatCloud::PropertyHandleT<SplatCloud_GeneralManager> SPLATCLOUD_GENERALMANAGER_HANDLE( "GeneralManager" );


static const unsigned int SPLATCLOUD_CAMERA_HAS_IMAGEPATH_FLAG               = 0;
static const unsigned int SPLATCLOUD_CAMERA_HAS_IMAGERESOLUTION_FLAG         = 1;
static const unsigned int SPLATCLOUD_SPLAT_VIEWLIST_HAS_FEATURE_INDICES_FLAG = 2;
static const unsigned int SPLATCLOUD_SPLAT_VIEWLIST_COORDS_NORMALIZED_FLAG   = 3;
static const unsigned int SPLATCLOUD_CLUSTER_HAS_QUAD_FLAG                   = 4;


#endif // SPLATCLOUD_EXTENSIONS_HH
