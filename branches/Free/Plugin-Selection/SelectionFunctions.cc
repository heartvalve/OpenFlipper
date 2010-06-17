/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#include "SelectionPlugin.hh"

//***********************************************************************************

/** \brief Toggle the selection state
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::toggleSelection(QMouseEvent* _event)
{

  if ( _event->type()   != QEvent::MouseButtonPress ) return;
  if ( _event->button() == Qt::RightButton ) return;

   unsigned int node_idx, target_idx;
   ACG::Vec3d   hit_point;


   // pick Anything to find all possible objects
   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
          _event->pos(),node_idx, target_idx, &hit_point))
   {

    BaseObjectData* object(0);
    if(PluginFunctions::getPickedObject(node_idx, object))
    {
      // OBJECT SELECTION
      if (selectionType_ & OBJECT){
        if (sourceSelection_){
          object->source( !object->source() );
        } else {
          object->target( !object->target() );
        }
      }

      // TRIANGLE MESHES
      if (object->dataType() == DATA_TRIANGLE_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

                if ( object->dataType(DATA_TRIANGLE_MESH) ){
                  toggleMeshSelection(PluginFunctions::triMesh(object), target_idx, hit_point);
                  emit updatedObject(object->id(), UPDATE_SELECTION);
                }

      // POLY MESHES
      }else if (object->dataType() == DATA_POLY_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

              if ( object->dataType(DATA_POLY_MESH) ) {
                toggleMeshSelection(PluginFunctions::polyMesh(object), target_idx, hit_point);
                emit updatedObject(object->id(), UPDATE_SELECTION);;
              }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
      }else if (object->dataType() == DATA_TSPLINE_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

              if ( object->dataType(DATA_TSPLINE_MESH) ) {
                toggleMeshSelection(PluginFunctions::tsplineMesh(object), target_idx, hit_point);
                emit updatedObject(object->id(), UPDATE_SELECTION);
              }
#endif
      }
      #ifdef ENABLE_POLYLINE_SUPPORT
      // POLYLINES
      else if (object->dataType() == DATA_POLY_LINE){
            togglePolyLineSelection(_event);
            emit updatedObject(object->id(), UPDATE_SELECTION);
      }
      #endif
      #ifdef ENABLE_BSPLINECURVE_SUPPORT
      // BSPLINECURVE
      else if (object->dataType() == DATA_BSPLINE_CURVE){
            toggleBSplineCurveSelection(_event);
            emit updatedObject(object->id(), UPDATE_SELECTION);
      }
      #endif
      #ifdef ENABLE_BSPLINESURFACE_SUPPORT
      // BSPLINESURFACE
      else if (object->dataType() == DATA_BSPLINE_SURFACE){
            toggleBSplineSurfaceSelection(_event);
            emit updatedObject(object->id(), UPDATE_SELECTION);
      }
      #endif
      #ifdef ENABLE_SKELETON_SUPPORT
      // SKELETON
      else if (object->dataType() == DATA_SKELETON){
            toggleSkeletonSelection(_event);
            emit updatedObject(object->id(), UPDATE_SELECTION);
      }
      #endif
      else{
        emit log(LOGERR,tr("toggleSelection : Unsupported dataType"));
      }
    }
  }
}

//***********************************************************************************

/** \brief Handle Mouse move event for sphere painting selection
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::paintSphereSelection(QMouseEvent* _event) {
   unsigned int        node_idx, target_idx;
   ACG::Vec3d       hit_point;

  if ( _event->button() == Qt::RightButton ){
    //do not bother the context menu
    sphere_node_->hide();
    return;
  }

   switch ( _event->type() ) {
      case QEvent::MouseButtonRelease :
         if ( sphere_selection_ ) {
            sphere_selection_ = false;
            for ( uint i = 0 ; i < selection_changes_.size() ; ++ i ) {
                emit createBackup(selection_changes_[i],tr("Sphere Paint Selection"));
            }
            selection_changes_.clear();
         }
         break;
      case QEvent::MouseButtonPress :
         /// Hide sphere, otherwise it might get picked
         sphere_node_->hide();

         if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING, _event->pos(),node_idx, target_idx, &hit_point)) {
               BaseObjectData* object;

               if ( PluginFunctions::getPickedObject(node_idx, object) ) {
                  sphere_node_->show();
                  sphere_node_->set_position(hit_point);
                  sphere_node_->set_size(sphere_radius_);
                  selection_changes_.push_back(object->id());
                  sphere_selection_ = true;
               }
         }
         /*break;*/ // dont break here //ok :)
      case QEvent::MouseMove :
         /// Hide sphere, otherwise it might get picked
         sphere_node_->hide();

         if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
               BaseObjectData* object;

               if ( PluginFunctions::getPickedObject(node_idx, object) ) {

                  // update brush sphere
                  sphere_node_->show();
                  sphere_node_->set_position(hit_point);
                  sphere_node_->set_size(sphere_radius_);

                  if ( _event->buttons() != Qt::NoButton ){

                    if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH) ) {
                      paintSphereSelection(PluginFunctions::triMesh(object), target_idx, hit_point);
                      emit updatedObject(object->id(), UPDATE_SELECTION);
                    }

                    if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH) ) {
                      paintSphereSelection(PluginFunctions::polyMesh(object), target_idx, hit_point);
                      emit updatedObject(object->id(), UPDATE_SELECTION);
                    }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
                    if ( object->picked(node_idx) && object->dataType(DATA_TSPLINE_MESH) ) {
                      paintSphereSelection(PluginFunctions::tsplineMesh(object), target_idx, hit_point);
                      emit updatedObject(object->id(), UPDATE_SELECTION);
                    }
