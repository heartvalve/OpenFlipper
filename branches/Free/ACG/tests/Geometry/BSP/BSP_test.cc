/*
 * BSP_test.cc
 *
 *  Created on: 31.07.2012
 *      Author: moebius
 */

#include <gtest/gtest.h>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <ACG/Geometry/bsp/TriangleBSPT.hh>


struct CustomTraits : public OpenMesh::DefaultTraits {
};

typedef OpenMesh::TriMesh_ArrayKernelT<CustomTraits> Mesh;

typedef OpenMeshTriangleBSPT< Mesh > BSP;

class BSP_CUBE_BASE : public testing::Test {

    protected:

        // This function is called before each test is run
        virtual void SetUp() {

          mesh_.clear();

            // Add some vertices
            Mesh::VertexHandle vhandle[8];
            vhandle[0] = mesh_.add_vertex(Mesh::Point(-1, -1,  1));
            vhandle[1] = mesh_.add_vertex(Mesh::Point( 1, -1,  1));
            vhandle[2] = mesh_.add_vertex(Mesh::Point( 1,  1,  1));
            vhandle[3] = mesh_.add_vertex(Mesh::Point(-1,  1,  1));
            vhandle[4] = mesh_.add_vertex(Mesh::Point(-1, -1, -1));
            vhandle[5] = mesh_.add_vertex(Mesh::Point( 1, -1, -1));
            vhandle[6] = mesh_.add_vertex(Mesh::Point( 1,  1, -1));
            vhandle[7] = mesh_.add_vertex(Mesh::Point(-1,  1, -1));

            // Add six faces to form a cube
            std::vector<Mesh::VertexHandle> face_vhandles;

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[0]);
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[3]);
            mesh_.add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[2]);
            face_vhandles.push_back(vhandle[3]);
            mesh_.add_face(face_vhandles);

            //=======================

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[7]);
            face_vhandles.push_back(vhandle[6]);
            face_vhandles.push_back(vhandle[5]);
            mesh_.add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[7]);
            face_vhandles.push_back(vhandle[5]);
            face_vhandles.push_back(vhandle[4]);
            mesh_.add_face(face_vhandles);

            //=======================

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[0]);
            face_vhandles.push_back(vhandle[4]);
            mesh_.add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[4]);
            face_vhandles.push_back(vhandle[5]);
            mesh_.add_face(face_vhandles);

            //=======================

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[2]);
            face_vhandles.push_back(vhandle[1]);
            face_vhandles.push_back(vhandle[5]);
            mesh_.add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[2]);
            face_vhandles.push_back(vhandle[5]);
            face_vhandles.push_back(vhandle[6]);
            mesh_.add_face(face_vhandles);


            //=======================

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[3]);
            face_vhandles.push_back(vhandle[2]);
            face_vhandles.push_back(vhandle[6]);
            mesh_.add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[3]);
            face_vhandles.push_back(vhandle[6]);
            face_vhandles.push_back(vhandle[7]);
            mesh_.add_face(face_vhandles);

            //=======================

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[0]);
            face_vhandles.push_back(vhandle[3]);
            face_vhandles.push_back(vhandle[7]);
            mesh_.add_face(face_vhandles);

            face_vhandles.clear();
            face_vhandles.push_back(vhandle[0]);
            face_vhandles.push_back(vhandle[7]);
            face_vhandles.push_back(vhandle[4]);
            mesh_.add_face(face_vhandles);


            // Test setup:
            //
            //
            //      3 ======== 2
            //     / \        /
            //    /   \      / |
            //   /     \    /  |
            //  /       \  /   |
            // 0 ======== 1    |
            // |       /  |    |
            // |      /   |    6    z
            // |     /    |   /     |   y
            // |    /     |  /      |  /
            // |   /      | /       | /
            // |  /       |/        |/
            // 4 ======== 5         -------> x
            //


            //      x ======== x
            //     / \        /
            //    /   \  1   / |
            //   /  0  \    /  |
            //  /       \  /   |
            // x ======== x    |
            // |       /  |    |
            // |  4   /   |    x    z
            // |     /    |   /     |   y
            // |    /  5  |  /      |  /
            // |   /      | /       | /
            // |  /       |/        |/
            // x ======== x         -------> x
            //

            // create Triangle BSP
            bsp_ = new BSP( mesh_ );

            // build Triangle BSP
            bsp_->reserve(mesh_.n_faces());

            Mesh::FIter f_it  = mesh_.faces_begin();
            Mesh::FIter f_end = mesh_.faces_end();

            for (; f_it!=f_end; ++f_it)
              bsp_->push_back(f_it.handle());

            bsp_->build(10, 100); //max vertices per leaf 10, max depth 100

        }

        // This function is called after all tests are through
        virtual void TearDown() {

            // Do some final stuff with the member data here...
        }

    // This member will be accessible in all tests
    Mesh mesh_;

    //This will be the tree
    BSP* bsp_;
};


