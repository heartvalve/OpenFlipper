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
  ACG::Vec3f vertices_[4];
} SplatCloud_Quad;


typedef struct
{
  SplatCloud_Surface surface_;
  SplatCloud_Quad    quad_;
} SplatCloud_Cluster;


typedef std::vector<SplatCloud_Camera>  SplatCloud_Cameras;
typedef std::vector<SplatCloud_Cluster> SplatCloud_Clusters;
typedef SplatCloud_Surface              SplatCloud_Unclustered;
typedef std::bitset<8>                  SplatCloud_Flags;


typedef SplatCloud::CloudPropertyT<SplatCloud_Cameras>     SplatCloud_CamerasProperty;
typedef SplatCloud::CloudPropertyT<SplatCloud_Clusters>    SplatCloud_ClustersProperty;
typedef SplatCloud::CloudPropertyT<SplatCloud_Unclustered> SplatCloud_UnclusteredProperty;
typedef SplatCloud::CloudPropertyT<SplatCloud_Flags>       SplatCloud_FlagsProperty;


static const SplatCloud::PropertyHandleT<SplatCloud_Cameras>     SPLATCLOUD_CAMERAS_HANDLE    ( "_Cameras_"     );
static const SplatCloud::PropertyHandleT<SplatCloud_Clusters>    SPLATCLOUD_CLUSTERS_HANDLE   ( "_Clusters_"    );
static const SplatCloud::PropertyHandleT<SplatCloud_Unclustered> SPLATCLOUD_UNCLUSTERED_HANDLE( "_Unclustered_" );
static const SplatCloud::PropertyHandleT<SplatCloud_Flags>       SPLATCLOUD_FLAGS_HANDLE      ( "_Flags_"       );


static const unsigned int SPLATCLOUD_CAMERA_HAS_IMAGEPATH_FLAG               = 0;
static const unsigned int SPLATCLOUD_SPLAT_VIEWLIST_HAS_FEATURE_INDICES_FLAG = 1;
static const unsigned int SPLATCLOUD_SPLAT_VIEWLIST_COORDS_NORMALIZED_FLAG   = 2;
static const unsigned int SPLATCLOUD_CLUSTER_HAS_QUAD_FLAG                   = 3;


#endif // SPLATCLOUD_EXTENSIONS_HH
