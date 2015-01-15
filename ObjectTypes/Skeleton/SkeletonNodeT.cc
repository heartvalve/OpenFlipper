/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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




//=============================================================================
//
//  CLASS SkeletonNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_SKELETONNODET_C

//== INCLUDES =================================================================

#include "SkeletonNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/GL/IRenderer.hh>
#include <vector>
#include <deque>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================


/** \brief Constructor
 * 
 */
template <class SkeletonType>
SkeletonNodeT<SkeletonType>::SkeletonNodeT(SkeletonType &_skeleton, BaseNode *_parent, std::string _name) :
               BaseNode(_parent, _name),
               bCoordFramesVisible_(false),
               skeleton_(_skeleton),
               hAni_(),
               fFrameSize_(0.0),
               slices_(6),
               stacks_(1)
{
  this->multipassNodeSetActive(3, true);

  sphere_ = new ACG::GLSphere(10, 10);
  cone_ = new ACG::GLCone(slices_, stacks_, 1.0f, 0.0f, false, false);
  cylinder_ = new ACG::GLCylinder(10, 2, 1.0f, true, true);
}


//------------------------------------------------------------------------------

/** \brief Destructor
 * 
 */
template <class SkeletonType>
SkeletonNodeT<SkeletonType>::~SkeletonNodeT()
{
  if (sphere_)
    delete sphere_;

  if (cone_)
    delete cone_;

  if (cylinder_)
    delete cylinder_;
}


//------------------------------------------------------------------------------

/** \brief Returns a pointer to the skeleton
 * 
 */
template <class SkeletonType>
SkeletonType& SkeletonNodeT<SkeletonType>::skeleton()
{
  return skeleton_;
}


//------------------------------------------------------------------------------

