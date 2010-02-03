/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  Plugin Functions
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>
#include <QGLWidget>

#include "PluginFunctions.hh"
#include "PluginFunctionsCore.hh"

namespace PluginFunctions {

 /** \brief DONT USE DIRECTLY!!
  *
  * This Pointer will be used to access the data trees root in the plugin. Never use it directly as
 *  the internal Structure will change. Use the Access functions instead */
static BaseObject* objectRoot_ = 0;

/** \brief DONT USE DIRECTLY!!
 *
 * This pointer is internally used to acces the examiner widget in the main apllication
 */
static std::vector< glViewer* > examiner_widgets_;

/** \brief DONT USE DIRECTLY!!
 *
 * This vector is used internally to access properties of all viewers
 */
static std::vector< Viewer::ViewerProperties* > viewerProperties_;

/// TODO : Remove this variable and implement multiView correctly here
static glViewer*  examiner_widget_;

/// Contains the currently active examiner
static unsigned int activeExaminer_ = 0;

/** \brief DONT USE DIRECTLY!!
 *
 * The pointer to the beginning of the scenegraph nodes ( only the nodes belonging to objects )
 * Between the actual root node ( sceneGraphRootNode_ ) and this node global nodes could be added
 * This pointer is internally used to access the scenegraphs root node
 */
static SeparatorNode* dataRootNode_ = 0;

/** This node is used to add nodes between the root node of the scenegraph and the objects separator node.
* It is directly below the sceneGraphRootNode_
*/
static SeparatorNode* dataSeparatorNode_ = 0;

/** \brief Scenegraph root node
 */
static SeparatorNode* sceneGraphRootNode_ = 0;

/** \brief a dummy properties object returned as a reference if the real object does not exist
 *
 */
static Viewer::ViewerProperties dummyProperties;


/** \brief DONT USE DIRECTLY!!
 *
 * This pointer is used internally
 */
static ViewObjectMarker* defaultMarker_ = 0;

/** \brief DONT USE DIRECTLY!!
 *
 * This pointer is used internally
 */
static QGLWidget* shareGLWidget_ = 0;

/** This is a unique id for the running OpenFlipper instance. Use it to identify yourself on the network
*/
static int viewerId_ = 0;

/// Number of currently open objects
static int objectCounter_ = 0;

/// Number of current target objects
static int targetCounter_ = 0;

void setDataRoot( BaseObject* _root ) {
   objectRoot_ = _root;
}

int viewers( ) {
  return examiner_widgets_.size();
}

int viewerId() {
  return viewerId_;
}

void setViewers( std::vector< glViewer* > _viewerWidgets ) {
   PluginFunctions::examiner_widgets_ = _viewerWidgets;
   PluginFunctions::examiner_widget_ =  examiner_widgets_[0];

   // Generate a (hopefully) unique viewer id
   QTime time = QTime::currentTime();
   qsrand( time.hour() * 10 + time.minute() * 100 + time.second() * 1000 + time.msec() * 10000  );
   viewerId_ = qrand();
}

void setViewerProperties( std::vector< Viewer::ViewerProperties* > _viewerProperties ) {
   PluginFunctions::viewerProperties_ = _viewerProperties;
}

void setActiveExaminer( const unsigned int _id ) {
  activeExaminer_ = _id;
}

glViewer* viewer(int  _viewerId ) {
   if ( _viewerId < 0 || _viewerId >= (int)examiner_widgets_.size() ) {
     std::cerr << "Requested unknown viewer with id : " << _viewerId << std::endl;
     return examiner_widgets_[activeExaminer()];
   }

   return( examiner_widgets_[_viewerId] );
}

unsigned int activeExaminer( ) {
  return activeExaminer_;
}

QString getEncodedExaminerView() {
  return getEncodedExaminerView(activeExaminer());
}

/// Get the encoded view for the given
QString getEncodedExaminerView(int _viewerId) {

  QString view;

  if ( _viewerId < 0 || _viewerId >= (int)examiner_widgets_.size() ) {
     std::cerr << "Requested unknown viewer with id : " << _viewerId << std::endl;
     examiner_widgets_[activeExaminer()]->encodeView ( view );
     return view;
  }

  examiner_widgets_[_viewerId]->encodeView ( view );
  return view;

}

void setEncodedExaminerView( QString _view ) {
  setEncodedExaminerView( activeExaminer() , _view );
}

void setEncodedExaminerView(int _viewerId , QString _view ) {

  if ( _viewerId < 0 || _viewerId >= (int)examiner_widgets_.size() ) {
     std::cerr << "Requested unknown viewer with id : " << _viewerId << std::endl;
     examiner_widgets_[activeExaminer()]->decodeView ( _view );
  }

  examiner_widgets_[_viewerId]->decodeView ( _view );
}

void setDataSeparatorNodes( SeparatorNode* _dataSeparatorNode ) {
  
  // The function should only be called once by the core. 

  // Set the separatorNode
  PluginFunctions::dataSeparatorNode_ = _dataSeparatorNode;

  
  if ( PluginFunctions::dataSeparatorNode_->nChildren() != 1 ){
    std::cerr << "Only one child allowed for dataSeparatorNode on initialization!" << std::endl;
    std::cerr << "The Core has initialized the scenegraph in a strange way!" << std::endl;
    ACG::SceneGraph::BaseNode* child = *(PluginFunctions::dataSeparatorNode_->childrenBegin());
  }
  
  // Set the root node for the data objects
  // which has to be a child of the dataSeparatorNode_
  PluginFunctions::dataRootNode_ = dynamic_cast<ACG::SceneGraph::SeparatorNode*> (*(PluginFunctions::dataSeparatorNode_->childrenBegin()) );
  
}

void setSceneGraphRootNode( SeparatorNode* _root_node ) {
   PluginFunctions::sceneGraphRootNode_ = _root_node;
}

bool getPickedObject(const unsigned int _node_idx , BaseObjectData*& _object) {
  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->picked( _node_idx ) ) {
      _object = *o_it;
      return true;
    }
  }
  return false;
}


