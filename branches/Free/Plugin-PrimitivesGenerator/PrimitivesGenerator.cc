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



#include "PrimitivesGenerator.hh"


#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/Geometry/Algorithms.hh>

PrimitivesGeneratorPlugin::PrimitivesGeneratorPlugin() :
        triMesh_(0),
        polyMesh_(0),
        slices_(50),
        stacks_(50)
{

}

PrimitivesGeneratorPlugin::~PrimitivesGeneratorPlugin()
{

}

void PrimitivesGeneratorPlugin::initializePlugin()
{
  emit setSlotDescription("addTetrahedron()"          ,tr("Generates a tetrahedron (ObjectId is returned)")            ,QStringList(), QStringList());
  emit setSlotDescription("addTriangulatedCube()"     ,tr("Generates a cube (ObjectId is returned)")                   ,QStringList(), QStringList());
  emit setSlotDescription("addIcosahedron()"          ,tr("Generates an icosahedron (ObjectId is returned)")           ,QStringList(), QStringList());
  emit setSlotDescription("addPyramid()"              ,tr("Generates a pyramid (ObjectId is returned)")                ,QStringList(), QStringList());
  emit setSlotDescription("addOctahedron()"           ,tr("Generates an octahedron (ObjectId is returned)")            ,QStringList(), QStringList());
  emit setSlotDescription("addDodecahedron()"         ,tr("Generates a dodecahedron (ObjectId is returned)")           ,QStringList(), QStringList());
  emit setSlotDescription("addSphere()"               ,tr("Generates a sphere (ObjectId is returned)")                 ,QStringList(), QStringList());
  emit setSlotDescription("addTriangulatedCylinder()" ,tr("Generates a triangulated cylinder (ObjectId is returned)")  ,QStringList(), QStringList());
}

void PrimitivesGeneratorPlugin::pluginsInitialized() {

  if ( OpenFlipper::Options::gui()) {
    // Primitives Menu
    QMenu *primitivesMenu;

    emit getMenubarMenu(tr("&Primitives"), primitivesMenu, true );

    QIcon* icon;
    QAction* action;

    action = primitivesMenu->addAction("Cube (Triangle Mesh)"       ,this,SLOT(addTriangulatedCube()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cube.png");
    action->setIcon(*icon);

    WhatsThisGenerator whatsThisGen("PrimitivesGenerator");
    whatsThisGen.setWhatsThis(action,tr("Create a Cube."),"Cube");

    action = primitivesMenu->addAction("Dodecahedron"               ,this,SLOT(addDodecahedron()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_dodecahedron.png");
    action->setIcon(*icon);
    whatsThisGen.setWhatsThis(action,tr("Create a Dodecahedron."), "Dodecahedron");

    action = primitivesMenu->addAction("Icosahedron"                ,this,SLOT(addIcosahedron()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_icosahedron.png");
    action->setIcon(*icon);
    whatsThisGen.setWhatsThis(action,tr("Create a Icosahedron.","Icosahedron"));

    action = primitivesMenu->addAction("Octahedron"                  ,this,SLOT(addOctahedron()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_octahedron.png");
    action->setIcon(*icon);
    whatsThisGen.setWhatsThis(action,tr("Create an Octahedron."),"Octahedron");

    action = primitivesMenu->addAction("Pyramid"                    ,this,SLOT(addPyramid()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_pyramid.png");
    action->setIcon(*icon);
    whatsThisGen.setWhatsThis(action,tr("Create a Pyramid."),"Pyramid");

    action = primitivesMenu->addAction("Cylinder (Triangle Mesh)"       ,this,SLOT(addTriangulatedCylinder()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cylinder.png");
    action->setIcon(*icon);

    action = primitivesMenu->addAction("Sphere",this,SLOT(addSphere()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_sphere.png");
    action->setIcon(*icon);
    whatsThisGen.setWhatsThis(action,tr("Create a Sphere. "),"Sphere");

    action = primitivesMenu->addAction("Tetrahedron",this,SLOT(addTetrahedron()));
    icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_tetrahedron.png");
    action->setIcon(*icon);
    whatsThisGen.setWhatsThis(action,tr("Create a Tetrahedron."),"Tetrahedron");
  }

}


int PrimitivesGeneratorPlugin::addTriMesh() {
  int objectId = -1;

  emit addEmptyObject( DATA_TRIANGLE_MESH, objectId );

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"Unable to create new Object");
    return -1;
  }

  return objectId;
}

int PrimitivesGeneratorPlugin::addPolyMesh() {
  int objectId = -1;

  emit addEmptyObject( DATA_POLY_MESH, objectId );

  PolyMeshObject* object;
  if ( !PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"Unable to create new Object");
    return -1;
  }

  return objectId;
}

int PrimitivesGeneratorPlugin::addTetrahedron() {

  int newObject = addTriMesh();

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) ) {
        emit log(LOGERR,"Unable to create new Object");
        return -1;
  } else {

    object->setName( "Tetrahedron " + QString::number(newObject) );

    triMesh_ =  object->mesh();

    triMesh_->clear();

    // Add 4 vertices
    vhandles_.resize(4);

    static const double size = 1.0;

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point(-size, -size,  size));
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( size,  size,  size));
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-size,  size, -size));
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point( size, -size, -size));

    // Add 4 faces
    add_face(0,1,2);
    add_face(0,2,3);
    add_face(2,1,3);
    add_face(3,1,0);

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return newObject;
  }

  return -1;
}

int PrimitivesGeneratorPlugin::addTriangulatedCube() {


  int newObject = addTriMesh();

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) ) {
        emit log(LOGERR,"Unable to create new Object");
        return -1;
  } else {

    object->setName( "Cube " + QString::number(newObject) );

    triMesh_ =  object->mesh();

    triMesh_->clear();

    // Add 8 vertices
    vhandles_.resize(8);

    static const double size = 1.0;

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point( size, -size, size));
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( size,  size, size));
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-size,  size, size));
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point(-size, -size, size));
    vhandles_[4] = triMesh_->add_vertex(TriMesh::Point( size, -size,-size));
    vhandles_[5] = triMesh_->add_vertex(TriMesh::Point( size,  size,-size));
    vhandles_[6] = triMesh_->add_vertex(TriMesh::Point(-size,  size,-size));
    vhandles_[7] = triMesh_->add_vertex(TriMesh::Point(-size, -size,-size));


    // Add 12 faces
    add_face(0,1,2);
    add_face(0,2,3);
    add_face(0,5,1);
    add_face(5,0,4);
    add_face(4,0,7);
    add_face(7,0,3);

    add_face(7,3,6);
    add_face(6,3,2);
    add_face(6,2,5);
    add_face(5,2,1);
    add_face(6,5,4);
    add_face(6,4,7);

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return newObject;
  }

  return -1;
}

