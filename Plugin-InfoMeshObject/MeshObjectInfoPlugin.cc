/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

//=============================================================================
//
//  CLASS InfoMeshObjectPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "MeshObjectInfoPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <QLocale>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <MeshTools/MeshInfoT.hh>
#include <OpenFlipper/common/BaseObject.hh>
#include <ACG/Geometry/Algorithms.hh>

#include <Math_Tools/Math_Tools.hh>

#include <cfloat>

#if QT_VERSION >= 0x050000 
#include <QtWidgets>
#else
#include <QtGui>
#endif

//== IMPLEMENTATION ==========================================================


InfoMeshObjectPlugin::InfoMeshObjectPlugin() :
        info_(0),
        infoBar_(0)
{
}

InfoMeshObjectPlugin::~InfoMeshObjectPlugin() {

  //Info bar and dialog will be deleted by core widget
}


void InfoMeshObjectPlugin::initializePlugin() {

}

/// initialize the plugin
void InfoMeshObjectPlugin::pluginsInitialized() {

  //set the slot descriptions
  setDescriptions();

  if ( OpenFlipper::Options::gui()) {

    // Create info bar
    infoBar_ = new InfoBar();

    // Create info dialog
    info_ = new InfoDialog();

    // Set default pick mode in dialog box
    info_->pickMode->setCurrentIndex(0); // PICK_FACES

    emit addWidgetToStatusbar(infoBar_);
    infoBar_->hideCounts();
  }
  
}

//-----------------------------------------------------------------------------

DataType InfoMeshObjectPlugin::supportedDataTypes() {
    return DataType(DATA_POLY_MESH | DATA_TRIANGLE_MESH);
}

//-----------------------------------------------------------------------------

