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
#include "TopologyPlugin.hh"
#include <iostream>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ACG/Geometry/Algorithms.hh>

#define EDGE_FLIP_POPUP "<B>Flip Edge</B><br>Rotate an edge"
#define EDGE_COLLAPSE_POPUP "<B>Collapse Edge</B><br>Collapse an edge into one of its vertices."
#define EDGE_SPLIT_POPUP "<B>Split Edge</B><br>Split an edge at the clicked point."
#define FACE_ADD_POPUP "<B>Add Face</B><br>Insert a face between clicked vertices."
#define FACE_SPLIT_POPUP "<B>Split Face</B><br>Split a face at a clicked point."
#define FACE_DELETE_POPUP "<B>Delete Face</B><br>Remove a clicked face."

#include <OpenMesh/Core/System/omstream.hh>
#include <float.h>

//******************************************************************************

TopologyPlugin::TopologyPlugin() :
        toolbar_(0),
        edgeFlipAction_(0),
        edgeSplitAction_(0),
        edgeCollapseAction_(0),
        faceAddAction_(0),
        faceDeleteAction_(0),
        faceSplitAction_(0)
{

}

//******************************************************************************

/** \brief initialize the Plugin
 *
 */
void TopologyPlugin::pluginsInitialized() {
   emit addHiddenPickMode(EDGE_FLIP_POPUP);
   emit addHiddenPickMode(EDGE_SPLIT_POPUP);
   emit addHiddenPickMode(EDGE_COLLAPSE_POPUP);
   emit addHiddenPickMode(FACE_ADD_POPUP);
   emit addHiddenPickMode(FACE_SPLIT_POPUP);
   emit addHiddenPickMode(FACE_DELETE_POPUP);


  toolbar_ = new QToolBar("Topology");

  QActionGroup* group = new QActionGroup(0);

  QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

  const QString baseHelpURL = "<a href='qthelp://org.openflipper.plugin-topology/Plugin-Topology/index.html";
  const QString clickText = tr("Click for more information</a>");


  edgeFlipAction_ = toolbar_->addAction( QIcon(iconPath + "topology-edgeFlip.png"), EDGE_FLIP_POPUP );
  edgeFlipAction_->setCheckable( true);
  edgeFlipAction_->setActionGroup(group);
  edgeFlipAction_->setWhatsThis(tr("Flip edge. ") + baseHelpURL+ "#flip_edge'>" + clickText);

  edgeSplitAction_ = toolbar_->addAction( QIcon(iconPath + "topology-edgeSplit.png"), EDGE_SPLIT_POPUP );
  edgeSplitAction_->setCheckable( true);
  edgeSplitAction_->setActionGroup(group);
  edgeSplitAction_->setWhatsThis(tr("Split edge. ") + baseHelpURL+ "#split_edge'>" + clickText);


  edgeCollapseAction_ = toolbar_->addAction( QIcon(iconPath + "topology-edgeCollapse.png"), EDGE_COLLAPSE_POPUP );
  edgeCollapseAction_->setCheckable( true);
  edgeCollapseAction_->setActionGroup(group);
  edgeCollapseAction_->setWhatsThis(tr("Collapse edge. ") + baseHelpURL+ "#collapse_edge'>" + clickText);


  toolbar_->addSeparator();
  faceAddAction_ = toolbar_->addAction( QIcon(iconPath + "topology-addFace.png"), FACE_ADD_POPUP );
  faceAddAction_->setCheckable( true);
  faceAddAction_->setActionGroup(group);
  faceAddAction_->setWhatsThis(tr("Add face.") + baseHelpURL+ "#add_face'>" + clickText);


  faceDeleteAction_ = toolbar_->addAction( QIcon(iconPath + "topology-deleteFace.png"), FACE_DELETE_POPUP );
  faceDeleteAction_->setCheckable( true);
  faceDeleteAction_->setActionGroup(group);
  faceDeleteAction_->setWhatsThis(tr("Delete face. ") + baseHelpURL+ "#delete_face'>" + clickText);


  faceSplitAction_ = toolbar_->addAction( QIcon(iconPath + "topology-splitFace.png"), FACE_SPLIT_POPUP );
  faceSplitAction_->setCheckable( true);
  faceSplitAction_->setActionGroup(group);
  faceSplitAction_->setWhatsThis(tr("Split face. ") + baseHelpURL+ "#split_face'>" + clickText);

  group->setExclusive(true);

  connect( toolbar_,  SIGNAL( actionTriggered(QAction*) ), this, SLOT( toolBarTriggered(QAction*) ));

  emit addToolbar( toolbar_ );
}


