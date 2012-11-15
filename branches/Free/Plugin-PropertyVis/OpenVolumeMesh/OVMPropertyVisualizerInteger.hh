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

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT

#ifndef OVM_PROPERTY_VISUALIZER_INTEGER_HH
#define OVM_PROPERTY_VISUALIZER_INTEGER_HH

#include "OVMPropertyVisualizer.hh"

#include "Widgets/IntegerWidget.hh"

#include <ACG/Utils/ColorGenerator.hh>

#include <iostream>

template <typename MeshT, typename T>
class OVMPropertyVisualizerInteger: public OVMPropertyVisualizer<MeshT>{

public:
    OVMPropertyVisualizerInteger(MeshT* _mesh, int objectID,  PropertyInfo _propertyInfo, bool isUnsigned);
    virtual ~OVMPropertyVisualizerInteger(){}

protected:

    template <typename PropType, typename EntityIterator>
    void visualizeProp(PropType prop, EntityIterator e_begin, EntityIterator e_end);
    virtual void duplicateProperty();

    virtual void visualizeFaceProp();
    virtual void visualizeEdgeProp();
    virtual void visualizeHalfedgeProp();
    virtual void visualizeVertexProp();
    virtual void visualizeCellProp();
    virtual void visualizeHalffaceProp();

    virtual QString getPropertyText(unsigned int index);

    T mNumericLimitMax;
    T mNumericLimitMin;

    ACG::ColorGenerator mColorGenerator;

};


#if defined(INCLUDE_TEMPLATES) && !defined(OVM_PROPERTY_VISUALIZER_INTEGER_CC)
#include "OVMPropertyVisualizerIntegerT.cc"
#endif

#endif /* OVM_PROPERTY_VISUALIZER_INTEGER_HH */

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
