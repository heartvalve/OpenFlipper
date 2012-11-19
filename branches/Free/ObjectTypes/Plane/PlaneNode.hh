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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

//=============================================================================
//
//  CLASS PointNode
//
//=============================================================================

#ifndef PLANENODE_HH
#define PLANENODE_HH

//== INCLUDES =================================================================

#include <ObjectTypes/Plane/PlaneType.hh>
#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Math/Matrix4x4T.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/IRenderer.hh>

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================

class DLLEXPORT PlaneNode : public ACG::SceneGraph::BaseNode
{
public:
    /** \brief Construct a plane rendering node
     *
     * @param _parent The parent node in the scenegraph
     * @param _name   The name of the new node (visible in the scenegraph dialogs)
     * @param _plane  A pointer to an existing plane
     */
    PlaneNode(Plane& _plane, BaseNode *_parent = 0, std::string _name = "<PlaneNode>");

    /// destructor
    ~PlaneNode();

    /// static name of this class
    ACG_CLASSNAME(PlaneNode);

    /// return available draw modes
    ACG::SceneGraph::DrawModes::DrawMode availableDrawModes() const;

    /// update bounding box
    void boundingBox(ACG::Vec3d & _bbMin, ACG::Vec3d & _bbMax);

    /// draw Plane
    void draw(ACG::GLState & _state, const ACG::SceneGraph::DrawModes::DrawMode & _drawMode);

    /// draw Plane for object picking
    void pick(ACG::GLState & _state, ACG::SceneGraph::PickTarget _target);

    /// get center position of the plane
    ACG::Vec3d position();

    /// get current normal
    ACG::Vec3d normal();

    /// local x direction (multiplied with width)
    ACG::Vec3d xDirection();

    /// local y direction (multiplied with height)
    ACG::Vec3d yDirection();

    /// Get the currently rendered plane
    Plane& getPlane();

    /// Set a new plane for rendering
    void setPlane(Plane plane);

    /** \brief Add the objects to the given renderer
     *
     * @param _renderer The renderer which will be used. Add your geometry into this class
     * @param _state    The current GL State when this object is called
     * @param _drawMode The active draw mode
     * @param _mat      Current material
     */
    void getRenderObjects(ACG::IRenderer* _renderer, ACG::GLState&  _state , const ACG::SceneGraph::DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat);

private:
    void drawPlane(ACG::GLState & _state);
    void drawManipulator(ACG::GLState & _state);
    void drawPlanePick(ACG::GLState & _state);
    void drawManipulatorPick(ACG::GLState & _state);

    Plane& plane_;

    unsigned int vbo_;

    ACG::VertexDeclaration vertexDecl_;

};

//=============================================================================
#endif // ACG_PLANENODE_HH defined
//=============================================================================