template< class MeshT >
void InfoMeshObjectPlugin::printMeshInfo( MeshT* _mesh , int _id, unsigned int _index, ACG::Vec3d& _hitPoint ) {

  bool face   = false;
  bool edge   = false;
  bool vertex = false;

  int closestVertexIndex = -1;
  int closestEdgeIndex   = -1;

  switch (info_->pickMode->currentIndex() ) {
    case 0 : //Face
      closestVertexIndex = getClosestVertexInFace(_mesh, _index, _hitPoint);
      closestEdgeIndex   = getClosestEdgeInFace  (_mesh, _index, _hitPoint);
      face = true;
      break;
    case 1 : //Edge
      closestVertexIndex = getClosestVertexFromEdge(_mesh, _index, _hitPoint);
      closestEdgeIndex   = _index;
      edge = true;
      break;
    case 2 : //Vertex
      closestVertexIndex = _index;
      vertex = true;
      break;
    default:
      emit log(LOGERR,"Error: unknown picking mode in printMeshInfo");
      return;
  }

  QLocale locale;

  QString name;

  // name
  BaseObject* obj = 0;
  if ( PluginFunctions::getObject(_id, obj) )
    info_->generalBox->setTitle( tr("General object information for %1").arg( obj->name() ) );

  // ID
  info_->id->setText( locale.toString(_id) );
  // Vertices
  info_->vertices->setText( locale.toString( qulonglong(_mesh->n_vertices() ) ) );
  // Faces
  info_->faces->setText( locale.toString( qulonglong( _mesh->n_faces() ) ) );
  // Edges
  info_->edges->setText( locale.toString( qulonglong( _mesh->n_edges() ) ) );

  if ( face ) {

    // Picked Face
    info_->closestFaceLabel->setText( tr("Picked Face:") );
    info_->closestFaceLabel->show();
    info_->faceHandle->setText( locale.toString( _index ) );
    info_->faceHandle->show();

    // Closest Vertex
    info_->closestVertexLabel->setText( tr("Closest Vertex:") );
    info_->vertexHandle->setText( locale.toString( closestVertexIndex ) );

    // Closest Edge
    info_->closestEdgeLabel->setText( tr("Closest Edge:") );
    info_->edgeHandle->setText( locale.toString( closestEdgeIndex ) );
    info_->closestEdgeLabel->show();
    info_->edgeHandle->show();

    // Closest Edge Length
    info_->edgeLengthLabel->setText( tr("Closest Edge Length:") );
    info_->edgeLengthLabel->show();
    const typename MeshT::Point from = _mesh->point(_mesh->from_vertex_handle( _mesh->halfedge_handle( _mesh->edge_handle(closestEdgeIndex),0 ) ));
    const typename MeshT::Point to   = _mesh->point(_mesh->to_vertex_handle( _mesh->halfedge_handle( _mesh->edge_handle(closestEdgeIndex),0 ) ));
    info_->edgeLength->setText( locale.toString( (to - from).norm() ) );
    info_->edgeLength->show();

    //adjacent vertex handles
    typename MeshT::FaceHandle fh = _mesh->face_handle(_index);

    typename MeshT::FaceVertexIter fv_it = _mesh->fv_iter(fh);
    QString adjacentVertices;

    if ( fv_it.is_valid() ){
      adjacentVertices = QString::number( fv_it->idx() );
      ++fv_it;
    }

    while( fv_it.is_valid() ){
      adjacentVertices += "; " + QString::number( fv_it->idx() );
      ++fv_it;
    }

    info_->adjVertexHandles->setText( adjacentVertices );
    info_->adjVertexHandles->show();
    info_->adjacentVertexLabel->show();

    //normal
    info_->normalLabel->setText(tr("Normal of picked face:"));
    info_->normalX->setText( QString::number( _mesh->normal(fh)[0],'f' ) );
    info_->normalY->setText( QString::number( _mesh->normal(fh)[1],'f' ) );
    info_->normalZ->setText( QString::number( _mesh->normal(fh)[2],'f' ) );
    info_->normalLabel->show();
    info_->normalLeft->show();
    info_->normalX->show();
    info_->normalY->show();
    info_->normalZ->show();
    info_->normalRight->show();

    // closest vertex coordinates
    info_->closestVertexPosLabel->setText(tr("Closest Vertex on the mesh:"));

  } else if (edge) {

    // Adjacent Faces
    info_->closestFaceLabel->setText( tr("Adjacent Faces:") );
    info_->closestFaceLabel->show();
    typename MeshT::HalfedgeHandle he1 = _mesh->halfedge_handle(_mesh->edge_handle(_index),0);
    typename MeshT::HalfedgeHandle he2 = _mesh->halfedge_handle(_mesh->edge_handle(_index),1);

    int fh1 = _mesh->face_handle(he1).idx();
    int fh2 = _mesh->face_handle(he2).idx();

    info_->faceHandle->setText( locale.toString( fh1 ) + ";" + locale.toString( fh2 ) );
    info_->faceHandle->show();

    // Adjacent vertices
    info_->adjVertexHandles->setText(QString::number( _mesh->from_vertex_handle(he1).idx() ) + ";" + QString::number( _mesh->to_vertex_handle(he1).idx() ));
    info_->adjVertexHandles->show();
    info_->adjacentVertexLabel->show();

    // Closest Vertex
    info_->closestVertexLabel->setText( tr("Closest Vertex:") );
    info_->vertexHandle->setText( locale.toString( closestVertexIndex ) );

    // Picked Edge
    info_->closestEdgeLabel->setText( tr("Picked Edge:") );
    info_->edgeHandle->setText( locale.toString( closestEdgeIndex ) );
    info_->closestEdgeLabel->show();
    info_->edgeHandle->show();

    // Edge Length
    info_->edgeLengthLabel->setText( tr("Edge Length:") );
    info_->edgeLengthLabel->show();
    const typename MeshT::Point from = _mesh->point(_mesh->from_vertex_handle( _mesh->halfedge_handle( _mesh->edge_handle(closestEdgeIndex),0 ) ));
    const typename MeshT::Point to   = _mesh->point(_mesh->to_vertex_handle( _mesh->halfedge_handle( _mesh->edge_handle(closestEdgeIndex),0 ) ));
    info_->edgeLength->setText( locale.toString( (to - from).norm() ) );
    info_->edgeLength->show();

    // Normal
    info_->normalLabel->hide();
    info_->normalLeft->hide();
    info_->normalX->hide();
    info_->normalY->hide();
    info_->normalZ->hide();
    info_->normalRight->hide();

    // closest vertex coordinates
    info_->closestVertexPosLabel->setText(tr("Closest Vertex on the mesh:"));

  } else if (vertex) {

    // Faces
    info_->closestFaceLabel->hide();
    info_->faceHandle->hide();

    // Adjacent vertices
    info_->adjVertexHandles->hide();
    info_->adjacentVertexLabel->hide();

    // Closest Vertex
    info_->closestVertexLabel->setText( tr("Picked Vertex:") );
    info_->vertexHandle->setText( locale.toString( closestVertexIndex ) );

    // Closest Edge
    info_->closestEdgeLabel->hide();
    info_->edgeHandle->hide();

    // Edge Length
    info_->edgeLengthLabel->hide();
    info_->edgeLength->hide();

    // Normal
    typename MeshT::VertexHandle vh = _mesh->vertex_handle(_index);
    info_->normalLabel->setText(tr("Normal of picked vertex:"));
    info_->normalX->setText( QString::number( _mesh->normal(vh)[0],'f' ) );
    info_->normalY->setText( QString::number( _mesh->normal(vh)[1],'f' ) );
    info_->normalZ->setText( QString::number( _mesh->normal(vh)[2],'f' ) );
    info_->normalLabel->show();
    info_->normalLeft->show();
    info_->normalX->show();
    info_->normalY->show();
    info_->normalZ->show();
    info_->normalRight->show();

    // closest vertex coordinates
    info_->closestVertexPosLabel->setText(tr("Picked Vertex on the mesh:"));

    // Adjacent Edges
    info_->closestFaceLabel->setText( tr("Adjacent Edges:") );
    info_->closestFaceLabel->show();

    //adjacent vertex handles

    typename MeshT::VertexEdgeIter ve_it = _mesh->ve_iter(vh);
    QString adjacentEdges;

    if ( ve_it.is_valid() ){
      adjacentEdges = QString::number( ve_it->idx() );
      ++ve_it;
    }

    while( ve_it.is_valid() ){
      adjacentEdges += "; " + QString::number( ve_it->idx() );
      ++ve_it;
    }

    info_->faceHandle->setText( adjacentEdges );
    info_->faceHandle->show();
  }

  // closest vertex coordinates
  info_->vertexX->setText( QString::number( _mesh->point( _mesh->vertex_handle(closestVertexIndex) )[0],'f' ) );
  info_->vertexY->setText( QString::number( _mesh->point( _mesh->vertex_handle(closestVertexIndex) )[1],'f' ) );
  info_->vertexZ->setText( QString::number( _mesh->point( _mesh->vertex_handle(closestVertexIndex) )[2],'f' ) );


  // Components
  int compo_count = MeshInfo::componentCount(_mesh);
  info_->components->setText( locale.toString(compo_count));
  // Boundaries
  int boundary_count = MeshInfo::boundaryCount(_mesh);
  info_->boundaries->setText( locale.toString(boundary_count) );
  // Genus
  int chi = _mesh->n_vertices();
  chi -= _mesh->n_edges();
  chi += _mesh->n_faces(); // chi = Euler characteristic
  // chi + n_holes = 2(n_components - genus) => genus = n_components - (chi + n_holes)/2;
  float genus = compo_count - 0.5*(chi + boundary_count);
  if(compo_count == 1 && boundary_count == 0)
    info_->genus->setText( QString::number(genus) );
  else if(compo_count != 1)
    info_->genus->setText( "(multiple components)" );
  else
    info_->genus->setText( "(not manifold)" );

  // Coordinates
  typename MeshT::VertexIter v_it;
  typename MeshT::VertexIter v_end = _mesh->vertices_end();

  float maxX = FLT_MIN;
  float minX = FLT_MAX;
  //float sumX = 0.0;
  float maxY = FLT_MIN;
  float minY = FLT_MAX;
  //float sumY = 0.0;
  float maxZ = FLT_MIN;
  float minZ = FLT_MAX;
  //float sumZ = 0.0;
  int minV = 999;
  int maxV = 0;
  int sumV = 0;
  float maxE = FLT_MIN;
  float minE = FLT_MAX;
  float sumE = 0.0;

  //iterate over all vertices
  for (v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it){
    typename MeshT::Point p = _mesh->point( *v_it );
    if (p[0] < minX) minX = p[0];
    if (p[0] > maxX) maxX = p[0];
    //sumX += p[0];
    if (p[1] < minY) minY = p[1];
    if (p[1] > maxY) maxY = p[1];
    //sumY += p[1];
    if (p[2] < minZ) minZ = p[2];
    if (p[2] > maxZ) maxZ = p[2];
    //sumZ += p[2];



    //check valence + edge length
    int valence = 0;
    typename MeshT::VertexVertexIter vv_it;

    for (vv_it=_mesh->vv_iter( *v_it ); vv_it.is_valid(); ++vv_it){
      valence++;

      typename MeshT::Point p2 = _mesh->point( *vv_it );
      typename MeshT::Scalar len = (p2 - p).norm();

      if (len < minE) minE = len;
      if (len > maxE) maxE = len;
      sumE += len;
    }

    if (valence < minV) minV = valence;
    if (valence > maxV) maxV = valence;
    sumV += valence;
  }

  
  //valence
  info_->valenceMin->setText( QString::number(minV) );
  info_->valenceMean->setText( QString::number( sumV / (float)_mesh->n_vertices(),'f' ) );
  info_->valenceMax->setText( QString::number(maxV) );

  //edge length
  info_->edgeMin->setText( QString::number(minE,'f') );
  info_->edgeMean->setText( QString::number( sumE / (_mesh->n_edges()*2),'f' )  );
  info_->edgeMax->setText( QString::number(maxE,'f') );

  //get aspect ratio

  typename MeshT::FaceIter f_it;
  typename MeshT::FaceIter f_end = _mesh->faces_end();

  float maxA = FLT_MIN;
  float minA = FLT_MAX;
  float sumA = 0.0;
  float maxI = FLT_MIN;
  float minI = FLT_MAX;
  //float sumI = 0.0;
  float maxD = FLT_MIN;
  float minD = FLT_MAX;
  float sumD = 0.0;
  int numD = 0;
  unsigned int maxFValence = -std::numeric_limits<float>::infinity();
  unsigned int minFValence = std::numeric_limits<float>::infinity();
  size_t sumFValence = 0;

  //iterate over all faces
  for (f_it = _mesh->faces_begin(); f_it != f_end; ++f_it){
    typename MeshT::ConstFaceVertexIter cfv_it = _mesh->cfv_iter(*f_it);

    const typename MeshT::Point v0 = _mesh->point( *cfv_it );
    ++cfv_it;
    const typename MeshT::Point v1 = _mesh->point( *cfv_it );
    ++cfv_it;
    const typename MeshT::Point v2 = _mesh->point( *cfv_it );

    const float aspect = ACG::Geometry::aspectRatio(v0, v1, v2);

    if (aspect < minA) minA = aspect;
    if (aspect > maxA) maxA = aspect;
    sumA += aspect;

    //inner triangle angles

    double angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(v2 - v0) | MathTools::sane_normalized(v1 - v0) )));

    if (angle < minI) minI = angle;
    if (angle > maxI) maxI = angle;
    //sumI += angle;

    angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(v2 - v1) | MathTools::sane_normalized(v0 - v1) )));

    if (angle < minI) minI = angle;
    if (angle > maxI) maxI = angle;
    //sumI += angle;

    angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(v1 - v2) | MathTools::sane_normalized(v0 - v2) )));

    if (angle < minI) minI = angle;
    if (angle > maxI) maxI = angle;
    //sumI += angle;

    //compute dihedral angles
    typename MeshT::FaceFaceIter ff_it;
    const typename MeshT::Normal n1 = _mesh->normal(*f_it);

    for (ff_it = _mesh->ff_iter(*f_it); ff_it.is_valid(); ++ff_it){
      
      const typename MeshT::Normal n2 = _mesh->normal(*ff_it);

      angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(n1) | MathTools::sane_normalized(n2) )));

      if (angle < minD) minD = angle;
      if (angle > maxD) maxD = angle;
      sumD += angle;
      numD ++;
    }

    const unsigned int valence = _mesh->valence(*f_it);
    minFValence = std::min(minFValence, valence);
    maxFValence = std::max(maxFValence, valence);
    sumFValence += valence;
  }

  info_->aspectMin->setText( QString::number(minA,'f') );
  info_->aspectMean->setText( QString::number( sumA / _mesh->n_faces(),'f' ) );
  info_->aspectMax->setText( QString::number(maxA,'f') );

  info_->angleMin->setText( QString::number(minI,'f') );
  info_->angleMean->setText( "-" );
  info_->angleMax->setText( QString::number(maxI,'f')  );

  info_->faceValenceMin->setText(trUtf8("%1").arg(minFValence));
  info_->faceValenceMax->setText(trUtf8("%1").arg(maxFValence));
  info_->faceValenceMean->setText(trUtf8("%1").arg(
          static_cast<float>(sumFValence) / _mesh->n_faces()));
  
  // Only one face or no face -> don't output angles 
  if ( _mesh->n_faces() > 1 ) {
    info_->dihedralMin->setText( QString::number(minD,'f') );
    info_->dihedralMean->setText( QString::number( sumD / numD,'f' ) );
    info_->dihedralMax->setText( QString::number(maxD,'f') );
  } else {
    info_->dihedralMin->setText( "-" );
    info_->dihedralMean->setText( "-" );
    info_->dihedralMax->setText( "-" );
  }

  //Calculate Bounding Box(min,max,cog)
  ACG::Vec3d min;
  ACG::Vec3d max;
  MeshInfo::getBoundingBox(_mesh, min, max);

  //Bounding Box Size
  ACG::Vec3d diff = max-min;

  info_->bbMinX->setText( QString::number(min[0],'f') );
  info_->bbMinY->setText( QString::number(min[1],'f') );
  info_->bbMinZ->setText( QString::number(min[2],'f') );

  info_->bbMaxX->setText( QString::number(max[0],'f') );
  info_->bbMaxY->setText( QString::number(max[1],'f') );
  info_->bbMaxZ->setText( QString::number(max[2],'f') );

  info_->bbSizeX->setText( QString::number(diff[0],'f') );
  info_->bbSizeY->setText( QString::number(diff[1],'f') );
  info_->bbSizeZ->setText( QString::number(diff[2],'f') );

  //COG
  ACG::Vec3d cog = MeshInfo::cog(_mesh);

  info_->cogX->setText( QString::number(cog[0],'f') );
  info_->cogY->setText( QString::number(cog[1],'f') );
  info_->cogZ->setText( QString::number(cog[2],'f') );

  //hitpoint
  info_->pointX->setText( QString::number( _hitPoint[0],'f' ) );
  info_->pointY->setText( QString::number( _hitPoint[1],'f' ) );
  info_->pointZ->setText( QString::number( _hitPoint[2],'f' ) );

  info_->setWindowFlags(info_->windowFlags() | Qt::WindowStaysOnTopHint);


  info_->show();
}

