/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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
//  CLASS InfoPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include <QtGui>

#include "InfoPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <QLocale>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <MeshTools/MeshInfoT.hh>
#include <OpenFlipper/common/BaseObject.hh>
#include <ACG/Geometry/Algorithms.hh>

#include <Math_Tools/Math_Tools.hh>

#include <float.h>

//== IMPLEMENTATION ==========================================================

void InfoPlugin::initializePlugin() {
  infoBar_ = new InfoBar();
}

/// initialize the plugin
void InfoPlugin::pluginsInitialized() {

  //set the slot descriptions
  setDescriptions();

  emit addWidgetToStatusbar(infoBar_);
  infoBar_->hideCounts();
  
  // Initialize hit point
  hit_point_ = ACG::Vec3d(0.0, 0.0, 0.0);
}

//-----------------------------------------------------------------------------

template< class MeshT >
void InfoPlugin::printMeshInfo( MeshT* _mesh , int _id, unsigned int _face, ACG::Vec3d& _hitPoint ) {

  QLocale locale;

  if (info_ == 0){
    info_ = new InfoDialog();
  }

  int closest_v_idx = getClosestVertex(_mesh, _face);
  int closest_e_idx = getClosestEdge(_mesh, _face);

  QString name;

  // name
  BaseObject* obj = 0;
  if ( PluginFunctions::getObject(_id, obj) )
    info_->generalBox->setTitle( tr("General object information for %1").arg( obj->name() ) );

  // ID
  info_->id->setText( locale.toString(_id) );
  // Vertices
  info_->vertices->setText( locale.toString( _mesh->n_vertices() ) );
  // Faces
  info_->faces->setText( locale.toString( _mesh->n_faces() ) );
  // Edges
  info_->edges->setText( locale.toString( _mesh->n_edges() ) );
  // Closest Vertex
  info_->vertexHandle->setText( locale.toString( closest_v_idx ) );
  // Closest Edge
  info_->edgeHandle->setText( locale.toString( closest_e_idx ) );
  // Picked Vertex
  info_->faceHandle->setText( locale.toString( _face ) );
  // Components
  info_->components->setText( locale.toString(MeshInfo::componentCount(_mesh)));
  // Boundaries
  info_->boundaries->setText( locale.toString(MeshInfo::boundaryCount(_mesh)) );
  // Genus
  int genus = (2 - _mesh->n_vertices() + _mesh->n_edges() - _mesh->n_faces()) / 2;
  info_->genus->setText( QString::number(genus) );

  // Coordinates
  typename MeshT::VertexIter v_it;
  typename MeshT::VertexIter v_end = _mesh->vertices_end();

  float maxX = FLT_MIN;
  float minX = FLT_MAX;
  float sumX = 0.0;
  float maxY = FLT_MIN;
  float minY = FLT_MAX;
  float sumY = 0.0;
  float maxZ = FLT_MIN;
  float minZ = FLT_MAX;
  float sumZ = 0.0;
  int minV = 999;
  int maxV = 0;
  int sumV = 0;
  float maxE = FLT_MIN;
  float minE = FLT_MAX;
  float sumE = 0.0;

  //iterate over all vertices
  for (v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it){
    typename MeshT::Point p = _mesh->point( v_it.handle() );
    if (p[0] < minX) minX = p[0];
    if (p[0] > maxX) maxX = p[0];
    sumX += p[0];
    if (p[1] < minY) minY = p[1];
    if (p[1] > maxY) maxY = p[1];
    sumY += p[1];
    if (p[2] < minZ) minZ = p[2];
    if (p[2] > maxZ) maxZ = p[2];
    sumZ += p[2];



    //check valence + edge length
    int valence = 0;
    typename MeshT::VertexVertexIter vv_it;

    for (vv_it=_mesh->vv_iter( v_it ); vv_it; ++vv_it){
      valence++;

      typename MeshT::Point p2 = _mesh->point( vv_it.handle() );
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
  float sumI = 0.0;
  float maxD = FLT_MIN;
  float minD = FLT_MAX;
  float sumD = 0.0;
  int numD = 0;

  //iterate over all faces
  for (f_it = _mesh->faces_begin(); f_it != f_end; ++f_it){
    typename MeshT::ConstFaceVertexIter cfv_it = _mesh->cfv_iter(f_it);

    typename MeshT::Point v0 = _mesh->point( cfv_it.handle() );
    ++cfv_it;
    typename MeshT::Point v1 = _mesh->point( cfv_it.handle() );
    ++cfv_it;
    typename MeshT::Point v2 = _mesh->point( cfv_it.handle() );

    float aspect = ACG::Geometry::aspectRatio(v0, v1, v2);

    if (aspect < minA) minA = aspect;
    if (aspect > maxA) maxA = aspect;
    sumA += aspect;

    //inner triangle angles

    double angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(v2 - v0) | MathTools::sane_normalized(v1 - v0) )));

    if (angle < minI) minI = angle;
    if (angle > maxI) maxI = angle;
    sumI += angle;

    angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(v2 - v1) | MathTools::sane_normalized(v0 - v1) )));

    if (angle < minI) minI = angle;
    if (angle > maxI) maxI = angle;
    sumI += angle;

    angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(v1 - v2) | MathTools::sane_normalized(v0 - v2) )));

    if (angle < minI) minI = angle;
    if (angle > maxI) maxI = angle;
    sumI += angle;

    //compute dihedral angles
    typename MeshT::FaceFaceIter ff_it;
    typename MeshT::Normal n1 = _mesh->normal(f_it);

    for (ff_it = _mesh->ff_iter(f_it); ff_it; ++ff_it){
      
      typename MeshT::Normal n2 = _mesh->normal(ff_it);

      angle = OpenMesh::rad_to_deg(acos(OpenMesh::sane_aarg( MathTools::sane_normalized(n1) | MathTools::sane_normalized(n2) )));

      if (angle < minD) minD = angle;
      if (angle > maxD) maxD = angle;
      sumD += angle;
      numD ++;
    }
  }

  info_->aspectMin->setText( QString::number(minA,'f') );
  info_->aspectMean->setText( QString::number( sumA / _mesh->n_faces(),'f' ) );
  info_->aspectMax->setText( QString::number(maxA,'f') );

  info_->angleMin->setText( QString::number(minI,'f') );
  info_->angleMean->setText( "-" );
  info_->angleMax->setText( QString::number(maxI,'f')  );

  
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

  typename MeshT::FaceHandle fh = _mesh->face_handle(_face);

  //adjacent vertex handles
  typename MeshT::FaceVertexIter fv_it = _mesh->fv_iter(fh);
  QString adjacentVertices;

  if ( fv_it ){
    adjacentVertices = QString::number( fv_it.handle().idx() );
    ++fv_it;
  }

  while( fv_it ){
    adjacentVertices += "; " + QString::number( fv_it.handle().idx() );
    ++fv_it;
  }

  info_->adjVertexHandles->setText( adjacentVertices );

  //Calculate Bounding Box(min,max,cog)
  ACG::Vec3d min;
  ACG::Vec3d max;
  MeshInfo::getBoundingBox(*_mesh, min, max);

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
  ACG::Vec3d cog = MeshInfo::cog(*_mesh);

  info_->cogX->setText( QString::number(cog[0],'f') );
  info_->cogY->setText( QString::number(cog[1],'f') );
  info_->cogZ->setText( QString::number(cog[2],'f') );

  //face-normal
  info_->normalX->setText( QString::number( _mesh->normal(fh)[0],'f' ) );
  info_->normalY->setText( QString::number( _mesh->normal(fh)[1],'f' ) );
  info_->normalZ->setText( QString::number( _mesh->normal(fh)[2],'f' ) );

  //hitpoint
  info_->pointX->setText( QString::number( _hitPoint[0],'f' ) );
  info_->pointY->setText( QString::number( _hitPoint[1],'f' ) );
  info_->pointZ->setText( QString::number( _hitPoint[2],'f' ) );


  //closest vertex
  info_->vertexX->setText( QString::number( _mesh->point( _mesh->vertex_handle(closest_v_idx) )[0],'f' ) );
  info_->vertexY->setText( QString::number( _mesh->point( _mesh->vertex_handle(closest_v_idx) )[1],'f' ) );
  info_->vertexZ->setText( QString::number( _mesh->point( _mesh->vertex_handle(closest_v_idx) )[2],'f' ) );


  info_->setWindowFlags(info_->windowFlags() | Qt::WindowStaysOnTopHint);

  info_->show();
}

