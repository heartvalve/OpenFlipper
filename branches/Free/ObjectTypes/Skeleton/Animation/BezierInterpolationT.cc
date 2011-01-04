#include "AnimationHelper.hh"
#include <utility>

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
std::vector<Scalar> BezierInterpolationT<Scalar>::getValue(Scalar _atX) {
  float bezierParam = AnimationHelper::approximateCubicBezierParameter<Scalar>(_atX, P0_.first, P1_.first, C0_.first, C1_.first);
  return AnimationHelper::evaluateBezier<Scalar>(bezierParam, P0_.second, P1_.second, C0_.second, C1_.second);
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
Scalar BezierInterpolationT<Scalar>::getMaxInput() {
  typedef std::vector<Scalar> Scalars;
  
  Scalars p0_x; p0_x.push_back(P0_.first);
  Scalars p1_x; p1_x.push_back(P1_.first);
  Scalars c0_x; c0_x.push_back(C0_.first);
  Scalars c1_x; c1_x.push_back(C1_.first);
  
  return AnimationHelper::evaluateBezier<Scalar>(1.0f, p0_x, p1_x, c0_x, c1_x)[0];
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
Scalar BezierInterpolationT<Scalar>::getMinInput() {
  typedef std::vector<Scalar> Scalars;
  
  Scalars p0_x; p0_x.push_back(P0_.first);
  Scalars p1_x; p1_x.push_back(P1_.first);
  Scalars c0_x; c0_x.push_back(C0_.first);
  Scalars c1_x; c1_x.push_back(C1_.first);
  
  return AnimationHelper::evaluateBezier<Scalar>(0.0f, p0_x, p1_x, c0_x, c1_x)[0];
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
typename BezierInterpolationT<Scalar>::Point&
BezierInterpolationT<Scalar>::P0(){
  return P0_;
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
typename BezierInterpolationT<Scalar>::Point&
BezierInterpolationT<Scalar>::C0(){
  return C0_;
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
typename BezierInterpolationT<Scalar>::Point&
BezierInterpolationT<Scalar>::C1(){
  return C1_;
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
typename BezierInterpolationT<Scalar>::Point&
BezierInterpolationT<Scalar>::P1(){
  return P1_;
}

//-----------------------------------------------------------------------------------------------------