//----------------------------------------------------------------------------------------------

/** \brief Find closest vertex to selection
 *
 * @param _mesh     Reference to the mesh
 * @param _face_idx Index of the face that has been clicked on
 * @param _hitPoint The point that is used as the reference
 *
 * @return index of the closest vertex of the face to the hitpoint
 */

template <class MeshT>
int InfoMeshObjectPlugin::getClosestVertexInFace(MeshT* _mesh, int _face_idx, ACG::Vec3d& _hitPoint) {

    typename MeshT::FaceVertexIter fv_it;

    int closest_v_idx = 0;
    double dist = DBL_MAX;

    ACG::Vec3d vTemp = ACG::Vec3d(0.0, 0.0, 0.0);
    typename MeshT::Point p;

    for (fv_it = _mesh->fv_iter(_mesh->face_handle(_face_idx)); fv_it.is_valid(); ++fv_it){

      p = _mesh->point( *fv_it );

      // Find closest vertex to selection
      vTemp = ACG::Vec3d(p[0], p[1], p[2]);
      const double temp_dist = (vTemp - _hitPoint).length();

      if (temp_dist < dist) {
          dist = temp_dist;
          closest_v_idx = fv_it->idx();
      }

    }
    return closest_v_idx;
}

//-------------------------------------------------------------------------------------------