bool getSourceIdentifiers( std::vector<int>& _identifiers  ) {
  _identifiers.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->source() )
      _identifiers.push_back ( o_it->id() );
  }
  return (_identifiers.size() >0 );
}

bool getTargetIdentifiers( std::vector<int>& _identifiers  ) {
  _identifiers.clear();

  for ( ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ; o_it != PluginFunctions::objectsEnd(); ++o_it) {
    if ( o_it->target() )
      _identifiers.push_back ( o_it->id() );
  }
  return (_identifiers.size() >0 );
}

// ===============================================================================
// Get objects
// ===============================================================================

bool getObject(  const int _identifier , BaseObject*& _object ) {

  if ( _identifier == -1 )
    return false;

  _object = objectRoot_->childExists( _identifier );
  return ( _object != 0 );
}

bool getObject(  const int _identifier , BaseObjectData*& _object ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot_->childExists( _identifier );
  _object = dynamic_cast< BaseObjectData* >(object);
  return ( _object != 0 );
}

int getObjectId( const QString _name ) {

	if(_name.isEmpty()) return -1;

	BaseObject* object = objectRoot_->childExists( _name );
	return object->id();
}

// ===============================================================================
// ===============================================================================

bool object_exists(  const int _identifier ) {

  if ( _identifier == -1 )
    return false;

  BaseObject* object = objectRoot_->childExists( _identifier );
  return ( object != 0 );
}

//===============================================================================

bool getAllMeshes( std::vector<int>& _identifiers  ) {

  _identifiers.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,typeId("TriangleMesh")) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    _identifiers.push_back( o_it->id() );
  }

  return (_identifiers.size() > 0);
}

bool get_all_object_identifiers( std::vector<int>& _identifiers  ) {

  _identifiers.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    _identifiers.push_back( o_it->id() );
  }

  return (_identifiers.size() > 0);
}

void setBackColor( OpenMesh::Vec4f _color) {
  for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
    PluginFunctions::viewerProperties(i).backgroundColor(_color);
}

