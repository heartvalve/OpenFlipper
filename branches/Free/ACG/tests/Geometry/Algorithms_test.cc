/*
 * BSP_test.cc
 *
 *  Created on: 31.07.2012
 *      Author: moebius
 */

#include <gtest/gtest.h>

#include <ACG/Math/VectorT.hh>
#include <ACG/Geometry/Algorithms.hh>

class ALGORITHM_TEST_BASE : public testing::Test {

    protected:

        // This function is called before each test is run
        virtual void SetUp() {

        }

        // This function is called after all tests are through
        virtual void TearDown() {

            // Do some final stuff with the member data here...
        }

};

TEST_F(ALGORITHM_TEST_BASE, triangleIntersection ) {


  // ==============================================
  // Triangle intersection algorithm
  // ==============================================

  /* All in z = 0 plane :

   (0,1)
    p2
    | \
    |  \
    |   \
    |    \
    |     \
    |      \
    p0 === p1
   (0,0)   (1,0)
   */

  // Triangle points
  ACG::Vec3f p0 ( 0.0,0.0,0.0);
  ACG::Vec3f p1 ( 1.0,0.0,0.0);
  ACG::Vec3f p2 ( 0.0,1.0,0.0);

  // Shooting ray origin and direction
  ACG::Vec3f origin( 0.1, 0.1, -1.0);
  ACG::Vec3f direction( 0.0, 0.0, 1.0);

  float distance,u,v;
  bool result = ACG::Geometry::triangleIntersection(origin, direction,
                                                    p0, p1, p2,
                                                    distance,u,v);

  EXPECT_TRUE( result )       << "Intersection failed!";

  EXPECT_EQ( 1.0f, distance ) << "Wrong distance!" << std::endl;
  EXPECT_EQ( 0.1f, u )        << "Wrong u!" << std::endl;
  EXPECT_EQ( 0.1f, v )        << "Wrong v!" << std::endl;

}

TEST_F(ALGORITHM_TEST_BASE, triangleIntersection_FlippedTriangleOrientation ) {


  // ==============================================
  // Triangle intersection algorithm
  // ==============================================

  /* All in z = 0 plane :

   (0,1)
    p2
    | \
    |  \
    |   \
    |    \
    |     \
    |      \
    p0 === p1
   (0,0)   (1,0)
   */

  // Triangle points
  ACG::Vec3f p0 ( 0.0,0.0,0.0);
  ACG::Vec3f p1 ( 1.0,0.0,0.0);
  ACG::Vec3f p2 ( 0.0,1.0,0.0);

  // Shooting ray origin and direction
  ACG::Vec3f origin( 0.1, 0.1, -1.0);
  ACG::Vec3f direction( 0.0, 0.0, 1.0);

  float distance,u,v;
  bool result = ACG::Geometry::triangleIntersection(origin, direction,
                                                    p0, p2, p1,
                                                    distance,u,v);

  EXPECT_TRUE( result )       << "Intersection failed!";

  EXPECT_EQ( 1.0f, distance ) << "Wrong distance!" << std::endl;
  EXPECT_EQ( 0.1f, u )        << "Wrong u!" << std::endl;
  EXPECT_EQ( 0.1f, v )        << "Wrong v!" << std::endl;

}

TEST_F(ALGORITHM_TEST_BASE, triangleIntersection_NegativeShootingDirection ) {


  // ==============================================
  // Triangle intersection algorithm
  // ==============================================

  /* All in z = 0 plane :

   (0,1)
    p2
    | \
    |  \
    |   \
    |    \
    |     \
    |      \
    p0 === p1
   (0,0)   (1,0)
   */

  // Triangle points
  ACG::Vec3f p0 ( 0.0,0.0,0.0);
  ACG::Vec3f p1 ( 1.0,0.0,0.0);
  ACG::Vec3f p2 ( 0.0,1.0,0.0);

  // Shooting ray origin and direction
  ACG::Vec3f origin( 0.1, 0.1, -1.0);
  ACG::Vec3f direction( 0.0, 0.0, -1.0);

  float distance,u,v;
  bool result = ACG::Geometry::triangleIntersection(origin, direction,
                                                    p0, p1, p2,
                                                    distance,u,v);

  EXPECT_TRUE( result )        << "Intersection failed!";

  EXPECT_EQ( -1.0f, distance ) << "Wrong distance!" << std::endl;
  EXPECT_EQ( 0.1f, u )         << "Wrong u!" << std::endl;
  EXPECT_EQ( 0.1f, v )         << "Wrong v!" << std::endl;

}

TEST_F(ALGORITHM_TEST_BASE, triangleIntersection_NegativeShootingDirection_FlippedTriangleOrientation ) {


  // ==============================================
  // Triangle intersection algorithm
  // ==============================================

  /* All in z = 0 plane :

   (0,1)
    p2
    | \
    |  \
    |   \
    |    \
    |     \
    |      \
    p0 === p1
   (0,0)   (1,0)
   */

  // Triangle points
  ACG::Vec3f p0 ( 0.0,0.0,0.0);
  ACG::Vec3f p1 ( 1.0,0.0,0.0);
  ACG::Vec3f p2 ( 0.0,1.0,0.0);

  // Shooting ray origin and direction
  ACG::Vec3f origin( 0.1, 0.1, -1.0);
  ACG::Vec3f direction( 0.0, 0.0, -1.0);

  float distance,u,v;
  bool result = ACG::Geometry::triangleIntersection(origin, direction,
                                                    p0, p2, p1,
                                                    distance,u,v);

  EXPECT_TRUE( result )        << "Intersection failed!";

  EXPECT_EQ( -1.0f, distance ) << "Wrong distance!" << std::endl;
  EXPECT_EQ( 0.1f, u )         << "Wrong u!" << std::endl;
  EXPECT_EQ( 0.1f, v )         << "Wrong v!" << std::endl;

}




