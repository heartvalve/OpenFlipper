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

#include <QtGui>

#include "MeshRepairPlugin.hh"

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "OpenFlipper/INIFile/INIFile.hh"

#include <MeshTools/MeshSelectionT.hh>
#include <ACG/Geometry/Algorithms.hh>
#include <Math_Tools/Math_Tools.hh>

//-----------------------------------------------------------------------------

void
MeshRepairPlugin::
initializePlugin()
{
  tool_ = new MeshRepairToolbarWidget();
  QSize size(300, 300);
  tool_->resize(size);
  
  
  // Vertex Selection/Removal
  connect(tool_->valenceThreeButton, SIGNAL(clicked()), this, SLOT(slotDetectFlatTriangles()) );
  connect(tool_->repairRemoveVButton, SIGNAL(clicked()), this, SLOT(slotRemoveSelectedVal3Vertices()) );

  // Edge Selection/Repairing
  connect(tool_->detectEShorterButton, SIGNAL(clicked()), this, SLOT(slotDetectEdgesShorter()) );
  connect(tool_->detectELargerButton, SIGNAL(clicked()), this, SLOT(slotDetectEdgesLonger()) );
  connect(tool_->repairCollapseEButton, SIGNAL(clicked()), this, SLOT(slotRemoveSelectedEdges()) );
  connect(tool_->detectCapAngle, SIGNAL(clicked()), this, SLOT(slotDetectSkinnyTriangleByAngle()) );
  connect(tool_->repairFlipEButton, SIGNAL(clicked()), this, SLOT(slotRemoveSkinnyTriangleByAngle()) );
  connect(tool_->detectFoldoverButton, SIGNAL(clicked()), this, SLOT(slotDetectFoldover()) );
  
  //Face operations
  connect(tool_->triangleAspectButton,SIGNAL(clicked()),this,SLOT(slotDetectTriangleAspect()));
  connect(tool_->flipOrientation,SIGNAL(clicked()),this,SLOT(slotFlipOrientation()));

  toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"meshrepair-toolbox.png");
  tool_->repairCollapseEButton->setIcon(*toolIcon_);
  tool_->repairFlipEButton->setIcon(*toolIcon_);
  tool_->repairRemoveVButton->setIcon(*toolIcon_);
  
  emit addToolbox( tr("Mesh Repair") , tool_,  toolIcon_);
}

//-----------------------------------------------------------------------------

/** \brief Initialization of the plugin when it is loaded by the core
 * 
 */
