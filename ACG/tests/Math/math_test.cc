/*
 * math_test.cc
 *
 *  Created on: 22.04.2014
 *      Author: tenter
 */

#include <gtest/gtest.h>

#include <ACG/Math/GLMatrixT.hh>


class MathTest : public testing::Test {

public:
  MathTest()
  {
  }

protected:

  // This function is called before each test is run
  virtual void SetUp() {
  }

  // This function is called after all tests are through
  virtual void TearDown() {
  }


};

TEST_F(MathTest, GLMatrixT_extract_planes ) {
  
  const double cmp_eps = 1e-7;

  const double near = 10.0;
  const double far = 25.0;

  // test clipping plane extraction on perspective and orthographic matrix
  ACG::GLMatrixd P;
  P.identity();

  EXPECT_FALSE(P.isPerspective());
  EXPECT_FALSE(P.isOrtho());

  P.perspective(45.0, 4.0/3.0, near, far);

  ACG::Vec2d planes = P.extract_planes_perspective();

  EXPECT_LE(fabs(planes[0] - near), cmp_eps);
  EXPECT_LE(fabs(planes[1] - far), cmp_eps);

  EXPECT_TRUE(P.isPerspective());
  EXPECT_FALSE(P.isOrtho());

  // test on orthographic matrix

  P.identity();
  P.ortho(-1.0, 1.0, -1.0, 1.0, near, far);

  planes = P.extract_planes_ortho();

  EXPECT_LE(fabs(planes[0] - near), cmp_eps);
  EXPECT_LE(fabs(planes[1] - far), cmp_eps);

  EXPECT_FALSE(P.isPerspective());
  EXPECT_TRUE(P.isOrtho());
}
