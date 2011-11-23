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


#include <ObjectTypes/PolyMesh/PolyMesh.hh>


#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

void PrimitivesGeneratorPlugin::initializePlugin()
{
  emit setSlotDescription("addTetrahedron()"     ,tr("Generates a tetrahedron (ObjectId is returned)")  ,QStringList(), QStringList());
  emit setSlotDescription("addTriangulatedCube()",tr("Generates a cube (ObjectId is returned)")         ,QStringList(), QStringList());
  emit setSlotDescription("addIcosahedron()"     ,tr("Generates an icosahedron (ObjectId is returned)") ,QStringList(), QStringList());
  emit setSlotDescription("addPyramid()"         ,tr("Generates a pyramid (ObjectId is returned)")      ,QStringList(), QStringList());
}

void PrimitivesGeneratorPlugin::pluginsInitialized() {
  // Primitives Menu
  QMenu *primitivesMenu;

  emit getMenubarMenu(tr("&Primitives"), primitivesMenu, true );

  QIcon* icon;
  QAction * action;

  action = primitivesMenu->addAction("Cube (Triangle Mesh)"       ,this,SLOT(addTriangulatedCube()));
  icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_cube.png");
  action->setIcon(*icon);

  action = primitivesMenu->addAction("Tetrahedron",this,SLOT(addTetrahedron()));
  icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_tetrahedron.png");
  action->setIcon(*icon);


  action = primitivesMenu->addAction("Icosahedron"                ,this,SLOT(addIcosahedron()));
  icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_icosahedron.png");
  action->setIcon(*icon);


  action = primitivesMenu->addAction("Pyramid"                    ,this,SLOT(addPyramid()));
  icon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"primitive_pyramid.png");
  action->setIcon(*icon);

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

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point(0, 0, 0));
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point(0, 1, 0));
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(1, 0, 0));
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point(0, 0, 1));

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

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point( 0.5, -0.5, 0.5));
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( 0.5,  0.5, 0.5));
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-0.5,  0.5, 0.5));
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point(-0.5, -0.5, 0.5));
    vhandles_[4] = triMesh_->add_vertex(TriMesh::Point( 0.5, -0.5,-0.5));
    vhandles_[5] = triMesh_->add_vertex(TriMesh::Point( 0.5,  0.5,-0.5));
    vhandles_[6] = triMesh_->add_vertex(TriMesh::Point(-0.5,  0.5,-0.5));
    vhandles_[7] = triMesh_->add_vertex(TriMesh::Point(-0.5, -0.5,-0.5));


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

int PrimitivesGeneratorPlugin::addSphere() {
std::cerr << "Todo :add Sphere" << std::endl;

return -1;
}

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

    vhandles_[0] = triMesh_->add_vertex(TriMesh::Point( 0.5 ,-0.5 ,  0));
    vhandles_[1] = triMesh_->add_vertex(TriMesh::Point( 0.5 , 0.5 ,  0));
    vhandles_[2] = triMesh_->add_vertex(TriMesh::Point(-0.5 , 0.5 ,  0));
    vhandles_[3] = triMesh_->add_vertex(TriMesh::Point(-0.5 ,-0.5 ,  0));
    vhandles_[4] = triMesh_->add_vertex(TriMesh::Point(   0 ,   0 ,  1));

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

    double phi  = (1.0 + sqrt(5.0) / 2.0);
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






Q_EXPORT_PLUGIN2( primitivesgeneratorplugin , PrimitivesGeneratorPlugin );