//========================================================================
// Cylinder
//========================================================================


ACG::Vec3d PrimitivesGeneratorPlugin::positionOnCylinder(int _sliceNumber, int _stackNumber)
{
  ACG::Vec3d position;

  const double height     = 5.0;
  const double ringRadius = 1.0;

  const ACG::Vec3d bottomPosition(0.0,0.0,0.0);
  const ACG::Vec3d axis(1.0,0.0,0.0);
  const ACG::Vec3d right = ACG::Geometry::perpendicular(axis);
  const ACG::Vec3d left  = cross( axis, right);


  //double alpha = (M_PI / double(stacks_)) * double(_stackNumber);
  double beta = ((2.0 * M_PI) / double(slices_)) * double(_sliceNumber);

  if ( _sliceNumber == 0 &&  _stackNumber == 0) {
    position[0] = 0.0;
    position[1] = 0.0;
    position[2] = height;
  } else  if ( _sliceNumber == slices_ &&  _stackNumber == stacks_ ) {
    position[0] = 0.0;
    position[1] = 0.0;
    position[2] = 0.0;
  } else {
    position[0] = sin(beta) * ringRadius;
    position[1] = cos(beta) * ringRadius;
    position[2] = height * double(stacks_ - _stackNumber -1 ) / double(stacks_-2);
  }

  position = bottomPosition + position[0] * right + position[1] * left + position[2] * axis ;

  return position;
}


