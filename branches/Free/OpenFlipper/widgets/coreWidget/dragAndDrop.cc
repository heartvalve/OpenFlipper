//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 2063 $
//   $Author: moebius $
//   $Date: 2008-06-22 17:32:25 +0200 (So, 22 Jun 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS CoreWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "CoreWidget.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>



//== IMPLEMENTATION ==========================================================

// Drag evencts view Magic ( header for drag and drop of views )
static const char VIEW_MAGIC[] = "ACG::QtWidgets::QGLViewerWidget encoded view";

//=============================================================================

void CoreWidget::startDrag ( QMouseEvent* _event )
{

   // Get the correct position in the widget
   QPoint position = _event->pos();

   // -1 if no object id found for the current picking position
   // otherwise the id of the object
   int objectId = -1;

   // Do picking in the gl area to find an object
   unsigned int    node_idx, target_idx;
   ACG::Vec3d      hit_point;
   BaseObjectData* object;
   if ( PluginFunctions::scenegraph_pick ( ACG::SceneGraph::PICK_ANYTHING, 
                                           position, 
                                           node_idx, 
                                           target_idx, 
                                           &hit_point ) ) {
      if ( PluginFunctions::get_picked_object ( node_idx, object ) )
         objectId = object->id();
   }

   if ( objectId != -1 ) {
     std::cerr << "dragEvent Picked Object" << std::endl;
   }


      QString view;
      examiner_widget_->encodeView ( view );

      QDrag     * drag = new QDrag ( examiner_widget_ );
      QMimeData * mime_data = new QMimeData;

      mime_data->setText ( view );
      drag->setMimeData ( mime_data );
      drag->start();

   }

   void CoreWidget::dragEnterEvent ( QDragEnterEvent* _event ) {
      if ( _event->mimeData()->hasFormat ( "text/plain" ) ) {
         QString view ( _event->mimeData()->text() );

         // view information entering via drag
         if ( view.left ( sizeof ( VIEW_MAGIC ) - 1 ) == QString ( VIEW_MAGIC ) ) {
            _event->acceptProposedAction();
         }

      }
   }

   void CoreWidget::dropEvent ( QDropEvent* _event ) {


      if ( _event->mimeData()->hasFormat ( "text/plain" ) ) {

         QString view ( _event->mimeData()->text() );

         // Dropped view information
         if ( view.left ( sizeof ( VIEW_MAGIC ) - 1 ) == QString ( VIEW_MAGIC ) ) {
            examiner_widget_->decodeView ( view );
            _event->acceptProposedAction();
            return;
         }

      }


   }

//=============================================================================