//----------------------------------------------------------------------------------------------

/** \brief Find closest vertex to selection
 *
 * @param _mesh Refernce to the mesh
 * @param _face_idx Index of the face that has been clicked on
 */

template <class MeshT>
int InfoPlugin::getClosestVertex(MeshT* _mesh, int _face_idx) {

    typename MeshT::FaceVertexIter fv_it;

    int closest_v_idx = 0;
    double dist = DBL_MAX;
    double temp_dist = 0.0;

    ACG::Vec3d vTemp = ACG::Vec3d(0.0, 0.0, 0.0);
    typename MeshT::Point p;

    for (fv_it = _mesh->fv_iter(_mesh->face_handle(_face_idx)); fv_it; ++fv_it){

      p = _mesh->point( fv_it.handle() );

      // Find closest vertex to selection
      vTemp = ACG::Vec3d(p[0], p[1], p[2]);
      temp_dist = (vTemp - hit_point_).length();

      if (temp_dist < dist) {
          dist = temp_dist;
          closest_v_idx = fv_it.handle().idx();
      }

    }
    return closest_v_idx;
}

//-------------------------------------------------------------------------------------------

/** \brief Find closest edge to selection
 *
 * @param _mesh Reference to the mesh
 * @param _face_idx Index of the face that has been clicked on
 */

