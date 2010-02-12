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
//  CLASS PointNode
//
//=============================================================================

#ifndef ACG_PLANENODE_HH
#define ACG_PLANENODE_HH

//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "DrawModes.hh"
#include <vector>
#include "../Math/Matrix4x4T.hh"

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================


/** \class PlaneNode PlaneNode.hh <ACG/Scenegraph/PlaneNode.hh>

    PlaneNode renders a plane

**/

class ACGDLLEXPORT PlaneNode : public BaseNode
{

  public:

    /// constructor
    PlaneNode( BaseNode* _parent=0, std::string _name="<PlaneNode>" )
              : BaseNode(_parent, _name){};

    /// destructor
    ~PlaneNode() {};

    /// static name of this class
    ACG_CLASSNAME(PlaneNode);

    /// return available draw modes
    DrawModes::DrawMode availableDrawModes() const;

    /// update bounding box
    void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

    /// draw Plane
    void draw(GLState& _state, DrawModes::DrawMode _drawMode);

    /// draw Plane for object picking
    void pick(GLState& _state, PickTarget _target);

    /// set center position and normal of plane
    void setPosition(const Vec3f& _position, const Vec3f& _normal);

    /// set center position, local xdirection (multiplied with width)
    /// and local ydirection (multiplied with height)
    void setPosition(const Vec3f& _position, const Vec3f& _xDirection, const Vec3f& _yDirection);

    /// tranform the plane with given matrix
    void transform(const ACG::Matrix4x4d& _mat);

    /// set plane size
    void setSize(double _xDirection, double _yDirection);

    void manipulatorVisible( bool _visible );

    bool manipulatorVisible();

    /// get center position of the plane
    Vec3f position();

    /// get current normal
    Vec3f normal();

    /// local x direction (multiplied with width)
    Vec3f xDirection();

    /// local y direction (multiplied with height)
    Vec3f yDirection();

  private:

    void drawPlane(GLState&  _state);
    void drawManipulator(GLState&  _state);
    void drawPlanePick(GLState&  _state);
    void drawManipulatorPick(GLState&  _state);

    Vec3f position_;
    Vec3f normal_;
    Vec3f xDirection_;
    Vec3f yDirection_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_PLANENODE_HH defined
//=============================================================================