void setFixedView(int _mode, int _viewer ) {

  if ( (_viewer != ACTIVE_VIEWER ) && ( ( _viewer < 0 ) || _viewer >= (int)examiner_widgets_.size()) ){
    std::cerr << "Unable to set fixed view. Wrong viewer id (" << _viewer << ")" << std::endl;
    return;
  }

  switch ( _mode ){
    case VIEW_TOP : //TOP
      PluginFunctions::viewingDirection( ACG::Vec3d(0.0, -1.0, 0.0), ACG::Vec3d(0.0, 0.0, -1.0), _viewer );
      PluginFunctions::allowRotation(false, _viewer);
      break;
    case VIEW_BOTTOM : //BOTTOM
      PluginFunctions::viewingDirection( ACG::Vec3d(0.0, 1.0, 0.0), ACG::Vec3d(0.0, 0.0, -1.0), _viewer );
      PluginFunctions::allowRotation(false, _viewer);
      break;
    case VIEW_LEFT : //LEFT
      PluginFunctions::viewingDirection( ACG::Vec3d(1.0, 0.0, 0.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      PluginFunctions::allowRotation(false, _viewer);
      break;
    case VIEW_RIGHT : //RIGHT
      PluginFunctions::viewingDirection( ACG::Vec3d(-1.0, 0.0, 0.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      PluginFunctions::allowRotation(false, _viewer);
      break;
    case VIEW_FRONT : //FRONT
      PluginFunctions::viewingDirection( ACG::Vec3d(0.0, 0.0, -1.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      PluginFunctions::allowRotation(false, _viewer);
      break;
    case VIEW_BACK : //BACK
      PluginFunctions::viewingDirection( ACG::Vec3d(0.0, 0.0, 1.0), ACG::Vec3d(0.0, 1.0, 0.0), _viewer );
      PluginFunctions::allowRotation(false, _viewer);
      break;
    default : //Free View
      PluginFunctions::allowRotation(true, _viewer);
      break;
  }

  if ( _viewer == ACTIVE_VIEWER )
    viewerProperties(activeExaminer()).currentViewingDirection( _mode );
  else
    viewerProperties( _viewer ).currentViewingDirection( _mode );
}

QPoint mapToGlobal(const QPoint _point ) {
  return examiner_widgets_[activeExaminer_]->glMapToGlobal(_point);
}

QPoint mapToLocal( const QPoint _point ) {
  return examiner_widgets_[activeExaminer_]->glMapFromGlobal(_point);
}

void setDrawMode( const unsigned int _mode , int _viewer) {

  if ( _viewer == ACTIVE_VIEWER )
    viewerProperties(activeExaminer()).drawMode(_mode);
  else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      viewerProperties(i).drawMode(_mode);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    viewerProperties(_viewer).drawMode(_mode);
  else
    std::cerr << "Requested illegal viewer for setting DrawMode!!" << std::endl;

}

/** Get the current draw Mode of the examiner widget.\n
 *
 * The DrawModes are defined at ACG/Scenegraph/DrawModes.hh \n
 * They can be combined.
 */
unsigned int drawMode( int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return viewerProperties(activeExaminer()).drawMode();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get viewing direction!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return viewerProperties(_viewer).drawMode();
  else
    std::cerr << "Requested illegal viewer for viewingDirection!!" << std::endl;

  return viewerProperties(activeExaminer()).drawMode();
}

bool scenegraphPick( ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr=0 ) {

   return examiner_widgets_[activeExaminer_]->pick( _pickTarget,_mousePos,_nodeIdx,_targetIdx,_hitPointPtr );
}

bool scenegraphPick( const unsigned int _examiner, ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr=0 ) {

  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return false;
  }
  return examiner_widgets_[_examiner]->pick( _pickTarget,_mousePos,_nodeIdx,_targetIdx,_hitPointPtr );
}

bool scenegraphRegionPick( ACG::SceneGraph::PickTarget                _pickTarget,
                           const QRegion&                             _region,
                           QList<QPair<unsigned int, unsigned int> >& _list)
{
  return examiner_widgets_[activeExaminer_]->pick_region( _pickTarget, _region, _list);
}

bool scenegraphRegionPick( const unsigned int                         _examiner,
                           ACG::SceneGraph::PickTarget                _pickTarget,
                           const QRegion&                             _region,
                           QList<QPair<unsigned int, unsigned int> >& _list)
{
  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return false;
  }
  return examiner_widgets_[_examiner]->pick_region( _pickTarget, _region, _list);
}



//Warning : Dont use template function as external static pointer for examiner widget is not resolved correctly!!
void traverse( ACG::SceneGraph::MouseEventAction  &_action ) {
  // Single pass action, as the mouse action will only update the graph.
  // If its changed, it will be set to dirty and an automatic redraw is triggered.
  ACG::SceneGraph::traverse(sceneGraphRootNode_, _action,viewerProperties().glState() );
}

//Warning : Dont use template function as external static pointer for examiner widget is not resolved correctly!!
void traverse( const unsigned int _examiner, ACG::SceneGraph::MouseEventAction  &_action ) {

  if ( _examiner >= examiner_widgets_.size() ) {
    std::cerr << "Wrong examiner id" << std::endl;
    return;
  }

  // Single pass action, as the mouse action will only update the graph.
  // If its changed, it will be set to dirty and an automatic redraw is triggered.
  ACG::SceneGraph::traverse(sceneGraphRootNode_, _action,viewerProperties(_examiner).glState() );
}


const std::string pickMode () {
  // No seperate draw modes available all should have the same so take first
  return viewerProperties().pickMode();
}

void pickMode ( std::string _mode) {
  // switch to default marker
  setViewObjectMarker (defaultViewObjectMarker ());
  viewerProperties().pickMode(_mode);
}

Viewer::ActionMode actionMode() {
   return viewerProperties().actionMode();
}

void actionMode ( Viewer::ActionMode _mode) {

  viewerProperties().actionMode(_mode);
}

void shareGLWidget (QGLWidget *_widget)
{
  shareGLWidget_ = _widget;
}

QGLWidget *shareGLWidget ()
{
  return shareGLWidget_;
}

void getCurrentViewImage(QImage& _image) {
  viewer( activeExaminer() )->snapshot( _image, true );
}

Viewer::ViewerProperties& viewerProperties(int _id) {
  if ( _id >= (int)viewerProperties_.size() ) {
    std::cerr << " Error, requested properties for non-existing Viewer!" << std::endl;
    return dummyProperties;
  }

  if ( _id == -1 )
    _id = activeExaminer_;

  return ( *viewerProperties_[_id] );

}

void perspectiveProjection( int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->perspectiveProjection();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->perspectiveProjection();
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->perspectiveProjection();
  else
    std::cerr << "Requested illegal viewer for perspectiveProjection()!!" << std::endl;
}

void orthographicProjection( int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->orthographicProjection();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->orthographicProjection();
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->orthographicProjection();
  else
    std::cerr << "Requested illegal viewer for orthographicProjection()!!" << std::endl;
}

void allowRotation(bool _mode, int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->allowRotation(_mode);
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->allowRotation(_mode);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->allowRotation(_mode);
  else
    std::cerr << "Requested illegal viewer for allowRotation!!" << std::endl;
}

bool allowRotation( int _viewer ) {

  if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return examiner_widgets_[_viewer]->allowRotation();
  else {

    std::cerr << "Requested illegal viewer for isRotationAllowed!!" << std::endl;
    return false;
  }
}

void setMainGLContext() {
  examiner_widget_->makeCurrent();
}

void viewingDirection(const ACG::Vec3d &_dir, const ACG::Vec3d &_up , int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->viewingDirection(_dir,_up);
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->viewingDirection(_dir,_up);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->viewingDirection(_dir,_up);
  else
    std::cerr << "Requested illegal viewer for viewingDirection!!" << std::endl;
}

const ACG::Vec3d trackBallCenter(const ACG::Vec3d& _center, int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return examiner_widgets_[activeExaminer_]->trackBallCenter();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      return examiner_widgets_[i]->trackBallCenter(  );
    else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
      return examiner_widgets_[_viewer]->trackBallCenter(  );
    else
      std::cerr << "Requested illegal viewer for setTrackBallCenter!!" << std::endl;

  return examiner_widgets_[activeExaminer_]->trackBallCenter();
}

void setTrackBallCenter(const ACG::Vec3d& _center, int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->setTrackBallCenter( _center );
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->setTrackBallCenter( _center );
    else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
      examiner_widgets_[_viewer]->setTrackBallCenter( _center );
    else
      std::cerr << "Requested illegal viewer for setTrackBallCenter!!" << std::endl;
}

void setScenePos(const ACG::Vec3d& _center,const double _radius, int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->setScenePos( _center, _radius );
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->setScenePos( _center, _radius );
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->setScenePos( _center, _radius );
  else
    std::cerr << "Requested illegal viewer for setScenePos!!" << std::endl;
}

void setScenePos(const ACG::Vec3d& _center, int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->setScenePos( _center, examiner_widgets_[activeExaminer_]->scene_radius() );
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->setScenePos( _center, examiner_widgets_[i]->scene_radius() );
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->setScenePos( _center, examiner_widgets_[_viewer]->scene_radius() );
  else
    std::cerr << "Requested illegal viewer for setScenePos!!" << std::endl;
}

void setSceneCenter(const ACG::Vec3d& _center, int _viewer) {

	if (_viewer == ACTIVE_VIEWER) {

		examiner_widgets_[activeExaminer_]->setSceneCenter(_center);
	} else if (_viewer == ALL_VIEWERS) {

		for (uint i = 0; i < examiner_widgets_.size(); ++i) {

			examiner_widgets_[i]->setSceneCenter(_center);
		}
	} else if ((_viewer >= 0) && _viewer < (int) examiner_widgets_.size()) {

		examiner_widgets_[_viewer]->setSceneCenter(_center);
	} else {
		std::cerr << "Requested illegal viewer for setSceneCenter!!"
				<< std::endl;
	}
}

const ACG::Vec3d& sceneCenter(int _viewer) {

	if (_viewer == ACTIVE_VIEWER) {
		return examiner_widgets_[activeExaminer_]->scene_center();
	} else if (_viewer == ALL_VIEWERS)
		std::cerr << "Please select viewer to get viewing direction!"
				<< std::endl;
	else if ((_viewer >= 0) && _viewer < (int) examiner_widgets_.size())
		return examiner_widgets_[_viewer]->scene_center();
	else
		std::cerr << "Requested illegal viewer for sceneCenter!!" << std::endl;

	return examiner_widgets_[activeExaminer_]->scene_center();
}

double sceneRadius() {
   return examiner_widgets_[activeExaminer_]->scene_radius();
}

double sceneRadius( int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return examiner_widgets_[activeExaminer_]->scene_radius();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Illegal request for scene radius. Please select one viewer!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return examiner_widgets_[_viewer]->scene_radius();
  else
    std::cerr << "Requested illegal viewer for translate!!" << std::endl;

  return -1;
}

void translate( const ACG::Vec3d &_vector , int _viewer ) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->translate(_vector);
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->translate(_vector);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->translate(_vector);
  else
    std::cerr << "Requested illegal viewer for translate!!" << std::endl;
}

void rotate(const ACG::Vec3d&  _axis,
            const double       _angle,
            const ACG::Vec3d&  _center,
            int                _viewer )
{
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->rotate(_axis,_angle,_center);
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->rotate(_axis,_angle,_center);
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->rotate(_axis,_angle,_center);
  else
    std::cerr << "Requested illegal viewer for rotate!!" << std::endl;
}

void viewHome(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->home();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->home();
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->home();
  else
    std::cerr << "Requested illegal viewer for viewHome!!" << std::endl;
}

void viewAll(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    examiner_widgets_[activeExaminer_]->viewAll();
  } else if ( _viewer == ALL_VIEWERS )
    for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
      examiner_widgets_[i]->viewAll();
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    examiner_widgets_[_viewer]->viewAll();
  else
    std::cerr << "Requested illegal viewer for viewAll!!" << std::endl;
}

ACG::Vec3d viewingDirection(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return viewerProperties(activeExaminer_).glState().viewing_direction();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get viewing direction!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return viewerProperties(_viewer).glState().viewing_direction();
  else
    std::cerr << "Requested illegal viewer for viewingDirection!!" << std::endl;

  return viewerProperties().glState().viewing_direction();
}

bool isProjectionOrthographic( int _viewer ) {

  if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() ){

    if ( examiner_widgets_[_viewer]->projectionMode() == 0) //ORTHOGRAPHIC_PROJECTION
      return true;
    else
      return false;

  } else
    std::cerr << "Requested illegal viewer for isProjectionOrthographic!!" << std::endl;

  return false;
}

ACG::Vec3d eyePos(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return viewerProperties(activeExaminer_).glState().eye();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get eyePos!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return viewerProperties(_viewer).glState().eye();
  else
    std::cerr << "Requested illegal viewer for eyePos!!" << std::endl;

  return viewerProperties().glState().eye();
}

ACG::Vec3d upVector(int _viewer) {
  if ( _viewer == ACTIVE_VIEWER ) {
    return viewerProperties(activeExaminer_).glState().up();
  } else if ( _viewer == ALL_VIEWERS )
    std::cerr << "Please select viewer to get up vector!" << std::endl;
  else if ( ( _viewer >= 0 ) && _viewer < (int)examiner_widgets_.size() )
    return viewerProperties(_viewer).glState().up();
  else
    std::cerr << "Requested illegal viewer for up vector!!" << std::endl;

  return viewerProperties().glState().up();
}

void setViewObjectMarker(ViewObjectMarker * _marker)
{
  for ( uint i = 0 ; i < examiner_widgets_.size(); ++i )
    PluginFunctions::viewerProperties(i).objectMarker(_marker);
}

void setDefaultViewObjectMarker(ViewObjectMarker * _marker)
{
  defaultMarker_ = _marker;
}

ViewObjectMarker * defaultViewObjectMarker()
{
  return defaultMarker_;
}


ACG::SceneGraph::BaseNode* getSceneGraphRootNode() {
   return PluginFunctions::sceneGraphRootNode_;
}

ACG::SceneGraph::BaseNode* getRootNode() {
  return PluginFunctions::dataRootNode_;
}

void addGlobalNode(ACG::SceneGraph::BaseNode* _node){
  if (PluginFunctions::sceneGraphRootNode_)
    _node->set_parent( PluginFunctions::sceneGraphRootNode_ );
}


void addObjectRenderingNode(ACG::SceneGraph::BaseNode* _node){
  if (PluginFunctions::sceneGraphRootNode_){
    
    // get the current parent Node 
    ACG::SceneGraph::BaseNode* parent = dataRootNode_->parent();

    // Move the node to the new parent
    _node->set_parent(parent);
    
    // move dataRootNode_ to the new parent
    dataRootNode_->set_parent(_node);
  }
  
}

int objectCount() {
  return(objectCounter_);
}



int targetCount() {
  return ( targetCounter_ );
}

int sourceCount() {
  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::SOURCE_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    ++count;
  }

  return ( count );
}

