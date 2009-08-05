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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
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
    unsigned int availableDrawModes() const;

    /// update bounding box
    void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);

    /// draw Coordsys
    void draw(GLState& _state, unsigned int _drawMode);

    /// draw Coordsys for object picking
    void pick(GLState& _state, PickTarget _target);

    /// set position
    void setPosition(const Vec3f& _position, const Vec3f& _normal);

    /// set plane size
    void setSize(double _width, double _height);

    void manipulatorVisible( bool _visible );

    bool manipulatorVisible();

    /// get current position
    Vec3f position();

    /// get current normal
    Vec3f normal();

    /// get id of the object on which the plane operates
    int objectID();

    /// set id of the object on which the plane operates
    void objectID(int _id);

  private:

    void drawPlane(GLState&  _state);
    void drawManipulator(GLState&  _state);
    void drawPlanePick(GLState&  _state);
    void drawManipulatorPick(GLState&  _state);

    Vec3f position_;
    Vec3f normal_;
    Vec3f u_;
    Vec3f v_;

    float width_;
    float height_;

    int objectID_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_PLANENODE_HH defined
//=============================================================================
