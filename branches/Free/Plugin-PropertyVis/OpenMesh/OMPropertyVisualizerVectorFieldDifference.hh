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

#ifndef OM_PROPERTY_VISUALIZER_VECTOR_FIELD_DIFFERENCE_HH
#define OM_PROPERTY_VISUALIZER_VECTOR_FIELD_DIFFERENCE_HH

#include "OMPropertyVisualizer.hh"

#include <ACG/Scenegraph/LineNode.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "Widgets/VectorFieldDifferenceWidget.hh"

#include <iostream>

#include <cmath>


template <typename MeshT>
class OMPropertyVisualizerVectorFieldDifference: public OMPropertyVisualizer<MeshT>{

public:
    OMPropertyVisualizerVectorFieldDifference(MeshT* _mesh, PropertyInfo _propertyInfo1, PropertyInfo _propertyInfo2);
    virtual ~OMPropertyVisualizerVectorFieldDifference(){}

    virtual QString getName() { return QObject::tr("Combination of %1 and %2").arg(PropertyVisualizer::propertyInfo.propName().c_str()).arg(propertyInfo2.propName().c_str()); }


protected:

    virtual void visualizeFaceProp(bool _setDrawMode = true);
    virtual void visualizeEdgeProp(bool _setDrawMode = true);
    virtual void visualizeHalfedgeProp(bool _setDrawMode = true);
    virtual void visualizeVertexProp(bool _setDrawMode = true);

    virtual void removeProperty(){} //no need to delete a property
    virtual void duplciateProperty(){ emit OMPropertyVisualizer<MeshT>::log("combined properties cannot be duplicated");}


    virtual QString getPropertyText(unsigned int index);

private:
    PropertyInfo propertyInfo2;

};


#if defined(INCLUDE_TEMPLATES) && !defined(OM_PROPERTY_VISUALIZER_VECTOR_FIELD_DIFFERENCE_CC)
#include "OMPropertyVisualizerVectorFieldDifferenceT.cc"
#endif
#endif /* OM_PROPERTY_VISUALIZER_VECTOR_FIELD_DIFFERENCE_HH */
