/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 15870 $                                                         *
 *   $Author: tenter $                                                      *
 *   $Date: 2012-11-26 13:10:04 +0100 (Mon, 26 Nov 2012) $                   *
 *                                                                           *
\*===========================================================================*/

#include "QtWidgetNode.hh"
#include <ACG/GL/gl.hh>
#include <iostream>
#include <cmath>
#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Geometry/Types/PlaneT.hh>

#include <QImage>
#include <QGLWidget>
#include <QMouseEvent>
 #include<QApplication>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {



//== IMPLEMENTATION ==========================================================
bool QtWidgetNode::NodeEventFilter::eventFilter(QObject *_obj, QEvent *_event)
{
  bool repaint = false;
  //repaint, if layout was changed or paint requested
  if (_event->type() == QEvent::LayoutRequest || _event->type() == QEvent::Paint)
    repaint = true;

  //repaint only, if initial geometry was created
  repaint = repaint && node_->planeCreated_;

  bool result = QObject::eventFilter(_obj, _event);


  if (repaint)
  {
    //check if geometry needs an update
    if( (node_->oldWidgetWidth_ != node_->widget_->width()) || (node_->oldWidgetHeight_ != node_->widget_->height()))
        node_->updateGeometry();

    //create new widget texture
    node_->createTexture();
    node_->setDirty(true);
  }
  return result;
}
//----------------------------------------------------------------------------

QtWidgetNode::QtWidgetNode(QWidget* _widget, BaseNode *_parent, std::string _name)
:BaseNode(_parent, _name),
 ef_(new NodeEventFilter(this)),
 vbo_(0),
 texID_(0),
 widget_(_widget),
 oldWidgetWidth_(0),
 oldWidgetHeight_(0),
 planeCreated_(false),
 state_(0),
 anisotropicSupport_(false)
{
  vertexDecl_.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_POSITION);
  vertexDecl_.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_NORMAL);
  vertexDecl_.addElement(GL_FLOAT, 2, ACG::VERTEX_USAGE_TEXCOORD);
  setWidget(_widget);
  plane_.position = ACG::Vec3d(0.0,0.0,0.0);
  plane_.xDirection = ACG::Vec3d(1.0,0.0,0.0);
  plane_.yDirection = ACG::Vec3d(0.0,1.0,0.0);
}

//----------------------------------------------------------------------------

QtWidgetNode::~QtWidgetNode()
{
  if ( vbo_)
    glDeleteBuffers(1,&vbo_);

  if (texID_)
    glDeleteTextures(1,&texID_);


}

//----------------------------------------------------------------------------

void QtWidgetNode::setWidget(QWidget* _w)
{
  if (widget_)
    widget_->removeEventFilter(ef_);

  widget_ = _w;
  if(!widget_)
  {
    oldWidgetWidth_ = oldWidgetHeight_ = 0;
    planeCreated_ = false;
    setDirty(true);
    return;
  }

  widget_->adjustSize();
  widget_->installEventFilter(ef_);
  if (planeCreated_)
  {
    updateGeometry();
    createTexture();
  }
  setDirty(true);

}

//----------------------------------------------------------------------------

void QtWidgetNode::boundingBox(ACG::Vec3d& _bbMin, ACG::Vec3d& _bbMax)
{
  ACG::Vec3d pos = plane_.position - plane_.xDirection * 0.5 - plane_.yDirection * 0.5;

  //add a little offset in normal direction
  ACG::Vec3d pos0 = ACG::Vec3d( pos + plane_.normal * 0.1 );
  ACG::Vec3d pos1 = ACG::Vec3d( pos - plane_.normal * 0.1 );

  ACG::Vec3d xDird = ACG::Vec3d( plane_.xDirection );
  ACG::Vec3d yDird = ACG::Vec3d( plane_.yDirection );

  _bbMin.minimize( pos0 );
  _bbMin.minimize( pos0 + xDird);
  _bbMin.minimize( pos0 + yDird);
  _bbMin.minimize( pos0 + xDird + yDird);
  _bbMax.maximize( pos1 );
  _bbMax.maximize( pos1 + xDird);
  _bbMax.maximize( pos1 + yDird);
  _bbMax.maximize( pos1 + xDird + yDird);
}

