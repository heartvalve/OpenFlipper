/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2013 by Computer Graphics Group, RWTH Aachen       *
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

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
#include "TetrahedralCuboidGenerator.hh"
#endif

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/Geometry/Algorithms.hh>

PrimitivesGeneratorPlugin::PrimitivesGeneratorPlugin() :
        triMesh_(0),
        polyMesh_(0),
        slices_(50),
        stacks_(50),
        primitivesMenu_(0)
{

}

PrimitivesGeneratorPlugin::~PrimitivesGeneratorPlugin()
{

  if ( OpenFlipper::Options::gui()) {
    delete primitivesMenu_;
  }

}

void PrimitivesGeneratorPlugin::initializePlugin()
{
  emit setSlotDescription("addTetrahedron(Vector, double)",
                          tr("Generates a tetrahedron (ObjectId is returned)"),
                          QString("Position,Length").split(","),
                          QString("Center position,Length of each edge").split(","));

  emit setSlotDescription("addIcosahedron(Vector, double)",
                          tr("Generates an icosahedron (ObjectId is returned)"),
                          QString("Position,Length").split(","),
                          QString("Center position,Length of each edge").split(","));

  emit setSlotDescription("addPyramid(Vector, double)",
                          tr("Generates a pyramid (ObjectId is returned)"),
                          QString("Position,Length").split(","),
                          QString("Center position,Length of each edge").split(","));

  emit setSlotDescription("addOctahedron(Vector, double)",
                          tr("Generates an octahedron (ObjectId is returned)"),
                          QString("Position,Length").split(","),
                          QString("Center position,Length of each edge").split(","));

  emit setSlotDescription("addDodecahedron(Vector, double)",
                          tr("Generates a dodecahedron (ObjectId is returned)"),
                          QString("Position,Length").split(","),
                          QString("Center position,Length of each edge").split(","));

  emit setSlotDescription("addSphere(Vector, double)",
                          tr("Generates a sphere (ObjectId is returned)"),
                          QString("Position, Radius").split(","),
                          QString("Center position,Radius").split(","));

  emit setSlotDescription("addTriangulatedCube(Vector, double)",
                          tr("Generates a triangular mesh of cube (ObjectId is returned)"),
                          QString("Position,Length").split(","),
                          QString("Center position,Length of each edge").split(","));

  emit setSlotDescription("addTriangulatedCylinder(Vector,Vector,double,double)",
                          tr("Generates a triangulated cylinder (ObjectId is returned)")  ,
                          QString("Position,Axis,Radius,Height,Top,Bottom").split(","),
                          QString("Bottom center vertex position,Center axis,radius,height,add top vertex,add bottom vertex").split(","));

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  emit setSlotDescription("addTetrahedralCube(Vector,double)",
                          tr("Generates a tetrahedral mesh of a cube (ObjectId is returned)"),
                          QString("Position,Length").split(","),
                          QString("Center position,Length of each edge").split(","));

  emit setSlotDescription("addTetrahedralCuboid(Vector,Vector,uint,uint,uint)",
                          tr("Generates a tetrahedral mesh of a cuboid (ObjectId is returned)"),
                          QString("Position,Lengths,Count,Count,Count").split(","),
                          QString("Center position,Length of each side,Number of units in x-axis,Number of units in y-axis,Number of units in z-axis").split(","));
#endif

}