/** \brief Find closest edge to selection
 *
 * @param _mesh     Reference to the mesh
 * @param _face_idx Index of the face that has been clicked on
 * @param _hitPoint The point which will be tested
 *
 * @return index of the closest edge in the face to the hitpoint
 */

template <class MeshT>
int InfoMeshObjectPlugin::getClosestEdgeInFace(MeshT* _mesh, int _face_idx, const ACG::Vec3d& _hitPoint) {

    typename MeshT::ConstFaceHalfedgeIter fh_it;
    typename MeshT::VertexHandle v1, v2;
    typename MeshT::Point p1, p2;

    ACG::Vec3d vp1, vp2, h;
    double dist = DBL_MAX;
    int closest_e_handle = 0;

    for (fh_it = _mesh->fh_iter(_mesh->face_handle(_face_idx)); fh_it.is_valid(); ++fh_it){

      v1 = _mesh->from_vertex_handle(*fh_it);
      v2 = _mesh->to_vertex_handle(*fh_it);

      p1 = _mesh->point(v1);
      p2 = _mesh->point(v2);

      vp1 = ACG::Vec3d(p1[0], p1[1], p1[2]);
      vp2 = ACG::Vec3d(p2[0], p2[1], p2[2]);

      const ACG::Vec3d e = (vp2 - vp1).normalized();
      const ACG::Vec3d g = _hitPoint - vp1;
      const double x = g | e;

      const double temp_dist = (_hitPoint - (vp1 + x * e)).length();

      if (temp_dist < dist) {
          dist = temp_dist;
      }
    }

    return closest_e_handle;
}

