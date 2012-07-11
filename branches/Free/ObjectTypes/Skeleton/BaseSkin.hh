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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef BASESKIN_HH
#define BASESKIN_HH

#include <map>

#include "OpenFlipper/common/perObjectData.hh"

#include <ObjectTypes/Skeleton/SkeletonT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#define OBJECTDATA_SKIN   "Skin Object-Data"
#define SKIN_WEIGHTS_PROP "skin-weights"
#define DEFAULTPOSE_PROP  "Default pose"

/**
 * @brief Abstract base class for the skin template, wrapping all template versions of the skin
 */
class BaseSkin : public PerObjectData
{

public:

  /**
   * @name Properties
   * These structures store mesh properties used to make the mesh a skin.
   */
  //@{
  /**
   * @brief Holds the skins default pose
   *
   * The default pose of the mesh is stored in a vertex property. From this pose all deformed poses are
   * derived. An additional point and normal are stored per vertex.
   */
  struct DefaultPose
  {
    DefaultPose() {};
    DefaultPose(OpenMesh::Vec3d point, OpenMesh::Vec3d normal) : point(point), normal(normal) {};

    OpenMesh::Vec3d point;  ///< The points position in the default pose
    OpenMesh::Vec3d normal; ///< The points normal in the default pose
  };
  
  /**
   * @brief Stores the joint weights per vertex
   *
   * In general every joint can influence every vertex. In practice only a small number of joints manipulate
   * a vertex. Therefore they are stored in a map using the joint identifier as key. The weights for one
   * vertex are supposed to sum to one.
   */
    typedef std::map<unsigned int, double> SkinWeights;
  //@}
  
  /**
   * @brief Possible deformation methods
   *
   * LBS = Linear Blend Skinning
   * DBS = Dual Quaternion Blend Skinning
   *
   */
  enum Method
  {
    M_LBS = 0,
    M_DBS = 1
  };

public:
  BaseSkin(int _skeletonId) {
    skeleton_ = _skeletonId;
  };

  ~BaseSkin() {};

public:
  /**
   * @name Skinning
   * These methods are related to using a mesh as skin with this skeleton.
   */
  //@{
  virtual void attachSkin() = 0;
  virtual void deformSkin() = 0;
  virtual void deformSkin(const AnimationHandle &_hAni, Method _method = M_LBS) = 0;
  virtual void releaseSkin() = 0;
  //@}

  /**
   * @name Information about the attached skeleton
   */
  //@{
  int skeletonId() {return skeleton_;};

  //@}

private:
  /// Holds the associated skeleton
  int skeleton_;
};


// ----------------------------------------------------------------------------
// support persistence for struct Weights
// ----------------------------------------------------------------------------

namespace OpenMesh {
  namespace IO {

    template <> struct binary<BaseSkin::SkinWeights>
    {
      typedef BaseSkin::SkinWeights value_type;

      static const bool is_streamable = true;

      // return binary size of the value

      static size_t size_of(void) 
      { 
        return UnknownSize;
      }

      static size_t size_of(const value_type& _v)
      { 
        if (_v.empty())
          return sizeof(unsigned int);
        
        value_type::const_iterator it = _v.begin();
        unsigned int   N     = _v.size();
        size_t         bytes = IO::size_of(N);

        for(;it!=_v.end(); ++it)
        {
          bytes += IO::size_of( it->first );
          bytes += IO::size_of( it->second );
        }
        return bytes;
      }

      static size_t store(std::ostream& _os, const value_type& _v, bool _swap=false)
      { 
        value_type::const_iterator it = _v.begin();
        unsigned int   N     = _v.size();
        
        size_t bytes;
        bytes  = IO::store( _os, N, _swap );

        for(;it!=_v.end(); ++it)
        {
          bytes += IO::store( _os, it->first,  _swap );
          bytes += IO::store( _os, it->second, _swap );
        }

        return _os.good() ? bytes : 0;
      }

      static size_t restore( std::istream& _is, value_type& _v, bool _swap=false)
      { 
        unsigned int   N     = _v.size();

        size_t bytes;
        bytes  = IO::restore( _is, N, _swap );

        for(unsigned int i=0; i < N; i++)
        {
          unsigned int first;
          double       second;

          bytes += IO::restore( _is, first,  _swap );
          bytes += IO::restore( _is, second, _swap );

          _v[first] = second;
        }

        return _is.good() ? bytes : 0;
      }
    };
  }
}


#endif //BASESKIN_HH
