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

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT

#ifndef OVM_PROPERTY_VISUALIZER_VECTOR_HH
#define OVM_PROPERTY_VISUALIZER_VECTOR_HH

#include "OVMPropertyVisualizer.hh"

#include <ACG/Scenegraph/LineNode.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <OpenVolumeMesh/Core/BaseEntities.hh>
#include <OpenVolumeMesh/Core/BaseProperty.hh>

#include "Widgets/VectorWidget.hh"

#include <iostream>

template <typename MeshT>
class OVMPropertyVisualizerVector: public OVMPropertyVisualizer<MeshT>{

public:
    OVMPropertyVisualizerVector(MeshT* _mesh, int objectID, PropertyInfo _propertyInfo);
    virtual ~OVMPropertyVisualizerVector(){ clear(); }

    virtual void clear();

protected:

    template<typename PropType, typename EntityIterator>
    void visualizeVectorAsColorForEntity(PropType prop, EntityIterator e_begin, EntityIterator e_end);
    virtual void duplicateProperty();

    virtual void visualizeFacePropAsStrokes();
    virtual void visualizeEdgePropAsStrokes();
    virtual void visualizeHalfedgePropAsStrokes();
    virtual void visualizeVertexPropAsStrokes();
    virtual void visualizeCellPropAsStrokes();
    virtual void visualizeHalffacePropAsStrokes();

    virtual void visualizeFaceProp(bool _setDrawMode = true);
    virtual void visualizeEdgeProp(bool _setDrawMode = true);
    virtual void visualizeHalfedgeProp(bool _setDrawMode = true);
    virtual void visualizeVertexProp(bool _setDrawMode = true);
    virtual void visualizeCellProp(bool _setDrawMode = true);
    virtual void visualizeHalffaceProp(bool _setDrawMode = true);

    virtual QString getPropertyText(unsigned int index);

    virtual void setCellPropertyFromText(unsigned int index, QString text);
    virtual void setFacePropertyFromText(unsigned int index, QString text);
    virtual void setHalffacePropertyFromText(unsigned int index, QString text);
    virtual void setEdgePropertyFromText(unsigned int index, QString text);
    virtual void setHalfedgePropertyFromText(unsigned int index, QString text);
    virtual void setVertexPropertyFromText(unsigned int index, QString text);

    ACG::SceneGraph::LineNode*  lineNode;

};

#if defined(INCLUDE_TEMPLATES) && !defined(OVM_PROPERTY_VISUALIZER_VECTOR_CC)
#include "OVMPropertyVisualizerVectorT.cc"
#endif

#endif /* OVM_PROPERTY_VISUALIZER_VECTOR_HH */

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