//******************************************************************************


/** \brief Toolbar action was triggered
 *
 * @param _action the action that was triggered
 */
void TopologyPlugin::toolBarTriggered(QAction* _action){
  if ( _action->text() == EDGE_FLIP_POPUP)
    PluginFunctions::pickMode(EDGE_FLIP_POPUP);
  else if ( _action->text() == EDGE_SPLIT_POPUP)
    PluginFunctions::pickMode(EDGE_SPLIT_POPUP);
  else if ( _action->text() == EDGE_COLLAPSE_POPUP)
    PluginFunctions::pickMode(EDGE_COLLAPSE_POPUP);
  else if ( _action->text() == FACE_ADD_POPUP)
    PluginFunctions::pickMode(FACE_ADD_POPUP);
  else if ( _action->text() == FACE_SPLIT_POPUP)
    PluginFunctions::pickMode(FACE_SPLIT_POPUP);
  else if ( _action->text() == FACE_DELETE_POPUP)
    PluginFunctions::pickMode(FACE_DELETE_POPUP);

  PluginFunctions::actionMode(Viewer::PickingMode);
}


//******************************************************************************

/** \brief Toggle actions when the PickMode changes
 *
 * @param _mode the new PickMode
 */
void TopologyPlugin::slotPickModeChanged( const std::string& _mode) {

  edgeFlipAction_->setChecked(     _mode == EDGE_FLIP_POPUP );
  edgeSplitAction_->setChecked(    _mode == EDGE_SPLIT_POPUP );
  edgeCollapseAction_->setChecked( _mode == EDGE_COLLAPSE_POPUP );
  faceAddAction_->setChecked(      _mode == FACE_ADD_POPUP );
  faceDeleteAction_->setChecked(   _mode == FACE_DELETE_POPUP );
  faceSplitAction_->setChecked(    _mode == FACE_SPLIT_POPUP );
}

//******************************************************************************

/** \brief this is called when a mouse event occurred
 *
 * @param _event the event that occurred
 */
void TopologyPlugin::slotMouseEvent( QMouseEvent* _event ) {
    if ( _event->buttons() == Qt::RightButton )
      return;

   if ( PluginFunctions::pickMode() == EDGE_FLIP_POPUP ) { flip_edge(_event); } else
   if ( PluginFunctions::pickMode() == EDGE_COLLAPSE_POPUP ) { collapse_edge(_event); } else
   if ( PluginFunctions::pickMode() == EDGE_SPLIT_POPUP ) { split_edge(_event); } else
   if ( PluginFunctions::pickMode() == FACE_ADD_POPUP ) { add_face(_event); } else
   if ( PluginFunctions::pickMode() == FACE_SPLIT_POPUP ) { split_face(_event); } else
   if ( PluginFunctions::pickMode() == FACE_DELETE_POPUP ) { delete_face(_event); }
}


//******************************************************************************

/** \brief Deselect the vertices from AddFace mode
 *
 */
void TopologyPlugin::clearAddFaceVertices() {
  for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject( addFaceVertices_[i].first , object ) )
      continue;

    if ( object->dataType(DATA_TRIANGLE_MESH) ) {
      TriMesh* m = PluginFunctions::triMesh(object);
      TriMesh::VertexHandle vh = m->vertex_handle( addFaceVertices_[i].second );
      if ( vh.is_valid()) {
        m->status(vh).set_selected(false);
      }
    }

    if ( object->dataType(DATA_POLY_MESH) ) {
      PolyMesh* m = PluginFunctions::polyMesh(object);
      PolyMesh::VertexHandle vh = m->vertex_handle( addFaceVertices_[i].second );
      if ( vh.is_valid()) {
        m->status(vh).set_selected(false);
      }
    }
    
    emit updatedObject(object->id(),UPDATE_SELECTION);
  }

  addFaceVertices_.clear();
  emit updateView();
}


