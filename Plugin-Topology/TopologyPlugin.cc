
#include <QtGui>
#include "TopologyPlugin.hh"
#include <iostream>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ACG/Geometry/Algorithms.hh>

#define EDGE_FLIP_POPUP "Flip Edge"
#define EDGE_COLLAPSE_POPUP "Collapse Edge"
#define EDGE_SPLIT_POPUP "Split Edge"
#define FACE_ADD_POPUP "Add Face"
#define FACE_SPLIT_POPUP "Split Face"
#define FACE_DELETE_POPUP "Delete Face"

#include <OpenMesh/Core/System/omstream.hh>
#include <float.h>

void TopologyPlugin::pluginsInitialized() {
   emit addHiddenPickMode(EDGE_FLIP_POPUP);
   emit addHiddenPickMode(EDGE_SPLIT_POPUP);
   emit addHiddenPickMode(EDGE_COLLAPSE_POPUP);
   emit addHiddenPickMode(FACE_ADD_POPUP);
   emit addHiddenPickMode(FACE_SPLIT_POPUP);
   emit addHiddenPickMode(FACE_DELETE_POPUP);
//    emit addPickMode("Add Face" );

  //create Topology Menu
  // Add context Menus for vertices
  trimeshMenu_  = new QMenu("Topology");
//   icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"selectVertices.png");
//   topologyMenu->setIcon(icon);
  QActionGroup* group = new QActionGroup(0);
  QAction* act;

  act = trimeshMenu_->addAction( EDGE_FLIP_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group);
  act = trimeshMenu_->addAction( EDGE_SPLIT_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group);
  act = trimeshMenu_->addAction( EDGE_COLLAPSE_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group);
  trimeshMenu_->addSeparator();
  act = trimeshMenu_->addAction( FACE_ADD_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group);
  act = trimeshMenu_->addAction( FACE_SPLIT_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group);
  act = trimeshMenu_->addAction( FACE_DELETE_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group);

  group->setExclusive(true);

  QActionGroup* group2 = new QActionGroup(0);

  polymeshMenu_ = new QMenu("Topology");
  act = polymeshMenu_->addAction( FACE_ADD_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group2);
  act = polymeshMenu_->addAction( FACE_DELETE_POPUP );
  act->setCheckable( true);
  act->setActionGroup(group2);

  group2->setExclusive(true);

  emit addContextMenu(trimeshMenu_ , DATA_TRIANGLE_MESH , CONTEXTTOPLEVELMENU );
  emit addContextMenu(polymeshMenu_ , DATA_POLY_MESH , CONTEXTTOPLEVELMENU );

  connect( trimeshMenu_,  SIGNAL( triggered(QAction*) ), this, SLOT( contextMenuTriggered(QAction*) ));
  connect( polymeshMenu_, SIGNAL( triggered(QAction*) ), this, SLOT( contextMenuTriggered(QAction*) ));

}

