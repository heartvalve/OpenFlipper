#ifndef MATERIAL_HH_INCLUDED
#define MATERIAL_HH_INCLUDED

#ifndef DOXY_IGNORE_THIS

#include <iomanip>

typedef OpenMesh::Vec3f Vec3f;

/** \brief Material
 *
 * This class provides means to store materials
 * like colors, texture information (texture indices, filenames, etc.).
 */
class Material
{
  public:

    Material() { cleanup(); }

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
    { Kd_=Vec3f(r,g,b); Kd_is_set_=true; }

    /// Set ambient color
    void set_Ka( float r, float g, float b )
    { Ka_=Vec3f(r,g,b); Ka_is_set_=true; }

    /// Set specular color
    void set_Ks( float r, float g, float b )
    { Ks_=Vec3f(r,g,b); Ks_is_set_=true; }

    /// Set transparency value (between 0 and 1)
    void set_Tr( float t )
    { Tr_=t;            Tr_is_set_=true; }

    /// Set texture file and index
    void set_map_Kd( std::string _name, int _index_Kd )
    { map_Kd_ = _name, index_Kd_ = _index_Kd; map_Kd_is_set_ = true; };

    /// Get diffuse color
    const Vec3f& Kd( void ) const { return Kd_; }
    
    /// Get ambient color
    const Vec3f& Ka( void ) const { return Ka_; }
    
    /// Get specular color
    const Vec3f& Ks( void ) const { return Ks_; }
    
    /// Get transparency value
    float Tr( void ) const { return Tr_; }
    
    /// Get texture file
    const std::string& map_Kd( void ) { return map_Kd_ ; }
    
    /// Get texture index
    const int& map_Kd_index( void ) { return index_Kd_ ; }
    
  private:
      
    void cleanup() {
      
        Kd_is_set_     = false;
        Ka_is_set_     = false;
        Ks_is_set_     = false;
        Tr_is_set_     = false;
        map_Kd_is_set_ = false;
        localNum_      = 0;
    }

    unsigned int localNum_;
    Vec3f Kd_;                          bool Kd_is_set_; // diffuse
    Vec3f Ka_;                          bool Ka_is_set_; // ambient
    Vec3f Ks_;                          bool Ks_is_set_; // specular
    float Tr_;                          bool Tr_is_set_; // transperency

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