template <class MeshT>
int InfoPlugin::getClosestEdge(MeshT* _mesh, int _face_idx) {

    typename MeshT::ConstFaceHalfedgeIter fh_it;
    typename MeshT::VertexHandle v1, v2;
    typename MeshT::Point p1, p2;

    ACG::Vec3d vp1, vp2, click, e, g, h;
    double x, temp_dist, dist = DBL_MAX;
    int closest_e_handle = 0;
    click = ACG::Vec3d(hit_point_[0], hit_point_[1], hit_point_[2]);

    for (fh_it = _mesh->fh_iter(_mesh->face_handle(_face_idx)); fh_it; ++fh_it){

      v1 = _mesh->from_vertex_handle(fh_it);
      v2 = _mesh->to_vertex_handle(fh_it);

      p1 = _mesh->point(v1);
      p2 = _mesh->point(v2);

      vp1 = ACG::Vec3d(p1[0], p1[1], p1[2]);
      vp2 = ACG::Vec3d(p2[0], p2[1], p2[2]);

      e = vp2 - vp1;
      e.normalize();
      g = click - vp1;
      x = g | e;

      temp_dist = (click - (vp1 + x * e)).length();

      if (temp_dist < dist) {
          dist = temp_dist;
          closest_e_handle = _mesh->edge_handle(fh_it.handle()).idx();
      }
    }

    return closest_e_handle;
}

//----------------------------------------------------------------------------------------------