int PrimitivesGeneratorPlugin::addTriangulatedCylinder() {

  // TODO: Generate texture coordinates for cylinder (Glu compatible)
  int newObject = addTriMesh();

  TriMeshObject* object;
  if (!PluginFunctions::getObject(newObject, object)) {
    emit log(LOGERR, "Unable to create new Object");
    return -1;
  } else {

    object->setName( "Cylinder " + QString::number(newObject) );

    triMesh_ = object->mesh();

    triMesh_->clear();

    //triMesh_->request_vertex_texcoords2D();

    TriMesh::VertexHandle vh;

    vh = triMesh_->add_vertex(positionOnCylinder(0, 0));
    //triMesh_->set_texcoord2D(vh, texCoordOnSphere(0, 0));

    for (int st = 1; st < stacks_; ++st) {
      for (int sl = 0; sl < slices_; ++sl) {
        vh = triMesh_->add_vertex(positionOnCylinder(sl, st));
        //triMesh_->set_texcoord2D(vh, texCoordOnSphere(sl, st));
      }
    }

    vh = triMesh_->add_vertex(positionOnCylinder(slices_, stacks_));
    //triMesh_->set_texcoord2D(vh, texCoordOnSphere(slices_, stacks_));

    std::vector<TriMesh::VertexHandle> vhandles;

    // Add top triangle fan ( Vertex index is shifted by one for the first slice )
    for (int sl = 1; sl < slices_ + 1; ++sl) {

      vhandles.clear();

      vhandles.push_back(triMesh_->vertex_handle(sl));
      vhandles.push_back(triMesh_->vertex_handle(0));
      vhandles.push_back(triMesh_->vertex_handle(1 * 1 + (sl % slices_)));

      triMesh_->add_face(vhandles);
    }

    for (int st = 0; st < stacks_ - 2; ++st) {

      // Move around one slice
      for (int sl = 0; sl < slices_; ++sl) {

        // Offset 1 because of singular vertex
        unsigned int startTop = 1 + slices_ * st;
        unsigned int startBottom = 1 + slices_ * (st + 1);

        vhandles.clear();

        vhandles.push_back(triMesh_->vertex_handle(startTop + sl));
        vhandles.push_back(triMesh_->vertex_handle(startTop + ((sl + 1) % slices_)));
        vhandles.push_back(triMesh_->vertex_handle(startBottom + sl));

        triMesh_->add_face(vhandles);

        vhandles.clear();

        vhandles.push_back(triMesh_->vertex_handle(startBottom + sl));
        vhandles.push_back(triMesh_->vertex_handle(startTop + ((sl + 1) % slices_)));
        vhandles.push_back(triMesh_->vertex_handle(startBottom + ((sl + 1) % slices_)));

        triMesh_->add_face(vhandles);
      }

    }

    const int startTop     = 1 + (stacks_ - 2) * slices_;
    const int bottomVertex = 1 + (stacks_ - 1) * slices_;

    // Add bottom triangle fan
    for (int sl = 0; sl < slices_; ++sl) {

      vhandles.clear();

      vhandles.push_back(triMesh_->vertex_handle(bottomVertex));
      vhandles.push_back(triMesh_->vertex_handle(startTop + sl));
      vhandles.push_back(triMesh_->vertex_handle(startTop + ((sl + 1) % slices_)));

      triMesh_->add_face(vhandles);
    }

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return object->id();
  }

}


//========================================================================
// Sphere
//========================================================================



ACG::Vec3d PrimitivesGeneratorPlugin::positionOnSphere(int _sliceNumber, int _stackNumber)
{
  ACG::Vec3d position;

  double alpha = (M_PI / double(stacks_)) * double(_stackNumber);
  double beta = ((2.0 * M_PI) / double(slices_)) * double(_sliceNumber);

  double ringRadius = sin(alpha);
  position[0] = sin(beta) * ringRadius;
  position[1] = cos(beta) * ringRadius;
  position[2] = cos(alpha);

  return position;
}

//------------------------------------------------------------------------

ACG::Vec2f PrimitivesGeneratorPlugin::texCoordOnSphere(int _sliceNumber, int _stackNumber)
{
  ACG::Vec2f texCoord;

  double alpha = (M_PI / double(stacks_)) * double(_stackNumber);
  texCoord[0] = double(_sliceNumber) / double(slices_);
  texCoord[1] = 0.5 * (cos(alpha) + 1.0);

  return texCoord;
}


//------------------------------------------------------------------------

