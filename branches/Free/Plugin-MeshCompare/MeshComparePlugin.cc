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



#include "MeshComparePlugin.hh"


#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <ACG/Geometry/bsp/TriangleBSPT.hh>
#include <ACG/Geometry/Algorithms.hh>
#include <ACG/Utils/ColorCoder.hh>


MeshComparePlugin::MeshComparePlugin() :
  tool_(0),
  maximalDistance_(-1),
  maxNormalDeviation_(-1),
  maxMeanCurvatureDev_(-1),
  maxGaussCurvatureDev_(-1)
#ifdef WITH_QWT
  ,plot_(0)
#endif
{

}

MeshComparePlugin::~MeshComparePlugin()
{

}

void MeshComparePlugin::initializePlugin()
{
  if ( OpenFlipper::Options::gui()) {
    tool_ = new MeshCompareToolbarWidget();

    connect( tool_->compare, SIGNAL(clicked()), this, SLOT(compareButton()) );
    connect( tool_->clear, SIGNAL(clicked()), this, SLOT(slotClear()) );

    QIcon* toolIcon = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"MeshCompare.png");
    emit addToolbox( tr("Mesh Compare") , tool_ , toolIcon);

    connect(tool_->doClamp,SIGNAL(toggled( bool)),this,SLOT(slotClampBox(bool)));

#ifdef WITH_QWT
    QVBoxLayout* layout = new QVBoxLayout(tool_->frame);

    plot_ = new QwtFunctionPlot(0);
    plot_->setMinimumHeight(150);

    layout->addWidget(plot_);
#else
    // Hide the extra frame as QWT is not available, we will not have an histogramm
    tool_->frame->hide();
#endif
  }
}

void MeshComparePlugin::pluginsInitialized() {
    
  //===========================================================
  // Describe scripting slots
  //===========================================================
  emit setSlotDescription(tr("compare(int,int)"), tr("Compare two meshes. Use lastMaximalDistance() and lastMaximalNormalDeviation() to get the results."),
      QStringList(tr("ObjectId,ObjectId")), QStringList(tr("Id of the reference mesh, Id of the comparison mesh")));
  emit setSlotDescription(tr("lastMaximalDistance()"), tr("Get the maximal distance between the meshes of the last comparison."),
      QStringList(tr("")), QStringList(tr("")));
  emit setSlotDescription(tr("lastMaximalNormalDeviation()"), tr("Get the maximal normal deviation in degree between the meshes of the last comparison."),
      QStringList(tr("")), QStringList(tr("")));
  emit setSlotDescription(tr("lastMaximalMeanCurvatureDeviation()"), tr("Get the maximal mean curvature deviation between the meshes of the last comparison."),
      QStringList(tr("")), QStringList(tr("")));
  emit setSlotDescription(tr("lastMaximalGaussCurvatureDeviation()"), tr("Get the maximal gauss curvature deviation between the meshes of the last comparison."),
      QStringList(tr("")), QStringList(tr("")));

  //===========================================================
  // Check mean curvature plugin and disable the box in gui mode
  //===========================================================
  bool meanCurvature = false;
  emit pluginExists( "meancurvature" , meanCurvature  );

  if ( OpenFlipper::Options::gui() && !meanCurvature )
    tool_->meanCurvature->setEnabled(false);

  //===========================================================
  // Check gauss curvature plugin and disable the box in gui mode
  //===========================================================
  bool gaussCurvature = false;
  emit pluginExists( "gausscurvature" , gaussCurvature  );

  if ( OpenFlipper::Options::gui() && !gaussCurvature )
    tool_->gaussCurvature->setEnabled(false);
}

void MeshComparePlugin::compareButton() {

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
    compare(sourceObject->id(),targetObject->id(), tool_->distance->isChecked() ,
                                                   tool_->normalAngle->isChecked(),
                                                   tool_->gaussCurvature->isChecked(),
                                                   tool_->meanCurvature->isChecked() );
  } else {
    emit log(LOGERR,tr("Please select one source and one target mesh to compare! Source will be the reference mesh."));
  }


}

void MeshComparePlugin::slotObjectUpdated( int _identifier, const UpdateType& _type ) {
  // Get source and target objects
  BaseObjectData* object = 0;

  PluginFunctions::getObject(_identifier,object);

  if ( object ) {
    ACG::SceneGraph::MaterialNode *pMatNode = 0;
    if ( object->getAdditionalNode(pMatNode,name(), "MeshCompareDistanceMaterial" ) )
      object->removeAdditionalNode(pMatNode,name(),"MeshCompareDistanceMaterial");
  }

}

void MeshComparePlugin::slotClear() {

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {

    ACG::SceneGraph::MaterialNode *pMatNode = 0;
    if ( o_it->getAdditionalNode(pMatNode,name(), "MeshCompareDistanceMaterial" ) )
      o_it->removeAdditionalNode(pMatNode,name(),"MeshCompareDistanceMaterial");

  }

  emit updateView();

}