/* Basic check if the test mesh is setup correctly
 */
TEST_F(BSP_CUBE_BASE, CheckCubeConstruction) {

  // Check setup of the mesh
  EXPECT_EQ(8u, mesh_.n_vertices() ) << "Wrong number of vertices";
  EXPECT_EQ(12u, mesh_.n_faces() )   << "Wrong number of faces";

}

/* Basic check if the bsp can be setup
 */
TEST_F(BSP_CUBE_BASE, SetupBSPfromCube ) {

  EXPECT_FALSE(bsp_->empty()) << "BSP should not be empty !";
  EXPECT_EQ(12u, bsp_->size()) << "Wrong number of triangles in BSP after construction";

}

/* Nearest neighbor check
 */
TEST_F(BSP_CUBE_BASE, NearestNeighbour_OnSurface_1 ) {

  // (-1,1)      (1,1)
  //    x ======== x
  //    |       /  |             y = -1 plane
  //    |  4   /   |        z
  //    |     /    |        |
  //    |    /  5  |        |
  //    |   /      |        |
  //    |  /       |        |
  //    x ======== x         -------> x
  // (-1,-1)     (1,-1)


  // Point close to face 5
  // (-1,1)      (1,1)
  //    x ======== x
  //    |       /  |
  //    |      /   |
  //    |     /    |
  //    |    /  p  |
  //    |   /      |
  //    |  /       |
  //    x ======== x
  // (-1,-1)     (1,-1)
  Mesh::Point p1(0.75,-1.0,0.0);
  BSP::NearestNeighbor nn = bsp_->nearest(p1);

  EXPECT_EQ(5, nn.handle.idx()) << "Wrong Handle on closest face";
}

TEST_F(BSP_CUBE_BASE, NearestNeighbour_OnSurface_2 ) {
  // Point close to face 4
  // (-1,1)      (1,1)
  //    x ======== x
  //    |       /  |
  //    |      /   |
  //    |     /    |
  //    | p  /     |
  //    |   /      |
  //    |  /       |
  //    x ======== x
  // (-1,-1)     (1,-1)
  Mesh::Point p1(-0.75,-1.0,0.0);
  BSP::NearestNeighbor nn = bsp_->nearest(p1);
  EXPECT_EQ(4, nn.handle.idx()) << "Wrong Handle on closest face";
}

TEST_F(BSP_CUBE_BASE, NearestNeighbour_OnSurface_3 ) {
  // Point close to face 4
  // (-1,1)      (1,1)
  //    x ======== x
  //    | p     /  |
  //    |      /   |
  //    |     /    |
  //    |    /     |
  //    |   /      |
  //    |  /       |
  //    x ======== x
  // (-1,-1)     (1,-1)
  Mesh::Point p1(-0.75,-1.0,0.75);
  BSP::NearestNeighbor nn = bsp_->nearest(p1);
  EXPECT_EQ(4, nn.handle.idx()) << "Wrong Handle on closest face";
}

TEST_F(BSP_CUBE_BASE, NearestNeighbour_OnSurface_4 ) {

  // Point close to face 5
  // (-1,1)      (1,1)
  //    x ======== x
  //    |       /  |
  //    |      /   |
  //    |     /    |
  //    |    /     |
  //    |   /      |
  //    |  /     p |
  //    x ======== x
  // (-1,-1)     (1,-1)
  Mesh::Point p1(0.75,-1.0,-0.75);
  BSP::NearestNeighbor nn = bsp_->nearest(p1);
  EXPECT_EQ(5, nn.handle.idx()) << "Wrong Handle on closest face";

}