int PrimitivesGeneratorPlugin::addSphere() {

  int newObject = addTriMesh();

  TriMeshObject* object;
  if (!PluginFunctions::getObject(newObject, object)) {
    emit log(LOGERR, "Unable to create new Object");
    return -1;
  } else {

    object->setName( "Sphere " + QString::number(newObject) );

    triMesh_ = object->mesh();

    triMesh_->clear();

    triMesh_->request_vertex_texcoords2D();

    TriMesh::VertexHandle vh;

    vh = triMesh_->add_vertex(positionOnSphere(0, 0));
    triMesh_->set_texcoord2D(vh, texCoordOnSphere(0, 0));

    for (int st = 1; st < stacks_; ++st) {
      for (int sl = 0; sl < slices_; ++sl) {
        vh = triMesh_->add_vertex(positionOnSphere(sl, st));
        triMesh_->set_texcoord2D(vh, texCoordOnSphere(sl, st));
      }
    }

    vh = triMesh_->add_vertex(positionOnSphere(slices_, stacks_));
    triMesh_->set_texcoord2D(vh, texCoordOnSphere(slices_, stacks_));

    std::vector<TriMesh::VertexHandle> vhandles;

    // Add top triangle fan ( Vertex index is shifted by one for the first slice )
    for (int sl = 1; sl < slices_ + 1; ++sl) {

      vhandles.clear();

      vhandles.push_back(triMesh_->vertex_handle(sl));
      vhandles.push_back(triMesh_->vertex_handle(0));
      vhandles.push_back(triMesh_->vertex_handle(1 * 1 + (sl % slices_)));

      triMesh_->add_face(vhandles);
    }

    for (int st = 0; st < stacks_ - 2; ++st) {

      // Move around one slice
      for (int sl = 0; sl < slices_; ++sl) {

        // Offset 1 because of singular vertex
        unsigned int startTop = 1 + slices_ * st;
        unsigned int startBottom = 1 + slices_ * (st + 1);

        vhandles.clear();

        vhandles.push_back(triMesh_->vertex_handle(startTop + sl));
        vhandles.push_back(triMesh_->vertex_handle(startTop + ((sl + 1) % slices_)));
        vhandles.push_back(triMesh_->vertex_handle(startBottom + sl));

        triMesh_->add_face(vhandles);

        vhandles.clear();

        vhandles.push_back(triMesh_->vertex_handle(startBottom + sl));
        vhandles.push_back(triMesh_->vertex_handle(startTop + ((sl + 1) % slices_)));
        vhandles.push_back(triMesh_->vertex_handle(startBottom + ((sl + 1) % slices_)));

        triMesh_->add_face(vhandles);
      }

    }

    const int startTop     = 1 + (stacks_ - 2) * slices_;
    const int bottomVertex = 1 + (stacks_ - 1) * slices_;

    // Add bottom triangle fan
    for (int sl = 0; sl < slices_; ++sl) {

      vhandles.clear();

      vhandles.push_back(triMesh_->vertex_handle(bottomVertex));
      vhandles.push_back(triMesh_->vertex_handle(startTop + sl));
      vhandles.push_back(triMesh_->vertex_handle(startTop + ((sl + 1) % slices_)));

      triMesh_->add_face(vhandles);
    }

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return object->id();
  }


}


//========================================================================
// Pyramid
//========================================================================

int PrimitivesGeneratorPlugin::addPyramid() {
  int newObject = addTriMesh();

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) ) {
        emit log(LOGERR,"Unable to create new Object");
        return -1;
  } else {

    object->setName( "Pyramid " + QString::number(newObject) );

    triMesh_ =  object->mesh();

    triMesh_->clear();

    // Add 5 vertices
    vhandles_.resize(5);

    static const double size   = 1.0;

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point( size, -size, 0.0));
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( size,  size, 0.0));
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-size,  size, 0.0));
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point(-size, -size, 0.0));

    vhandles_[4] = triMesh_->add_vertex(TriMesh::Point(0.0, 0.0, sqrt(2.0)*size));

    // Add 6 faces
    add_face(2,1,0);
    add_face(3,2,0);
    add_face(4,0,1);

    add_face(3,0,4);
    add_face(4,2,3);
    add_face(1,2,4);

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return newObject;
  }

  return -1;
}

void PrimitivesGeneratorPlugin::add_face( int _vh1 , int _vh2, int _vh3 ) {
 std::vector<TriMesh::VertexHandle> vhandles;

 vhandles.push_back(vhandles_[_vh1]);
 vhandles.push_back(vhandles_[_vh2]);
 vhandles.push_back(vhandles_[_vh3]);

 triMesh_->add_face(vhandles);

}

