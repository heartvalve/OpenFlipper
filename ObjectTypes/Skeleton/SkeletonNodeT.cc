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
//   $Revision: 1961 $
//   $Author: bommes $
//   $Date: 2008-06-11 19:18:04 +0200 (Wed, 11 Jun 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS SkeletonNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_SKELETONNODET_C

//== INCLUDES =================================================================

#include "SkeletonNodeT.hh"
#include <ACG/GL/gl.hh>
#include <vector>
#include <deque>

//== NAMESPACES ===============================================================
using namespace std;

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================


/** \brief Constructor
 * 
 */
template <class Skeleton>
SkeletonNodeT<Skeleton>::SkeletonNodeT(Skeleton &_skeleton, BaseNode *_parent, std::string _name) :
               BaseNode(_parent, _name),
               bCoordFramesVisible_(false),
               skeleton_(_skeleton),
               hAni_(),
               quadricInitialized_(false),
               quadric_(0),
               fFrameSize_(0.0)
{
  this->multipassNodeSetActive(3, true);
}


//------------------------------------------------------------------------------

/** \brief Destructor
 * 
 */
template <class Skeleton>
SkeletonNodeT<Skeleton>::~SkeletonNodeT()
{
  // delete quadric if initialized
  if (quadricInitialized_)
    gluDeleteQuadric(quadric_);
}


//------------------------------------------------------------------------------

/** \brief Returns a pointer to the skeleton
 * 
 */
template <class Skeleton>
Skeleton& SkeletonNodeT<Skeleton>::skeleton()
{
  return skeleton_;
}


//------------------------------------------------------------------------------

/** \brief Returns the bounding box of this node
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{  
  if(skeleton_.jointCount() == 0)
    return; // no joints, no contribution to the bound box

  Pose *pose = skeleton_.pose(hAni_);
  typename Skeleton::Iterator it;
  for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
  {
    _bbMin.minimize( (Vec3d)pose->globalTranslation((*it)->id()) );
    _bbMax.maximize( (Vec3d)pose->globalTranslation((*it)->id()) );
  }
  
  //choose a proper size for the coordinate frames
  fFrameSize_ = (_bbMin - _bbMax).norm() * 0.07;
}


//----------------------------------------------------------------------------

/** \brief Returns available draw modes
 * 
 */
template <class Skeleton>
DrawModes::DrawMode SkeletonNodeT<Skeleton>::availableDrawModes() const
{
  return (DrawModes::WIREFRAME           | DrawModes::POINTS | DrawModes::SOLID_FLAT_SHADED
        | DrawModes::SOLID_FACES_COLORED | DrawModes::SOLID_FACES_COLORED_FLAT_SHADED);
}


//----------------------------------------------------------------------------