#endif
                    
                  }

                  bool found = false;
                  for ( uint i = 0 ; i < selection_changes_.size() ; ++ i )
                     if (selection_changes_[i] == object->id()) {
                        found = true;
                        break;
                     }
                  if ( ! found)
                     selection_changes_.push_back(object->id());
            }
         }
         break;
      default:
         break;
   }

   emit updateView();
}

//***********************************************************************************

/** \brief select the closest boundary to the picked point
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::closestBoundarySelection(QMouseEvent* _event){
  if (_event->type() == QEvent::MouseButtonPress){

    unsigned int node_idx , target_idx;
    ACG::Vec3d   hit_point;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos() ,node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

        if ( object->dataType(DATA_TRIANGLE_MESH) ) {

          TriMesh* m = PluginFunctions::triMesh(object);
          TriMesh::VertexHandle vh = m->fv_iter(m->face_handle(target_idx)).handle();

          closestBoundarySelection(m, vh.idx(), selectionType_);

          emit updatedObject(object->id(), UPDATE_SELECTION);

        } else if ( object->dataType(DATA_POLY_MESH) ) {

          PolyMesh* m = PluginFunctions::polyMesh(object);
          PolyMesh::VertexHandle vh = m->fv_iter(m->face_handle(target_idx)).handle();

          closestBoundarySelection(m, vh.idx(), selectionType_ );

          emit updatedObject(object->id(), UPDATE_SELECTION);

#ifdef ENABLE_TSPLINEMESH_SUPPORT
        } else if ( object->dataType(DATA_TSPLINE_MESH) ) {

          TSplineMesh* m = PluginFunctions::tsplineMesh(object);
          TSplineMesh::VertexHandle vh = m->fv_iter(m->face_handle(target_idx)).handle();

          closestBoundarySelection(m, vh.idx(), selectionType_ );

          emit updatedObject(object->id(), UPDATE_SELECTION);

#endif
        }
        emit updateView();
      }
    }
  }
}

//***********************************************************************************

/** \brief select all connected components starting from the picked point
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::componentSelection(QMouseEvent* _event) {
   if ( _event->type() != QEvent::MouseButtonPress )
      return;

   unsigned int        node_idx, target_idx;
   ACG::Vec3d       hit_point;

   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {
         if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH) ){

            componentSelection(PluginFunctions::triMesh(object), target_idx);
         }

         if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH) ) {

            componentSelection(PluginFunctions::polyMesh(object), target_idx);
         }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
         if ( object->picked(node_idx) && object->dataType(DATA_TSPLINE_MESH) ) {

            componentSelection(PluginFunctions::tsplineMesh(object), target_idx);
         }
#endif

         emit updatedObject(object->id(), UPDATE_SELECTION);
         emit createBackup(object->id(),tr("Component Selection"));
      }
   }

   emit updateView();

}

//***********************************************************************************

/** \brief FloodFill an area starting from the selected element
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::floodFillSelection(QMouseEvent* _event)
{

  if ( _event->type()   != QEvent::MouseButtonPress ) return;
  if ( _event->button() == Qt::RightButton ) return;

   unsigned int node_idx, target_idx;
   ACG::Vec3d   hit_point;


   // pick Anything to find all possible objects
   if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
          _event->pos(),node_idx, target_idx, &hit_point))
   {

    BaseObjectData* object(0);
    if(PluginFunctions::getPickedObject(node_idx, object))
    {
      // OBJECT SELECTION
      if (selectionType_ & OBJECT)
        return;

      maxFloodFillAngle_ = tool_->maxFloodFillAngle->value();

      // TRIANGLE MESHES
      if (object->dataType() == DATA_TRIANGLE_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

                if ( object->dataType(DATA_TRIANGLE_MESH) ){
                  floodFillSelection(PluginFunctions::triMesh(object), target_idx);
                  emit updatedObject(object->id(), UPDATE_SELECTION);
                }

      // POLY MESHES
      }else if (object->dataType() == DATA_POLY_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

              if ( object->dataType(DATA_POLY_MESH) ) {
                floodFillSelection(PluginFunctions::polyMesh(object), target_idx);
                emit updatedObject(object->id(), UPDATE_SELECTION);
              }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
      }else if (object->dataType() == DATA_TSPLINE_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

              if ( object->dataType(DATA_TSPLINE_MESH) ) {
                floodFillSelection(PluginFunctions::tsplineMesh(object), target_idx);
                emit updatedObject(object->id(), UPDATE_SELECTION);
              }
#endif
      }
      else{
        emit log(LOGERR,tr("floodFillSelection : Unsupported dataType"));
      }
    }
  }
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_POLYLINE_SUPPORT

/** \brief surface lasso selection
 *
 * this function calls the actual selection function after the polyLine was drawn
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::surfaceLassoSelection(QMouseEvent* _event){
  if (_event->type() == QEvent::MouseButtonPress){

    unsigned int node_idx , target_idx;
    ACG::Vec3d   hit_point;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos() ,node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

        if ( polyLineID_ == -1){
          emit log("Polyline -1");

		      //switch back to PolyLine drawing
		      PluginFunctions::actionMode( Viewer::PickingMode );
		      waitingForPolyLineSelection_ = true;
          PluginFunctions::pickMode("PolyLine");

          return;
        }
        //is object a polyLine?
        BaseObjectData *obj;
        PluginFunctions::getObject(polyLineID_, obj);

        // get polyline object
        PolyLineObject* pline  = PluginFunctions::polyLineObject(obj);


        if ( object->dataType(DATA_TRIANGLE_MESH) ) {
          TriMesh* m = PluginFunctions::triMesh(object);
          TriMesh::FaceHandle fh = m->face_handle(target_idx);

          // get pointer to triangle bsp
          OpenMeshTriangleBSPT< TriMesh >* tbsp = PluginFunctions::triMeshObject(object)->requestTriangleBsp();

          surfaceLassoSelection(m, pline->line(), fh, tbsp );

          emit deleteObject(polyLineID_);

          emit updatedObject(object->id(), UPDATE_SELECTION);

          emit updateView();

          polyLineID_ = -1;

		      //switch back to PolyLine drawing
		      PluginFunctions::actionMode( Viewer::PickingMode );
          waitingForPolyLineSelection_ = true;
          PluginFunctions::pickMode("PolyLine");

          PluginFunctions::actionMode( Viewer::ExamineMode );

        } else if ( object->dataType(DATA_POLY_MESH) ) {
          emit log(LOGERR,tr("PolyLine on quad meshes not fully supported!! "));
          polyLineID_ = -1;

		      //switch back to PolyLine drawing
          PluginFunctions::actionMode( Viewer::PickingMode );
		      waitingForPolyLineSelection_ = true;
          PluginFunctions::pickMode("PolyLine");

          return;
#ifdef ENABLE_TSPLINEMESH_SUPPORT
        } else if ( object->dataType(DATA_TSPLINE_MESH) ) {
          emit log(LOGERR,tr("PolyLine on tspline meshes not fully supported!! "));
          polyLineID_ = -1;

		      //switch back to PolyLine drawing
          PluginFunctions::actionMode( Viewer::PickingMode );
		      waitingForPolyLineSelection_ = true;
          PluginFunctions::pickMode("PolyLine");

          return;
#endif
        }

      }
    }
  }
}


#endif


/** \brief Handle Mouse move event for sphere painting selection
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::handleLassoSelection(QMouseEvent* _event, bool _volume) {
   unsigned int        node_idx, target_idx;
   ACG::Vec3d          hit_point;
   int                 y = PluginFunctions::viewerProperties().glState().context_height() - _event->pos().y();

  if ( _event->button() == Qt::RightButton ){
    //do not bother the context menu
    line_node_->hide();
    return;
  }

   switch ( _event->type() ) {
      case QEvent::MouseButtonDblClick :
        if (lasso_selection_) {
          line_node_->add_point(lasso_points_[0]);

          line_node_->hide();

          lasso_2Dpoints_ << lasso_2Dpoints_[0];

          QRegion region (lasso_2Dpoints_);
          QList <QPair<unsigned int, unsigned int> >list;
          QSet <unsigned int> objects;

          if (!_volume)
          {
            if (selectionType_ & (FACE | OBJECT) && PluginFunctions::scenegraphRegionPick(ACG::SceneGraph::PICK_FACE, region, list))
            {
              for (QList<QPair<unsigned int, unsigned int> >::iterator it = list.begin();
                   it != list.end(); ++it)
              {
                objects << (*it).first;
              }
              if (selectionType_ & FACE)
                forEachObject (list, FACE);
            }
            if (selectionType_ & VERTEX && PluginFunctions::scenegraphRegionPick(ACG::SceneGraph::PICK_FRONT_VERTEX, region, list))
            {
              forEachObject (list, VERTEX);
            }
            if (selectionType_ & EDGE && PluginFunctions::scenegraphRegionPick(ACG::SceneGraph::PICK_FRONT_EDGE, region, list))
            {
              forEachObject (list, EDGE);
            }

            // Object selection
            if (selectionType_ & OBJECT) {
              BaseObjectData* object(0);
              foreach (unsigned int i, objects)
                if(PluginFunctions::getPickedObject(i, object))
                {
                  if (sourceSelection_){
                    object->source( !deselection_ );
                  } else {
                    object->target( !deselection_ );
                  }
                }
            }
          }
          else
          {
            ACG::GLState &state = PluginFunctions::viewerProperties().glState();
            bool         updateGL = state.updateGL ();
            state.set_updateGL (false);

            SelectVolumeAction action(&region, this, state);
            ACG::SceneGraph::traverse (PluginFunctions::getRootNode(), action );

            state.set_updateGL (updateGL);
          }

          lasso_selection_ = false;
        }
        break;
      case QEvent::MouseButtonPress :
        if (!lasso_selection_)
        {
          line_node_->show();
          line_node_->clear();
          lasso_points_.clear();
          lasso_2Dpoints_.clear();
          lasso_selection_ = true;
        }

        if (deselection_)
          line_node_->set_color(ACG::Vec4f(1.0, 0.0, 0.0, 1.0));
        else
          line_node_->set_color(ACG::Vec4f(0.0, 1.0, 0.0, 1.0));

        if (!PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))
          hit_point = PluginFunctions::viewerProperties().glState().unproject(ACG::Vec3d(_event->pos().x(),y,0.5));

        lasso_points_.push_back (hit_point);
        line_node_->clear();
        for (std::vector< ACG::Vec3d >::iterator it = lasso_points_.begin(); it != lasso_points_.end(); ++it)
          line_node_->add_point(*it);
        line_node_->add_point(lasso_points_[0]);

        lasso_2Dpoints_ << _event->pos();

        break;
      case QEvent::MouseMove :
        if (lasso_selection_)
        {
          if (deselection_)
            line_node_->set_color(ACG::Vec4f(1.0, 0.0, 0.0, 1.0));
          else
            line_node_->set_color(ACG::Vec4f(0.0, 1.0, 0.0, 1.0));

          if (!PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))
            hit_point = PluginFunctions::viewerProperties().glState().unproject(ACG::Vec3d(_event->pos().x(),y,0.5));

          if (!_event->buttons() && lasso_points_.size () > 1)
          {
            lasso_points_.pop_back ();
            lasso_2Dpoints_.pop_back ();
          }

          lasso_points_.push_back (hit_point);
          line_node_->clear();
          for (std::vector< ACG::Vec3d >::iterator it = lasso_points_.begin(); it != lasso_points_.end(); ++it)
            line_node_->add_point(*it);
          line_node_->add_point(lasso_points_[0]);

          lasso_2Dpoints_ << _event->pos();
        }
        break;
      default:
         break;
   }

   emit updateView();
}

/** \brief Execute mesh element selection for each object
 *
 * @param _list object/element pair list
 * @param _type selection type
 */
