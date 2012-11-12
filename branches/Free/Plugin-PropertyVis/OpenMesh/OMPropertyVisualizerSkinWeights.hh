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

#ifdef ENABLE_SKELETON_SUPPORT

#ifndef OM_PROPERTY_VISUALIZER_SKIN_WEIGHTS_HH
#define OM_PROPERTY_VISUALIZER_SKIN_WEIGHTS_HH

#include "OMPropertyVisualizer.hh"

#include "Widgets/SkinWeightsWidget.hh"

#include <ObjectTypes/Skeleton/BaseSkin.hh>

#include <iostream>

template <typename MeshT>
class OMPropertyVisualizerSkinWeights: public OMPropertyVisualizer<MeshT>{

public:
    OMPropertyVisualizerSkinWeights(MeshT* _mesh, PropertyInfo _propertyInfo);
    virtual ~OMPropertyVisualizerSkinWeights(){}

protected:

    virtual void visualizeFaceProp();
    virtual void visualizeEdgeProp();
    virtual void visualizeHalfedgeProp();
    virtual void visualizeVertexProp();

    QString getPropertyText(unsigned int index);

    virtual void setFacePropertyFromText(unsigned int index, QString text);
    virtual void setEdgePropertyFromText(unsigned int index, QString text);
    virtual void setHalfedgePropertyFromText(unsigned int index, QString text);
    virtual void setVertexPropertyFromText(unsigned int index, QString text);

    virtual void removeProperty();
    virtual void duplicateProperty();

};

#if defined(INCLUDE_TEMPLATES) && !defined(OM_PROPERTY_VISUALIZER_SKIN_WEIGHTS_CC)
#include "OMPropertyVisualizerSkinWeightsT.cc"
#endif

#endif /* OM_PROPERTY_VISUALIZER_SKIN_WEIGHTS_HH */

#endif /* ENABLE_SKELETON_SUPPORT */