//----------------------------------------------------------------------------------------------

/** \brief Find closest vertex on the edge (endpoint)
 *
 * @param _mesh     Reference to the mesh
 * @param _edge_idx Index of the edge that has been clicked on
 * @param _hitPoint The point which will be tested
 *
 * @return index of the closest vertex on the edge
 */

template <class MeshT>
int InfoMeshObjectPlugin::getClosestVertexFromEdge(MeshT* _mesh, int _edge_idx, ACG::Vec3d& _hitPoint) {

    ACG::Vec3d toVertex   = _mesh->point( _mesh->to_vertex_handle(   _mesh->halfedge_handle(_mesh->edge_handle(_edge_idx),0 )) );
    ACG::Vec3d fromVertex = _mesh->point( _mesh->from_vertex_handle( _mesh->halfedge_handle(_mesh->edge_handle(_edge_idx),0 )) );

    double distTo   = (_hitPoint - toVertex  ).norm();
    double distFrom = (_hitPoint - fromVertex).norm();

    if ( distTo > distFrom )
      return _mesh->from_vertex_handle( _mesh->halfedge_handle(_mesh->edge_handle(_edge_idx),0 ) ).idx();
    else
      return _mesh->to_vertex_handle(   _mesh->halfedge_handle(_mesh->edge_handle(_edge_idx),0 ) ).idx();

}

