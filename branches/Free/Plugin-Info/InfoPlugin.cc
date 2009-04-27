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

/// initialize the plugin
void InfoPlugin::pluginsInitialized() {

  //set the slot descriptions
  setDescriptions();

  // Initialize hit point
  hit_point_ = ACG::Vec3d(0.0, 0.0, 0.0);
}

//-----------------------------------------------------------------------------

template< class MeshT >
void InfoPlugin::printMeshInfo( MeshT* _mesh , int _id, unsigned int _face ) {

  QLocale locale;

  if (info_ == 0){
    info_ = new InfoDialog();
  }

  int closest_v_idx = getClosestVertex(_mesh, _face);
  int closest_e_idx = getClosestEdge(_mesh, _face);

  // ID
  info_->id->setText( "<B>Object ID:</B> " + locale.toString(_id) );
  // Vertices
  info_->vertices->setText( "<B>Vertices:</B> " + locale.toString( _mesh->n_vertices() ) );
  // Faces
  info_->faces->setText( "<B>Faces:</B> " + locale.toString( _mesh->n_faces() ) );
  // Edges
  info_->edges->setText( "<B>Edges:</B> " + locale.toString( _mesh->n_edges() ) );
  // Closest Vertex
  info_->closestv->setText( "<B>Closest vertex:</B> " + locale.toString( closest_v_idx ) );
  // Closest Edge
  info_->closeste->setText( "<B>Closest edge:</B> " + locale.toString( closest_e_idx ) );
  // Picked Vertex
  info_->pickedFace->setText( "<B>Picked face:</B> " + locale.toString( _face ) );
  // Components
  info_->components->setText( "<B>Components:</B> " + locale.toString(MeshInfo::componentCount(_mesh)));
  // Boundaries
  info_->boundaries->setText( "<B>Boundaries:</B> " + locale.toString(MeshInfo::boundaryCount(_mesh)) );
  // Genus
  int genus = 1 - (_mesh->n_vertices() - _mesh->n_edges() + _mesh->n_faces() ) / 2;
  info_->genus->setText( "<B>Genus:</B> " + QString::number(genus) );

  info_->table->clear();

  info_->table->setColumnCount ( 3 );
  info_->table->setRowCount ( 8 );


  QStringList headerdata;
  headerdata << "Min" << "Mean" << "Max";

  info_->table->setHorizontalHeaderLabels(headerdata);

  headerdata.clear();
  headerdata << "X Coordinate" << "Y Coordinate" << "Z Coordinate" << "Vertex Valence" << "Edge Length" << "Aspect Ratio";
  headerdata << "Inner Face Angles" << "Dihedral Angles";

  info_->table->setVerticalHeaderLabels(headerdata);

  //set tooltips
  info_->table->verticalHeaderItem(0)->setToolTip("minimum, maximum and arithmetic mean of the vertex x-coordinates");
  info_->table->verticalHeaderItem(1)->setToolTip("minimum, maximum and arithmetic mean of the vertex y-coordinates");
  info_->table->verticalHeaderItem(2)->setToolTip("minimum, maximum and arithmetic mean of the vertex z-coordinates");
  info_->table->verticalHeaderItem(3)->setToolTip("minimum, maximum and arithmetic mean of the vertex valences");
  info_->table->verticalHeaderItem(4)->setToolTip("minimum, maximum and arithmetic mean of the edge lengthes");
  info_->table->verticalHeaderItem(5)->setToolTip("minimum, maximum and arithmetic mean of the aspect ratio."
                                                  " i.e. the ratio between longest and shortest edge in a triangle.");
  info_->table->verticalHeaderItem(6)->setToolTip("minimum, maximum and arithmetic mean of the inner angles in a face.");
  info_->table->verticalHeaderItem(7)->setToolTip("minimum, maximum and arithmetic mean of the dihedral angles"
                                                  " i.e. the angles between neighboring faces.");

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

  int row=0;
  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minX,'f') ) );
  info_->table->setItem(row,1, new QTableWidgetItem( QString::number( sumX / _mesh->n_vertices(),'f' )) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxX,'f') ) );
  row++;
  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minY,'f') ) );
  info_->table->setItem(row,1, new QTableWidgetItem( QString::number( sumY / _mesh->n_vertices(),'f' )) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxY,'f') ) );
  row++;
  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minZ,'f') ) );
  info_->table->setItem(row,1, new QTableWidgetItem( QString::number( sumZ / _mesh->n_vertices(),'f' )) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxZ,'f') ) );
  row++;
  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minV) ) );
  info_->table->setItem(row,1, new QTableWidgetItem( QString::number( sumV / (float)_mesh->n_vertices(),'f' )) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxV) ) );
  row++;
  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minE,'f') ) );
  info_->table->setItem(row,1, new QTableWidgetItem( QString::number( sumE / (_mesh->n_edges()*2),'f' )) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxE,'f') ) );

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

    }
  }

  row++;
  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minA,'f') ) );
  info_->table->setItem(row,1, new QTableWidgetItem( QString::number( sumA / _mesh->n_faces(),'f' )) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxA,'f') ) );
  row++;
  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minI,'f') ) );
  info_->table->setItem(row,1, new QTableWidgetItem( "-" ) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxI,'f') ) );
  row++;

  info_->table->setItem(row,0, new QTableWidgetItem( QString::number(minD,'f') ) );
  info_->table->setItem(row,1, new QTableWidgetItem( QString::number( sumD / (_mesh->n_faces()*3),'f' )) );
  info_->table->setItem(row,2, new QTableWidgetItem( QString::number(maxD,'f') ) );

  info_->table->resizeColumnsToContents();


  info_->table3->clear();

  info_->table3->setColumnCount ( 3 );


  headerdata.clear();
  headerdata << "X" << "Y" << "Z";

  info_->table3->setHorizontalHeaderLabels(headerdata);

  typename MeshT::FaceHandle fh = _mesh->face_handle(_face);

  headerdata.clear();
  headerdata << "Bounding Box (Minimum)" << "Bounding Box (Maximum)" << "BoundingBox (Size)" << "Center of Gravity";
  headerdata << "Picked Face Normal (Handle "+ QString::number(fh.idx()) +")";

  typename MeshT::FaceVertexIter fv_it = _mesh->fv_iter(fh);
  int vertexCount = 0;
  while( fv_it ){
    headerdata << "Adjacent Vertex (Handle " + QString::number(fv_it.handle().idx()) + ")";
    ++fv_it;
    vertexCount++;
  }

  info_->table3->setRowCount ( 5 + vertexCount );
  info_->table3->setVerticalHeaderLabels(headerdata);

  //set tooltips
  info_->table3->verticalHeaderItem(0)->setToolTip("minimum corner coordinates of the bounding box");
  info_->table3->verticalHeaderItem(1)->setToolTip("maximum corner coordinates of the bounding box");
  info_->table3->verticalHeaderItem(2)->setToolTip("diagonal size of the bounding box");
  info_->table3->verticalHeaderItem(3)->setToolTip("coordinates of the center of gravity");
  info_->table3->verticalHeaderItem(4)->setToolTip("direction of the face normal that was picked");
  for (int i=0; i < vertexCount; i++)
    info_->table3->verticalHeaderItem(5 + i)->setToolTip("Coordinates of a vertex which is adjacent to the picked face");

  //Calculate Bounding Box(min,max,cog)
  ACG::Vec3d min;
  ACG::Vec3d max;
  MeshInfo::getBoundingBox(*_mesh, min, max);

  //Bounding Box Size
  ACG::Vec3d diff = max-min;

  row=0;
  info_->table3->setItem(row,0, new QTableWidgetItem( QString::number(min[0],'f') ) );
  info_->table3->setItem(row,1, new QTableWidgetItem( QString::number(min[1],'f') ) );
  info_->table3->setItem(row,2, new QTableWidgetItem( QString::number(min[2],'f') ) );
  row++;
  info_->table3->setItem(row,0, new QTableWidgetItem( QString::number(max[0],'f') ) );
  info_->table3->setItem(row,1, new QTableWidgetItem( QString::number(max[1],'f') ) );
  info_->table3->setItem(row,2, new QTableWidgetItem( QString::number(max[2],'f') ) );
  row++;
  info_->table3->setItem(row,0, new QTableWidgetItem( QString::number(diff[0],'f') ) );
  info_->table3->setItem(row,1, new QTableWidgetItem( QString::number(diff[1],'f') ) );
  info_->table3->setItem(row,2, new QTableWidgetItem( QString::number(diff[2],'f') ) );

  //COG
  ACG::Vec3d cog = MeshInfo::cog(*_mesh);

  row++;
  info_->table3->setItem(row,0, new QTableWidgetItem( QString::number(cog[0],'f') ) );
  info_->table3->setItem(row,1, new QTableWidgetItem( QString::number(cog[1],'f') ) );
  info_->table3->setItem(row,2, new QTableWidgetItem( QString::number(cog[2],'f') ) );

  //face-normal
  row++;
  info_->table3->setItem(row,0, new QTableWidgetItem( QString::number( _mesh->normal(fh)[0],'f' ) ) );
  info_->table3->setItem(row,1, new QTableWidgetItem( QString::number( _mesh->normal(fh)[1],'f' ) ) );
  info_->table3->setItem(row,2, new QTableWidgetItem( QString::number( _mesh->normal(fh)[2],'f' ) ) );

  fv_it = _mesh->fv_iter(fh);
  while( fv_it )
  {
    row++;
    info_->table3->setItem(row,0, new QTableWidgetItem( QString::number( _mesh->point(fv_it)[0],'f' ) ) );
    info_->table3->setItem(row,1, new QTableWidgetItem( QString::number( _mesh->point(fv_it)[1],'f' ) ) );
    info_->table3->setItem(row,2, new QTableWidgetItem( QString::number( _mesh->point(fv_it)[2],'f' ) ) );

    ++fv_it;
  }

  info_->table3->resizeColumnsToContents();

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
            printMeshInfo( PluginFunctions::triMesh(object) , object->id(), target_idx );

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH) )
            printMeshInfo( PluginFunctions::polyMesh(object) , object->id(), target_idx );

      } else return;
    }
  }
}

//------------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( InfoPlugin , InfoPlugin );