/** \brief Returns the bounding box of this node
 * 
 */
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{  
  if(skeleton_.jointCount() == 0)
    return; // no joints, no contribution to the bound box

  Pose *pose = skeleton_.pose(hAni_);
  typename SkeletonType::Iterator it;
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
template <class SkeletonType>
DrawModes::DrawMode SkeletonNodeT<SkeletonType>::availableDrawModes() const
{
  return (DrawModes::WIREFRAME           | DrawModes::POINTS | DrawModes::SOLID_FLAT_SHADED
        | DrawModes::SOLID_FACES_COLORED | DrawModes::SOLID_FACES_COLORED_FLAT_SHADED);
}


//----------------------------------------------------------------------------


template <class SkeletonType>
double SkeletonNodeT<SkeletonType>::frameSize() {
	return fFrameSize_;
}


//----------------------------------------------------------------------------


/** \brief Normalizes a coordinate frame defined by the given matrix
 * 
 */
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::NormalizeCoordinateFrame(Matrix &_mat)
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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::HSVtoRGB(const Vec4f& _HSV, Vec4f& _RGB)
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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::RGBtoHSV(const Vec4f& _RGB, Vec4f& _HSV)
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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
  
  ACG::GLState::disable(GL_LIGHTING);
  
  glPushAttrib(GL_ENABLE_BIT);
  
  Pose *pose = skeleton_.pose(hAni_);
  typename SkeletonType::Iterator it;
  


  // draw points
  //
  if (_drawMode & DrawModes::POINTS)
  {
    Vec4f jointColor;
    getJointColor(_state.diffuse_color(), jointColor);
    
    // draw all joint positions
    glPointSize(12);

    //we will set the specular color, otherwise the color cannot be seen
    ACG::Vec4f oldSpecular = _state.specular_color();
    ACG::Vec4f oldDiffuse  = _state.diffuse_color();
                
    for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
    {

      // If the vertex is selected, it will be always red
      if ( (*it)->selected() )
      {
        _state.set_color(ACG::Vec4f(1.0f, 0.0f, 0.0f ,1.0f));
      	_state.set_diffuse_color(ACG::Vec4f(1.0f, 0.0f, 0.0f ,1.0f));
      	_state.set_specular_color(ACG::Vec4f(1.0f, 0.0f, 0.0f ,1.0f));
      }
      else {
        // If it is the root joint, it will get some kind of orange color
        // Otherwise the the Base color is used
        if ( (*it)->isRoot() )
        {
        	ACG::Vec4f root_color = ACG::Vec4f(1.0f, 0.66f, 0.0f ,1.0f);
        	_state.set_color(root_color);
        	_state.set_diffuse_color(root_color);
        	_state.set_specular_color(root_color);
        }
        else {
        	_state.set_color(ACG::Vec4f(jointColor[0], jointColor[1] , jointColor[2], 1.0));
        	_state.set_ambient_color(ACG::Vec4f(jointColor[0], jointColor[1] , jointColor[2], 1.0));
        	_state.set_diffuse_color(ACG::Vec4f(jointColor[0], jointColor[1] , jointColor[2], 1.0));
        	_state.set_specular_color(ACG::Vec4f(jointColor[0], jointColor[1] , jointColor[2], 1.0));
        }
      }


      // Simulate glPointSize() with a sphere

      const double sphereSize = unprojectPointSize((double)_state.point_size(),
                  pose->globalTranslation( (*it)->id() ), _state);

      sphere_->draw(_state,sphereSize,ACG::Vec3f(pose->globalTranslation( (*it)->id() )));

    }

    _state.set_specular_color(oldSpecular);
    _state.set_diffuse_color(oldDiffuse);


    // draw the local coordinate frames
    if(bCoordFramesVisible_)
    {
      ACG::GLState::enable(GL_DEPTH_TEST);
      glPointSize(6.0);
      glBegin(GL_POINTS);
      for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
      {
        typename SkeletonType::Matrix global = pose->globalMatrix( (*it)->id() );
        NormalizeCoordinateFrame(global);
        glColor3f(0.8f, 0.2f, 0.2f);
        glVertex( global.transform_point(Point(fFrameSize_, 0, 0)) );
        glColor3f(0.2f, 0.8f, 0.2f);
        glVertex( global.transform_point(Point(0, fFrameSize_, 0)) );
        glColor3f(0.2f, 0.2f, 0.8f);
        glVertex( global.transform_point(Point(0, 0, fFrameSize_)) );
      }
      glEnd();
      glPointSize(_state.point_size());
    }
  }


  // draw bones
  //
  if ( (_drawMode & DrawModes::WIREFRAME)
    || (_drawMode & DrawModes::SOLID_FLAT_SHADED)
    || (_drawMode & DrawModes::SOLID_FACES_COLORED)
    || (_drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) )
  {

    if ( (_drawMode & DrawModes::SOLID_FLAT_SHADED)
      || (_drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ){

      ACG::GLState::shadeModel(GL_FLAT);
      ACG::GLState::enable(GL_LIGHTING);
    }
    
    if (_drawMode & DrawModes::SOLID_FACES_COLORED)
      ACG::GLState::shadeModel(GL_FLAT);
    
    
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
      Vec4f baseColor = _state.ambient_color();
      glColor4fv( &baseColor[0] );

      Vec3d parentPos = pose->globalTranslation(parent->id());
      Vec3d jointPos  = pose->globalTranslation(joint->id());

      Vec3d boneVector = (jointPos - parentPos);

      draw_bone(_state, _drawMode, parentPos, boneVector);
    }


    // draw the local coordinate frames
    if(bCoordFramesVisible_)
    {
      ACG::GLState::disable(GL_COLOR_MATERIAL);
      ACG::GLState::disable(GL_LIGHTING);

      glLineWidth(3.0);
      glBegin(GL_LINES);
      for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
      {
        unsigned int index = (*it)->id();
        typename SkeletonType::Matrix global = pose->globalMatrix(index);
        NormalizeCoordinateFrame(global);
        glColor3f(0.8f, 0.2f, 0.2f);
        glVertex( pose->globalTranslation(index));
        glVertex( global.transform_point(Point(fFrameSize_, 0, 0)) );
        glColor3f(0.2f, 0.8f, 0.2f);
        glVertex( pose->globalTranslation(index));
        glVertex( global.transform_point(Point(0, fFrameSize_, 0)) );
        glColor3f(0.2f, 0.2f, 0.8f);
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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::getJointColor( const Vec4f& _baseColor, Vec4f& _result )
{
  Vec4f hsv;
  RGBtoHSV(_baseColor, hsv);
  
  hsv[0] += 0.4f;
  if (hsv[0] > 1.0) hsv[0] -= 1.0;
  
  hsv[1] += 0.1f;
  if (hsv[1] > 1.0) hsv[1] = 1.0;
  
  HSVtoRGB(hsv, _result);
}


//----------------------------------------------------------------------------

/** \brief Renders the node in picking mode, restricted to the node components given by \e _target
 * 
 */
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::pick(GLState& _state, PickTarget _target)
{
  unsigned int n_of_vertices = skeleton_.jointCount();

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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::pick_vertices(GLState &_state)
{
  glPointSize(16.0);
  typename SkeletonType::Pose* pose = skeleton_.pose(hAni_);
  for(typename SkeletonType::Iterator it = skeleton_.begin(); it != skeleton_.end(); ++it)
  {
    typename SkeletonType::Joint *joint = *it;
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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::pick_edges(GLState &_state)
{
  glLineWidth(10);
  typename SkeletonType::Pose* pose = skeleton_.pose(hAni_);
  for(typename SkeletonType::Iterator it = skeleton_.begin(); it != skeleton_.end(); ++it)
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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::draw_bone(GLState &_state, DrawModes::DrawMode _drawMode, const Point& _parent, const Point& _axis)
{
  _state.push_modelview_matrix();

  // compute modelview matrix of each cone
  GLMatrixf mv0, mv1;
  mv0 = mv1 = _state.modelview();

  computeConeMatrices(_state.modelview(), _parent, _axis, &mv0, &mv1);

  // draw cones
  _state.set_modelview(mv0);
  cone_->draw_primitive();

  _state.set_modelview(mv1);
  cone_->draw_primitive();
  
  _state.pop_modelview_matrix();
}


//----------------------------------------------------------------------------


/** \brief Helper function to create a renderobject for bones
 * 
 */
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::addBoneToRenderer(IRenderer* _renderer, RenderObject& _base, const Point& _parent, const Point& _axis)
{
  // define cone mesh data
  _base.vertexDecl = cone_->getVertexDecl();
  _base.vertexBuffer = cone_->getVBO();
  _base.glDrawArrays(GL_TRIANGLES, 0, cone_->getNumTriangles() * 3);

  // save previous modelview transform
  GLMatrixf prevTransform = _base.modelview;

  // build modelview matrix for each cone
  GLMatrixf mv0, mv1;
  computeConeMatrices(prevTransform, _parent, _axis, &mv0, &mv1);

  // add cone objects
  _base.modelview = mv0;
  _renderer->addRenderObject(&_base);

  _base.modelview = mv1;
  _renderer->addRenderObject(&_base);

  // restore previous modelview matrix
  _base.modelview = prevTransform;
}


template <class SkeletonType>
void ACG::SceneGraph::SkeletonNodeT<SkeletonType>::computeConeMatrices( const GLMatrixf& _modelView, const Point& _parent, const Point& _axis, GLMatrixf* _outCone0, GLMatrixf* _outCone1 )
{
  Point midPoint = _parent + 0.1 * _axis;

  *_outCone0 = _modelView;

  _outCone0->translate(midPoint[0], midPoint[1], midPoint[2]);

  Point  direction = _axis;
  Point  z_axis(0,0,1);
  Point  rot_normal;
  double rot_angle;

  direction.normalize();
  rot_angle  = acos((z_axis | direction))*180/M_PI;
  rot_normal = ((z_axis % direction).normalize());


  if(fabs(rot_angle) > 0.0001 && fabs(180-rot_angle) > 0.0001)
    _outCone0->rotate(rot_angle, rot_normal[0], rot_normal[1], rot_normal[2]);
  else
    _outCone0->rotate(rot_angle, 1, 0, 0);

  double boneLength = _axis.norm();
  double radius     = boneLength * 0.07;

  *_outCone1 = *_outCone0;

  //draw the large cone from midPoint to the end of the bone
  _outCone0->scale(radius, radius, boneLength*0.9);

  //rotate 180.0 and draw the the small cone from midPoint to the start
  _outCone1->scale(radius, radius, boneLength*0.1);
  _outCone1->rotateX(180.0f);
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
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::setActivePose(const AnimationHandle &_hAni)
{
  hAni_ = _hAni;
}


//----------------------------------------------------------------------------

/**
 * @brief Returns the active pose set with SkeletonNodeT::setActivePose
 */
template <class SkeletonType>
AnimationHandle SkeletonNodeT<SkeletonType>::activePose()
{
  return hAni_;
}


//----------------------------------------------------------------------------

/** \brief Toggle visibility of coordinate frames for all joints
 * 
 */
template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::showCoordFrames(bool _bVisible)
{
	bCoordFramesVisible_ = _bVisible;
}

template <class SkeletonType>
bool SkeletonNodeT<SkeletonType>::coordFramesVisible()
{
  return bCoordFramesVisible_;
}

//----------------------------------------------------------------------------

/** \brief Compute the radius of a sphere simulating glPointSize after projection.
 * 
 */
template <class SkeletonType>
double SkeletonNodeT<SkeletonType>::unprojectPointSize(double _pointSize, const Vec3d& _point, GLState& _state)
{
  // 1. Project point to screen
  ACG::Vec3d projected = _state.project( _point );

  // 2. Shift it by the requested point size
  //    glPointSize defines the diameter but we want the radius, so we divide it by two
  ACG::Vec3d shifted = projected;
  shifted[0] = shifted[0] + _pointSize / 2.0 ;

  // 3. un-project into 3D
  ACG::Vec3d unProjectedShifted = _state.unproject( shifted );

  // 4. The difference vector defines the radius in 3D for the sphere
  ACG::Vec3d difference = unProjectedShifted - _point;

  return difference.norm();
}


//----------------------------------------------------------------------------

/**
 * @brief Generates renderobjects instead of direct draw calls for better rendering management.
 */

template <class SkeletonType>
void SkeletonNodeT<SkeletonType>::getRenderObjects(IRenderer* _renderer, 
                                                   GLState& _state,
                                                   const DrawModes::DrawMode& _drawMode,
                                                   const Material* _mat)
{
  RenderObject ro;
  ro.initFromState(&_state);

  // render states
  ro.depthTest = true;
  ro.depthWrite = true;
  
  ro.culling = true;
  ro.blending = false;


  Pose *pose = skeleton_.pose(hAni_);
  typename SkeletonType::Iterator it;

  Vec4f jointColor;
  getJointColor(_mat->diffuseColor(), jointColor);


  const int numJoints = skeleton_.jointCount();

  // draw points
  for (unsigned int i = 0; i < _drawMode.getNumLayers(); ++i)
  {
    const DrawModes::DrawModeProperties* props = _drawMode.getLayer(i);

    switch (props->primitive())
    {

    case DrawModes::PRIMITIVE_POINT:
      {
        ro.debugName = "SkeletonNode.point";
        ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

        if (VertexDeclaration::supportsInstancedArrays())
        {
          /*
          per instance data:

          combined modelview matrix: 3x float4 rows
          vertex color = emissive as rgba8_unorm
          */

          const int instanceDataSize = 4*3*4 + 4; // float4x3 + uint
          const int instanceBufSize = numJoints * instanceDataSize;
          if (numJoints)
          {
            const int numFloats = instanceBufSize/4;
            std::vector<float> instanceData(numFloats);

            // compute per instance data
            int instanceDataOffset = 0;

            for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
            {
              Vec4f vcolor = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);

              // If the vertex is selected, it will be always red
              // If it is not selected,
              if ( (*it)->selected() )
                vcolor = Vec4f(1.0f, 0.0f, 0.0f, 1.0f);
              else {
                // If it is the root joint, it will get some kind of orange color
                // Otherwise the the Base color is used
                if ( (*it)->isRoot() )
                  vcolor = Vec4f(1.0f,0.66f, 0.0f, 1.0f);
                else
                  vcolor = jointColor;
              }

              Vec3d globalPosD = pose->globalTranslation( (*it)->id() );

              float sphereSize = float(unprojectPointSize((double)_state.point_size(), 
                globalPosD,
                _state));

              GLMatrixf modelview = ro.modelview;
              modelview.translate(globalPosD[0], globalPosD[1], globalPosD[2]);
              modelview.scale(sphereSize, sphereSize, sphereSize);

              // store matrix
              for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 4; ++c)
                  instanceData[instanceDataOffset++] = modelview(r,c);

              // store color
              unsigned int uicolor = 0xff000000;
              uicolor |= (unsigned int)(vcolor[0] * 255.0f) & 0x000000ff;
              uicolor |= ((unsigned int)(vcolor[1] * 255.0f) << 8)  & 0x0000ff00;
              uicolor |= ((unsigned int)(vcolor[2] * 255.0f) << 16) & 0x00ff0000;

              // union instead of pointer casting
              union ufunion
              {
                unsigned int u;
                float f;
              } uitofloat;

              uitofloat.u = uicolor;

              instanceData[instanceDataOffset++] = uitofloat.f;
            }

            // store instance data in vbo
            pointInstanceData_.bind();
            pointInstanceData_.upload(instanceBufSize, &instanceData[0], GL_DYNAMIC_DRAW);
            pointInstanceData_.unbind();


            // init declaration
            if (!pointInstanceDecl_.getNumElements())
            {
              pointInstanceDecl_ = *sphere_->getVertexDecl();

              pointInstanceDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, size_t(0), "inModelView0", 1, pointInstanceData_.id());
              pointInstanceDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, size_t(0), "inModelView1", 1, pointInstanceData_.id());
              pointInstanceDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, size_t(0), "inModelView2", 1, pointInstanceData_.id());
              pointInstanceDecl_.addElement(GL_UNSIGNED_BYTE, 4, VERTEX_USAGE_COLOR, size_t(0), 0, 1, pointInstanceData_.id());
            }

            ro.shaderDesc.vertexColors = true;
            ro.shaderDesc.vertexTemplateFile = "Skeleton/instanced_vs.glsl";

            ro.vertexDecl = &pointInstanceDecl_;
            ro.vertexBuffer = sphere_->getVBO();
            ro.glDrawArraysInstanced(GL_TRIANGLES, 0, sphere_->getNumTriangles() * 3, numJoints);

            _renderer->addRenderObject(&ro);

            ro.shaderDesc.vertexColors = false;
            ro.numInstances = 0;
            ro.shaderDesc.vertexTemplateFile = "";
          }
        }
        else
        {
          // create a separate renderobject for each joint

          for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
          {
            // If the vertex is selected, it will be always red
            // If it is not selected,
            if ( (*it)->selected() )
              ro.emissive = Vec3f(1.0f, 0.0f, 0.0f);
            else {
              // If it is the root joint, it will get some kind of orange color
              // Otherwise the the Base color is used
              if ( (*it)->isRoot() )
                ro.emissive = Vec3f(1.0f,0.66f, 0.0f);
              else
                ro.emissive = Vec3f(jointColor[0], jointColor[1] , jointColor[2]);
            }


            // simulate glPointSize( ) with sphere

            const double sphereSize = unprojectPointSize((double)_state.point_size(), 
              pose->globalTranslation( (*it)->id() ),
              _state);

            sphere_->addToRenderer(_renderer, &ro, sphereSize, ACG::Vec3f(pose->globalTranslation( (*it)->id() )));
          }
        }


      } break;


    case DrawModes::PRIMITIVE_POLYGON:
      {
        ro.debugName = "SkeletonNode.bone"; 

        ro.setupShaderGenFromDrawmode(props);

        ro.setMaterial(_mat);
        ACG::Vec4f baseColor = _state.ambient_color();
        ro.emissive = ACG::Vec3f(baseColor[0],baseColor[1],baseColor[2]);



        if (VertexDeclaration::supportsInstancedArrays())
        {
          /*
          per instance data:

          combined modelview matrix: 3x float4 rows
          inverse transpose of modelview: 3x float3 rows
          */

          const int instanceDataFloats = 3*4 + 3*3;
          const int instanceDataSize = instanceDataFloats * 4;
          const int instanceBufSize = 2 * numJoints * instanceDataSize; // 2 cones per bone
          if (numJoints)
          {
            const int numFloats = instanceBufSize/4;
            std::vector<float> instanceData(numFloats);

            // compute per instance data
            int instanceDataOffset = 0;
            GLMatrixf cone0, cone1, cone0IT, cone1IT;

            for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
            {
              //joint is the (unique) tail joint of the bone
              Joint* joint  = *it;
              Joint* parent = joint->parent();

              // root can be ignored
              // we only want to draw bones from (parent -> joint)
              if (parent == 0)
                continue;

              Vec3d parentPos = pose->globalTranslation(parent->id());
              Vec3d jointPos  = pose->globalTranslation(joint->id());

              Vec3d boneVector = (jointPos - parentPos);

              // compute cone modelview matrices
              computeConeMatrices(ro.modelview, parentPos, boneVector, &cone0, &cone1);

              // compute inverse transpose for normal transform
              cone0IT = cone0;
              cone1IT = cone1;
              cone0IT.invert();
              cone1IT.invert();
            
              // store matrices
              for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 4; ++c)
                  instanceData[instanceDataOffset++] = cone0(r,c);

              for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c)
                  instanceData[instanceDataOffset++] = cone0IT(c,r);

              for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 4; ++c)
                  instanceData[instanceDataOffset++] = cone1(r,c);

              for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c)
                  instanceData[instanceDataOffset++] = cone1IT(c,r);
            }

            const int numBones = instanceDataOffset / instanceDataFloats;

            // store instance data in vbo
            boneInstanceData_.bind();
            boneInstanceData_.upload(instanceDataOffset * 4, &instanceData[0], GL_DYNAMIC_DRAW);
            boneInstanceData_.unbind();


            // init declaration

            if (!boneInstanceDecl_.getNumElements())
            {
              boneInstanceDecl_ = *cone_->getVertexDecl();

              boneInstanceDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, (size_t)0, "inModelView0", 1, boneInstanceData_.id());
              boneInstanceDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, (size_t)0, "inModelView1", 1, boneInstanceData_.id());
              boneInstanceDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_SHADER_INPUT, (size_t)0, "inModelView2", 1, boneInstanceData_.id());
              boneInstanceDecl_.addElement(GL_FLOAT, 3, VERTEX_USAGE_SHADER_INPUT, (size_t)0, "inModelViewIT0", 1, boneInstanceData_.id());
              boneInstanceDecl_.addElement(GL_FLOAT, 3, VERTEX_USAGE_SHADER_INPUT, (size_t)0, "inModelViewIT1", 1, boneInstanceData_.id());
              boneInstanceDecl_.addElement(GL_FLOAT, 3, VERTEX_USAGE_SHADER_INPUT, (size_t)0, "inModelViewIT2", 1, boneInstanceData_.id());
            }

            ro.vertexDecl = &boneInstanceDecl_;
            ro.vertexBuffer = cone_->getVBO();
            ro.glDrawArraysInstanced(GL_TRIANGLES, 0, cone_->getNumTriangles() * 3, numBones);

            ro.shaderDesc.vertexTemplateFile = "Skeleton/instanced_wvit_vs.glsl";

            _renderer->addRenderObject(&ro);

            ro.numInstances = 0;
            ro.shaderDesc.vertexTemplateFile = "";
          }
        }
        else
        {
          // create separate renderobject for each bone

          for(it = skeleton_.begin(); it != skeleton_.end(); ++it) {

            //joint is the (unique) tail joint of the bone
            Joint* joint  = *it;
            Joint* parent = joint->parent();

            // root can be ignored
            // we only want to draw bones from (parent -> joint)
            if (parent == 0)
              continue;

            Vec3d parentPos = pose->globalTranslation(parent->id());
            Vec3d jointPos  = pose->globalTranslation(joint->id());

            Vec3d boneVector = (jointPos - parentPos);

            addBoneToRenderer(_renderer, ro, parentPos, boneVector);
          }
        }

      } break;

    default: break;
    }
  }


  // draw coordframes

  if (bCoordFramesVisible_)
  {
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

    for(it = skeleton_.begin(); it != skeleton_.end(); ++it)
    {
      unsigned int index = (*it)->id();
      typename SkeletonType::Matrix global = pose->globalMatrix(index);
      NormalizeCoordinateFrame(global);

      // color for each arrow
      Vec3f colors[] = {
        Vec3f(0.8f, 0.2f, 0.2f),
        Vec3f(0.2f, 0.8f, 0.2f),
        Vec3f(0.2f, 0.2f, 0.8f)
      };

      Point points[] = {
        Point(fFrameSize_, 0, 0),
        Point(0, fFrameSize_, 0),
        Point(0, 0, fFrameSize_)
      };


      // simulate glLineWidth(3) with cylinder
      float lineWidth = (float)unprojectPointSize(3.0f, pose->globalTranslation(index), _state);

      // glPointSize(6)
      float sphereSize = (float)unprojectPointSize(6.0f, pose->globalTranslation(index), _state);

      // draw coordframe arrows
      for (int i = 0; i < 3; ++i)
      {
        ro.emissive = colors[i];

        // cylinder start and end points
        Vec3f vstart = (Vec3f)pose->globalTranslation(index);
        Vec3f vend = (Vec3f)global.transform_point(points[i]);

        Vec3f vdir = vend - vstart;
        float height = vdir.length();

        cylinder_->addToRenderer(_renderer, &ro, height, vstart, vdir, lineWidth);
        sphere_->addToRenderer(_renderer, &ro, sphereSize, vend);
      }
    }



  }


}






//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
