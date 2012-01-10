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

#include "QtPlaneSelect.hh"

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <ACG/Scenegraph/GlutPrimitiveNode.hh>
#define PLUGINFUNCTIONS_C
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ACG/GL/gl.hh>
#include <ACG/QtWidgets/QtColorTranslator.hh>

#include <QEvent>
#include <QMouseEvent>



/*******************************************************************************
         Initialization and de initialization
*******************************************************************************/
QtPlaneSelect::QtPlaneSelect( ACG::GLState& glState )
  : glState( glState ),
    nodeIdx_(0),
    targetIdx_(0),
    isDragging( false ),
    transformNode( NULL ),
    transformNodeSource( NULL ),
    transformNodeDrag( NULL ),
    lineNode( NULL )
{

}

QtPlaneSelect::~QtPlaneSelect( )
{
}


/*******************************************************************************
        Implementation of public slots
*******************************************************************************/

void QtPlaneSelect::slotKeyReleaseEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Escape){
    if( transformNode )
    {
        transformNode->delete_subtree( );
        transformNode = NULL;
    }

    emit updateViewProxy( );

    // Trigger the event
    isDragging = false;
  }
}

void QtPlaneSelect::slotMouseEvent(QMouseEvent* event)
{
//  unsigned int width  = glState.viewport_width();
  unsigned int height = glState.viewport_height();

  //cancel on rightclick
  if (event->button() == Qt::RightButton){
    if( transformNode )
    {
        transformNode->delete_subtree( );
        transformNode = NULL;
    }

    emit updateViewProxy( );

    // Trigger the event
    isDragging = false;

    return;
  }


  switch( event->type() )
  {
    case QEvent::MouseButtonPress:
    {
      // Only react on the left button and ignore the others
      if ( event->button() != Qt::LeftButton )
        return;

      unsigned int     node_idx, target_idx;

      if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE,
                                            event->pos(),
                                            node_idx,
                                            target_idx,
                                            &sourcePoint3D))
      {
        isDragging = true;
        if( transformNode ) transformNode->delete_subtree( );
        transformNode = new ACG::SceneGraph::TransformNode( PluginFunctions::getRootNode( ), "Volcoretest2" );
        transformNode->translate( sourcePoint3D );

        transformNodeSource = new ACG::SceneGraph::TransformNode( transformNode );
        // TODO compute distance of point to camera and use that as the distance.
        transformNodeSource->scale( ACG::Vec3d(0.2,0.2,0.2) );

        ACG::SceneGraph::GlutPrimitiveNode*           sphere_node;
        sphere_node = new ACG::SceneGraph::GlutPrimitiveNode(ACG::SceneGraph::GlutPrimitiveNode::SPHERE, transformNodeSource, "Volcoretest");
        sphere_node->drawMode(ACG::SceneGraph::DrawModes::SOLID_SMOOTH_SHADED);
        sphere_node->hide();


        transformNodeDrag = new ACG::SceneGraph::TransformNode( transformNode );

        lineNode = new ACG::SceneGraph::LineNode( ACG::SceneGraph::LineNode::LineSegmentsMode, transformNodeDrag, "wuhasdlfkjasdf" );
        lineNode->set_line_width(3.0);
        lineNode->show();

        emit updateViewProxy( );

        nodeIdx_   = node_idx;
        targetIdx_ = target_idx;
      }
    } break;

    case QEvent::MouseMove:
      {
          if( isDragging )
          {
              Vec3d source2D = glState.project( sourcePoint3D );
              Vec3d target2D(event->pos().x(), height-event->pos().y()-1, 0);

              source2D[2] = 0;

              Vec3d diff = source2D - target2D;
              //diff.normalize( );  <- this is bad
              Vec3d ortho(-diff[1], diff[0], 0 );

              Vec3d left = glState.unproject( source2D+ortho*10+Vec3d(0,0,0) );
              Vec3d right= glState.unproject( source2D-ortho*10+Vec3d(0,0,0) );

              Vec3d leftvec = left-sourcePoint3D;
              leftvec.normalize( );
              Vec3d rightvec = right-sourcePoint3D;
              rightvec.normalize( );

              normal = cross( rightvec, leftvec );
              normal.normalize( );

              lineNode->clear( );

              lineNode->add_line( leftvec  * 10 , rightvec * 10  ); 
              lineNode->add_line( normal   * 10 , ACG::Vec3d(0,0,0) );
              lineNode->add_line( leftvec  * 10 , ACG::Vec3d(0,0,0) );
              lineNode->add_line( rightvec * 10 , ACG::Vec3d(0,0,0) );


              emit updateViewProxy( );
          }
      } break;

  case QEvent::MouseButtonRelease:
      {
          if( isDragging )
          {
              if( transformNode )
              {
                  transformNode->delete_subtree( );
                  transformNode = NULL;
              }

              emit updateViewProxy( );

              emit( signalTriggerCut( ) );
              // Trigger the event
              isDragging = false;
          }
      } break;

  default:
      break;
  }


}




