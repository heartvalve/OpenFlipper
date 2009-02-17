

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
          emit updatedObject(object->id());

        } else {
          object->target( !object->target() );

          emit activeObjectChanged();
          emit updatedObject(object->id());
        }
      }

      // TRIANGLE MESHES
      if (object->dataType() == DATA_TRIANGLE_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

                if ( object->dataType(DATA_TRIANGLE_MESH) ){
                  toggleMeshSelection(PluginFunctions::triMesh(object), target_idx, hit_point);
                  emit updatedObject(object->id());
                }

      // POLY MESHES
      }else if (object->dataType() == DATA_POLY_MESH) {

            if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point))

              if ( PluginFunctions::getPickedObject(node_idx, object) )

              if ( object->dataType(DATA_POLY_MESH) ) {
                toggleMeshSelection(PluginFunctions::polyMesh(object), target_idx, hit_point);
                emit updatedObject(object->id());
              }
      }
      #ifdef ENABLE_POLYLINE_SUPPORT
      // POLYLINES
      else if (object->dataType() == DATA_POLY_LINE){
            togglePolyLineSelection(_event);
            emit updatedObject(object->id());
      }
      #endif
      #ifdef ENABLE_BSPLINECURVE_SUPPORT
      // BSPLINECURVE
      else if (object->dataType() == DATA_BSPLINE_CURVE){
            toggleBSplineCurveSelection(_event);
            emit updatedObject(object->id());
      }
      #endif
      else{
        emit log(LOGERR,"toggleSelection : Unsupported dataType");
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
                emit createBackup(selection_changes_[i],"Sphere Paint Selection");
            }
            selection_changes_.clear();
         }
         break;
      case QEvent::MouseButtonPress :
         /// Hide sphere, otherwise it might get picked
         sphere_node_->hide();

         if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, target_idx, &hit_point)) {
               BaseObjectData* object;

               if ( PluginFunctions::getPickedObject(node_idx, object) ) {

                  sphere_node_->show();
                  sphere_node_->set_position((ACG::Vec3f)hit_point);
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
                  sphere_node_->set_position((ACG::Vec3f)hit_point);
                  sphere_node_->set_size(sphere_radius_);

                  if ( _event->buttons() != Qt::NoButton ){

                    if ( object->picked(node_idx) && object->dataType(DATA_TRIANGLE_MESH) ) {
                      paintSphereSelection(PluginFunctions::triMesh(object), target_idx, hit_point);
                    }

                    if ( object->picked(node_idx) && object->dataType(DATA_POLY_MESH) ) {
                      paintSphereSelection(PluginFunctions::polyMesh(object), target_idx, hit_point);
                    }

                    object->update();

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

          emit updatedObject(-1);
          PluginFunctions::triMeshObject(object)->updateSelection();

        } else if ( object->dataType(DATA_POLY_MESH) ) {

          PolyMesh* m = PluginFunctions::polyMesh(object);
          PolyMesh::VertexHandle vh = m->fv_iter(m->face_handle(target_idx)).handle();

          closestBoundarySelection(m, vh.idx(), selectionType_ );

          emit updatedObject(-1);
          PluginFunctions::polyMeshObject(object)->updateSelection();

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

         object->update();
         emit createBackup(object->id(),"Component Selection");
      }
   }

   emit updateView();

}


//***********************************************************************************

/** \brief Slots calls the actual selection function after QtLasso finished selecting
 *
 * @param _event mouse event that occurred
 */
void SelectionPlugin::slotLassoSelection(ACG::QtLasso::SelectionMode /*_mode*/) {
   PluginFunctions::IteratorRestriction targets;

   if ( tool_->selectOnAll->isChecked() || (selectionType_ & OBJECT) )
      targets = PluginFunctions::ALL_OBJECTS;
   else
      targets = PluginFunctions::TARGET_OBJECTS;

   for ( PluginFunctions::ObjectIterator o_it(targets) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
     if ( o_it->visible() ){
        if ( o_it->dataType( DATA_TRIANGLE_MESH ) ) {
          bool sel = lassoSelection(*(PluginFunctions::triMesh(*o_it)));

          if ( (selectionType_ & OBJECT) && sel ){
            if (sourceSelection_){

              o_it->source( !deselection_ );
              emit updatedObject(o_it->id());

            } else {
              o_it->target( !deselection_ );

              emit activeObjectChanged();
              emit updatedObject(o_it->id());
            }
          }
        }

        if ( o_it->dataType( DATA_POLY_MESH ) ) {
          bool sel = lassoSelection(*(PluginFunctions::polyMesh(*o_it)));

          if ( (selectionType_ & OBJECT) && sel ){
            if (sourceSelection_){

              o_it->source( !deselection_ );
              emit updatedObject(o_it->id());

            } else {
              o_it->target( !deselection_ );

              emit activeObjectChanged();
              emit updatedObject(o_it->id());
            }
          }
        }

        o_it->update();
        emit createBackup(o_it->id(),"Lasso Selection");
     }
   }

   lasso_->reset();
   emit updateView();
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

          PluginFunctions::deleteObject(polyLineID_);

          emit updatedObject(-1);

          PluginFunctions::triMeshObject(object)->updateSelection();
          emit updateView();

          polyLineID_ = -1;

		      //switch back to PolyLine drawing
		      PluginFunctions::actionMode( Viewer::PickingMode );
          waitingForPolyLineSelection_ = true;
          PluginFunctions::pickMode("PolyLine");

          PluginFunctions::actionMode( Viewer::ExamineMode );

        } else if ( object->dataType(DATA_POLY_MESH) ) {
          emit log(LOGERR,"PolyLine on quad meshes not fully supported!! ");
          polyLineID_ = -1;

		      //switch back to PolyLine drawing
          PluginFunctions::actionMode( Viewer::PickingMode );
		      waitingForPolyLineSelection_ = true;
          PluginFunctions::pickMode("PolyLine");

          return;
        }

      }
    }
  }
}

#endif