void SelectionPlugin::forEachObject(QList<QPair<unsigned int, unsigned int> > &_list, SelectionPrimitive _type)
{
  QSet <unsigned int> objects;
  IdList              elements;
  BaseObjectData*     object(0);

  for (QList<QPair<unsigned int, unsigned int> >::iterator it = _list.begin();
       it != _list.end(); ++it)
  {
    objects << (*it).first;
  }

  foreach (unsigned int obj, objects)
  {
    elements.clear();
    for (QList<QPair<unsigned int, unsigned int> >::iterator it = _list.begin();
       it != _list.end(); ++it)
    {
      if ((*it).first == obj)
        elements.push_back((*it).second);
    }

    if(PluginFunctions::getPickedObject(obj, object))
    {
      switch (_type)
      {
        case VERTEX:
          if (deselection_)
            unselectVertices (object->id (), elements);
          else
            selectVertices (object->id (), elements);
          break;
        case FACE:
          if (deselection_)
            unselectFaces (object->id (), elements);
          else
            selectFaces (object->id (), elements);
          break;
        case EDGE:
          if (deselection_)
            unselectEdges (object->id (), elements);
          else
            selectEdges (object->id (), elements);
          break;
        default:
          break;
      }
    }
  }
}

/// Create a mesh containing the selection of the given mesh
int SelectionPlugin::createMeshFromSelection( int _objectId ){
        
    // get object
    BaseObjectData *obj = 0;
    PluginFunctions::getObject(_objectId, obj);
    
    if (obj == 0){
        emit log(LOGERR, tr("Unable to get object"));
        return -1;
    }
    
    if ( obj->dataType(DATA_TRIANGLE_MESH) ) {
        TriMesh* mesh = PluginFunctions::triMesh(obj);
        
        if ( mesh == 0 ) {
            emit log(LOGERR, tr("Unable to get mesh"));
            return -1;
        }
        
        //add an empty mesh
        int id = -1;
        emit addEmptyObject(DATA_TRIANGLE_MESH, id);
        
        if (id == -1){
            emit log(LOGERR, tr("Unable to add empty object"));
            return -1;
        }
        
        BaseObjectData *newObj;
        PluginFunctions::getObject(id, newObj);
        
        TriMesh* newMesh = PluginFunctions::triMesh(newObj);
        
        if ( newMesh == 0 ) {
            emit log(LOGERR, tr("Unable to get mesh"));
            return -1;
        }
        
        //fill the empty mesh with the selection
        createMeshFromSelection( *mesh, *newMesh);
        
        
        emit updatedObject(id, UPDATE_ALL);
        
        return id;
        
    } else if( obj->dataType(DATA_POLY_MESH) ) {
        PolyMesh* mesh = PluginFunctions::polyMesh(obj);
        
        if ( mesh == 0 ) {
            emit log(LOGERR, tr("Unable to get mesh"));
            return -1;
        }
        
        //add an empty mesh
        int id;
        emit addEmptyObject(DATA_POLY_MESH, id);
        
        if (id == -1){
            emit log(LOGERR, tr("Unable to add empty object"));
            return -1;
        }
        
        BaseObjectData *newObj;
        PluginFunctions::getObject(id, newObj);
        
        PolyMesh* newMesh = PluginFunctions::polyMesh(newObj);
        
        if ( newMesh == 0 ) {
            emit log(LOGERR, tr("Unable to get mesh"));
            return -1;
        }
        
        //fill the empty mesh with the selection
        createMeshFromSelection( *mesh, *newMesh);
        
        emit updatedObject(id, UPDATE_ALL);
        
        return id;
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    } else if( obj->dataType(DATA_TSPLINE_MESH) ) {
        TSplineMesh* mesh = PluginFunctions::tsplineMesh(obj);
        
        if ( mesh == 0 ) {
            emit log(LOGERR, tr("Unable to get mesh"));
            return -1;
        }
        
        //add an empty mesh
        int id;
        emit addEmptyObject(DATA_TSPLINE_MESH, id);
        
        if (id == -1){
            emit log(LOGERR, tr("Unable to add empty object"));
            return -1;
        }
        
        BaseObjectData *newObj;
        PluginFunctions::getObject(id, newObj);
        
        TSplineMesh* newMesh = PluginFunctions::tsplineMesh(newObj);
        
        if ( newMesh == 0 ) {
            emit log(LOGERR, tr("Unable to get mesh"));
            return -1;
        }
        
        //fill the empty mesh with the selection
        createMeshFromSelection( *mesh, *newMesh);
        
        emit updatedObject(id, UPDATE_ALL);
        
        return id;
#endif
    }else {
        emit log(LOGERR, tr("DataType not supported"));
        return -1;
    }
    
}


/// Traverse the scenegraph and call the selection function for nodes
bool SelectVolumeAction::operator()(BaseNode* _node)
{
  BaseObjectData *object = 0;
  if (PluginFunctions::getPickedObject(_node->id (), object))
  {
    bool selected = false;
    if ( object->dataType(DATA_TRIANGLE_MESH) ) {

      TriMesh* m = PluginFunctions::triMesh(object);
      selected = plugin_->volumeSelection (m, state_, region_);

    } else if ( object->dataType(DATA_POLY_MESH) ) {

      PolyMesh* m = PluginFunctions::polyMesh(object);
      selected = plugin_->volumeSelection (m, state_, region_);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    } else if ( object->dataType(DATA_TSPLINE_MESH) ) {

      TSplineMesh* m = PluginFunctions::tsplineMesh(object);
      selected = plugin_->volumeSelection (m, state_, region_);
#endif
    }

    if (selected && plugin_->selectionType_ & OBJECT) {

      if (plugin_->sourceSelection_){
        object->source( !plugin_->deselection_ );
      } else {
        object->target( !plugin_->deselection_ );
      }

    } else if (selected)
      emit plugin_->updatedObject( object->id(), UPDATE_SELECTION );
  }
  return true;
}





