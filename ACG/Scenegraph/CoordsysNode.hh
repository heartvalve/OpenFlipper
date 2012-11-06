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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS PointNode
//
//=============================================================================


#ifndef ACG_COORDSYSNODE_HH
#define ACG_COORDSYSNODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "DrawModes.hh"
#include <ACG/GL/GLPrimitives.hh>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================


/** \class TextNode CoordsysNode.hh <ACG/Scenegraph/CoordsysNode.hh>

    CoordsysNode renders A coordinate system.
   
   TODO: Den Fall mode_ == POSITION implementieren. 25.11.08

**/

class ACGDLLEXPORT CoordsysNode : public BaseNode
{

public:
  
  /// projection mode
  enum ProjectionMode {
    ORTHOGRAPHIC_PROJECTION, //!< orthographic
    PERSPECTIVE_PROJECTION   //!< perspective
  };
  
  enum CoordsysMode
  {
    POSITION,   ///< Draws the Coordsys at the coordsys origin
    SCREENPOS   ///< Draws the Coordsys at the upper right position on the screen
  };

  /** default constructor
   * @param _parent Define the parent Node this node gets attached to
   * @param _name   Name of this Node
   * @param _mode   upper right of the screen or position based
   * @param _projectionMode Draw an orthogonal coordinate system or also enable projection mode
   */
  CoordsysNode(
      BaseNode* _parent = 0,
      std::string    _name = "<TextNode>",
      CoordsysMode   _mode = SCREENPOS,
      ProjectionMode _projectionMode = PERSPECTIVE_PROJECTION);

  /// destructor
  ~CoordsysNode();

  /// static name of this class
  ACG_CLASSNAME(CoordsysNode);

  /// return available draw modes
  ACG::SceneGraph::DrawModes::DrawMode  availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw Coordsys
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// add renderobjects for shader pipeline renderer
  void getRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat);
  
  /// draw Coordsys for object picking
  void pick(GLState& _state, PickTarget _target);
	
  /// set mode to either POSITION or SCREENPOS
  void setMode(const CoordsysMode _mode);
	
  /// set mode to either ORTHOGRAPHIC_PROJECTION or PERSPECTIVE_PROJECTION
  void setProjectionMode(const ProjectionMode _mode);
	
  /// set position of the coordsys
  void setPosition(const Vec3f& _pos);
	
  /// get current mode
  CoordsysMode getMode() const;
	
  /// get current projection mode
  ProjectionMode getProjectionMode() const;
	
  private:

    void drawCoordsys(GLState&  _state);
    void drawCoordsys(IRenderer* _renderer, RenderObject* _baseRO);
    void drawCoordsysPick(GLState&  _state);
    void clearPickArea(GLState&  _state, bool _draw, GLfloat _depth);
    void boundingCircle(std::vector<Vec2f> &_in, Vec2f &_center, float &_radius);

    CoordsysMode mode_;
    ProjectionMode projectionMode_;
	
    Vec3f pos3f_;


    ACG::GLSphere*   sphere_;
    ACG::GLCylinder* cylinder_;
    ACG::GLCone*     cone_;
    ACG::GLDisk*     disk_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_COORDSYSNODE_HH defined