//******************************************************************************

/** \brief Add a face
 *
 * @param _event mouse position where one of the vertices is picked
 */
void TopologyPlugin::add_face(QMouseEvent* _event) {
  if (( _event->type() != QEvent::MouseButtonPress) && (_event->type() != QEvent::MouseButtonDblClick))
    return;

  unsigned int        node_idx, target_idx;
  ACG::Vec3d       hit_point;

  if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {

      BaseObjectData* object;
      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

         //--- Add Face for TriMesh
         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH) ) {
            TriMesh& m = *PluginFunctions::triMesh(object);
            TriMesh::FaceHandle fh = m.face_handle(target_idx);

            TriMesh::FaceVertexIter fv_it(m,fh);
            TriMesh::VertexHandle closest = fv_it.handle();
            // float shortest_distance = (m.point(closest) - hit_point).sqrnorm();

            ++fv_it;
            if ( (m.point(fv_it.handle() ) - hit_point).sqrnorm() < shortest_distance ) {
              // shortest_distance = (m.point(fv_it.handle() ) - hit_point).sqrnorm();
               closest = fv_it.handle();
            }

            ++fv_it;
            if ( (m.point(fv_it.handle() ) - hit_point).sqrnorm() < shortest_distance ) {
               // shortest_distance = (m.point(fv_it.handle() ) - hit_point).sqrnorm();
               closest = fv_it.handle();
            }


            std::pair<int,int> newVertex(object->id(), closest.idx() );

            for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
              if ( ( addFaceVertices_[i].first  == newVertex.first  ) &&
                   ( addFaceVertices_[i].second == newVertex.second ) ) {
                addFaceVertices_.erase(addFaceVertices_.begin()+i);
                m.status(closest).set_selected(false);
                emit updatedObject(object->id(),UPDATE_SELECTION);
                emit updateView();
                return;
              }
            }

            // New Vertex so add it to the list
            addFaceVertices_.push_back( std::pair<int,int>(object->id(), closest.idx() ) );
            m.status(closest).set_selected(true);

            // We need 3 in the list to proceed
            if ( addFaceVertices_.size() < 3 ) {
              emit updatedObject(object->id(),UPDATE_SELECTION);
              emit updateView();
              return;
            }

            // check if the objects are of same type
            DataType dt = object->dataType();
            for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
              BaseObjectData* tmpObject;
              if ( ! PluginFunctions::getObject( addFaceVertices_[i].first , tmpObject ) ) {
                emit log(LOGERR,"Unable to get object for adding face");
                clearAddFaceVertices();
                return;
              }

              if ( tmpObject->dataType() != dt ) {
                emit log(LOGERR,"Adding faces between different type of meshes is not supported!");
                clearAddFaceVertices();
                return;
              }
            }

            // check if we add a face between multiple objects
            ///@todo: Support adding faces between objects ( and merging them into one )
            int objectId = addFaceVertices_[0].first;
            for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
              if ( addFaceVertices_[i].first != objectId ) {
                 emit log(LOGERR,"Adding faces between different objects!");
                 clearAddFaceVertices();
                 return;
              }
            }

            TriMesh::VertexHandle vh0 = m.vertex_handle( addFaceVertices_[0].second );
            TriMesh::VertexHandle vh1 = m.vertex_handle( addFaceVertices_[1].second );
            TriMesh::VertexHandle vh2 = m.vertex_handle( addFaceVertices_[2].second );

            // store state and disable output
            bool errlog = omerr().is_enabled();
            omerr().disable();

            fh = m.add_face(vh0,vh1,vh2);
            if ( !fh.is_valid() ) {
              fh = m.add_face(vh2,vh1,vh0);
            }
            
            emit updatedObject(object->id(),UPDATE_ALL);
            emit updateView();

            // reenable output if it was enabled
            if (errlog)
              omerr().enable();

            clearAddFaceVertices();

            if ( fh.is_valid() )
              emit createBackup(object->id(),"Add Face", UPDATE_TOPOLOGY);
            else
              emit log(LOGERR,"Unable to add face!");

         }

        //--- Add Face for PolyMesh
         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH) ) {

            PolyMesh& m = *PluginFunctions::polyMesh(object);
            PolyMesh::FaceHandle fh = m.face_handle(target_idx);

            //find the closest vertex in the picked face
            PolyMesh::VertexHandle closest;
            float shortest_distance = FLT_MAX;

            for (PolyMesh::FaceVertexIter fv_it(m,fh); fv_it; ++fv_it){
              float distance = (m.point( fv_it.handle() ) - hit_point).sqrnorm();

              if (distance < shortest_distance){
                shortest_distance = distance;
                closest = fv_it.handle();
              }
            }

            if (!closest.is_valid())
              return;

            if (_event->type() != QEvent::MouseButtonDblClick){

              std::pair<int,int> newVertex(object->id(), closest.idx() );

              for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
                if ( ( addFaceVertices_[i].first  == newVertex.first  ) &&
                      ( addFaceVertices_[i].second == newVertex.second ) ) {
                  addFaceVertices_.erase(addFaceVertices_.begin()+i);
                  m.status(closest).set_selected(false);
                  emit updatedObject(object->id(),UPDATE_SELECTION);
                  emit updateView();
                  return;
                }
              }

              // New Vertex so add it to the list
              addFaceVertices_.push_back( std::pair<int,int>(object->id(), closest.idx() ) );
              m.status(closest).set_selected(true);
            }

            // We need at least 3 in the list to proceed
            if ( (addFaceVertices_.size() < 3) || (_event->type() != QEvent::MouseButtonDblClick) ) {
              emit updatedObject(object->id(),UPDATE_SELECTION);
              emit updateView();
              return;
            }

            // check if the objects are of same type
            DataType dt = object->dataType();
            for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
              BaseObjectData* tmpObject;
              if ( ! PluginFunctions::getObject( addFaceVertices_[i].first , tmpObject ) ) {
                emit log(LOGERR,"Unable to get object for adding face");
                clearAddFaceVertices();
                return;
              }

              if ( tmpObject->dataType() != dt ) {
                emit log(LOGERR,"Adding faces between different type of meshes is not supported!");
                clearAddFaceVertices();
                return;
              }
            }

            // check if we add a face between multiple objects
            ///@todo: Support adding faces between objects ( and merging them into one )
            int objectId = addFaceVertices_[0].first;
            for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
              if ( addFaceVertices_[i].first != objectId ) {
                  emit log(LOGERR,"Adding faces between different objects!");
                  clearAddFaceVertices();
                  return;
              }
            }

            std::vector< PolyMesh::VertexHandle > vhs;
            for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i )
              vhs.push_back( m.vertex_handle( addFaceVertices_[i].second ) );

            // store state and disable output
            bool errlog = omerr().is_enabled();
            omerr().disable();

            fh = m.add_face(vhs);

            if (!fh.is_valid()){
              std::vector< PolyMesh::VertexHandle > rvhs;
              //reverse vector
              while (!vhs.empty()){
                rvhs.push_back( vhs.back() );
                vhs.pop_back();
              }
              
              fh = m.add_face(rvhs);
              
            }
            
            emit updatedObject(object->id(),UPDATE_ALL);
            emit updateView();

            // reenable output if it was enabled
            if (errlog)
              omerr().enable();

            clearAddFaceVertices();

            if ( fh.is_valid() )
              emit createBackup(object->id(),"Add Face", UPDATE_TOPOLOGY);
            else
              emit log(LOGERR,"Unable to add face!");

         }
      }
  }
}