int visibleCount() {
  int count = 0;

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    if ( o_it->visible() )
    ++count;
  }

  return ( count );

}


/** Returns a vector containing all currently available objects
 *
 * @param _objects vector of all objects
 */
void get_all_objects( std::vector < BaseObjectData*>& _objects ) {

   _objects.clear();

  // find changed manipulator
  for ( PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,DATA_ALL) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    _objects.push_back( *o_it );
  }

}

///   Fly to point and viewing direction (animated).
void   flyTo (const ACG::Vec3d &_position, const ACG::Vec3d &_center, double _time) {
   examiner_widgets_[activeExaminer_]->flyTo(_position,_center,_time);
}


// ===============================================================================
// Getting data from objects and casting between them
// ===============================================================================

BaseObjectData* baseObjectData( BaseObject* _object ){
  if ( _object == 0 )
    return 0;

  return dynamic_cast< BaseObjectData* >(_object);
}

// ===============================================================================
// Get the root of the object structure
// ===============================================================================
BaseObject*& objectRoot() {
  return (objectRoot_);
}

void increaseObjectCount() {
  objectCounter_++;
}

// Increase the number of current Object
void decreaseObjectCount() {
  objectCounter_--;
  
  if ( objectCounter_ < 0 )
    std::cerr << "Deleted more objects than created!!!" << std::endl;
}

void increaseTargetCount() {
  targetCounter_++;
}

// Increase the number of current Object
void decreaseTargetCount() {
  targetCounter_--;
  
  if ( targetCounter_ < 0 )
    std::cerr << "target object counter underflow!!!" << std::endl;
}

}