/** \brief Normalizes a coordinate frame defined by the given matrix
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::NormalizeCoordinateFrame(Matrix &_mat)
{
  ACG::Vec3d vec;
  for(int i = 0; i < 3; ++i)
  {
    for(int x = 0; x < 3; ++x)
      vec[x] = _mat(x, i);
    vec.normalize();
    for(int x = 0; x < 3; ++x)
      _mat(x, i) = vec[x];
  }
}


//----------------------------------------------------------------------------

/**
 * @brief Converts the color from HSV color space to RGB
 *
 * H is in range [0, 360], S and V in [0, 1].
 *
 * @param[in] _HSV An array of 3 GLfloats, holding H, S and V
 * @param[out] _RGB An array of 3 GLfloats, holding R, G and B
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::HSVtoRGB(const Vec4f& _HSV, Vec4f& _RGB)
{
  int h;
  float f, p, q, t;
  h = int(floor(_HSV[0] / 60.0));
  f = _HSV[0] / 60.0 - float(h);
  p = _HSV[2] * (1 - _HSV[1]);
  q = _HSV[2] * (1 - _HSV[1] * f);
  t = _HSV[2] * (1 - _HSV[1] * (1 - f));

  switch(h)
  {
  case 0:
  case 6:
    _RGB[0] = _HSV[2];
    _RGB[1] = t;
    _RGB[2] = p;
    break;
  case 1:
    _RGB[0] = q;
    _RGB[1] = _HSV[2];
    _RGB[2] = p;
    break;
  case 2:
    _RGB[0] = p;
    _RGB[1] = _HSV[2];
    _RGB[2] = t;
    break;
  case 3:
    _RGB[0] = p;
    _RGB[1] = q;
    _RGB[2] = _HSV[2];
    break;
  case 4:
    _RGB[0] = t;
    _RGB[1] = p;
    _RGB[2] = _HSV[2];
    break;
  case 5:
    _RGB[0] = _HSV[2];
    _RGB[1] = p;
    _RGB[2] = q;
    break;
  }
  
  //always set alpha to 1.0
  _RGB[3] = 1.0;
}


//----------------------------------------------------------------------------

/**
 * @brief Converts a color from RGB color space to HSV
 *
 * Red, green and blue are in the range [0, 1].
 *
 * @param[out] _RGB An array of 3 GLfloats, holding R, G and B
 * @param[in] _HSV An array of 3 GLfloats, holding H, S and V
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::RGBtoHSV(const Vec4f& _RGB, Vec4f& _HSV)
{
  double maxC = _RGB[2];
  if(maxC < _RGB[1])
    maxC = _RGB[1];
  if(maxC < _RGB[0])
    maxC = _RGB[0];
  double minC = _RGB[2];
  if(minC > _RGB[1])
    minC = _RGB[1];
  if(minC > _RGB[0])
    minC = _RGB[0];

  double delta = maxC - minC;

  double V = maxC;
  double S = 0;
  double H = 0;

  if(delta == 0)
  {
    H = 0;
    S = 0;
  }else{
    S = delta / maxC;
    double dR = 60 * (maxC - _RGB[0]) / delta + 180;
    double dG = 60 * (maxC - _RGB[1]) / delta + 180;
    double dB = 60 * (maxC - _RGB[2]) / delta + 180;
    if (_RGB[0] == maxC)
      H = dB - dG;
    else if (_RGB[1] == maxC)
      H = 120 + dR - dB;
    else
      H = 240 + dG - dR;
  }

  if(H < 0)
    H += 360;
  if(H >= 360)
    H -= 360;

  _HSV[0] = H;
  _HSV[1] = S;
  _HSV[2] = V;
}

//----------------------------------------------------------------------------

/** \brief Renders the nodes contents using the given render state and draw mode
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::draw(GLState& _state, DrawModes::DrawMode _drawMode)
{
  
  glDisable(GL_LIGHTING);
  
  glPushAttrib(GL_ENABLE_BIT);
  
  Pose *pose = skeleton_.pose(hAni_);
  typename Skeleton::Iterator it;
  
  // draw points
  //
  if (_drawMode == DrawModes::POINTS)
  {
    
    Vec4f jointColor;
    getJointColor(_state.base_color(), jointColor);
    
    // draw all joint positions
    glPointSize(12);
                
    glBegin(GL_POINTS);
    
    for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
    {
      //select joint color
      
      if ( (*it)->selected() )
        glColor3f(1.0, 0.0, 0.0);
      else{
        glColor3fv( &jointColor[0] );
      }

      //and draw the point
      glVertex( pose->globalTranslation( (*it)->id() ) );
    }
    glEnd();


    // draw the local coordinate frames
    if(bCoordFramesVisible_)
    {
      glEnable(GL_DEPTH_TEST);
      glPointSize(6.0);
      glBegin(GL_POINTS);
      for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
      {
        typename Skeleton::Matrix global = pose->globalMatrix( (*it)->id() );
        NormalizeCoordinateFrame(global);
        glColor3f(0.8, 0.2, 0.2);
        glVertex( global.transform_point(Point(fFrameSize_, 0, 0)) );
        glColor3f(0.2, 0.8, 0.2);
        glVertex( global.transform_point(Point(0, fFrameSize_, 0)) );
        glColor3f(0.2, 0.2, 0.8);
        glVertex( global.transform_point(Point(0, 0, fFrameSize_)) );
      }
      glEnd();
      glPointSize(_state.point_size());
    }
  }


  // draw bones
  //
  if ( (_drawMode == DrawModes::WIREFRAME)
    || (_drawMode == DrawModes::SOLID_FLAT_SHADED)
    || (_drawMode == DrawModes::SOLID_FACES_COLORED)
    || (_drawMode == DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) )
  {
    
    Vec4f baseColor = _state.base_color();
    
    if ( (_drawMode == DrawModes::SOLID_FLAT_SHADED)
      || (_drawMode == DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ){

      glShadeModel(GL_FLAT);
      glEnable(GL_LIGHTING);
    }
    
    if (_drawMode == DrawModes::SOLID_FACES_COLORED)
      glShadeModel(GL_FLAT);
    
    
    glLineWidth(1.5);
    
    // draw the bones
    for(it = skeleton_.begin(); it != skeleton_.end(); ++it) {

      //joint is the (unique) tail joint of the bone
      Joint* joint  = *it;
      Joint* parent = joint->parent();

      // root can be ignored
      // we only want to draw bones from (parent -> joint)
      if (parent == 0)
        continue;

      //select joint color
      glColor4fv( &baseColor[0] );

      Vec3d parentPos = pose->globalTranslation(parent->id());
      Vec3d jointPos  = pose->globalTranslation(joint->id());

      Vec3d boneVector = (jointPos - parentPos);

      draw_bone(_state, _drawMode, parentPos, boneVector);
    }


    // draw the local coordinate frames
    if(bCoordFramesVisible_)
    {
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_LIGHTING);

      glLineWidth(3.0);
      glBegin(GL_LINES);
      for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
      {
        unsigned int index = (*it)->id();
        typename Skeleton::Matrix global = pose->globalMatrix(index);
        NormalizeCoordinateFrame(global);
        glColor3f(0.8, 0.2, 0.2);
        glVertex( pose->globalTranslation(index));
        glVertex( global.transform_point(Point(fFrameSize_, 0, 0)) );
        glColor3f(0.2, 0.8, 0.2);
        glVertex( pose->globalTranslation(index));
        glVertex( global.transform_point(Point(0, fFrameSize_, 0)) );
        glColor3f(0.2, 0.2, 0.8);
        glVertex( pose->globalTranslation(index));
        glVertex( global.transform_point(Point(0, 0, fFrameSize_)) );
      }
      glEnd();
      glLineWidth(_state.line_width());
    }
  }


  glColor(_state.base_color());

  glPopAttrib();
}

//----------------------------------------------------------------------------

/** \brief get suitable color for the joint
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::getJointColor( const Vec4f& _baseColor, Vec4f& _result )
{
  Vec4f hsv;
  RGBtoHSV(_baseColor, hsv);
  
  hsv[0] += 0.4;
  if (hsv[0] > 1.0) hsv[0] -= 1.0;
  
  hsv[1] += 0.1;
  if (hsv[1] > 1.0) hsv[1] = 1.0;
  
  HSVtoRGB(hsv, _result);
}


//----------------------------------------------------------------------------

/** \brief Renders the node in picking mode, restricted to the node components given by \e _target
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::pick(GLState& _state, PickTarget _target)
{
  unsigned int n_of_vertices = skeleton_.jointCount();
  unsigned int n_of_edges = n_of_vertices - 1;

  switch (_target)
  {
  case PICK_VERTEX:
    _state.pick_set_maximum(n_of_vertices);
    pick_vertices(_state);
    break;

  case PICK_EDGE:
    _state.pick_set_maximum(n_of_vertices);
    pick_edges(_state);
    break;

  case PICK_ANYTHING:
    _state.pick_set_maximum(n_of_vertices);
    pick_vertices(_state);
    pick_edges(_state);
    break;

  default:
    break;
  }
}


//----------------------------------------------------------------------------

/** \brief Pick method for vertices
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::pick_vertices(GLState &_state)
{
  glPointSize(16.0);
  typename Skeleton::Pose* pose = skeleton_.pose(hAni_);
  for(typename Skeleton::Iterator it = skeleton_.begin(); it != skeleton_.end(); ++it)
  {
    typename Skeleton::Joint *joint = *it;
    _state.pick_set_name(joint->id());

    Vec3d p = pose->globalTranslation(joint->id());

    glBegin(GL_POINTS);
    glVertex(p);
    glEnd();
  }
}


//----------------------------------------------------------------------------

/** \brief Pick method for edges
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::pick_edges(GLState &_state)
{
  glLineWidth(10);
  typename Skeleton::Pose* pose = skeleton_.pose(hAni_);
  for(typename Skeleton::Iterator it = skeleton_.begin(); it != skeleton_.end(); ++it)
  {
    Joint *joint = *it;
    for(typename Joint::ChildIter it_ch = joint->begin(); it_ch != joint->end(); ++it_ch)
    {
      Joint *child = *it_ch;

      _state.pick_set_name(child->id());

      Vec3d p0 = pose->globalTranslation(joint->id()),
          p1 = pose->globalTranslation(child->id());

      glBegin(GL_LINES);
      glVertex(p0);
      glVertex(p1);
      glEnd();
    }
  }
}


//----------------------------------------------------------------------------

/** \brief Helper function to draw the bones
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::draw_bone(GLState &_state, DrawModes::DrawMode _drawMode, const Point& _parent, const Point& _axis)
{
 
  _state.push_modelview_matrix();

  Point midPoint = _parent + 0.1 * _axis;
  
  _state.translate(midPoint[0], midPoint[1], midPoint[2]);

  unsigned int slices(6);
  unsigned int stacks(1);

  Point  direction = _axis;
  Point  z_axis(0,0,1);
  Point  rot_normal;
  double rot_angle;

  direction.normalize();
  rot_angle  = acos((z_axis | direction))*180/M_PI;
  rot_normal = ((z_axis % direction).normalize());


  if(fabs(rot_angle) > 0.0001 && fabs(180-rot_angle) > 0.0001)
    _state.rotate(rot_angle, rot_normal[0], rot_normal[1], rot_normal[2]);
  else
    _state.rotate(rot_angle, 1, 0, 0);

  if (!quadricInitialized_)
    quadric_ = gluNewQuadric();

  if ( _drawMode == DrawModes::WIREFRAME ){
    gluQuadricDrawStyle(quadric_, GLU_LINE);
  } else {
    gluQuadricDrawStyle(quadric_, GLU_FILL);
  }

  gluQuadricNormals(quadric_, GLU_SMOOTH);    


  double boneLength = _axis.norm();
  double radius     = boneLength * 0.07;
  
  //draw the large cone from midPoint to the end of the bone
  gluCylinder(quadric_, radius, 0, boneLength * 0.9, slices, stacks);

  //rotate 180.0 and draw the the small cone from midPoint to the start
  _state.rotate(180.0, 1, 0, 0);  
  gluCylinder(quadric_, radius, 0, boneLength * 0.1, slices, stacks);
  
  _state.pop_modelview_matrix();
}


//----------------------------------------------------------------------------

/**
 * @brief Selects the active pose
 *
 * The active pose is the pose that is drawn to the viewport. If the animation handle is invalid (-1),
 * the default pose will be drawn.
 *
 * @warning Do not call this method directly, call SkeletonObject::setActivePose instead. The object has
 * to be notified, in order to properly update the text node positions.
 *
 * @param _hAni A handle to an animation, or an invalid handle for the default pose.
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::setActivePose(const AnimationHandle &_hAni)
{
  hAni_ = _hAni;
}


//----------------------------------------------------------------------------

/**
 * @brief Returns the active pose set with SkeletonNodeT::setActivePose
 */
template <class Skeleton>
AnimationHandle SkeletonNodeT<Skeleton>::activePose()
{
  return hAni_;
}


//----------------------------------------------------------------------------

/** \brief Toggle visibility of coordinate frames for all joints
 * 
 */
template <class Skeleton>
void SkeletonNodeT<Skeleton>::showCoordFrames(bool _bVisible)
{
	bCoordFramesVisible_ = _bVisible;
}

template <class Skeleton>
bool SkeletonNodeT<Skeleton>::coordFramesVisible()
{
  return bCoordFramesVisible_;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