void TopologyPlugin::contextMenuTriggered(QAction* _action){
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

void TopologyPlugin::slotUpdateContextMenu( int /*_objectId*/ ){

  //update trimesh checkstate
  for (int i=0; i < trimeshMenu_->actions().count(); i++)
    if ( (trimeshMenu_->actions()[i])->text().toStdString() == PluginFunctions::pickMode())
      (trimeshMenu_->actions()[i])->setChecked( true );
    else
      (trimeshMenu_->actions()[i])->setChecked( false );

  //update polymesh checkstate
  for (int i=0; i < polymeshMenu_->actions().count(); i++)
    if ( (polymeshMenu_->actions()[i])->text().toStdString() == PluginFunctions::pickMode())
      (polymeshMenu_->actions()[i])->setChecked( true );
    else
      (polymeshMenu_->actions()[i])->setChecked( false );

}

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
        object->update();
      }
    }

    if ( object->dataType(DATA_POLY_MESH) ) {
      PolyMesh* m = PluginFunctions::polyMesh(object);
      PolyMesh::VertexHandle vh = m->vertex_handle( addFaceVertices_[i].second );
      if ( vh.is_valid()) {
        m->status(vh).set_selected(false);
        object->update();
      }
    }

  }

  addFaceVertices_.clear();
  emit updateView();
}

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
            float shortest_distance = (m.point(closest) - hit_point).sqrnorm();

            ++fv_it;
            if ( (m.point(fv_it.handle() ) - hit_point).sqrnorm() < shortest_distance ) {
               shortest_distance = (m.point(fv_it.handle() ) - hit_point).sqrnorm();
               closest = fv_it.handle();
            }

            ++fv_it;
            if ( (m.point(fv_it.handle() ) - hit_point).sqrnorm() < shortest_distance ) {
               shortest_distance = (m.point(fv_it.handle() ) - hit_point).sqrnorm();
               closest = fv_it.handle();
            }


            std::pair<int,int> newVertex(object->id(), closest.idx() );

            for ( uint i = 0 ; i < addFaceVertices_.size() ; ++i ) {
              if ( ( addFaceVertices_[i].first  == newVertex.first  ) &&
                   ( addFaceVertices_[i].second == newVertex.second ) ) {
                addFaceVertices_.erase(addFaceVertices_.begin()+i);
                m.status(closest).set_selected(false);
                object->update();
                emit updateView();
                return;
              }
            }

            // New Vertex so add it to the list
            addFaceVertices_.push_back( std::pair<int,int>(object->id(), closest.idx() ) );
            m.status(closest).set_selected(true);

            // We need 3 in the list to proceed
            if ( addFaceVertices_.size() < 3 ) {
              object->update();
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

            // reenable output if it was enabled
            if (errlog)
              omerr().enable();

            clearAddFaceVertices();

            if ( fh.is_valid() )
              emit createBackup(object->id(),"Add Face");
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
                  object->update();
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
              object->update();
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

            // reenable output if it was enabled
            if (errlog)
              omerr().enable();

            clearAddFaceVertices();

            if ( fh.is_valid() )
              emit createBackup(object->id(),"Add Face");
            else
              emit log(LOGERR,"Unable to add face!");

         }
      }
  }

}

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
            m.update_normals();

            object->update();
            emit updatedObject(object->id());
            emit updateView();
            emit createBackup(object->id(),"Split Face");
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
            emit log(LOGWARN,"Face Splits not supported for Poly Meshes");
         }
      } else return;
   }
}

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

         object->update();
         emit updatedObject(object->id());
         emit updateView();
         emit createBackup(object->id(),"Delete Face");
      } else return;
   }
}

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

            double min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e1 )), m.point(m.from_vertex_handle( e1 )));
            TriMesh::EdgeHandle closest_edge = m.edge_handle(e1);

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )), m.point(m.from_vertex_handle( e2 ))) < min_dist ) {
               min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )),m.point(m.from_vertex_handle( e2 )));
               closest_edge = m.edge_handle(e2);
            }

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 ))) < min_dist) {
               min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 )));
               closest_edge = m.edge_handle(e3);
            }

            if ( m.is_flip_ok(closest_edge) )
               m.flip(closest_edge);
            else
              emit log(LOGERR,"Flip is not allowed here!");

            emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

            object->update();
            emit updatedObject(object->id());
            emit updateView();
            emit createBackup(object->id(),"Edge Flip");
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
            emit log(LOGWARN,"Edge Flips not supported for Poly Meshes");
         }

      } else return;
   }
}

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

            double min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e1 )), m.point(m.from_vertex_handle( e1 )));
            TriMesh::HalfedgeHandle closest_edge = e1;

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )), m.point(m.from_vertex_handle( e2 ))) < min_dist ) {
               min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )),m.point(m.from_vertex_handle( e2 )));
               closest_edge = e2;
            }

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 ))) < min_dist) {
               min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 )));
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
            } else
              emit log(LOGERR,"Collapse is not allowed here!");

            emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

            object->update();
            emit updatedObject(object->id());
            emit updateView();
            emit createBackup(object->id(),"Edge Collapse");
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
           emit log(LOGWARN,"Edge Collapses not supported for Poly Meshes");
         }

      } else return;
   }
}

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

            double min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e1 )), m.point(m.from_vertex_handle( e1 )));
            TriMesh::EdgeHandle closest_edge = m.edge_handle(e1);

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )), m.point(m.from_vertex_handle( e2 ))) < min_dist ) {
               min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e2 )),m.point(m.from_vertex_handle( e2 )));
               closest_edge = m.edge_handle(e2);
            }

            if ( ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 ))) < min_dist) {
               min_dist = ACG::Geometry::distPointLineSquared(hit_point,m.point(m.to_vertex_handle( e3 )),m.point(m.from_vertex_handle( e3 )));
               closest_edge = m.edge_handle(e3);
            }

            m.split(closest_edge,hit_point);

            emit log(LOGOUT,"Picked Edge " + QString::number(closest_edge.idx()));

            object->update();
            emit updatedObject(object->id());
            emit updateView();
            emit createBackup(object->id(),"Edge Split");
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH)  ) {
           emit log(LOGWARN,"Edge Splits not supported for Poly Meshes");
         }

      } else return;
   }
}

Q_EXPORT_PLUGIN2( topologyplugin , TopologyPlugin );

