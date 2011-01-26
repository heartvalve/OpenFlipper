/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef QTPLANESELECT_HH
#define QTPLANESELECT_HH

#include <ACG/GL/GLState.hh>
#include <ACG/Math/VectorT.hh>

#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Scenegraph/TransformNode.hh>
#include <ACG/Scenegraph/LineNode.hh>

#include <qnamespace.h>
#include <qgl.h>
#include <QObject>

#include <list>

class DLLEXPORT QtPlaneSelect : public QObject
{
    Q_OBJECT

    typedef ACG::Vec2i              Vec2i;
    typedef ACG::Vec2f              Vec2f;
    typedef ACG::Vec3d              Vec3d;

    /*******************************************************************************
             Initialization and deinitialization
     *******************************************************************************/
    public:
        QtPlaneSelect( ACG::GLState& glState );
        ~QtPlaneSelect();

    
    /*******************************************************************************
             Public slots.
     *******************************************************************************/
    public slots:

        /*******************************************************************************
                     Call this to notify the scissor about mouse actions.
         *******************************************************************************/
        void slotMouseEvent(QMouseEvent* _event);

        /*******************************************************************************
                     Call this to notify the scissor about mouse actions.
         *******************************************************************************/
        void slotKeyReleaseEvent(QKeyEvent* _event);

    /*******************************************************************************
             Signals ("callbacks")
     *******************************************************************************/
    signals:
        void signalTriggerCut( );
        void updateViewProxy();


    /*******************************************************************************
            Public accessors.
     *******************************************************************************/
    public:
        Vec3d                getNormal( ) { return normal; }
        Vec3d                getSourcePoint( ) { return sourcePoint3D; }
        unsigned int         getNode() { return nodeIdx_; };
        unsigned int         getTargetIndex() { return targetIdx_; };

    
    /*******************************************************************************
             Members
     *******************************************************************************/
    protected:
        ACG::GLState&       glState;
        Vec3d               sourcePoint3D;
        Vec3d               normal;
        unsigned int        nodeIdx_;
        unsigned int        targetIdx_;
        bool                isDragging;

        ACG::SceneGraph::TransformNode*     transformNode;
        ACG::SceneGraph::TransformNode*     transformNodeSource;
        ACG::SceneGraph::TransformNode*     transformNodeDrag;
        ACG::SceneGraph::LineNode*          lineNode;
};



#endif // QTPLANESELECT_HH
