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



//=============================================================================
//
//  CLASS GLPrimitive
//
//=============================================================================

#ifndef ACG_GLPRIMITIVES_HH
#define ACG_GLPRIMITIVES_HH


//== INCLUDES =================================================================


#include <ACG/Config/ACGDefines.hh>
#include <ACG/GL/GLState.hh>
#include <ACG/Math/VectorT.hh>
#include <ACG/GL/VertexDeclaration.hh>


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================

  
class ACGDLLEXPORT GLPrimitive {
public:
  GLPrimitive();
  virtual ~GLPrimitive();

  // bind vbo + gl draw call
  void draw();

  // add to deferred draw call to renderer
  void addToRenderer(class IRenderer* _renderer, struct RenderObject* _ro);

  // triangle count must be known before updateVBO
  virtual int getNumTriangles() = 0;

  enum NormalOrientation {OUTSIDE, INSIDE};

protected:

  // calls addTriangleToVBO to fill vertex buffer
  virtual void updateVBO() = 0;

  void addTriangleToVBO(const ACG::Vec3f* _p, const ACG::Vec3f* _n, const ACG::Vec2f* _tex);

  void bindVBO();

  bool checkVBO();

  void unBindVBO();

  bool vboDataInvalid_;

  NormalOrientation normalOrientation_;

private:

  void updateVBOData();

  int numTris_;
  float* vboData_;
  int curTriPtr_;

  VertexDeclaration vertexDecl_;

  unsigned int vbo_;
};

//------------------------------------------------------------------------

class ACGDLLEXPORT GLSphere: public GLPrimitive {
public:

  GLSphere(int _slices, int _stacks);
  ~GLSphere();

  void draw(GLState& _state, float _radius, const ACG::Vec3f& _center = ACG::Vec3f(0.0f, 0.0f, 0.0f));

  void addToRenderer(class IRenderer* _renderer, const struct RenderObject* _base, float _radius, const ACG::Vec3f& _center = ACG::Vec3f(0.0f, 0.0f, 0.0f));

  int getNumTriangles();

private:

  void updateVBO();

  void addTriangle(int sl0, int st0, int sl1, int st1, int sl2, int st2);

  ACG::Vec3f positionOnSphere(int _sliceNumber, int _stackNumber);
  ACG::Vec2f texCoordOnSphere(int _sliceNumber, int _stackNumber);

private:

  int slices_;
  int stacks_;

};

//------------------------------------------------------------------------

class ACGDLLEXPORT GLCone: public GLPrimitive {
public:

  GLCone(int _slices, int _stacks, float _bottomRadius, float _topRadius, bool _bottomCap_, bool _topCap);
  ~GLCone();

  void draw(
      GLState& _state,
      float _height,
      const ACG::Vec3f& _center = ACG::Vec3f(0.0f, 0.0f, 0.0f),
      ACG::Vec3f _upDir = ACG::Vec3f(0.0f, 0.0f, 1.0f));


  void addToRenderer(class IRenderer* _renderer, const struct RenderObject* _base, 
    float _height,
    const ACG::Vec3f& _center = ACG::Vec3f(0.0f, 0.0f, 0.0f),
    ACG::Vec3f _upDir = ACG::Vec3f(0.0f, 0.0f, 1.0f));

  int getNumTriangles();

  void updateVBO();
  void setBottomRadius(float _bottomRadius);
  void setTopRadius(float _topRadius);
  void setNormalOrientation(NormalOrientation orienation);

private:

  void addTriangle(int sl0, int st0, int sl1, int st1, int sl2, int st2);

  ACG::Vec3f positionOnCone(int _sliceNumber, int _stackNumber);
  ACG::Vec2f texCoordOnCone(int _sliceNumber, int _stackNumber);
  ACG::Vec3f normalOnCone(int _sliceNumber, int _stackNumber);

private:

  int slices_;
  int stacks_;

  float bottomRadius_;
  float topRadius_;

  bool bottomCap_;
  bool topCap_;

};

//------------------------------------------------------------------------

class ACGDLLEXPORT GLCylinder: public GLCone {
public:
  GLCylinder(int _slices, int _stacks, float _radius, bool _bottomCap, bool _topCap);
};

//------------------------------------------------------------------------

class ACGDLLEXPORT GLPartialDisk: public GLPrimitive {
public:
  GLPartialDisk(int _slices, int _loops, float _innerRadius, float _outerRadius, float _startAngle, float _sweepAngle);

  void setInnerRadius(float _innerRadius);
  void setOuterRadius(float _outerRadius);
  int getNumTriangles();

  void draw(
      GLState& _state,
      const ACG::Vec3f& _center = ACG::Vec3f(0.0f, 0.0f, 0.0f),
      ACG::Vec3f _upDir = ACG::Vec3f(0.0f, 0.0f, 1.0f));

protected:
  void updateVBO();

private:
  int slices_;
  int loops_;
  float innerRadius_;
  float outerRadius_;
  float startAngle_;
  float sweepAngle_;
};

//------------------------------------------------------------------------

class ACGDLLEXPORT GLDisk: public GLPartialDisk {
public:
  GLDisk(int _slices, int _loops, float _innerRadius, float _outerRadius);
};

//------------------------------------------------------------------------

} // namespace ACG

#endif // ACG_GLPRIMITIVES_HH defined