void MeshComparePlugin::slotClampBox(bool _checked) {
  if ( _checked ) {
    tool_->minVal->setValue(tool_->minValue->text().toDouble());
    tool_->maxVal->setValue(tool_->maxValue->text().toDouble());
  }
}

void MeshComparePlugin::compare(int _sourceId,int _targetId,bool _computeDist, bool _computeNormal, bool _computeGauss , bool _computeMean) {


  TriMeshObject* source = PluginFunctions::triMeshObject(_sourceId);
  TriMeshObject* target = PluginFunctions::triMeshObject(_targetId);

  if ( (target == 0 ) || (source == 0) ) {
    emit log(LOGERR,tr("Please select one source and one target mesh to compare! Source will be the reference mesh."));
    emit log(LOGERR,tr("Only triangle meshes are currently supported!"));
    return;
  }

  TriMesh* refMesh  = PluginFunctions::triMesh(_sourceId);
  TriMesh* compMesh = PluginFunctions::triMesh(_targetId);

  ACG::SceneGraph::PointNode *pNode = 0;

  // Check if we already attached a PointNode
  if ( OpenFlipper::Options::gui() ) {

    if ( !target->getAdditionalNode(pNode,name(), "MeshCompareDistance" ) ) {

      ACG::SceneGraph::MaterialNode *pMatNode = 0;
      pMatNode = new ACG::SceneGraph::MaterialNode(target->manipulatorNode(), "MeshCompare distance visualization material");
      target->addAdditionalNode(pMatNode, name(), "MeshCompareDistanceMaterial");
      pMatNode->set_point_size(5);
      pMatNode->applyProperties(ACG::SceneGraph::MaterialNode::PointSize);

      pNode = new ACG::SceneGraph::PointNode(pMatNode, "MeshCompare distance visualization");
      pNode->drawMode(ACG::SceneGraph::DrawModes::POINTS_COLORED);
      target->addAdditionalNode(pNode, name(), "MeshCompareDistance");
    }

    // Clear but reserve memory for the required vertices
    pNode->clear();
    pNode->reserve(refMesh->n_vertices(),refMesh->n_vertices(),refMesh->n_vertices() );
  }

  // Get a bsp for the target, as we will project the reference mesh onto the target mesh.
  // It will be build automatically at this point.
  TriMeshObject::OMTriangleBSP* compBSP = target->requestTriangleBsp();

  // ================================================================
  // Compute mean curvature on both meshes ( if plugin is available )
  // ================================================================
  bool meanCurvature = false;
  OpenMesh::VPropHandleT< double > meanRef;
  OpenMesh::VPropHandleT< double > meanComp;

  if ( _computeMean ) {
    emit pluginExists( "meancurvature" , meanCurvature  );

    if ( meanCurvature ) {
      RPC::callFunction("meancurvature","computeMeanCurvature",_sourceId);
      RPC::callFunction("meancurvature","computeMeanCurvature",_targetId);
    }

    if( meanCurvature &&
        ((!refMesh->get_property_handle(  meanRef , "Mean Curvature") ) ||
            (!compMesh->get_property_handle( meanComp, "Mean Curvature") ))) {
      meanCurvature = false;
    }
  }

  // ================================================================
  // Compute mean curvature on both meshes ( if plugin is available )
  // ================================================================
  bool gaussCurvature = false;
  OpenMesh::VPropHandleT< double > gaussRef;
  OpenMesh::VPropHandleT< double > gaussComp;

  if ( _computeGauss ) {
    emit pluginExists( "gausscurvature" , gaussCurvature  );

    if ( gaussCurvature ) {
      RPC::callFunction("gausscurvature","computeGaussCurvature",_sourceId);
      RPC::callFunction("gausscurvature","computeGaussCurvature",_targetId);
    }

    if( gaussCurvature &&
        ((!refMesh->get_property_handle(  gaussRef , "Gaussian Curvature") ) ||
            (!compMesh->get_property_handle( gaussComp, "Gaussian Curvature") ))) {
      gaussCurvature = false;
    }
  }



  // ================================================================
  // Remember the maximal values as output and for specifying color coding range
  // ================================================================
  maximalDistance_      = -1.0;
  maxNormalDeviation_   = -1.0;
  maxMeanCurvatureDev_  = -1.0;
  maxGaussCurvatureDev_ = -1.0;


  // Remember distances for colorCoding after we know the maximal distance
  std::vector<double> distances;
  std::vector<double> normalAngles;
  std::vector<double> meanCurvatures;
  std::vector<double> gaussCurvatures;

  for ( TriMesh::VertexIter v_it = refMesh->vertices_begin() ; v_it != refMesh->vertices_end(); ++ v_it) {

    TriMeshObject::OMTriangleBSP::NearestNeighbor nearest = compBSP->nearest(refMesh->point(v_it));
    TriMesh::FaceHandle closestFace = nearest.handle;

    // Remember the maximal distance between the meshes
    if (nearest.dist > maximalDistance_)
      maximalDistance_ = nearest.dist;

    // Remember distance for color coding
    distances.push_back(nearest.dist);

    // Get the vertices around that face and their properties
    TriMesh::CFVIter fv_it = compMesh->cfv_iter(closestFace);

    const TriMesh::Point& p0 = compMesh->point(fv_it);
    const TriMesh::Normal n0 = compMesh->normal(fv_it);
    const TriMesh::VertexHandle& v0 = fv_it.handle();

    const TriMesh::Point& p1 = compMesh->point(++fv_it);
    const TriMesh::Normal n1 = compMesh->normal(fv_it);
    const TriMesh::VertexHandle& v1 = fv_it.handle();

    const TriMesh::Point& p2 = compMesh->point(++fv_it);
    const TriMesh::Normal n2 = compMesh->normal(fv_it);
    const TriMesh::VertexHandle& v2 = fv_it.handle();

    // project original point to current mesh
    TriMesh::Point projectedPoint;
    ACG::Geometry::distPointTriangle(refMesh->point(v_it), p0, p1, p2, projectedPoint);

    // Add the position to the point node
    if (pNode)
      pNode->add_point(projectedPoint);

    // compute Barycentric coordinates
    ACG::Geometry::baryCoord(projectedPoint, p0, p1, p2, projectedPoint);

    if ( _computeNormal) {
      // interpolate normal on the compare mesh at the projected point via barycentric coordinates.
      TriMesh::Normal normal;
      normal = n0 * projectedPoint[0];
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

      // Remember normal deviation for color coding
      normalAngles.push_back(normalDeviation);

      if (normalDeviation > maxNormalDeviation_)
        maxNormalDeviation_ = normalDeviation;
    }

    if (meanCurvature) {

      TriMesh::Scalar curvature = 0.0;
      curvature =  compMesh->property(meanComp, v0) * projectedPoint[0];
      curvature += compMesh->property(meanComp, v1) * projectedPoint[1];
      curvature += compMesh->property(meanComp, v2) * projectedPoint[2];

      const double curvatureDev = fabs(refMesh->property(meanRef, v_it) - curvature);

      meanCurvatures.push_back(curvatureDev);

      if (curvatureDev > maxMeanCurvatureDev_)
        maxMeanCurvatureDev_ = curvatureDev;
    }

    if (gaussCurvature) {

      TriMesh::Scalar curvature = 0.0;
      curvature =  compMesh->property(gaussComp, v0) * projectedPoint[0];
      curvature += compMesh->property(gaussComp, v1) * projectedPoint[1];
      curvature += compMesh->property(gaussComp, v2) * projectedPoint[2];

      const double curvatureDev = fabs(refMesh->property(gaussRef, v_it) - curvature);

      gaussCurvatures.push_back(curvatureDev);

      if (curvatureDev > maxGaussCurvatureDev_)
        maxGaussCurvatureDev_ = curvatureDev;
    }

  }

  // Generate the colors
  if ( pNode ) {

    tool_->minValue->setText( QString::number(0.0) );

    if ( tool_->distance->isChecked() ) {
      visualizeData(distances,maximalDistance_,pNode);
    } else if ( tool_->normalAngle->isChecked() ) {
      visualizeData(normalAngles,maxNormalDeviation_,pNode);
    } else if ( tool_->meanCurvature->isChecked() ) {
      visualizeData(meanCurvatures,maxMeanCurvatureDev_,pNode);
    } else if ( tool_->gaussCurvature->isChecked() ) {
      visualizeData(gaussCurvatures,maxGaussCurvatureDev_,pNode);
    }

    emit updateView();
  }

}

void MeshComparePlugin::visualizeData( const std::vector<double>& _data, double _maxValue, ACG::SceneGraph::PointNode* _pnode ) {

  // Set the current real maximal value in the label to show it to the user
  tool_->maxValue->setText( QString::number(_maxValue) );

  // If the clamping check box is set, we take the values from the spin boxes
  double min = 0.0;
  double max = 1.0;
  if ( tool_->doClamp->isChecked() ) {
    min = tool_->minVal->value();
    max = std::min(tool_->maxVal->value(),_maxValue);
  } else
    max = _maxValue;

  ACG::ColorCoder cCoder(min,max);

  for ( unsigned int i = 0 ; i < _data.size() ; ++i) {
    _pnode->add_color(cCoder.color_float4(_data[i]));
  }

#ifdef WITH_QWT
  plot_->setMinMax(min,max);
  plot_->setFunction( _data );
  plot_->replot();
#endif

}



Q_EXPORT_PLUGIN2( meshcompareplugin , MeshComparePlugin );

