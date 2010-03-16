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
    isDragging( false ),
    transformNode( NULL )
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

              lineNode->add_line( ACG::Vec3f( leftvec*10) , ACG::Vec3f( rightvec*10 ) );
              lineNode->add_line( ACG::Vec3f(normal*10)   , ACG::Vec3f(0,0,0) );
              lineNode->add_line( ACG::Vec3f( leftvec*10) , ACG::Vec3f(0,0,0) );
              lineNode->add_line( ACG::Vec3f(rightvec*10) , ACG::Vec3f(0,0,0) );


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