void PrimitivesGeneratorPlugin::pluginsInitialized() {

  if ( OpenFlipper::Options::gui()) {

    emit getMenubarMenu(tr("&Primitives"), primitivesMenu_, true );

    QAction* action;

    action = primitivesMenu_->addAction("Cube (Triangle Mesh)"       ,this,SLOT(addTriangulatedCube()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cube.png"));

    WhatsThisGenerator whatsThisGen("PrimitivesGenerator");
    whatsThisGen.setWhatsThis(action,tr("Create a Cube."),"Cube");

    action = primitivesMenu_->addAction("Dodecahedron"               ,this,SLOT(addDodecahedron()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_dodecahedron.png"));
    whatsThisGen.setWhatsThis(action,tr("Create a Dodecahedron."), "Dodecahedron");

    action = primitivesMenu_->addAction("Icosahedron"                ,this,SLOT(addIcosahedron()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_icosahedron.png"));
    whatsThisGen.setWhatsThis(action,tr("Create a Icosahedron.","Icosahedron"));

    action = primitivesMenu_->addAction("Octahedron"                  ,this,SLOT(addOctahedron()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_octahedron.png"));
    whatsThisGen.setWhatsThis(action,tr("Create an Octahedron."),"Octahedron");

    action = primitivesMenu_->addAction("Pyramid"                    ,this,SLOT(addPyramid()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_pyramid.png"));
    whatsThisGen.setWhatsThis(action,tr("Create a Pyramid."),"Pyramid");

    action = primitivesMenu_->addAction("Cylinder (Triangle Mesh)"       ,this,SLOT(addTriangulatedCylinder()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cylinder.png"));

    action = primitivesMenu_->addAction("Sphere",this,SLOT(addSphere()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_sphere.png"));
    whatsThisGen.setWhatsThis(action,tr("Create a Sphere. "),"Sphere");

    action = primitivesMenu_->addAction("Tetrahedron",this,SLOT(addTetrahedron()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_tetrahedron.png"));
    whatsThisGen.setWhatsThis(action,tr("Create a Tetrahedron."),"Tetrahedron");

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
    action = primitivesMenu_->addAction("Cube (Tetrahedral Mesh)"    ,this,SLOT(addTetrahedralCube()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cube.png"));
    whatsThisGen.setWhatsThis(action,tr("Create a Tetrahedral Cube."), "Cube");

    action = primitivesMenu_->addAction("Cuboid (Tetrahedral Mesh)"  ,this,SLOT(addTetrahedralCuboid()));
    action->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cube.png"));
    whatsThisGen.setWhatsThis(action,tr("Create a Tetrahedral Cuboid."), "Cuboid");
#endif
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


#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
int PrimitivesGeneratorPlugin::addPolyhedralMesh() {
    int objectId = -1;

    emit addEmptyObject( DATA_POLYHEDRAL_MESH, objectId );

    PolyhedralMeshObject* object;
    if (!PluginFunctions::getObject(objectId, object) ) {
        emit log(LOGERR, "Unable to create new PolyhedralMesh object");
        return -1;
    }

    return objectId;
}
#endif

int PrimitivesGeneratorPlugin::addTetrahedron(const Vector& _position, const double _length) {

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

    const double halfSize = 0.5*_length;

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point(-halfSize, -halfSize,  halfSize)+_position);
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( halfSize,  halfSize,  halfSize)+_position);
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-halfSize,  halfSize, -halfSize)+_position);
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point( halfSize, -halfSize, -halfSize)+_position);

    // Add 4 faces
    add_face(0,1,2);
    add_face(0,2,3);
    add_face(2,1,3);
    add_face(3,1,0);

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    PluginFunctions::viewAll();

    return newObject;
  }

  return -1;
}

int PrimitivesGeneratorPlugin::addTriangulatedCube(const Vector& _position,const double _length) {


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

    const double halfSize = 0.5*_length;

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point( halfSize, -halfSize, halfSize)+_position);
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( halfSize,  halfSize, halfSize)+_position);
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-halfSize,  halfSize, halfSize)+_position);
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point(-halfSize, -halfSize, halfSize)+_position);
    vhandles_[4] = triMesh_->add_vertex(TriMesh::Point( halfSize, -halfSize,-halfSize)+_position);
    vhandles_[5] = triMesh_->add_vertex(TriMesh::Point( halfSize,  halfSize,-halfSize)+_position);
    vhandles_[6] = triMesh_->add_vertex(TriMesh::Point(-halfSize,  halfSize,-halfSize)+_position);
    vhandles_[7] = triMesh_->add_vertex(TriMesh::Point(-halfSize, -halfSize,-halfSize)+_position);


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

    PluginFunctions::viewAll();

    return newObject;
  }

  return -1;
}

//========================================================================
// Tetrahedral cube
//========================================================================

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT

int PrimitivesGeneratorPlugin::addTetrahedralCube(const Vector& _position, const double _length)
{
    return addTetrahedralCuboid(_position, Vector(_length, _length, _length), 1, 1, 1);
}

#endif

//========================================================================
// Tetrahedral Cuboid
//========================================================================

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT

int PrimitivesGeneratorPlugin::addTetrahedralCuboid(const Vector& _position,
        const Vector& _length, const unsigned int  n_x, const unsigned int  n_y, const unsigned int  n_z)
{

    int object_id = addPolyhedralMesh();
    PolyhedralMeshObject* object;

    if (!PluginFunctions::getObject(object_id, object)) {
        return -1;
    }

    object->setName("Cuboid " + QString::number(object_id));

    TetrahedralCuboidGenerator gen;
    gen.generate(*(object->mesh()), _position, _length, n_x, n_y, n_z);

    emit updatedObject(object_id, UPDATE_ALL);

    object->setObjectDrawMode(ACG::SceneGraph::DrawModes::getDrawMode("Cells (flat shaded)"));
    PluginFunctions::viewAll();

    return object_id;
}

#endif

//========================================================================
// Cylinder
//========================================================================


ACG::Vec3d PrimitivesGeneratorPlugin::positionOnCylinder(const int _sliceNumber,
                                                         const int _stackNumber,
                                                         const Vector _position,
                                                         const Vector _axis,
                                                         const double _radius,
                                                         const double _height)
{
  ACG::Vec3d position;

  const ACG::Vec3d right = (ACG::Geometry::perpendicular(_axis)).normalized();
  const ACG::Vec3d left  = (cross( _axis, right)).normalized();

  double beta = ((2.0 * M_PI) / double(slices_)) * double(_sliceNumber);

  if ( _sliceNumber == 0 &&  _stackNumber == 0) {
    position[0] = 0.0;
    position[1] = 0.0;
    position[2] = _height;
  } else  if ( _sliceNumber == slices_ &&  _stackNumber == stacks_ ) {
    position[0] = 0.0;
    position[1] = 0.0;
    position[2] = 0.0;
  } else {
    position[0] = sin(beta) * _radius;
    position[1] = cos(beta) * _radius;
    position[2] = _height * double(stacks_ - _stackNumber -1 ) / double(stacks_-2);
  }

  position = _position + position[0] * right + position[1] * left + position[2] * _axis ;

  return position;
}

int PrimitivesGeneratorPlugin::addTriangulatedCylinder(const Vector& _position,const Vector& _axis,const double _radius,const double _height,const bool _top,const bool   _bottom ) {

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
    TriMesh::VertexHandle top = triMesh_->add_vertex(positionOnCylinder(0, 0,_position,_axis,_radius,_height));
    //triMesh_->set_texcoord2D(vh, texCoordOnSphere(0, 0));

    for (int st = 1; st < stacks_; ++st) {
      for (int sl = 0; sl < slices_; ++sl) {
        vh = triMesh_->add_vertex(positionOnCylinder(sl, st,_position,_axis,_radius,_height));
        //triMesh_->set_texcoord2D(vh, texCoordOnSphere(sl, st));
      }
    }

    TriMesh::VertexHandle bottom = triMesh_->add_vertex(positionOnCylinder(slices_, stacks_,_position,_axis,_radius,_height));
    //triMesh_->set_texcoord2D(vh, texCoordOnSphere(slices_, stacks_));

    std::vector<TriMesh::VertexHandle> vhandles;

    // Add top triangle fan ( Vertex index is shifted by one for the first slice )
    if ( _top ) {
      for (int sl = 1; sl < slices_ + 1; ++sl) {

        vhandles.clear();

        vhandles.push_back(triMesh_->vertex_handle(sl));
        vhandles.push_back(triMesh_->vertex_handle(0));
        vhandles.push_back(triMesh_->vertex_handle(1 * 1 + (sl % slices_)));

        triMesh_->add_face(vhandles);
      }
    } else {
      triMesh_->delete_vertex(top);
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
    if ( _bottom) {
      for (int sl = 0; sl < slices_; ++sl) {

        vhandles.clear();

        vhandles.push_back(triMesh_->vertex_handle(bottomVertex));
        vhandles.push_back(triMesh_->vertex_handle(startTop + sl));
        vhandles.push_back(triMesh_->vertex_handle(startTop + ((sl + 1) % slices_)));

        triMesh_->add_face(vhandles);
      }
    } else {
      triMesh_->delete_vertex(bottom);
    }

    // Cleanup if bottom or top vertex is missing
    triMesh_->garbage_collection();

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    PluginFunctions::viewAll();

    return object->id();
  }

}


//========================================================================
// Sphere
//========================================================================


ACG::Vec3d PrimitivesGeneratorPlugin::positionOnSphere(int _sliceNumber, int _stackNumber, double _radius, const Vector& _position)
{
  ACG::Vec3d position;

  double alpha = (M_PI / double(stacks_)) * double(_stackNumber);
  double beta = ((2.0 * M_PI) / double(slices_)) * double(_sliceNumber);

  double ringRadius = sin(alpha);
  position[0] = sin(beta) * ringRadius * _radius;
  position[1] = cos(beta) * ringRadius * _radius;
  position[2] = cos(alpha)* _radius;

  return _position+position;
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

int PrimitivesGeneratorPlugin::addSphere(const Vector& _position, const double _radius)
{
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

    vh = triMesh_->add_vertex(positionOnSphere(0, 0, _radius,_position));
    triMesh_->set_texcoord2D(vh, texCoordOnSphere(0, 0));

    for (int st = 1; st < stacks_; ++st) {
      for (int sl = 0; sl < slices_; ++sl) {
        vh = triMesh_->add_vertex(positionOnSphere(sl, st, _radius,_position));
        triMesh_->set_texcoord2D(vh, texCoordOnSphere(sl, st));
      }
    }

    vh = triMesh_->add_vertex(positionOnSphere(slices_, stacks_, _radius,_position));
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

    PluginFunctions::viewAll();

    return object->id();
  }


}


//========================================================================
// Pyramid
//========================================================================

int PrimitivesGeneratorPlugin::addPyramid(const Vector& _position,const double _length) {
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

    const double halfLength   = 0.5*_length;

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point( halfLength, -halfLength, 0.0)+_position);
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( halfLength,  halfLength, 0.0)+_position);
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-halfLength,  halfLength, 0.0)+_position);
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point(-halfLength, -halfLength, 0.0)+_position);

    vhandles_[4] = triMesh_->add_vertex(TriMesh::Point(0.0, 0.0, sqrt(2.0)*halfLength));

    // Add 6 faces
    add_face(2,1,0);
    add_face(3,2,0);
    add_face(4,0,1);

    add_face(3,0,4);
    add_face(4,2,3);
    add_face(1,2,4);

    triMesh_->update_normals();

    emit updatedObject(newObject,UPDATE_ALL);

    PluginFunctions::viewAll();

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

int PrimitivesGeneratorPlugin::addIcosahedron(const Vector& _position,const double _length) {
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

    const double phi  = 0.5 * (1.0 + sqrt(5.0));
    //double norm = 1.0 / sqrt(1.0 + phi*phi);
    const double norm = 1.0;
    const double halfLength = 0.5*_length;

    vhandles_[0 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,  -halfLength , -phi )+_position);
    vhandles_[1 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,   halfLength , -phi )+_position);
    vhandles_[2 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,   halfLength ,  phi )+_position);
    vhandles_[3 ] = triMesh_->add_vertex(norm * TriMesh::Point(  0.0 ,  -halfLength ,  phi )+_position);

    vhandles_[4 ] = triMesh_->add_vertex(norm * TriMesh::Point( -halfLength , -phi ,  0.0  )+_position);
    vhandles_[5 ] = triMesh_->add_vertex(norm * TriMesh::Point(  halfLength , -phi ,  0.0  )+_position);
    vhandles_[6 ] = triMesh_->add_vertex(norm * TriMesh::Point(  halfLength ,  phi ,  0.0  )+_position);
    vhandles_[7 ] = triMesh_->add_vertex(norm * TriMesh::Point( -halfLength ,  phi ,  0.0  )+_position);

    vhandles_[8 ] = triMesh_->add_vertex(norm * TriMesh::Point( -phi ,  0.0 , -halfLength  )+_position);
    vhandles_[9 ] = triMesh_->add_vertex(norm * TriMesh::Point( -phi ,  0.0 ,  halfLength  )+_position);
    vhandles_[10] = triMesh_->add_vertex(norm * TriMesh::Point(  phi ,  0.0 ,  halfLength  )+_position);
    vhandles_[11] = triMesh_->add_vertex(norm * TriMesh::Point(  phi ,  0.0 , -halfLength  )+_position);


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

    PluginFunctions::viewAll();

    return newObject;
  }

  return -1;
}

