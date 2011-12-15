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
*                                                                            *
 *   $Revision$                                                       *
 *   $LastChangedBy$                                                *
 *   $Date$                     *
 *                                                                            *
 \*===========================================================================*/

#include "GLPrimitives.hh"

namespace ACG {

//========================================================================
// GLPrimitive base class
//========================================================================

GLPrimitive::GLPrimitive() :
        numTris_(0),
        vboData_(0),
        curTriPtr_(0),
        vbo_(0)
{

}

//------------------------------------------------------------------------

GLPrimitive::~GLPrimitive()
{
  if (vbo_)
    glDeleteBuffersARB(1, &vbo_);

  delete[] vboData_;
}

//------------------------------------------------------------------------

void GLPrimitive::addTriangleToVBO(const ACG::Vec3f* _p, const ACG::Vec3f* _n, const ACG::Vec2f* _tex)
{
  if (!numTris_)
    numTris_ = getNumTriangles();

  if (!numTris_)
    return;

  if (!vboData_)
    vboData_ = new float[8 * 3 * numTris_];

  if (curTriPtr_ == numTris_)
    return;

  float* pTri = &vboData_[0] + (curTriPtr_++) * 3 * 8;

  // copy triangle
  for (int i = 0; i < 3; ++i) {
    for (int k = 0; k < 3; ++k)
      *(pTri++) = _p[i][k];

    for (int k = 0; k < 3; ++k)
      *(pTri++) = _n[i][k];

    for (int k = 0; k < 2; ++k)
      *(pTri++) = _tex[i][k];
  }
}

//------------------------------------------------------------------------

void GLPrimitive::bindVBO()
{
  if (!vbo_) {
    if (!vboData_ || !numTris_)
      return;

    // create vbo
    glGenBuffersARB(1, &vbo_);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, numTris_ * 3 * 8 * 4, vboData_, GL_STATIC_DRAW_ARB);

    delete[] vboData_;
    vboData_ = 0;
  } else
    glBindBufferARB(GL_ARRAY_BUFFER, vbo_);

  glVertexPointer(3, GL_FLOAT, 32, 0);
  glEnableClientState(GL_VERTEX_ARRAY);

  glNormalPointer(GL_FLOAT, 32, (GLvoid*) 12);
  glEnableClientState(GL_NORMAL_ARRAY);

