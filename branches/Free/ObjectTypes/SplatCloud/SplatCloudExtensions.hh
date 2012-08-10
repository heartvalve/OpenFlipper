#ifndef SPLATCLOUD_EXTENSIONS_HH
#define SPLATCLOUD_EXTENSIONS_HH


#include <ObjectTypes/SplatCloud/SplatCloud.hh>

#include <OpenFlipper/common/Types.hh>

#include <string>
#include <vector>
#include <bitset>


typedef struct
{
  double f_, k1_, k2_;
  double r_[3][3];
  double t_[3];
} SplatCloud_Projection;


typedef struct
{
  int                   objectId_;
  SplatCloud_Projection projection_;
  std::string           imagePath_;
} SplatCloud_Camera;


typedef struct
{
  int          firstSplatIdx_;
  unsigned int numSplats_;
} SplatCloud_Surface;


typedef struct
{
  ACG::Vec3d vertices_[4];
} SplatCloud_Quad;


typedef struct
{
  SplatCloud_Surface surface_;
  SplatCloud_Quad    quad_;
} SplatCloud_Cluster;


typedef std::vector<SplatCloud_Camera>  SplatCloud_Cameras;
typedef std::vector<SplatCloud_Cluster> SplatCloud_Clusters;
typedef std::bitset<8>                  SplatCloud_Flags; 


typedef struct
{
  SplatCloud_Cameras cameras_;
} SplatCloud_CameraManager;


typedef struct
{
  int                 objectId_;
  SplatCloud_Surface  unclustered_;
  SplatCloud_Clusters clusters_;
} SplatCloud_ClusterManager;


typedef struct
{
  SplatCloud_Flags flags_;
} SplatCloud_GeneralManager;


typedef SplatCloud::CloudPropertyT<SplatCloud_CameraManager>  SplatCloud_CameraManagerProperty;
typedef SplatCloud::CloudPropertyT<SplatCloud_ClusterManager> SplatCloud_ClusterManagerProperty;
typedef SplatCloud::CloudPropertyT<SplatCloud_GeneralManager> SplatCloud_GeneralManagerProperty;


static const SplatCloud::PropertyHandleT<SplatCloud_CameraManager>  SPLATCLOUD_CAMERAMANAGER_HANDLE ( "CameraManager"  );
static const SplatCloud::PropertyHandleT<SplatCloud_ClusterManager> SPLATCLOUD_CLUSTERMANAGER_HANDLE( "ClusterManager" );
static const SplatCloud::PropertyHandleT<SplatCloud_GeneralManager> SPLATCLOUD_GENERALMANAGER_HANDLE( "GeneralManager" );


static const unsigned int SPLATCLOUD_CAMERA_HAS_IMAGEPATH_FLAG               = 0;
static const unsigned int SPLATCLOUD_SPLAT_VIEWLIST_HAS_FEATURE_INDICES_FLAG = 1;
static const unsigned int SPLATCLOUD_SPLAT_VIEWLIST_COORDS_NORMALIZED_FLAG   = 2;
static const unsigned int SPLATCLOUD_CLUSTER_HAS_QUAD_FLAG                   = 3;


#endif // SPLATCLOUD_EXTENSIONS_HH