void
InfoPlugin::
  slotMouseEventIdentify( QMouseEvent* _event ) {
  if (_event->type() == QEvent::MouseButtonPress)
  {
    unsigned int   node_idx, target_idx;
    ACG::Vec3d     hit_point;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

//     BaseObject* obj = dynamic_cast< BaseObject* > (object);

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

         emit log( LOGINFO , object->getObjectinfo() );

         // Set hit point
         hit_point_ = ACG::Vec3d(hit_point[0], hit_point[1], hit_point[2]);

         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH) )
            printMeshInfo( PluginFunctions::triMesh(object) , object->id(), target_idx, hit_point_ );

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH) )
            printMeshInfo( PluginFunctions::polyMesh(object) , object->id(), target_idx, hit_point_ );

      } else return;
    }
  }
}

//------------------------------------------------------------------------------

template< class MeshT >
void InfoPlugin::getEdgeLengths(MeshT* _mesh, double &min, double &max, double &mean)
{
  typename MeshT::ConstEdgeIter e_it(_mesh->edges_sbegin()),
                                e_end(_mesh->edges_end());

  min = FLT_MAX;
  max = FLT_MIN;
  mean = 0.0;
  for (; e_it!=e_end; ++e_it)
  {
    typename MeshT::Scalar len = (_mesh->point(_mesh->to_vertex_handle(_mesh->halfedge_handle(e_it, 0))) -
                                  _mesh->point(_mesh->to_vertex_handle(_mesh->halfedge_handle(e_it, 1)))).norm ();
    if (len < min) min = len;
    if (len > max) max = len;
    mean += len;
  }

  mean /= _mesh->n_edges();
}

//------------------------------------------------------------------------------

bool InfoPlugin::getEdgeLengths(int _id, double &min, double &max, double &mean)
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

void InfoPlugin::slotObjectUpdated( int /*_identifier*/ , const UpdateType _type){

  if ( (PluginFunctions::objectCount() == 1) || (PluginFunctions::targetCount() == 1) ){

    // This block is only interesting for topology changes
    if ( ! _type.contains(UPDATE_TOPOLOGY) ) {
      return;
    }
      
    if ( !infoBar_ ) {
      return;    
    }

    bool found = false;
    
    PluginFunctions::IteratorRestriction restriction;
    
    if ( PluginFunctions::targetCount() == 1 )
      restriction = PluginFunctions::TARGET_OBJECTS;
    else
      restriction = PluginFunctions::ALL_OBJECTS;
    
    
    for ( PluginFunctions::ObjectIterator o_it(restriction,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH )) ;
      o_it != PluginFunctions::objectsEnd(); ++o_it)   {
      
      if (o_it->dataType(DATA_TRIANGLE_MESH)){
        
        TriMesh* mesh = PluginFunctions::triMesh(*o_it);
      
        infoBar_->vertices->setText( QLocale::system().toString( mesh->n_vertices() ) );
        infoBar_->edges->setText( QLocale::system().toString( mesh->n_edges() ) );
        infoBar_->faces->setText( QLocale::system().toString( mesh->n_faces() ) );
      }
      
      if (o_it->dataType(DATA_POLY_MESH)){

        PolyMesh* mesh = PluginFunctions::polyMesh(*o_it);
      
        infoBar_->vertices->setText( QLocale::system().toString( mesh->n_vertices() ) );
        infoBar_->edges->setText( QLocale::system().toString( mesh->n_edges() ) );
        infoBar_->faces->setText( QLocale::system().toString( mesh->n_faces() ) );
    
      }
      found = true;
    }
    
    if (found) 
      infoBar_->showCounts();
    else
      infoBar_->hideCounts();
  } else {
    if ( PluginFunctions::targetCount() > 0 )
      infoBar_->showTargetCount( PluginFunctions::targetCount() );
    else
      infoBar_->hideCounts();
  }
}

//------------------------------------------------------------------------------

void InfoPlugin::slotObjectSelectionChanged( int _identifier ){
  slotObjectUpdated( _identifier , UPDATE_ALL );
}

//------------------------------------------------------------------------------

void InfoPlugin::slotAllCleared(){
  infoBar_->hideCounts();
}

Q_EXPORT_PLUGIN2( InfoPlugin , InfoPlugin );


