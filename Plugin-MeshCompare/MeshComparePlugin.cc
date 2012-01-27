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
*   $Revision: 13361 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-01-12 16:33:16 +0100 (Do, 12 Jan 2012) $                     *
*                                                                            *
\*===========================================================================*/



#include "MeshComparePlugin.hh"


#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <OpenFlipper/common/bsp/TriangleBSPT.hh>
#include <ACG/Geometry/Algorithms.hh>

MeshComparePlugin::MeshComparePlugin()
{

}

MeshComparePlugin::~MeshComparePlugin()
{

}

void MeshComparePlugin::initializePlugin()
{
   // Create the Toolbox Widget
   QWidget* toolBox = new QWidget();

   QGridLayout* layout = new QGridLayout(toolBox);

   QPushButton* compareButton = new QPushButton("&Mesh Compare",toolBox);

   QLabel* label = new QLabel("TODO:");

   layout->addWidget( label             , 0, 0);
   layout->addWidget( compareButton     , 1, 1);

   layout->addItem(new QSpacerItem(10,10,QSizePolicy::Expanding,QSizePolicy::Expanding),2,0,1,2);

   connect( compareButton, SIGNAL(clicked()), this, SLOT(compare()) );

   //QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"smoother1.png");
   emit addToolbox( tr("Mesh Compare") , toolBox );
}

void MeshComparePlugin::pluginsInitialized() {
    
//    // Emit slot description
//    emit setSlotDescription(tr("simpleLaplace(int)"),   tr("Smooth mesh using the Laplace operator with uniform weights."),
//                            QStringList(tr("iterations")), QStringList(tr("Number of iterations")));
}

void MeshComparePlugin::slotObjectUpdated( int _identifier, const UpdateType& _type ) {
}

void MeshComparePlugin::compare() {

  // Get source and target objects
  BaseObjectData* targetObject = 0;
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    if ( o_it->dataType(DATA_TRIANGLE_MESH)  || o_it->dataType(DATA_POLY_MESH) ) {

      // If we found a second target, something is wrong!
      if ( targetObject != 0 ) {
        emit log(LOGERR,tr("Please select one source and one target mesh to compare! Source will be the reference mesh."));
        return;
      }

      targetObject = (*o_it);
    }
  }

  BaseObjectData* sourceObject = 0;
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    if ( o_it->dataType(DATA_TRIANGLE_MESH)  || o_it->dataType(DATA_POLY_MESH) ) {

      // If we found a second target, something is wrong!
      if ( sourceObject != 0 ) {
        emit log(LOGERR,tr("Please select one source and one target mesh to compare! Source will be the reference mesh."));
        return;
      }

      sourceObject = (*o_it);
    }
  }


  if ( (targetObject != 0) && (sourceObject != 0)  ) {
    compare(sourceObject->id(),targetObject->id());
  } else {
    emit log(LOGERR,tr("Please select one source and one target mesh to compare! Source will be the reference mesh."));
  }


}

void MeshComparePlugin::compare(int _sourceId,int _targetId) {


  TriMeshObject* source = PluginFunctions::triMeshObject(_sourceId);
  TriMeshObject* target = PluginFunctions::triMeshObject(_targetId);

  if ( (target == 0 ) || (source == 0) ) {
    emit log(LOGERR,tr("Please select one source and one target mesh to compare! Source will be the reference mesh."));
    emit log(LOGERR,tr("Only triangle meshes are currently supported!"));
    return;
  }

  TriMesh* refMesh  = PluginFunctions::triMesh(_sourceId);
  TriMesh* compMesh = PluginFunctions::triMesh(_targetId);

  // Generate a property, where we store the information
  OpenMesh::VPropHandleT<double> vertexDistanceProp;
  if ( ! refMesh->get_property_handle(vertexDistanceProp,"MeshCompare_VertexDistance") ) {
    refMesh->add_property(vertexDistanceProp,"MeshCompare_VertexDistance");
  }


  // Get a bsp for the target, as we will project the reference mesh onto the target mesh.
  // It will be build automatically at this point.
  TriMeshObject::OMTriangleBSP* compBSP = target->requestTriangleBsp();

  TriMesh::Scalar maximalDistance    = 0.0;
  TriMesh::Scalar maxNormalDeviation = 0.0;

  for ( TriMesh::VertexIter v_it = refMesh->vertices_begin() ; v_it != refMesh->vertices_end(); ++ v_it) {

    TriMeshObject::OMTriangleBSP::NearestNeighbor nearest = compBSP->nearest( refMesh->point(v_it) );
    TriMesh::FaceHandle closestFace = nearest.handle;

    // Remember the maximal distance between the meshes
    if ( nearest.dist > maximalDistance )
      maximalDistance = nearest.dist;

    // Set it to the reference mesh
    refMesh->property(vertexDistanceProp,v_it) = nearest.dist;

    // Get the vertices around that face and their properties
    TriMesh::CFVIter        fv_it = compMesh->cfv_iter(closestFace);

    const TriMesh::Point&   p0    = compMesh->point(fv_it);
    const TriMesh::Normal   n0    = compMesh->normal(fv_it);

    const TriMesh::Point&   p1    = compMesh->point(++fv_it);
    const TriMesh::Normal   n1    = compMesh->normal(fv_it);

    const TriMesh::Point&   p2    = compMesh->point(++fv_it);
    const TriMesh::Normal   n2    = compMesh->normal(fv_it);

    // project original point to current mesh
    TriMesh::Point projectedPoint;
    ACG::Geometry::distPointTriangle(refMesh->point(v_it), p0, p1, p2, projectedPoint);

    // compute Barycentric coordinates
    ACG::Geometry::baryCoord(projectedPoint, p0, p1, p2, projectedPoint);

    // interpolate normal on the compare mesh at the projected point via barycentric coordinates.
    TriMesh::Normal normal;
    normal  = n0 * projectedPoint[0];
    normal += n1 * projectedPoint[1];
    normal += n2 * projectedPoint[2];
    normal.normalize();


    // Compute normal deviation in degrees
    double normalDeviation = (refMesh->normal(v_it) | normal);

    if (normalDeviation < -1.0)
      normalDeviation = -1.0;
    else if (normalDeviation > 1.0)
      normalDeviation = 1.0;

    normalDeviation = 180.0 / M_PI * acos(normalDeviation);

    if (normalDeviation > maxNormalDeviation)
      maxNormalDeviation = normalDeviation;

  }

  std::cerr << "Maximal distance: " << maximalDistance << std::endl;
  std::cerr << "Maximal Normal deviation in degree: " << maxNormalDeviation << std::endl;

}


Q_EXPORT_PLUGIN2( meshcompareplugin , MeshComparePlugin );

