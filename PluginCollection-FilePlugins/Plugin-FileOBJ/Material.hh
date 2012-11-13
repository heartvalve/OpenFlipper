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

#ifndef MATERIAL_HH_INCLUDED
#define MATERIAL_HH_INCLUDED

#ifndef DOXY_IGNORE_THIS

#include <iomanip>

/** \brief Material
 *
 * This class provides means to store materials
 * like colors, texture information (texture indices, filenames, etc.).
 */
class Material
{
  public:

    Material() :
          localNum_(0),
          Kd_is_set_(false),
          Ka_is_set_(false),
          Ks_is_set_(false),
          Tr_(0.0),
          Tr_is_set_(false),
          map_Kd_(""),
          index_Kd_(0),
          map_Kd_is_set_(false)
    {
    }


    
    void cleanup() {
        localNum_      = 0;
        Kd_is_set_     = false;
        Ka_is_set_     = false;
        Ks_is_set_     = false;
        Tr_            = 0.0;
        Tr_is_set_     = false;
        map_Kd_        = "";
        index_Kd_      = 0;
        map_Kd_is_set_ = false;
    }

    bool is_valid(void) const
    { return Kd_is_set_ || Ka_is_set_ || Ks_is_set_ || Tr_is_set_; }

    /// Does material have a diffuse color?
    bool has_Kd(void)     { return Kd_is_set_;     }
    
    /// Does material have an ambient color?
    bool has_Ka(void)     { return Ka_is_set_;     }
    
    /// Does material have a specular color?
    bool has_Ks(void)     { return Ks_is_set_;     }
    
    /// Does material have transparency?
    bool has_Tr(void)     { return Tr_is_set_;     }
    
    /// Does material have a texture?
    bool has_Texture() const { return map_Kd_is_set_; }
    
    /// Compare operator - Compares all class members
    bool operator== (Material& _mat) {
        if( _mat.Kd_ != Kd_ ||
            _mat.Ka_ != Ka_ ||
            _mat.Ks_ != Ks_ ||
            _mat.Tr_ != Tr_ ||
            _mat.map_Kd_   != map_Kd_ ||
            _mat.index_Kd_ != index_Kd_ )
            return false;
        return true;
    }
    
    /// Not equal operator
    bool operator!= (Material& _mat) { return !(*this == _mat); }

    /// Returns the material number (externally set)
    int material_number() const { return localNum_; }
    void material_number(unsigned int _n) { localNum_ = _n; }

    /// Set diffuse color
    void set_Kd( float r, float g, float b )
    { Kd_=OpenMesh::Vec3f(r,g,b); Kd_is_set_=true; }

    /// Set ambient color
    void set_Ka( float r, float g, float b )
    { Ka_=OpenMesh::Vec3f(r,g,b); Ka_is_set_=true; }

    /// Set specular color
    void set_Ks( float r, float g, float b )
    { Ks_=OpenMesh::Vec3f(r,g,b); Ks_is_set_=true; }

    /// Set transparency value (between 0 and 1)
    void set_Tr( float t )
    { Tr_=t;            Tr_is_set_=true; }

    /// Set texture file and index
    void set_map_Kd( std::string _name, int _index_Kd )
    { map_Kd_ = _name, index_Kd_ = _index_Kd; map_Kd_is_set_ = true; };

    /// Get diffuse color
    const OpenMesh::Vec3f& Kd( void ) const { return Kd_; }
    
    /// Get ambient color
    const OpenMesh::Vec3f& Ka( void ) const { return Ka_; }
    
    /// Get specular color
    const OpenMesh::Vec3f& Ks( void ) const { return Ks_; }
    
    /// Get transparency value
    float Tr( void ) const { return Tr_; }
    
    /// Get texture file
    const std::string& map_Kd( void ) { return map_Kd_ ; }
    
    /// Get texture index
    const int& map_Kd_index( void ) { return index_Kd_ ; }
    
  private:
    
    unsigned int localNum_;
    OpenMesh::Vec3f Kd_;                          bool Kd_is_set_; // diffuse
    OpenMesh::Vec3f Ka_;                          bool Ka_is_set_; // ambient
    OpenMesh::Vec3f Ks_;                          bool Ks_is_set_; // specular
    float Tr_;                                    bool Tr_is_set_; // transperency

    std::string map_Kd_; int index_Kd_; bool map_Kd_is_set_; // Texture
};

inline std::ostream&
operator<< (std::ostream& _os, const Material& _mat) {
    _os << "mat" << std::setfill('0') << std::setw(3) << _mat.material_number();
    
    return _os;
}

typedef std::map<std::string, Material> MaterialList;

#endif

#endif // MATERIAL_HH_INCLUDED