TEST_F(BSP_CUBE_BASE, RayIntersectionAboveSurface_1 ) {


  // ==============================================
  // Test some Rays
  // ==============================================

  // Shoot through face 4 in y direction
  // Start point is not on face 4
  //      x ======== x
  //     / \        /
  //    /   \  1   / |
  //   /  0  \    /  |
  //  /       \  /   |
  // x ======== x    |
  // |       /  |    |
  // |  4   /   |    x    z
  // |     /    |   /     |   y
  // | p  /  5  |  /      |  /
  // |   /      | /       | /
  // |  /       |/        |/
  // x ======== x         -------> x

  Mesh::Point yDirection(0.0,1.0,0.0);
  Mesh::Point p1(-0.5,-2.0,0.0);
  BSP::RayCollision rc = bsp_->raycollision(p1,yDirection);

  EXPECT_EQ(2u, rc.hit_vertices.size() ) << "Wrong number of hit faces in ray collision test 1";
  if ( rc.hit_vertices.size() == 2u ) { // Don't crash on wrong size
    EXPECT_EQ(4, rc.hit_vertices[0].idx() )         << "Wrong handle of first face in ray collision test 1";
    EXPECT_EQ(9, rc.hit_vertices[1].idx() )         << "Wrong handle of second face in ray collision test 1";
  }

  EXPECT_EQ(4,  rc.handle.idx() )         << "Wrong handle of closest face in ray collision test 1";
}

//TEST_F(BSP_CUBE_BASE, RayIntersectionAboveSurfaceNegativeDirection_1 ) {
//  // ==============================================
//  // Test some Rays
//  // ==============================================
//
//  // Shoot through face 4 in y direction
//  // Start point is not on face 4
//  //      x ======== x
//  //     / \        /
//  //    /   \  1   / |
//  //   /  0  \    /  |
//  //  /       \  /   |
//  // x ======== x    |
//  // |       /  |    |
//  // |  4   /   |    x    z
//  // |     /    |   /     |   y
//  // | p  /  5  |  /      |  /
//  // |   /      | /       | /
//  // |  /       |/        |/
//  // x ======== x         -------> x
//
//  Mesh::Point nyDirection(0.0,-1.0,0.0);
//  Mesh::Point p1(-0.5,-2.0,0.0);
//  BSP::RayCollision rc = bsp_->raycollision(p1,nyDirection);
//
//  EXPECT_EQ(2u, rc.hit_vertices.size() ) << "Wrong number of hit faces in ray collision test 1";
//  if ( rc.hit_vertices.size() == 2u ) { // Don't crash on wrong size
//    EXPECT_EQ(4, rc.hit_vertices[0].idx() )         << "Wrong handle of first face in ray collision test 1";
//    EXPECT_EQ(9, rc.hit_vertices[1].idx() )         << "Wrong handle of second face in ray collision test 1";
//  }
//
//  EXPECT_EQ(4,  rc.handle.idx() )         << "Wrong handle of closest face in ray collision test 1";
//}

TEST_F(BSP_CUBE_BASE, RayIntersectionAboveSurface_2 ) {
  // Shoot through face 5 in y direction
  // Start point is not on face 5
  //      x ======== x
  //     / \        /
  //    /   \  1   / |
  //   /  0  \    /  |
  //  /       \  /   |
  // x ======== x    |
  // |       /  |    |
  // |  4   /   |    x    z
  // |     /    |   /     |   y
  // | p  /  5  |  /      |  /
  // |   /      | /       | /
  // |  /       |/        |/
  // x ======== x         -------> x

  Mesh::Point yDirection(0.0,1.0,0.0);
  Mesh::Point p1(0.5,-2.0,0.0);
  BSP::RayCollision rc = bsp_->raycollision(p1,yDirection);

  EXPECT_EQ(2u, rc.hit_vertices.size() ) << "Wrong number of hit faces in ray collision test 2";
  if ( rc.hit_vertices.size() == 2u ) { // Don't crash on wrong size
    EXPECT_EQ(5, rc.hit_vertices[0].idx() )         << "Wrong handle of first face in ray collision test 2";
    EXPECT_EQ(8, rc.hit_vertices[1].idx() )         << "Wrong handle of second face in ray collision test 2";
  }

  EXPECT_EQ(5,  rc.handle.idx() )         << "Wrong handle of closest face in ray collision test 2";
}




