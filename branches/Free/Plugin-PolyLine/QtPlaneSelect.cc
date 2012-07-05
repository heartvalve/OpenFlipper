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
    planeNode_(0)
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
    if( planeNode_ )
    {
        planeNode_->delete_subtree( );
        planeNode_ = 0;
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
    if( planeNode_ )
    {
        planeNode_->delete_subtree( );
        planeNode_ = 0;
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

        // Shift toward viewer
        //sourcePoint3D = sourcePoint3D + PluginFunctions::viewingDirection();

        isDragging = true;

        if ( planeNode_ == 0 ) {
          planeNode_ = new PlaneNode(plane_,PluginFunctions::getRootNode(),"PolyLine generation Plane"  );
          planeNode_->hide();
        }

        ACG::Vec3d sourcePoint3Dd(sourcePoint3D);
        plane_.setPlane(sourcePoint3Dd,ACG::Vec3d(1.0,0.0,0.0));
        plane_.setSize(PluginFunctions::sceneRadius(),PluginFunctions::sceneRadius());

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

              ACG::Vec3d sourcePoint3Df(sourcePoint3D);
              ACG::Vec3d normald(normal);
              plane_.setPlane(sourcePoint3Df,normald);
              plane_.setSize(PluginFunctions::sceneRadius(),PluginFunctions::sceneRadius());

              planeNode_->show();

              emit updateViewProxy( );
          }
      } break;

  case QEvent::MouseButtonRelease:
      {
          if( isDragging )
          {
              if( planeNode_ )
              {
                  planeNode_->delete_subtree( );
                  planeNode_ = NULL;
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