//----------------------------------------------------------------------------

ACG::SceneGraph::DrawModes::DrawMode
QtWidgetNode::availableDrawModes() const
{
  return ( ACG::SceneGraph::DrawModes::POINTS |
             ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED );
}


//----------------------------------------------------------------------------

void QtWidgetNode::uploadPlane()
{

  const ACG::Vec3d outerCorner = plane_.xDirection + plane_.yDirection;

  // Array of coordinates for the plane
  // Interleaved with normals

  // 4 vertices with 3 pos, 3 normal, 2 texcoords
  const size_t buffersize = 4*8;
  float vboData_[buffersize] = { 0.0,0.0,0.0,
      (float)plane_.normal[0],(float)plane_.normal[1],(float)plane_.normal[2],
      0.0,0.0,
      (float)plane_.xDirection[0],(float)plane_.xDirection[1],(float)plane_.xDirection[2],
      (float)plane_.normal[0],(float)plane_.normal[1],(float)plane_.normal[2],
      1.0,0.0,
      (float)outerCorner[0],(float)outerCorner[1],(float)outerCorner[2],
      (float)plane_.normal[0],(float)plane_.normal[1],(float)plane_.normal[2],
      1.0,1.0,
      (float)plane_.yDirection[0],(float)plane_.yDirection[1],(float)plane_.yDirection[2],
      (float)plane_.normal[0],(float)plane_.normal[1],(float)plane_.normal[2],
      0.0,1.0};

  // Create buffer for vertex coordinates if necessary
  if ( ! vbo_ ) {
    glGenBuffersARB(1, &vbo_);
  }
  // Bind buffer
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);

  // Upload to buffer
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, buffersize * sizeof(float), &vboData_[0], GL_STATIC_DRAW_ARB);
}
//----------------------------------------------------------------

void QtWidgetNode::createTexture()
{
  //grab texture from qt
  widget_->removeEventFilter(ef_);
  QPixmap pix = QPixmap::grabWidget(widget_);
  widget_->installEventFilter(ef_);

  QImage image = pix.toImage();
  image = QGLWidget::convertToGLFormat( image );

  // generate texture
  if (!texID_)
  {
    glGenTextures( 1, &texID_ );
    anisotropicSupport_ = ACG::checkExtensionSupported("GL_EXT_texture_filter_anisotropic");
  }
  glBindTexture( GL_TEXTURE_2D, texID_ );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  if (anisotropicSupport_)
  {
    GLfloat anisotropyValue;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropyValue);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyValue);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA,
      GL_UNSIGNED_BYTE, image.bits());
  glGenerateMipmap(GL_TEXTURE_2D);
}
//----------------------------------------------------------------

void QtWidgetNode::createGeometry(GLState& _state)
{
  // compute the widgetsize in world space
  oldWidgetWidth_ = widget_->width();
  oldWidgetHeight_= widget_->height();;
  ACG::Vec3d projPos = ACG::Vec3d(widget_->width(),widget_->height(),0.0);
  ACG::Vec3d projNullPos = ACG::Vec3d(0.0,0.0,0.0);

  //unproject the 2D coordinates
  projPos = _state.unproject(projPos);
  projNullPos = _state.unproject(projNullPos);


  // create plane
  const ACG::Vec3d xDirection = Vec3d(projPos[0]-projNullPos[0],0.0,0.0);
  const ACG::Vec3d yDirection = Vec3d(0.0,projPos[1]-projNullPos[1],0.0);

  plane_.setPlane(ACG::Vec3d(0.0,0.0,0.0),xDirection,yDirection);

  //update plane
  uploadPlane();
}
//----------------------------------------------------------------

void QtWidgetNode::updateGeometry()
{
  double widthRatio = widget_->width() / (double)oldWidgetWidth_;
  double heightRatio = widget_->height() / (double)oldWidgetHeight_;

  oldWidgetWidth_ = widget_->width();
  oldWidgetHeight_ = widget_->height();

  plane_.setPlane(plane_.position,plane_.xDirection*widthRatio,plane_.yDirection*heightRatio);

  uploadPlane();
}

//----------------------------------------------------------------

