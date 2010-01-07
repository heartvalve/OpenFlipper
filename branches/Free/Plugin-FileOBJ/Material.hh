#ifndef DOXY_IGNORE_THIS

typedef OpenMesh::Vec3f Vec3f;

class Material
{
  
  public:

    Material() { cleanup(); }

    void cleanup()
    {
      Kd_is_set_     = false;
      Ka_is_set_     = false;
      Ks_is_set_     = false;
      Tr_is_set_     = false;
      map_Kd_is_set_ = false;
    }

    bool is_valid(void) const
    { return Kd_is_set_ || Ka_is_set_ || Ks_is_set_ || Tr_is_set_; }

    bool has_Kd(void)     { return Kd_is_set_;     }
    bool has_Ka(void)     { return Ka_is_set_;     }
    bool has_Ks(void)     { return Ks_is_set_;     }
    bool has_Tr(void)     { return Tr_is_set_;     }
    bool has_map_Kd(void) { return map_Kd_is_set_; }

    void set_Kd( float r, float g, float b )
    { Kd_=Vec3f(r,g,b); Kd_is_set_=true; }

    void set_Ka( float r, float g, float b )
    { Ka_=Vec3f(r,g,b); Ka_is_set_=true; }

    void set_Ks( float r, float g, float b )
    { Ks_=Vec3f(r,g,b); Ks_is_set_=true; }

    void set_Tr( float t )
    { Tr_=t;            Tr_is_set_=true; }

    void set_map_Kd( std::string _name, int _index_Kd )
    { map_Kd_ = _name, index_Kd_ = _index_Kd; map_Kd_is_set_ = true; };

    const Vec3f& Kd( void ) const { return Kd_; }
    const Vec3f& Ka( void ) const { return Ka_; }
    const Vec3f& Ks( void ) const { return Ks_; }
    float  Tr( void ) const { return Tr_; }
    const std::string& map_Kd( void ) { return map_Kd_ ; }
    const int& map_Kd_index( void ) { return index_Kd_ ; }

  private:

    Vec3f Kd_;                          bool Kd_is_set_; // diffuse
    Vec3f Ka_;                          bool Ka_is_set_; // ambient
    Vec3f Ks_;                          bool Ks_is_set_; // specular
    float Tr_;                          bool Tr_is_set_; // transperency

    std::string map_Kd_; int index_Kd_; bool map_Kd_is_set_; // Texture

};

  typedef std::map<std::string, Material> MaterialList;

#endif