int PrimitivesGeneratorPlugin::addOctahedron(const Vector& _position,const double _length) {
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

    const double sqrtLength = sqrt(_length);

    vhandles_[0 ] = triMesh_->add_vertex(TriMesh::Point(-sqrtLength,   0.0,   0.0)+_position);
    vhandles_[1 ] = triMesh_->add_vertex(TriMesh::Point(  0.0, -sqrtLength,   0.0)+_position);
    vhandles_[2 ] = triMesh_->add_vertex(TriMesh::Point( sqrtLength,   0.0,   0.0)+_position);
    vhandles_[3 ] = triMesh_->add_vertex(TriMesh::Point(  0.0,  sqrtLength,   0.0)+_position);

    vhandles_[4 ] = triMesh_->add_vertex(TriMesh::Point(  0.0,   0.0,  sqrtLength)+_position);
    vhandles_[5 ] = triMesh_->add_vertex(TriMesh::Point(  0.0,   0.0, -sqrtLength)+_position);


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

    PluginFunctions::viewAll();

    return newObject;
  }

  return -1;
}

int PrimitivesGeneratorPlugin::addDodecahedron(const Vector& _position,const double _length) {
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

    const double phi = (1.0 + sqrt(5.0)) / 2.0;
    const double halfLength = 0.5*_length;


    vphandles_[0 ] = polyMesh_->add_vertex(TriMesh::Point(  halfLength ,  halfLength , halfLength )+_position);
    vphandles_[1 ] = polyMesh_->add_vertex(TriMesh::Point(  halfLength ,  halfLength ,-halfLength )+_position);
    vphandles_[2 ] = polyMesh_->add_vertex(TriMesh::Point(  halfLength , -halfLength , halfLength )+_position);
    vphandles_[3 ] = polyMesh_->add_vertex(TriMesh::Point(  halfLength , -halfLength ,-halfLength )+_position);
    vphandles_[4 ] = polyMesh_->add_vertex(TriMesh::Point( -halfLength ,  halfLength , halfLength )+_position);
    vphandles_[5 ] = polyMesh_->add_vertex(TriMesh::Point( -halfLength ,  halfLength ,-halfLength )+_position);
    vphandles_[6 ] = polyMesh_->add_vertex(TriMesh::Point( -halfLength , -halfLength , halfLength )+_position);
    vphandles_[7 ] = polyMesh_->add_vertex(TriMesh::Point( -halfLength , -halfLength ,-halfLength )+_position);

    vphandles_[8 ] = polyMesh_->add_vertex(TriMesh::Point( 0.0 ,  halfLength / phi ,  phi )+_position);
    vphandles_[9 ] = polyMesh_->add_vertex(TriMesh::Point( 0.0 ,  halfLength / phi , -phi )+_position);
    vphandles_[10] = polyMesh_->add_vertex(TriMesh::Point( 0.0 , -halfLength / phi ,  phi )+_position);
    vphandles_[11] = polyMesh_->add_vertex(TriMesh::Point( 0.0 , -halfLength / phi , -phi )+_position);

    vphandles_[12] = polyMesh_->add_vertex(TriMesh::Point(  halfLength / phi ,  phi, 0.0)+_position);
    vphandles_[13] = polyMesh_->add_vertex(TriMesh::Point(  halfLength / phi , -phi, 0.0)+_position);
    vphandles_[14] = polyMesh_->add_vertex(TriMesh::Point( -halfLength / phi ,  phi, 0.0)+_position);
    vphandles_[15] = polyMesh_->add_vertex(TriMesh::Point( -halfLength / phi , -phi, 0.0)+_position);

    vphandles_[16] = polyMesh_->add_vertex(TriMesh::Point(  phi , 0.0 , halfLength / phi)+_position);
    vphandles_[17] = polyMesh_->add_vertex(TriMesh::Point(  phi , 0.0 ,-halfLength / phi)+_position);
    vphandles_[18] = polyMesh_->add_vertex(TriMesh::Point( -phi , 0.0 , halfLength / phi)+_position);
    vphandles_[19] = polyMesh_->add_vertex(TriMesh::Point( -phi , 0.0 ,-halfLength / phi)+_position);

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

    PluginFunctions::viewAll();

    return newObject;
  }

  return -1;
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( primitivesgeneratorplugin , PrimitivesGeneratorPlugin );
#endif