void QtWidgetNode::mouseEvent(GLState&  _state , QMouseEvent*  _event )
{
  if (BaseNode::hidden() || !BaseNode::pickingEnabled() || !widget_)
      return;

  ACG::Vec3d eventPos = ACG::Vec3d(_event->pos().x(),_state.viewport_height()-_event->pos().y(),0.0);

  //compute 2d event position in world space
  ACG::Vec3d unprojEventPos = _state.unproject(eventPos);

  // get intersection position
  ACG::Vec3d intersecPos;
  double distance;

  ACG::Vec3d pos = plane_.position;
  ACG::Geometry::Planed intersectPlane(pos,plane_.normal);

  ACG::Vec3d rayDir = (unprojEventPos - _state.eye()).normalize();
  if (!intersectPlane.intersect_ray(_state.eye(),_state.eye()+rayDir,intersecPos,distance))
    return;//no intersection was found


  ACG::Vec3d topLeftCorner = pos - 0.5*plane_.xDirection - 0.5*plane_.yDirection;
  ACG::Vec3d toInter = intersecPos - topLeftCorner;
  ACG::Vec3d xDir = plane_.xDirection;
  ACG::Vec3d yDir = plane_.yDirection;
  double sX = (xDir | toInter)/xDir.sqrnorm();
  double sY = (yDir | toInter)/yDir.sqrnorm();

  if (sX < 0.0 || sX > 1.0 || sY < 0.0 || sY > 1.0)
      return;


  QPoint widgetEventPos(sX * widget_->width(), sY * widget_->height());

  //Search for the widget where the event should be sent
  QWidget *childWidget = widget_->childAt(widgetEventPos);
  if (!childWidget)
    return;

  //compute event pos in child space
  widgetEventPos -= childWidget->pos();

  //send event
  QMouseEvent event(_event->type(),widgetEventPos,_event->button(),_event->buttons(),_event->modifiers());
  QApplication::sendEvent(childWidget,&event);

  //update widget
  QEvent paintEvent(QEvent::Paint);
  QApplication::sendEvent(widget_,&paintEvent);
}
//----------------------------------------------------------------------------

void QtWidgetNode::mouseEvent(QMouseEvent*  _event )
{
  if (!state_)
      return;
  mouseEvent(*state_,_event);
}
//----------------------------------------------------------------------------

void QtWidgetNode::getRenderObjects(ACG::IRenderer* _renderer, ACG::GLState&  _state , const ACG::SceneGraph::DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat) {

  // init base render object
  state_ = &_state;
  if (BaseNode::hidden() || !widget_)
    return;

  ACG::RenderObject ro;

  ro.initFromState(&_state);

  if (!planeCreated_)
  {
    //one initial creation of the plane
    createGeometry(_state);
    //texture updates will be done via QWidget events (see eventfilter)
    createTexture();
    planeCreated_ = true;
  }


  ACG::Vec3d pos = plane_.position - plane_.xDirection*0.5 - plane_.yDirection*0.5;
  _state.push_modelview_matrix();
  _state.translate(pos[0], pos[1], pos[2]);
  ro.modelview = _state.modelview();
  _state.pop_modelview_matrix();

  // Render with depth test enabled
  ro.depthTest = true;

  // Set the buffers for rendering
  ro.vertexBuffer = vbo_;
  ro.vertexDecl   = &vertexDecl_;

  // Set Texture
  RenderObject::Texture texture;
  texture.id = texID_;
  texture.type = GL_TEXTURE_2D;
  texture.shadow = false;
  ro.addTexture(texture);

  // Set shading
  ro.shaderDesc.vertexColors = false;
  ro.shaderDesc.shadeMode = SG_SHADE_PHONG;

  ACG::SceneGraph::Material localMaterial;

  localMaterial.baseColor(ACG::Vec4f(1.0, 1.0, 1.0, 1.0 ));
  localMaterial.ambientColor(ACG::Vec4f(0.0, 0.0, 0.0, 0.5 ));
  localMaterial.diffuseColor(ACG::Vec4f(0.0, 0.0, 0.0, 0.5 ));
  localMaterial.specularColor(ACG::Vec4f(0.0, 0.0, 0.0, 0.5 ));
  ro.setMaterial(&localMaterial);


  ro.glDrawArrays(GL_QUADS, 0, 4);
  _renderer->addRenderObject(&ro);
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
