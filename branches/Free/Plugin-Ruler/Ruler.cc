#include "Ruler.hh"

//------------------------------------------------------------------------------
Ruler::Ruler(BaseObjectData* _obj,const QString &_pluginName, unsigned index)
:pluginName_(_pluginName),
 lineNodeName_(QString("Ruler-Plugin LineNode%1").arg(index).toStdString()),
 textNodeName_(QString("Ruler-Plugin TextNode%1").arg(index).toStdString()),
 textTransformNodeName_(QString("Ruler-Plugin TransformNode%1").arg(index).toStdString()),
 lineNode_(0),
 textNode_(0),
 textTransformNode_(0),
 obj_(_obj)
{
  points_[0] = points_[1] = ACG::Vec3d(0.0,0.0,0.0);

  //create line, point, text and transformation nodes
  if (!_obj->getAdditionalNode(lineNode_,_pluginName,lineNodeName_.c_str()))
  {
    lineNode_ = new ACG::SceneGraph::LineNode(ACG::SceneGraph::LineNode::LineSegmentsMode,
        _obj->manipulatorNode(),lineNodeName_);
    _obj->addAdditionalNode(lineNode_,_pluginName,lineNodeName_.c_str());
  }

  if (!_obj->getAdditionalNode(textTransformNode_,_pluginName,textTransformNodeName_.c_str()))
  {
    textTransformNode_ = new ACG::SceneGraph::TransformNode(lineNode_,textTransformNodeName_.c_str());
    _obj->addAdditionalNode(textTransformNode_,_pluginName,textTransformNodeName_.c_str());
  }

  if (!_obj->getAdditionalNode(textNode_,_pluginName,textNodeName_.c_str()))
  {
    textNode_ = new ACG::SceneGraph::TextNode(textTransformNode_,textNodeName_,ACG::SceneGraph::TextNode::SCREEN_ALIGNED,true);
    _obj->addAdditionalNode(textNode_,_pluginName,textNodeName_.c_str());

  }
}
//------------------------------------------------------------------------------
Ruler::~Ruler()
{
  if (obj_->getAdditionalNode(textNode_,pluginName_,textNodeName_.c_str()))
  {
    obj_->removeAdditionalNode(textNode_,pluginName_,textNodeName_.c_str());
  }
  if (obj_->getAdditionalNode(textTransformNode_,pluginName_,textTransformNodeName_.c_str()))
  {
    obj_->removeAdditionalNode(textTransformNode_,pluginName_,textTransformNodeName_.c_str());
  }
  if (obj_->getAdditionalNode(lineNode_,pluginName_,lineNodeName_.c_str()))
  {
    obj_->removeAdditionalNode(lineNode_,pluginName_,lineNodeName_.c_str());
  }
}
//------------------------------------------------------------------------------
void Ruler::setPoints(const ACG::Vec3d &_start,const ACG::Vec3d &_end)
{
  points_[0] = _start;
  points_[1] = _end;
  updateNodes();
}

void Ruler::setStartPoint(const ACG::Vec3d& _start)
{
  points_[0] = _start;
  updateNodes();
}
//------------------------------------------------------------------------------
void Ruler::setEndPoint(const ACG::Vec3d& _end)
{
  points_[1] = _end;
  updateNodes();
}
//------------------------------------------------------------------------------

void Ruler::updateNodes()
{
  ACG::Vec3d Point1 = points_[0];
  ACG::Vec3d Point2 = points_[1];

  //creates the line
  lineNode_->clear_points();
  lineNode_->set_color(OpenMesh::Vec4f(1.0f,0.0f,0.0f,1.0f));
  lineNode_->set_line_width(3);
  lineNode_->add_line(Point1,Point2);
  lineNode_->alwaysOnTop() = true;

  //set params for the text
  ACG::Vec3d distVec = Point1 - Point2;
  QString distanceStr = QString().number((distVec).length());
  textNode_->setText(distanceStr.toStdString());
  textNode_->multipassNodeSetActive(8, true);
  textNode_->setSize(1);

  //translate and scale text
  textTransformNode_->loadIdentity();
  textTransformNode_->translate(Point1);
  ACG::Vec3d halfDist = distVec/2.f;
  textTransformNode_->translate(-halfDist);
  textTransformNode_->scale(distVec.length()*0.125);

  emit updateView();
}