//----------------------------------------------------------------------------------------------

void
InfoMeshObjectPlugin::
  slotInformationRequested(const QPoint _clickedPoint, DataType _type) {

    // Only respond on mesh objects
    if((_type != DATA_TRIANGLE_MESH) && (_type != DATA_POLY_MESH)) return;

    ACG::SceneGraph::PickTarget target = ACG::SceneGraph::PICK_FACE;

    unsigned int   node_idx, target_idx;
    ACG::Vec3d     hit_point;

    if (info_->isHidden())
    {
      //user couldn't select the pick mode,
      //so we have to do this
      target = ACG::SceneGraph::PICK_ANYTHING;
      if (!PluginFunctions::scenegraphPick(target, _clickedPoint, node_idx, target_idx, &hit_point))
        return;

      BaseObjectData* object;
      if (!PluginFunctions::getPickedObject(node_idx, object) )
        return;

      //object is picked, now we can decide, what the user wants to pick
      //priority: face > edge > vertex
      if ( object->dataType(DATA_TRIANGLE_MESH) )
      {
        TriMesh* mesh = PluginFunctions::triMesh(object);
        if (mesh->n_faces() != 0)
          info_->pickMode->setCurrentIndex(0);
        else if (mesh->n_edges() != 0)
          info_->pickMode->setCurrentIndex(1);
        else
          info_->pickMode->setCurrentIndex(2);
      }
      else if ( object->dataType(DATA_POLY_MESH) )
      {
        PolyMesh* mesh = PluginFunctions::polyMesh(object);
        if (mesh->n_faces() != 0)
          info_->pickMode->setCurrentIndex(0);
        else if (mesh->n_edges() != 0)
          info_->pickMode->setCurrentIndex(1);
        else
          info_->pickMode->setCurrentIndex(2);
      }
    }

    target = ACG::SceneGraph::PICK_FACE;
    if (info_->pickMode->currentIndex() == 1 )
      target = ACG::SceneGraph::PICK_EDGE;
    else if (info_->pickMode->currentIndex() == 2 )
      target = ACG::SceneGraph::PICK_VERTEX;

    if (PluginFunctions::scenegraphPick(target, _clickedPoint, node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

         emit log( LOGINFO , object->getObjectinfo() );

         if ( object->dataType(DATA_TRIANGLE_MESH) )
           printMeshInfo( PluginFunctions::triMesh(object) , object->id(), target_idx, hit_point );

         if ( object->dataType(DATA_POLY_MESH) )
           printMeshInfo( PluginFunctions::polyMesh(object) , object->id(), target_idx, hit_point );
      } else return;
    }
    else
    {
      emit log( LOGERR , tr("Unable to pick object.") );
    }
}