void PrimitivesGeneratorPlugin::add_face( int _vh1 , int _vh2, int _vh3, int _vh4 , int _vh5 ) {
  std::vector<PolyMesh::VertexHandle> vhandles;

  vhandles.push_back(vphandles_[_vh1]);
  vhandles.push_back(vphandles_[_vh2]);
  vhandles.push_back(vphandles_[_vh3]);
  vhandles.push_back(vphandles_[_vh4]);
  vhandles.push_back(vphandles_[_vh5]);

  polyMesh_->add_face(vhandles);
}

int PrimitivesGeneratorPlugin::addIcosahedron() {
  int newObject = addTriMesh();

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) ) {
        emit log(LOGERR,"Unable to create new Object");
        return -1;
  } else {

    object->setName( "Icosahedron " + QString::number(newObject) );

    triMesh_ =  object->mesh();

    triMesh_->clear();

    // Add 12 vertices
    vhandles_.resize(12);

    double phi  = 0.5 * (1.0 + sqrt(5.0));
    //double norm = 1.0 / sqrt(1.0 + phi*phi);
    double norm = 1.0;

    vhandles_[0 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,  -1.0 , -phi ));
    vhandles_[1 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,   1.0 , -phi ));
    vhandles_[2 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,   1.0 ,  phi ));
    vhandles_[3 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,  -1.0 ,  phi ));

    vhandles_[4 ] = triMesh_->add_vertex(norm * TriMesh::Point( -1.0 , -phi ,  0.0  ));
    vhandles_[5 ] = triMesh_->add_vertex(norm * TriMesh::Point(  1.0 , -phi ,  0.0  ));
    vhandles_[6 ] = triMesh_->add_vertex(norm * TriMesh::Point(  1.0 ,  phi ,  0.0  ));
    vhandles_[7 ] = triMesh_->add_vertex(norm * TriMesh::Point( -1.0 ,  phi ,  0.0  ));

    vhandles_[8 ] = triMesh_->add_vertex(norm * TriMesh::Point( -phi ,  0.0 , -1.0  ));
    vhandles_[9 ] = triMesh_->add_vertex(norm * TriMesh::Point( -phi ,  0.0 ,  1.0  ));
    vhandles_[10] = triMesh_->add_vertex(norm * TriMesh::Point(  phi ,  0.0 ,  1.0  ));
    vhandles_[11] = triMesh_->add_vertex(norm * TriMesh::Point(  phi ,  0.0 , -1.0  ));


    // Add 20 faces
    add_face(2,6,7);
    add_face(7,6,1);

    add_face(11,0,1);
    add_face(0, 8,1);

    add_face(4,9,8);
    add_face(8,9,7);

    add_face(9,3,2);
    add_face(10,2,3);

    add_face(5,11,10);
    add_face(11,6,10);

    add_face(0,5,4);
    add_face(5,3,4);

    // Upper block
    add_face(6,2,10);
    add_face(6,11,1);
    add_face(1,8,7);
    add_face(9,2,7);

    // Lower block
    add_face(3,5,10);
    add_face(0,11,5);
    add_face(3,9,4);
    add_face(0,4,8);

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return newObject;
  }

  return -1;
}

int PrimitivesGeneratorPlugin::addOctahedron() {
  int newObject = addTriMesh();

  TriMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) ) {
    emit log(LOGERR,"Unable to create new Object");
    return -1;
  } else {

    object->setName( "Octahedron " + QString::number(newObject) );

    triMesh_ =  object->mesh();

    triMesh_->clear();

    // Add 6 vertices
    vhandles_.resize(6);

    static const double size = sqrt(2.0);

    vhandles_[0 ] = triMesh_->add_vertex(TriMesh::Point(-size,   0.0,   0.0));
    vhandles_[1 ] = triMesh_->add_vertex(TriMesh::Point(  0.0, -size,   0.0));
    vhandles_[2 ] = triMesh_->add_vertex(TriMesh::Point( size,   0.0,   0.0));
    vhandles_[3 ] = triMesh_->add_vertex(TriMesh::Point(  0.0,  size,   0.0));

    vhandles_[4 ] = triMesh_->add_vertex(TriMesh::Point(  0.0,   0.0,  size));
    vhandles_[5 ] = triMesh_->add_vertex(TriMesh::Point(  0.0,   0.0, -size));


    // Add 8 faces
    add_face(0,1,4);
    add_face(1,2,4);

    add_face(2,3,4);
    add_face(0,4,3);

    add_face(5,1,0);
    add_face(5,2,1);

    add_face(5,3,2);
    add_face(5,0,3);

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return newObject;
  }

  return -1;
}