//******************************************************************************

/** \brief Split a face at the given point
 *
 * @param _event mouse position where the face is picked
 */
void TopologyPlugin::split_face(QMouseEvent* _event) {
   if ( _event->type() != QEvent::MouseButtonPress )
      return;

   unsigned int        node_idx, target_idx;
   ACG::Vec3d       hit_point;

   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {
         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH) ) {
            TriMesh& m = *PluginFunctions::triMesh(object);
            TriMesh::FaceHandle fh = m.face_handle(target_idx);

            emit( log(LOGOUT,"Picked Face " + QString::number(fh.idx()) + ", normal (" +
                                              QString::number(m.normal(fh)[0]) + "," +
                                              QString::number(m.normal(fh)[1]) + "," +
                                              QString::number(m.normal(fh)[2]) + ") ") );

            TriMesh::VertexHandle vh = m.add_vertex(hit_point);
            m.split(fh,vh);
            m.garbage_collection();
            m.update_normals();

            emit updatedObject(object->id(), UPDATE_TOPOLOGY);
            emit updateView();
            emit createBackup(object->id(),"Split Face", UPDATE_TOPOLOGY);
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
           PolyMesh& m = *PluginFunctions::polyMesh(object);
           PolyMesh::FaceHandle fh = m.face_handle(target_idx);

           emit( log(LOGOUT,"Picked Face " + QString::number(fh.idx()) + ", normal (" +
               QString::number(m.normal(fh)[0]) + "," +
               QString::number(m.normal(fh)[1]) + "," +
               QString::number(m.normal(fh)[2]) + ") ") );

           PolyMesh::VertexHandle vh = m.add_vertex(hit_point);
           m.split(fh,vh);
           m.garbage_collection();
           m.update_normals();

           emit updatedObject(object->id(), UPDATE_TOPOLOGY);
           emit updateView();
           emit createBackup(object->id(),"Split Face", UPDATE_TOPOLOGY);
         }
      } else return;
   }
}


