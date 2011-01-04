#ifndef SKELETON_INTERPOLATIONT_HH
#define SKELETON_INTERPOLATIONT_HH

#include <utility>
#include <vector>


template<typename Scalar>
class InterpolationT
{
  public:
    typedef std::vector<Scalar>         Target;
    typedef std::pair<Scalar, Target>   Point;
    
    InterpolationT<Scalar>(Point _P0, Point _P1) : P0_(_P0), P1_(_P1) {}
    
    virtual Target getValue(Scalar _atInput) = 0;
    virtual Scalar getMaxInput() = 0;
    virtual Scalar getMinInput() = 0;
    
  protected:
    Point P0_;
    Point P1_;
    
  private:
    
    
};

template<typename Scalar>
class BezierInterpolationT : public InterpolationT<Scalar>
{
  public:
    typedef std::vector<Scalar>         Target;
    typedef std::pair<Scalar, Target>   Point;
    
    BezierInterpolationT<Scalar>(Point _P0, Point _P1)
        : InterpolationT<Scalar>(_P0, _P1) {}
    BezierInterpolationT<Scalar>(Point _P0, Point _P1, Point _C0, Point _C1)
        : InterpolationT<Scalar>(_P0, _P1), C0_(_C0), C1_(_C1) {}
    
    Target getValue(Scalar atX);
    Scalar getMaxInput();
    Scalar getMinInput();
    
  protected:
    using InterpolationT<Scalar>::P0_;
    using InterpolationT<Scalar>::P1_;
    Point C0_;
    Point C1_;
    
    
  public:
    Point& P0();
    Point& C0();
    Point& C1();
    Point& P1();
};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(INTERPOLATIONT_CC)
#define INTERPOLATIONT_TEMPLATES
#include "BezierInterpolationT.cc"
#endif
//=============================================================================

#endif //SKELETON_INTERPOLATIONT_HH