int PrimitivesGeneratorPlugin::addDodecahedron() {
  int newObject = addPolyMesh();

  PolyMeshObject* object;
  if ( !PluginFunctions::getObject(newObject,object) ) {
    emit log(LOGERR,"Unable to create new Object");
    return -1;
  } else {

    object->setName( "Dodecahedron " + QString::number(newObject) );

    polyMesh_ =  object->mesh();

    polyMesh_->clear();

    // Add 20 vertices
    vphandles_.resize(20);

    double phi = (1.0 + sqrt(5.0)) / 2.0;

    vphandles_[0 ] = polyMesh_->add_vertex(TriMesh::Point(  1.0 ,  1.0 , 1.0 ));
    vphandles_[1 ] = polyMesh_->add_vertex(TriMesh::Point(  1.0 ,  1.0 ,-1.0 ));
    vphandles_[2 ] = polyMesh_->add_vertex(TriMesh::Point(  1.0 , -1.0 , 1.0 ));
    vphandles_[3 ] = polyMesh_->add_vertex(TriMesh::Point(  1.0 , -1.0 ,-1.0 ));
    vphandles_[4 ] = polyMesh_->add_vertex(TriMesh::Point( -1.0 ,  1.0 , 1.0 ));
    vphandles_[5 ] = polyMesh_->add_vertex(TriMesh::Point( -1.0 ,  1.0 ,-1.0 ));
    vphandles_[6 ] = polyMesh_->add_vertex(TriMesh::Point( -1.0 , -1.0 , 1.0 ));
    vphandles_[7 ] = polyMesh_->add_vertex(TriMesh::Point( -1.0 , -1.0 ,-1.0 ));

    vphandles_[8 ] = polyMesh_->add_vertex(TriMesh::Point( 0.0 ,  1.0 / phi ,  phi ));
    vphandles_[9 ] = polyMesh_->add_vertex(TriMesh::Point( 0.0 ,  1.0 / phi , -phi ));
    vphandles_[10] = polyMesh_->add_vertex(TriMesh::Point( 0.0 , -1.0 / phi ,  phi ));
    vphandles_[11] = polyMesh_->add_vertex(TriMesh::Point( 0.0 , -1.0 / phi , -phi ));

    vphandles_[12] = polyMesh_->add_vertex(TriMesh::Point(  1.0 / phi ,  phi, 0.0));
    vphandles_[13] = polyMesh_->add_vertex(TriMesh::Point(  1.0 / phi , -phi, 0.0));
    vphandles_[14] = polyMesh_->add_vertex(TriMesh::Point( -1.0 / phi ,  phi, 0.0));
    vphandles_[15] = polyMesh_->add_vertex(TriMesh::Point( -1.0 / phi , -phi, 0.0));

    vphandles_[16] = polyMesh_->add_vertex(TriMesh::Point(  phi , 0.0 , 1.0 / phi));
    vphandles_[17] = polyMesh_->add_vertex(TriMesh::Point(  phi , 0.0 ,-1.0 / phi));
    vphandles_[18] = polyMesh_->add_vertex(TriMesh::Point( -phi , 0.0 , 1.0 / phi));
    vphandles_[19] = polyMesh_->add_vertex(TriMesh::Point( -phi , 0.0 ,-1.0 / phi));

    // Add 12 faces
    add_face(14, 5,19,18, 4);
    add_face( 5, 9,11, 7,19);
    add_face( 6,15,13, 2,10);
    add_face(12, 0,16,17, 1);

    add_face( 0, 8,10, 2,16);
    add_face(16, 2,13, 3,17);
    add_face( 3,13,15, 7,11);
    add_face( 7,15, 6,18,19);

    add_face( 4,18, 6,10, 8);
    add_face( 4, 8, 0,12,14);
    add_face(14,12, 1, 9, 5);
    add_face( 9, 1,17, 3,11);

    polyMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    return newObject;
  }

  return -1;
}




Q_EXPORT_PLUGIN2( primitivesgeneratorplugin , PrimitivesGeneratorPlugin );