//******************************************************************************

/** \brief Delete a face at the given position
 *
 * @param _event mouse position where the face is picked
 */
void TopologyPlugin::delete_face(QMouseEvent* _event) {
   if ( _event->type() != QEvent::MouseButtonPress )
      return;

   unsigned int        node_idx, target_idx;
   ACG::Vec3d       hit_point;

   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {
         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH)  ) {
            TriMesh& m = *PluginFunctions::triMesh(object);
            TriMesh::FaceHandle fh = m.face_handle(target_idx);
            emit( log(LOGOUT,"Picked Face " + QString::number(fh.idx()) + ", normal (" +
                                   QString::number(m.normal(fh)[0]) + "," +
                                   QString::number(m.normal(fh)[1]) + "," +
                                   QString::number(m.normal(fh)[2]) + ") ") );

            m.delete_face(fh);
            m.garbage_collection();
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
            PolyMesh& m = *PluginFunctions::polyMesh(object);
            PolyMesh::FaceHandle fh = m.face_handle(target_idx);
            emit( log(LOGOUT,"Picked Face " + QString::number(fh.idx()) + ", normal (" +
                       QString::number(m.normal(fh)[0]) + "," +
                       QString::number(m.normal(fh)[1]) + "," +
                       QString::number(m.normal(fh)[2]) + ") ") );

            m.delete_face(fh);
            m.garbage_collection();
         }

         emit updatedObject(object->id(), UPDATE_TOPOLOGY);
         emit updateView();
         emit createBackup(object->id(),"Delete Face", UPDATE_TOPOLOGY);
      } else return;
   }
}


//******************************************************************************

/** \brief Flip an edge at the given position
 *
 * @param _event mouse position where the edge is picked
 */
void TopologyPlugin::flip_edge(QMouseEvent* _event) {
   if ( _event->type() != QEvent::MouseButtonPress )
      return;

   unsigned int        node_idx, target_idx;
   ACG::Vec3d       hit_point;

   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;
      if ( PluginFunctions::getPickedObject(node_idx, object) ) {
         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH)  ) {
            TriMesh& m = *PluginFunctions::triMesh(object);
            TriMesh::FaceHandle fh = m.face_handle(target_idx);

            TriMesh::FaceEdgeIter fe_it(m,fh);
            TriMesh::HalfedgeHandle e1 = m.halfedge_handle(fe_it.handle(),0);

            ++fe_it;
            TriMesh::HalfedgeHandle e2 = m.halfedge_handle(fe_it.handle(),0);

            ++fe_it;
            TriMesh::HalfedgeHandle e3 = m.halfedge_handle(fe_it.handle(),0);

            const double min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e1 )), m.point(m.from_vertex_handle( e1 )));
            TriMesh::EdgeHandle closest_edge = m.edge_handle(e1);

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )), m.point(m.from_vertex_handle( e2 ))) < min_dist ) {
               // min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )),m.point(m.from_vertex_handle( e2 )));
               closest_edge = m.edge_handle(e2);
            }

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 ))) < min_dist) {
               // min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 )));
               closest_edge = m.edge_handle(e3);
            }

            if ( m.is_flip_ok(closest_edge) )
               m.flip(closest_edge);
            else
              emit log(LOGERR,"Flip is not allowed here!");

            emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

            emit updatedObject(object->id(), UPDATE_TOPOLOGY);
            emit updateView();
            emit createBackup(object->id(),"Edge Flip", UPDATE_TOPOLOGY);
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
            emit log(LOGWARN,"Edge Flips not supported for Poly Meshes");
         }

      } else return;
   }
}