//------------------------------------------------------------------------------

template< class MeshT >
void InfoMeshObjectPlugin::getEdgeLengths(MeshT* _mesh, double &min, double &max, double &mean)
{
  typename MeshT::ConstEdgeIter e_it(_mesh->edges_sbegin()),
                                e_end(_mesh->edges_end());

  min = FLT_MAX;
  max = FLT_MIN;
  mean = 0.0;
  for (; e_it!=e_end; ++e_it)
  {
    typename MeshT::Scalar len = (_mesh->point(_mesh->to_vertex_handle(_mesh->halfedge_handle(*e_it, 0))) -
                                  _mesh->point(_mesh->to_vertex_handle(_mesh->halfedge_handle(*e_it, 1)))).norm ();
    if (len < min) min = len;
    if (len > max) max = len;
    mean += len;
  }

  mean /= _mesh->n_edges();
}

//------------------------------------------------------------------------------

bool InfoMeshObjectPlugin::getEdgeLengths(int _id, double &min, double &max, double &mean)
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return false;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return false;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,tr("Unable to get mesh"));
      return false;
    }

    getEdgeLengths (mesh, min, max, mean);
    return true;

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,tr("Unable to get mesh"));
      return false;
    }

    getEdgeLengths (mesh, min, max, mean);
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------

void InfoMeshObjectPlugin::slotObjectUpdated( int _identifier , const UpdateType& _type){

  if ( !infoBar_ ) {
    return;
  }

  // We only show the information in the status bar if one target mesh is selected.
  if ( PluginFunctions::targetCount() ==1 ) {

    BaseObjectData* object;
    PluginFunctions::getObject(_identifier,object);

    // We only need to update something, if the updated object is the target object
    if (object && object->target() ) {

      if (object->dataType(DATA_TRIANGLE_MESH)){

        TriMesh* mesh = PluginFunctions::triMesh(object);

        infoBar_->vertices->setText( QLocale::system().toString( qulonglong(mesh->n_vertices()) ) );
        infoBar_->edges->setText( QLocale::system().toString( qulonglong(mesh->n_edges()) ) );
        infoBar_->faces->setText( QLocale::system().toString( qulonglong(mesh->n_faces()) ) );

        infoBar_->showCounts();

        return;
      }

      if (object->dataType(DATA_POLY_MESH)){

        PolyMesh* mesh = PluginFunctions::polyMesh(object);

        infoBar_->vertices->setText( QLocale::system().toString( qulonglong(mesh->n_vertices()) ) );
        infoBar_->edges->setText( QLocale::system().toString( qulonglong(mesh->n_edges()) ) );
        infoBar_->faces->setText( QLocale::system().toString( qulonglong(mesh->n_faces()) ) );

        infoBar_->showCounts();
        return;
      }

    }

  } else {
    // Display only count information
    if ( PluginFunctions::targetCount() > 0 ) {
      infoBar_->showTargetCount( PluginFunctions::targetCount() );
    } else
      infoBar_->hideCounts();
  }

}

//------------------------------------------------------------------------------

void InfoMeshObjectPlugin::slotObjectSelectionChanged( int _identifier ){
  slotObjectUpdated( _identifier , UPDATE_ALL );
}

//------------------------------------------------------------------------------

void InfoMeshObjectPlugin::slotAllCleared(){
  if ( infoBar_ )
    infoBar_->hideCounts();
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( InfoMeshObjectPlugin , InfoMeshObjectPlugin );
#endif