  glClientActiveTexture(GL_TEXTURE0);
  glTexCoordPointer(2, GL_FLOAT, 32, (GLvoid*) 24);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

//------------------------------------------------------------------------

void GLPrimitive::draw()
{
  bindVBO();

  glDrawArrays(GL_TRIANGLES, 0, getNumTriangles() * 3);
}

//========================================================================
// GLSphere
//========================================================================



GLSphere::GLSphere(int _slices, int _stacks) :
        slices_(_slices),
        stacks_(_stacks)
{
  updateVBO();
}

//------------------------------------------------------------------------

GLSphere::~GLSphere()
{

}

//------------------------------------------------------------------------

void GLSphere::draw(GLState& _state, float _radius, const ACG::Vec3f& _center)
{
  _state.push_modelview_matrix();

  _state.translate(ACG::Vec3d(_center));
  _state.scale(_radius, _radius, _radius);

  GLPrimitive::draw();

  _state.pop_modelview_matrix();
}

//------------------------------------------------------------------------

int GLSphere::getNumTriangles()
{
  return 2 * slices_ + (stacks_ - 2) * slices_ * 2;
}

//------------------------------------------------------------------------

void GLSphere::updateVBO()
{
  for (int sl = 0; sl < slices_; ++sl) {
    // top triangle:
    {
      int st = 0;
      addTriangle(0, st, sl + 1, st + 1, sl, st + 1);
    }
    // middle quads:
    for (int st = 1; st < stacks_ - 1; ++st) {
      addTriangle(sl, st, sl + 1, st, sl, st + 1);
      addTriangle(sl + 1, st, sl + 1, st + 1, sl, st + 1);
    }
    // bottom triangle:
    {
      addTriangle(0, stacks_, sl, stacks_ - 1, sl + 1, stacks_ - 1);
    }
  }
}

//------------------------------------------------------------------------

void GLSphere::addTriangle(int sl0, int st0, int sl1, int st1, int sl2, int st2)
{
  ACG::Vec3f p[3];
  ACG::Vec3f n[3];
  ACG::Vec2f tex[3];

  n[0] = p[0] = positionOnSphere(sl0, st0);
  n[1] = p[1] = positionOnSphere(sl1, st1);
  n[2] = p[2] = positionOnSphere(sl2, st2);
  n[0].normalize();
  n[1].normalize();
  n[2].normalize();
  tex[0] = texCoordOnSphere(sl0, st0);
  tex[1] = texCoordOnSphere(sl1, st1);
  tex[2] = texCoordOnSphere(sl2, st2);

  addTriangleToVBO(p, n, tex);
}

//------------------------------------------------------------------------

ACG::Vec3f GLSphere::positionOnSphere(int _sliceNumber, int _stackNumber)
{
  ACG::Vec3f position;

  double alpha = (M_PI / double(stacks_)) * double(_stackNumber);
  double beta = ((2.0 * M_PI) / double(slices_)) * double(_sliceNumber);

  double ringRadius = sin(alpha);
  position[0] = sin(beta) * ringRadius;
  position[1] = cos(beta) * ringRadius;
  position[2] = cos(alpha);

  return position;
}

//------------------------------------------------------------------------

ACG::Vec2f GLSphere::texCoordOnSphere(int _sliceNumber, int _stackNumber)
{
  ACG::Vec2f texCoord;

  double alpha = (M_PI / double(stacks_)) * double(_stackNumber);
  texCoord[0] = double(_sliceNumber) / double(slices_);
  texCoord[1] = 0.5 * (cos(alpha) + 1.0);

  return texCoord;
}

//========================================================================
// GLCone
//========================================================================

GLCone::GLCone(int _slices, int _stacks, float _bottomRadius, float _topRadius, bool _bottomCap, bool _topCap) :
        slices_(_slices),
        stacks_(_stacks),
        bottomRadius_(_bottomRadius),
        topRadius_(_topRadius),
        bottomCap_(_bottomCap),
        topCap_(_topCap)
{
  updateVBO();
}

//------------------------------------------------------------------------

GLCone::~GLCone()
{

}

//------------------------------------------------------------------------

void GLCone::draw(GLState& _state, float _height, const ACG::Vec3f& _center, ACG::Vec3f _upDir)
{
//  ACG::mat4 mWorld = ACG::translate(ACG::mat4(1.0f), _center);

  _state.push_modelview_matrix();

  // translate
  _state.translate(ACG::Vec3d(_center));

  _upDir.normalize();

  // compute rotation matrix mAlign
  //  such that vBindDir rotates to _upDir
  ACG::GLMatrixd mAlign;
  mAlign.identity();

  ACG::Vec3f vBindDir(0.0f, 0.0f, 1.0f);

  ACG::Vec3f vRotAxis = OpenMesh::cross(_upDir, vBindDir);
  vRotAxis.normalize();

  ACG::Vec3f vUp = OpenMesh::cross(_upDir, vRotAxis);

  // rotate
  for (int i = 0; i < 3; ++i) {
    mAlign(i, 0) = vRotAxis[i];
    mAlign(i, 1) = vUp[i];
    mAlign(i, 2) = _upDir[i];
  }

  ACG::Vec3f vDelta = vBindDir - _upDir;
  if (fabsf(OpenMesh::dot(vDelta, vDelta) < 1e-3f))
    mAlign.identity();

  // scale
  mAlign.scale(1.0, 1.0, _height);

  ACG::GLMatrixd mAlignInv(mAlign);
  mAlignInv.invert();

  _state.mult_matrix(mAlign, mAlignInv);

  GLPrimitive::draw();

  _state.pop_modelview_matrix();
}

//------------------------------------------------------------------------

int GLCone::getNumTriangles()
{
  int numTris = stacks_ * slices_ * 2;

  if (bottomCap_)
    numTris += slices_;
  if (topCap_)
    numTris += slices_;

  return numTris;
}

//------------------------------------------------------------------------

ACG::Vec3f GLCone::positionOnCone(int _sliceNumber, int _stackNumber)
{
  ACG::Vec3f position;

  double beta = ((2.0 * M_PI) / slices_) * _sliceNumber;

  double relativeHeight = _stackNumber / (double) stacks_;
  double ringRadius = (1.0 - relativeHeight) * bottomRadius_ + relativeHeight * topRadius_;
  position[0] = sin(beta) * ringRadius;
  position[1] = cos(beta) * ringRadius;
  position[2] = relativeHeight;

  return position;
}

//------------------------------------------------------------------------

ACG::Vec2f GLCone::texCoordOnCone(int _sliceNumber, int _stackNumber)
{
  ACG::Vec2f texCoord;

  texCoord[0] = _sliceNumber / (double) slices_;
  texCoord[1] = _stackNumber / (double) stacks_;

  return texCoord;
}

//------------------------------------------------------------------------

ACG::Vec3f GLCone::normalOnCone(int _sliceNumber, int _stackNumber)
{
  ACG::Vec3f normal;

  double beta = ((2.0 * M_PI) / slices_) * _sliceNumber;
  double relativeHeight = _stackNumber / (double) stacks_;
  double ringRadius = (1.0 - relativeHeight) * bottomRadius_ + relativeHeight * topRadius_;

  normal[0] = sin(beta) * ringRadius;
  normal[1] = cos(beta) * ringRadius;
  normal[2] = (bottomRadius_ - topRadius_);

  normal.normalize();
  return normal;
}

//------------------------------------------------------------------------

void GLCone::addTriangle(int sl0, int st0, int sl1, int st1, int sl2, int st2)
{
  ACG::Vec3f p[3];
  ACG::Vec3f n[3];
  ACG::Vec2f tex[3];

  p[0] = positionOnCone(sl0, st0);
  p[1] = positionOnCone(sl1, st1);
  p[2] = positionOnCone(sl2, st2);
  n[0] = normalOnCone(sl0, st0);
  n[1] = normalOnCone(sl1, st1);
  n[2] = normalOnCone(sl2, st2);
  tex[0] = texCoordOnCone(sl0, st0);
  tex[1] = texCoordOnCone(sl1, st1);
  tex[2] = texCoordOnCone(sl2, st2);

  addTriangleToVBO(p, n, tex);
}

//------------------------------------------------------------------------

void GLCone::updateVBO()
{
  for (int sl = 0; sl < slices_; ++sl) {
    // top triangle:
    if (topCap_) {
      ACG::Vec3f p[3];
      ACG::Vec3f n[3];
      ACG::Vec2f tex[3];

      p[0] = ACG::Vec3f(0.0, 0.0, 1.0);
      p[1] = positionOnCone(sl + 1, stacks_);
      p[2] = positionOnCone(sl, stacks_);
      n[0] = ACG::Vec3f(0.0, 0.0, 1.0);
      n[1] = ACG::Vec3f(0.0, 0.0, 1.0);
      n[2] = ACG::Vec3f(0.0, 0.0, 1.0);

      tex[0] = ACG::Vec2f(0.5, 0.5);
      double beta = ((2.0 * M_PI) / slices_) * (sl + 1);
      tex[1] = ACG::Vec2f(sin(beta), cos(beta));
      beta = ((2.0 * M_PI) / slices_) * (sl);
      tex[2] = ACG::Vec2f(sin(beta), cos(beta));

      addTriangleToVBO(p, n, tex);
    }
    // middle quads:
    for (int st = 0; st < stacks_; ++st) {
      addTriangle(sl, st, sl, st + 1, sl + 1, st);
      addTriangle(sl + 1, st, sl, st + 1, sl + 1, st + 1);
    }
    // bottom triangle:
    if (bottomCap_) {
      ACG::Vec3f p[3];
      ACG::Vec3f n[3];
      ACG::Vec2f tex[3];

      p[0] = ACG::Vec3f(0.0, 0.0, 0.0);
      p[1] = positionOnCone(sl, 0);
      p[2] = positionOnCone(sl + 1, 0);
      n[0] = ACG::Vec3f(0.0, 0.0, -1.0);
      n[1] = ACG::Vec3f(0.0, 0.0, -1.0);
      n[2] = ACG::Vec3f(0.0, 0.0, -1.0);

      tex[0] = ACG::Vec2f(0.5, 0.5);
      double beta = ((2.0 * M_PI) / slices_) * (sl);
      tex[1] = ACG::Vec2f(sin(beta), cos(beta));
      beta = ((2.0 * M_PI) / slices_) * (sl + 1);
      tex[2] = ACG::Vec2f(sin(beta), cos(beta));

      addTriangleToVBO(p, n, tex);
    }
  }
}

//========================================================================
// GLCylinder
//========================================================================

GLCylinder::GLCylinder(int _slices, int _stacks, float _radius, bool _bottomCap, bool _topCap) :
        GLCone(_slices, _stacks, _radius, _radius, _bottomCap, _topCap)
{
}

//------------------------------------------------------------------------

}