void MeshRepairPlugin::pluginsInitialized() {

  emit setSlotDescription("decimate(int,QVariantMap)",tr("Decimate a given object"),
                          QString(tr("objectId,constraints")).split(","),
                          QString(tr("ID of an object; Object that can has one or more constraint properties (distance,normal_deviation,roundness,vertices)")).split(";"));
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectEdgesShorter(){
  double length = tool_->edgeSpin->value();
  
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) 
    selectEdgesShorterThan(o_it->id(),length);
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectEdgesLonger(){
  double length = tool_->edgeSpin->value();

  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
     selectEdgesLongerThan(o_it->id(),length);

  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::selectEdgesShorterThan(int _objectId,double _length) {
  selectionEdgeLength(_objectId,_length,false);
  
  emit scriptInfo( "selectEdgesShorterThan(" + QString::number(_objectId) + ", " + QString::number(_length) + ")" );
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::selectEdgesLongerThan(int _objectId,double _length) {
  selectionEdgeLength(_objectId,_length,true);
  
  emit scriptInfo( "selectEdgesLongerThan(" + QString::number(_objectId) + ", " + QString::number(_length) + ")" );
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::selectionEdgeLength(int _objectId, double _length, bool _larger) {

  // get the target mesh
  TriMesh* triMesh = 0;
  
  PluginFunctions::getMesh(_objectId,triMesh);
  
  if ( triMesh ) {
    TriMesh::EdgeIter e_it, e_end=triMesh->edges_end();
    
    // Clear current edge selection
    MeshSelection::clearEdgeSelection(triMesh);
   
    // Iterate over all edges
    for (e_it = triMesh->edges_begin(); e_it != e_end ; ++e_it) {
      TriMesh::HalfedgeHandle he = triMesh->halfedge_handle( e_it, 0 );
      TriMesh::Point p1 = triMesh->point( triMesh->from_vertex_handle( he ) );
      TriMesh::Point p2 = triMesh->point( triMesh->to_vertex_handle( he )   );
      
      if ( _larger ) {
        if ( (p1 - p2).norm() > _length)
          triMesh->status(e_it).set_selected(true);
      } else {
        if ( (p1 - p2).norm() < _length)
          triMesh->status(e_it).set_selected(true);
      }
    }
    
    emit updatedObject(_objectId,UPDATE_SELECTION);
    emit createBackup( _objectId, "Select Edges", UPDATE_SELECTION);
    
    return;
  }
  
  // get the target mesh
  PolyMesh* polyMesh = 0;
  PluginFunctions::getMesh(_objectId,polyMesh);
  
  if ( polyMesh ) {
    PolyMesh::EdgeIter e_it, e_end=polyMesh->edges_end();
    
    // Clear current edge selection
    MeshSelection::clearEdgeSelection(polyMesh);
    
    // Iterate over all edges
    for (e_it = polyMesh->edges_begin(); e_it != e_end ; ++e_it) {
      PolyMesh::HalfedgeHandle he = polyMesh->halfedge_handle( e_it, 0 );
      PolyMesh::Point p1 = polyMesh->point( polyMesh->from_vertex_handle( he ) );
      PolyMesh::Point p2 = polyMesh->point( polyMesh->to_vertex_handle( he )   );
      
      if ( _larger ) {
        if ( (p1 - p2).norm() > _length)
          polyMesh->status(e_it).set_selected(true);
      } else {
        if ( (p1 - p2).norm() < _length)
          polyMesh->status(e_it).set_selected(true);
      }
    }
 
    emit updatedObject(_objectId,UPDATE_SELECTION);
    emit createBackup( _objectId, "Select Edges", UPDATE_SELECTION);
    
    return;
  }
  
  emit log( LOGERR,tr("Unsupported Object Type for edge selection") );
  
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectFlatTriangles() {
    double angle = tool_->valenceThreeSpinbox->value();

    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it)
        detectFlatTriangles(o_it->id(), angle);
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::detectFlatTriangles(int _objectId, double _angle) {

    unsigned int count(0);

    // get the target mesh
    TriMesh* mesh = 0;
    PluginFunctions::getMesh(_objectId,mesh);

    if ( mesh ) {

        // Clear current triangle selection
        MeshSelection::clearVertexSelection(mesh);

        TriMesh::VertexIter                 v_it, v_end(mesh->vertices_end());
        TriMesh::VVIter                     vv_it;
        TriMesh::VFIter                     vf_it;
        TriMesh::FaceHandle                 fh;
        std::vector<TriMesh::VertexHandle>  vh(3);
        unsigned int                        i;
        TriMesh::Scalar                     cosangle(cos(_angle/180.0*M_PI));

        for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it)
        {
            if (!mesh->status(v_it).deleted() && !mesh->is_boundary(v_it) && mesh->valence(v_it) == 3)
            {
                vf_it = mesh->vf_iter(v_it);
                const TriMesh::Normal& n0 = mesh->normal(vf_it);
                const TriMesh::Normal& n1 = mesh->normal(++vf_it);
                const TriMesh::Normal& n2 = mesh->normal(++vf_it);

                if ( (n0|n1) > cosangle &&
                        (n0|n2) > cosangle &&
                        (n1|n2) > cosangle )
                {

                    mesh->status(v_it).set_selected(true);
                    ++count;
                }
            }
        }
    }
    else {
      emit log(LOGERR, "Cannot detect flat triangles on non-trimesh " + QString::number(_objectId) + ".");
    }
    
    if (count > 0) {
        emit updatedObject(_objectId, UPDATE_SELECTION);
        emit createBackup(_objectId, "Select vertices", UPDATE_SELECTION);
    }
    emit log ("Selected " + QString::number(count) + " vertices on object " + QString::number(_objectId) + " with face angle difference smaller than " + QString::number(_angle) + ".");
    emit scriptInfo( "detectFlatTriangles(" + QString::number(_objectId) + ", " + QString::number(_angle) + ")" );
    
    emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotRemoveSelectedVal3Vertices() {

    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) {

        float angle = tool_->detectFoldoverSpinbox->value();
        unsigned int count(0);

        if (o_it->dataType() != DATA_TRIANGLE_MESH) {
            emit log("Cannot delete/merge vertices on non-trimesh " + QString::number(o_it->id()) + ".");
            continue;
        }

        // get the target mesh
        TriMesh* mesh = 0;
        PluginFunctions::getMesh(o_it->id(),mesh);

        if ( mesh ) {

            TriMesh::VertexIter                 v_it, v_end(mesh->vertices_end());
            TriMesh::VVIter                     vv_it;
            TriMesh::VFIter                     vf_it;
            int                                 i;
            std::vector<TriMesh::VertexHandle>  vh(3);

            for (v_it=mesh->vertices_begin(); v_it!=v_end; ++v_it)
            {
                vf_it = mesh->vf_iter(v_it);
                if ((mesh->status(v_it).selected()) && !mesh->status(v_it).feature() && mesh->valence(v_it) == 3)
                {
                    for (i=0, vv_it=mesh->vv_iter(v_it); vv_it; ++vv_it, ++i)
                        vh[2-i] = vv_it.handle();

                    mesh->delete_vertex(v_it, false);
                    mesh->add_face(vh);

                    ++count;
                }
            }
            if (count > 0)
                mesh->garbage_collection();
        }

        if (count > 0) {
            emit updatedObject(o_it->id(), UPDATE_ALL);
            emit createBackup(o_it->id(), "Delete/merge selected vertices", UPDATE_ALL);
        }
        emit log ("Deleted " + QString::number(count) + " vertices on object " + QString::number(o_it->id()) + ".");
    }

    emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::removeSelectedEdges(int _objectId){
  
  // get the target mesh
  TriMesh* triMesh = 0;
  
  PluginFunctions::getMesh(_objectId,triMesh);
  
  if ( triMesh ) {
    
    TriMesh::EdgeIter e_it, e_end=triMesh->edges_end();
    
    for (e_it = triMesh->edges_begin(); e_it != e_end ; ++e_it) {
      
      if ( !triMesh->status(e_it).deleted() && triMesh->status(e_it).selected() ) {
      
        const TriMesh::VHandle   v0 = triMesh->to_vertex_handle(triMesh->halfedge_handle(e_it, 0));
        const TriMesh::VHandle   v1 = triMesh->to_vertex_handle(triMesh->halfedge_handle(e_it, 1));
        
        const bool boundary0 = triMesh->is_boundary(v0);
        const bool boundary1 = triMesh->is_boundary(v1);
        
        const bool feature0 = triMesh->status(v0).feature();
        const bool feature1 = triMesh->status(v1).feature();
        const bool featureE = triMesh->status(e_it).feature();
        
        // Collapsing v1 into vo:
        // collapse is ok, if collapsed vertex is not a feature vertex or the target vertex is a feature
        // and if we collapse along an feature edge or if the collapsed vertex is not a feature
        if ((!boundary1 || boundary0) && (!feature1 || (feature0 && featureE)) && triMesh->is_collapse_ok(triMesh->halfedge_handle(e_it, 0)))
          triMesh->collapse(triMesh->halfedge_handle(e_it, 0));
        else if ((!boundary0 || boundary1) && (!feature0 || (feature1 && featureE)) && triMesh->is_collapse_ok(triMesh->halfedge_handle(e_it, 1)))
          triMesh->collapse(triMesh->halfedge_handle(e_it, 1));
        
      }
      
    }
    
    triMesh->garbage_collection();
    triMesh->update_normals();
    
    
    emit updatedObject(_objectId,UPDATE_ALL);
    emit createBackup( _objectId, "Removed selected Edges", UPDATE_ALL);
    emit scriptInfo( "removeSelectedEdges(" + QString::number(_objectId) + ")" );
    
    return;
  }
  
  emit log( LOGERR,tr("Unsupported Object Type for edge removal") );
    
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotRemoveSelectedEdges(){
  
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) 
    removeSelectedEdges(o_it->id());
  
  emit updateView();
}


//-----------------------------------------------------------------------------

void
MeshRepairPlugin::
flipOrientation(int _objectId)
{
  // get the target mesh
  TriMesh* triMesh = 0;
  
  PluginFunctions::getMesh(_objectId,triMesh);
  
  if ( triMesh ) {
    
    TriMesh::FaceIter f_it, f_end=triMesh->faces_end();
    
    std::vector< TriMesh::VertexHandle > handles;
    handles.reserve( triMesh->n_faces() * 3 );
    
    for (f_it = triMesh->faces_begin(); f_it != f_end ; ++f_it) {
      
	if (triMesh->status(f_it).selected()) {
	// Collect vertex handles
	TriMesh::FaceVertexIter fv_it = triMesh->fv_iter(f_it);
	handles.push_back( fv_it.handle() );
	++fv_it;
	handles.push_back( fv_it.handle() );
	++fv_it;
	handles.push_back( fv_it.handle() );
	
	// delete the corresponding face
	triMesh->delete_face(f_it, false);
	}
    }
    
    // clean the mesh
    triMesh->garbage_collection();
    
    // read the faces
    for (uint i=0; i < handles.size(); i += 3)
      triMesh->add_face(handles[ i + 2 ],handles[ i + 1 ],handles[ i + 0 ]);

    triMesh->update_normals();
    
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Flipped Normals", UPDATE_ALL);
    emit scriptInfo( "flipOrientation(" + QString::number(_objectId) + ")" );
    
    return;
  }
  
  // get the target mesh
  PolyMesh* polyMesh = 0;
  
  PluginFunctions::getMesh(_objectId,polyMesh);
  
  if ( polyMesh ) {
    
    PolyMesh::FaceIter f_it, f_end=polyMesh->faces_end();
    
    std::vector< PolyMesh::VertexHandle > handles;
    std::vector< uint > valence;
    // Reserve for a quad mesh, might be to small!
    handles.reserve( polyMesh->n_faces() * 4 );
    valence.reserve( polyMesh->n_faces() );
    
    
    for (f_it = polyMesh->faces_begin(); f_it != f_end ; ++f_it) {
      
      // Collect vertex handles
      PolyMesh::FaceVertexIter fv_it = polyMesh->fv_iter(f_it);
      
      valence.push_back( polyMesh->valence(f_it) );
      while( fv_it) {
        handles.push_back( fv_it.handle() );
        ++fv_it;
      }

      // delete the corresponding face
      polyMesh->delete_face(f_it, false);
    }
    
    // clean the mesh
    polyMesh->garbage_collection();
    
    // read the faces
    uint pos = 0;
    for (uint i=0; i < valence.size(); i++) {
      
      std::vector< PolyMesh::VertexHandle > faceVertices;
      
      pos += valence[i];
      
      // add valence vertices in the inverse order
      for (uint j = 1 ; j <= valence[i] ; ++j ) 
        faceVertices.push_back(handles[pos - j]);
      
      polyMesh->add_face(faceVertices);
    }
    
    polyMesh->update_normals();
    
    emit updatedObject(_objectId, UPDATE_ALL);
    emit createBackup( _objectId, "Flipped Normals", UPDATE_ALL);
    emit scriptInfo( "flipOrientation(" + QString::number(_objectId) + ")" );
    
    return;
  }

    
  emit log( LOGERR,tr("Unsupported Object Type for normal flipping!") );
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotFlipOrientation(){
  
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) 
    flipOrientation(o_it->id());
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::detectSkinnyTriangleByAngle(int _objectId, double _angle, bool _remove)
{
  // get the target mesh
  TriMesh* triMesh = 0;
  
  PluginFunctions::getMesh(_objectId,triMesh);
  
  if ( triMesh ) {
    
    // Clear current edge selection
    MeshSelection::clearEdgeSelection(triMesh);
    
    double maxAngle = cos(_angle * M_PI / 180.0);
    double angle = 0.0;
    TriMesh::EdgeIter e_it, e_end=triMesh->edges_end();
    
    for (e_it = triMesh->edges_begin(); e_it != e_end ; ++e_it) {
      
      // Check prerequisites
      if (!triMesh->status(e_it).deleted() && !triMesh->status(e_it).feature() && triMesh->is_flip_ok(e_it)) {
        
        // For both halfedges
        for (unsigned int h=0; h<2; ++h) {
          TriMesh::HalfedgeHandle hh = triMesh->halfedge_handle(e_it.handle(), h);
          const TriMesh::Point& a = triMesh->point(triMesh->from_vertex_handle(hh));
          const TriMesh::Point& b = triMesh->point(triMesh->to_vertex_handle(hh));
          hh = triMesh->next_halfedge_handle(hh);
          const TriMesh::Point& c  = triMesh->point(triMesh->to_vertex_handle(hh));
          
          angle  = ( (a-c).normalize() | (b-c).normalize() );
          
          if (angle < maxAngle) {
            
            // selcet it
            triMesh->status(e_it).set_selected(true);
            
            // remove it if requested
            if (_remove) 
              triMesh->flip(e_it);
            
          }
          
        }
        
      }
    }
    
    
    if ( _remove ) {
      emit updatedObject(_objectId, UPDATE_ALL);
      emit createBackup( _objectId, tr("Removed cap edges"), UPDATE_ALL );
    } else {
      emit updatedObject(_objectId, UPDATE_SELECTION);
      emit createBackup( _objectId, tr("Selected cap edges"), UPDATE_ALL );
    }
    emit scriptInfo( "detectSkinnyTriangleByAngle(" + QString::number(_objectId) + "," + QString::number(_angle) + "," + _remove + ")" );
    
    return;
  }
  
  emit log( LOGERR,tr("Unsupported Object Type for Cap detection!") );
  
  
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectSkinnyTriangleByAngle()
{
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) 
    detectSkinnyTriangleByAngle( o_it->id(), tool_->capAngleSpinbox->value(), false );
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotRemoveSkinnyTriangleByAngle()
{
  //rewrite!!!
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) 
    detectSkinnyTriangleByAngle( o_it->id(), tool_->capAngleSpinbox->value(), true );
  
  emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectFoldover() {

    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) {

        float angle = tool_->detectFoldoverSpinbox->value();
        unsigned int count(0);

	//is there any way to get around duplicating this code?
	if (o_it->dataType() == DATA_TRIANGLE_MESH) {

            // get the target mesh
            TriMesh* mesh = 0;
            PluginFunctions::getMesh(o_it->id(),mesh);

            if ( mesh ) {

                // Clear current edge selection
                MeshSelection::clearEdgeSelection(mesh);

                TriMesh::EdgeIter      e_it, e_end(mesh->edges_end());
                TriMesh::FaceHandle    fh;
                TriMesh::Scalar        a, cosa = cos(angle/180.0*M_PI);

                for (e_it=mesh->edges_begin(); e_it!=e_end; ++e_it)
                {
                    if (!mesh->is_boundary(e_it))
                    {
                        a = ( mesh->normal(mesh->face_handle(mesh->halfedge_handle(e_it, 0))) |
                              mesh->normal(mesh->face_handle(mesh->halfedge_handle(e_it, 1))) );

                        if (a < cosa)
                        {
                            mesh->status(mesh->edge_handle(mesh->halfedge_handle(e_it, 0))).
                            set_selected(true);
                            ++count;
                        }
                    }
                }
            }
        }
        else if (o_it->dataType() == DATA_POLY_MESH) {

            // get the target mesh
            PolyMesh* mesh = 0;
            PluginFunctions::getMesh(o_it->id(),mesh);

            if ( mesh ) {

                // Clear current edge selection
                MeshSelection::clearEdgeSelection(mesh);

                PolyMesh::EdgeIter      e_it, e_end(mesh->edges_end());
                PolyMesh::FaceHandle    fh;
                PolyMesh::Scalar        a, cosa = cos(angle/180.0*M_PI);

                for (e_it=mesh->edges_begin(); e_it!=e_end; ++e_it)
                {
                    if (!mesh->is_boundary(e_it))
                    {
                        a = ( mesh->normal(mesh->face_handle(mesh->halfedge_handle(e_it, 0))) |
                              mesh->normal(mesh->face_handle(mesh->halfedge_handle(e_it, 1))) );

                        if (a < cosa)
                        {
                            mesh->status(mesh->edge_handle(mesh->halfedge_handle(e_it, 0))).
                            set_selected(true);
                            ++count;
                        }
                    }
                }
            }
        }
        
        if (count > 0) {
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit createBackup(o_it->id(), "Select edges", UPDATE_SELECTION);
        }
        emit log ("Selected " + QString::number(count) + " fold-overs on object " + QString::number(o_it->id()) + " with angle greater than " + QString::number(angle) + ".");
    }

    emit updateView();
}

//-----------------------------------------------------------------------------

void MeshRepairPlugin::slotDetectTriangleAspect() {

    for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS,DataType( DATA_TRIANGLE_MESH | DATA_POLY_MESH ) );  o_it != PluginFunctions::objectsEnd(); ++o_it) {

	if (o_it->dataType() != DATA_TRIANGLE_MESH) {
	    emit log("Cannot detect skinny triangles on non-trimesh " + QString::number(o_it->id()) + ".");
	    continue;
	}
	
        float aspect = tool_->triangleAspectSpinbox->value();
        unsigned int count(0);

        // get the target mesh
        TriMesh* mesh = 0;
        PluginFunctions::getMesh(o_it->id(),mesh);

        if ( mesh ) {

            // Clear current face selection
            MeshSelection::clearFaceSelection(mesh);

            TriMesh::FaceIter      f_it, f_end(mesh->faces_end());
            TriMesh::FVIter        fv_it;
            TriMesh::FEIter        fe_it;


            for (f_it=mesh->faces_begin(); f_it!=f_end; ++f_it)
            {
                fv_it = mesh->fv_iter(f_it);

                const TriMesh::Point& p0 = mesh->point(fv_it);
                const TriMesh::Point& p1 = mesh->point(++fv_it);
                const TriMesh::Point& p2 = mesh->point(++fv_it);

                if (ACG::Geometry::aspectRatio(p0, p1, p2) > aspect)
                {
                    mesh->status(f_it).set_selected(true);
                    ++count;
                }
            }
        }
        if (count > 0) {
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
            emit createBackup(o_it->id(), "Select triangles", UPDATE_SELECTION);
        }
        emit log ("Selected " + QString::number(count) + " triangles on object " + QString::number(o_it->id()) + " with aspect ratio greater than " + QString::number(aspect) + ".");
    }

    emit updateView();
}

//-----------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( meshrepairplugin , MeshRepairPlugin );