//******************************************************************************

/** \brief Collapse an edge at the given position
 *
 * @param _event mouse position where the edge is picked
 */
void TopologyPlugin::collapse_edge(QMouseEvent* _event) {
   if ( _event->type() != QEvent::MouseButtonPress )
      return;

   unsigned int        node_idx, target_idx;
   ACG::Vec3d       hit_point;

   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;
      if ( PluginFunctions::getPickedObject(node_idx, object) ) {
         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH)  ) {
            TriMesh& m = *PluginFunctions::triMesh(object);
            TriMesh::FaceHandle fh = m.face_handle(target_idx);

            TriMesh::FaceEdgeIter fe_it(m,fh);
            TriMesh::HalfedgeHandle e1 = m.halfedge_handle(fe_it.handle(),0);

            ++fe_it;
            TriMesh::HalfedgeHandle e2 = m.halfedge_handle(fe_it.handle(),0);

            ++fe_it;
            TriMesh::HalfedgeHandle e3 = m.halfedge_handle(fe_it.handle(),0);

            const double min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e1 )), m.point(m.from_vertex_handle( e1 )));
            TriMesh::HalfedgeHandle closest_edge = e1;

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )), m.point(m.from_vertex_handle( e2 ))) < min_dist ) {
               // min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )),m.point(m.from_vertex_handle( e2 )));
               closest_edge = e2;
            }

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 ))) < min_dist) {
               // min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 )));
               closest_edge = e3;
            }

            // collapse into to point which is closer to hitpoint
            TriMesh::Point to = m.point( m.to_vertex_handle(closest_edge) );
            TriMesh::Point from = m.point( m.from_vertex_handle(closest_edge) );

            if ( (hit_point - to).sqrnorm() > (hit_point - from).sqrnorm() )
               closest_edge = m.opposite_halfedge_handle(closest_edge);

            if ( m.is_collapse_ok(closest_edge) ){
   //             m.point(m.to_vertex_handle(closest_edge)) = hit_point;
               m.collapse(closest_edge );
               m.garbage_collection();
            } else
              emit log(LOGERR,"Collapse is not allowed here!");

            emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

            emit updatedObject(object->id(), UPDATE_TOPOLOGY);
            emit updateView();
            emit createBackup(object->id(),"Edge Collapse", UPDATE_TOPOLOGY);
         }

         // Poly Meshes
         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
           PolyMesh& m = *PluginFunctions::polyMesh(object);
           PolyMesh::FaceHandle fh = m.face_handle(target_idx);

           // find closest edge
           PolyMesh::HalfedgeHandle closest_edge(-1);
           double min_dist = FLT_MAX;

           PolyMesh::FaceEdgeIter fe_it(m,fh);
           for(; fe_it; ++fe_it)
           {
             PolyMesh::HalfedgeHandle heh = m.halfedge_handle(fe_it.handle(),0);
             double dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( heh )), m.point(m.from_vertex_handle( heh )));

             if( dist < min_dist)
             {
               min_dist = dist;
               closest_edge = heh;
             }
           }

           // collapse into to point which is closer to hitpoint
           PolyMesh::Point to = m.point( m.to_vertex_handle(closest_edge) );
           PolyMesh::Point from = m.point( m.from_vertex_handle(closest_edge) );

           if ( (hit_point - to).sqrnorm() > (hit_point - from).sqrnorm() )
              closest_edge = m.opposite_halfedge_handle(closest_edge);

