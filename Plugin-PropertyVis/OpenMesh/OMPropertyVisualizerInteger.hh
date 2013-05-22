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

#ifndef OM_PROPERTY_VISUALIZER_INTEGER_HH
#define OM_PROPERTY_VISUALIZER_INTEGER_HH

#include "OMPropertyVisualizer.hh"

#include "Widgets/IntegerWidget.hh"

#include <ACG/Utils/ColorGenerator.hh>

#include <iostream>

template <typename MeshT, typename T>
class OMPropertyVisualizerInteger: public OMPropertyVisualizer<MeshT>{

public:
    OMPropertyVisualizerInteger(MeshT* _mesh, PropertyInfo _propertyInfo, bool isUnsigned);
    virtual ~OMPropertyVisualizerInteger(){}

protected:

    virtual void visualizeFaceProp(bool _setDrawMode = true);
    virtual void visualizeEdgeProp(bool _setDrawMode = true);
    virtual void visualizeHalfedgeProp(bool _setDrawMode = true);
    virtual void visualizeVertexProp(bool _setDrawMode = true);

    virtual void removeProperty();
    virtual void duplicateProperty();

    virtual T getValue(OpenMesh::FPropHandleT< T > prop, typename MeshT::FaceIter iter)     { return OMPropertyVisualizer<MeshT>::mesh->property(prop, iter) ; }
    virtual T getValue(OpenMesh::EPropHandleT< T > prop, typename MeshT::EdgeIter iter)     { return OMPropertyVisualizer<MeshT>::mesh->property(prop, iter) ; }
    virtual T getValue(OpenMesh::HPropHandleT< T > prop, typename MeshT::HalfedgeIter iter) { return OMPropertyVisualizer<MeshT>::mesh->property(prop, iter) ; }
    virtual T getValue(OpenMesh::VPropHandleT< T > prop, typename MeshT::VertexIter iter)  { return OMPropertyVisualizer<MeshT>::mesh->property(prop, iter) ; }

    virtual QString getPropertyText(unsigned int index);

    virtual void setFacePropertyFromText(unsigned int index, QString text);
    virtual void setEdgePropertyFromText(unsigned int index, QString text);
    virtual void setHalfedgePropertyFromText(unsigned int index, QString text);
    virtual void setVertexPropertyFromText(unsigned int index, QString text);

    inline int             strToT   (QString str, int)             { return this->strToInt(str);  }
    inline unsigned int    strToT   (QString str, unsigned int)    { return this->strToUInt(str); }
    inline QString         TToStr   (int i)                        { return this->intToStr(i);    }
    inline QString         TToStr   (unsigned int i)               { return this->uintToStr(i);   }



    ACG::ColorGenerator mColorGenerator;

};


#if defined(INCLUDE_TEMPLATES) && !defined(OM_PROPERTY_VISUALIZER_INTEGER_CC)
#include "OMPropertyVisualizerIntegerT.cc"
#endif

#endif /* OM_PROPERTY_VISUALIZER_INTEGER_HH */
