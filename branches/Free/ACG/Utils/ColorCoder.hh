//=============================================================================
//
//  CLASS ColorCoder
//
//=============================================================================


#ifndef ACG_COLORCODER_HH
#define ACG_COLORCODER_HH

//== INCLUDES =================================================================


#include <ACG/Math/VectorT.hh>

// required for windows to avoid collision with min and max macros
#ifdef WIN32
#undef min
#undef max
#endif

//== NAMESPACES ===============================================================

namespace ACG {

  //== CLASS DEFINITION =========================================================


  class ColorCoder
  {
    public:

      /// Default constructor.
      ColorCoder(float _min=0.0, float _max=1.0, bool _signed=false)
      { set_range(_min, _max, _signed); }


      /// set the color coding range for unsigned coding
      void set_range(float _min, float _max, bool _signed)
      {
        if (_min == _max)
        {
          val0_ = val1_ = val2_ = val3_ = val4_ = _min;
        }
        else
        {
          if (_min > _max) std::swap(_min, _max);
          val0_ = _min;
          val4_ = _max;
          val2_ = 0.5 * (val0_ + val4_);
          val1_ = 0.5 * (val0_ + val2_);
          val3_ = 0.5 * (val2_ + val4_);
        }
        signed_mode_ = _signed;
      }


      /// color coding
      ACG::Vec4uc color4(float _v) const {
        return signed_mode_ ? color_signed(_v) : color_unsigned(_v);
      }

      /// color coding
      ACG::Vec3uc color(float _v) const {
        ACG::Vec4uc c;
        if(signed_mode_) c=color_signed(_v); else c=color_unsigned(_v);
        return (ACG::Vec3uc(c[0],c[1],c[2])/255.f);
      }


      /// color coding
      ACG::Vec3f color_float(float _v) const {
        ACG::Vec4uc c;
        if(signed_mode_) c=color_signed(_v); else c=color_unsigned(_v);
        return (ACG::Vec3f(c[0],c[1],c[2])/255.f);
      }

      /// color coding
      ACG::Vec4f color_float4(float _v) const {
        ACG::Vec4uc c;
        if(signed_mode_) c=color_signed(_v); else c=color_unsigned(_v);
        return (ACG::Vec4f(c[0],c[1],c[2],c[3])/255.f);
      }


      /// min scalar value
      float min() const { return val0_; }
      /// max scalar value
      float max() const { return val4_; }


    private:


      ACG::Vec4uc color_unsigned(float _v) const {
        if (val4_ <= val0_) return ACG::Vec4uc(0, 0, 255, 255);

        unsigned char u;

        if (_v < val0_) return ACG::Vec4uc(0, 0, 255, 255);
        if (_v > val4_) return ACG::Vec4uc(255, 0, 0, 255);


        if (_v <= val2_)
        {
          // [v0, v1]
          if (_v <= val1_) {
            u = (unsigned char) (255.0 * (_v - val0_) / (val1_ - val0_));
            return ACG::Vec4uc(0, u, 255, 255);
          }
          // ]v1, v2]
          else {
            u = (unsigned char) (255.0 * (_v - val1_) / (val2_ - val1_));
            return ACG::Vec4uc(0, 255, 255-u, 255);
          }
        }
        else
        {
          // ]v2, v3]
          if (_v <= val3_) {
            u = (unsigned char) (255.0 * (_v - val2_) / (val3_ - val2_));
            return ACG::Vec4uc(u, 255, 0, 255);
          }
          // ]v3, v4]
          else {
            u = (unsigned char) (255.0 * (_v - val3_) / (val4_ - val3_));
            return ACG::Vec4uc(255, 255-u, 0, 255);
          }
        }
      }


      ACG::Vec4uc color_signed(float _v) const  {
        if (val4_ <= val0_) return ACG::Vec4uc(0, 255, 0, 255);

        unsigned char r,g,b;

        if      (_v < val0_) _v = val0_;
        else if (_v > val4_) _v = val4_;

        if (_v < 0.0)
        {
          r = val0_ ? (unsigned char)(255.0 * _v / val0_) : 0;
          b = 0;
        }
        else
        {
          r = 0;
          b = val4_ ? (unsigned char)(255.0 * _v / val4_) : 0;
        }
        g = 255 - r - b;

        return ACG::Vec4uc(r, g, b, 255);
      }




      float  val0_, val1_, val2_, val3_, val4_;
      bool   signed_mode_;
  };


  //=============================================================================
} // namespace mb
//=============================================================================
#endif // ACG_COLORCODER_HH defined
//=============================================================================