//           if ( m.is_collapse_ok(closest_edge) ){
  //             m.point(m.to_vertex_handle(closest_edge)) = hit_point;
              m.collapse(closest_edge );
              m.garbage_collection();
//           } else
//             emit log(LOGERR,"Collapse is not allowed here!");

           emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

           emit updatedObject(object->id(), UPDATE_TOPOLOGY);
           emit updateView();
           emit createBackup(object->id(),"Edge Collapse", UPDATE_TOPOLOGY);
         }

      } else return;
   }
}


//******************************************************************************

/** \brief Split an edge at the given position
 *
 * @param _event mouse position where the edge is picked
 */
void TopologyPlugin::split_edge(QMouseEvent* _event) {
   if ( _event->type() != QEvent::MouseButtonPress )
      return;

   unsigned int        node_idx, target_idx;
   ACG::Vec3d       hit_point;

   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;
      if ( PluginFunctions::getPickedObject(node_idx, object) ) {
         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH)  ) {
            TriMesh& m = *PluginFunctions::triMesh(object);
            TriMesh::FaceHandle fh = m.face_handle(target_idx);

            TriMesh::FaceEdgeIter fe_it(m,fh);
            TriMesh::HalfedgeHandle e1 = m.halfedge_handle(fe_it.handle(),0);

            ++fe_it;
            TriMesh::HalfedgeHandle e2 = m.halfedge_handle(fe_it.handle(),0);

            ++fe_it;
            TriMesh::HalfedgeHandle e3 = m.halfedge_handle(fe_it.handle(),0);

            const double min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e1 )), m.point(m.from_vertex_handle( e1 )));
            TriMesh::EdgeHandle closest_edge = m.edge_handle(e1);

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )), m.point(m.from_vertex_handle( e2 ))) < min_dist ) {
               // min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )),m.point(m.from_vertex_handle( e2 )));
               closest_edge = m.edge_handle(e2);
            }

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 ))) < min_dist) {
               // min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 )));
               closest_edge = m.edge_handle(e3);
            }

            m.split(closest_edge,hit_point);

            emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

            emit updatedObject(object->id(), UPDATE_TOPOLOGY);
            emit updateView();
            emit createBackup(object->id(),"Edge Split", UPDATE_TOPOLOGY);
         }
         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
        	 PolyMesh& m = *PluginFunctions::polyMesh(object);
        	 PolyMesh::FaceHandle fh = m.face_handle(target_idx);

        	 PolyMesh::FaceEdgeIter fe_it(m,fh);

        	 std::vector<PolyMesh::HalfedgeHandle> halfEdgeHandles;
        	 //get all edges which belongs to the picked face
        	 for (;fe_it; ++fe_it)
        	 {
        		 halfEdgeHandles.push_back(fe_it.current_halfedge_handle());
        	 }

        	 //search for the nearest edge
        	 PolyMesh::EdgeHandle closest_edge = m.edge_handle(*halfEdgeHandles.begin());
        	 double min_dist = ACG::Geometry::distPointLineSquared(hit_point, m.point(m.to_vertex_handle( *halfEdgeHandles.begin() )), m.point(m.from_vertex_handle( *halfEdgeHandles.begin() )));

        	 for (std::vector<PolyMesh::HalfedgeHandle>::iterator iter = halfEdgeHandles.begin(); iter != halfEdgeHandles.end(); ++iter)
        	 {
        		 double dist = ACG::Geometry::distPointLineSquared(hit_point, m.point(m.to_vertex_handle( *iter )), m.point(m.from_vertex_handle( *iter )));
        		 if (dist < min_dist)
        		 {
        			 closest_edge = m.edge_handle(*iter);
        			 min_dist = dist;
        		 }
        	 }

        	 m.split(closest_edge,hit_point);

        	 emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

        	 emit updatedObject(object->id(), UPDATE_TOPOLOGY);
        	 emit updateView();
        	 emit createBackup(object->id(),"Edge Split", UPDATE_TOPOLOGY);
         }

      } else return;
   }
}

Q_EXPORT_PLUGIN2( topologyplugin , TopologyPlugin );

