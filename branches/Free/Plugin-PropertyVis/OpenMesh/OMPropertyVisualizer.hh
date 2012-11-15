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

#ifndef OM_PROPERTY_VISUALIZER_HH
#define OM_PROPERTY_VISUALIZER_HH

#include "../PropertyVisualizer.hh"

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#include <iostream>

template <typename MeshT>
class OMPropertyVisualizer: public PropertyVisualizer{

public:
    OMPropertyVisualizer(MeshT* _mesh, PropertyInfo _propertyInfo)
        : PropertyVisualizer(_propertyInfo),
          mesh(_mesh)
    { }

    virtual ~OMPropertyVisualizer(){clear();}


    /// Visualizes the property.
    virtual void visualize();

    /**
     * @brief Removes the property.
     *
     * This will remove the property from the mesh. After this method the PropertyVisualizer
     * should be deleted.
     */
    virtual void removeProperty(){ /*implemented by subclass*/}

    /**
     * @brief Duplicates the property.
     *
     * This will duplicate the property.
     */
    virtual void duplicateProperty(){ /*implemented by subclass*/}

    /// Clears the property.
    virtual void clear();

    /// Returns the ID of the closest primitive.
    unsigned int getClosestPrimitiveId(unsigned int _face, ACG::Vec3d &_hitPoint);

    virtual QString getPropertyText(unsigned int index);

protected:
    MeshT* mesh;

    virtual void visualizeFaceProp();
    virtual void visualizeEdgeProp();
    virtual void visualizeHalfedgeProp();
    virtual void visualizeVertexProp();

    template <typename PropType>
    void removeProperty_stage1();
    template <typename PropType>
    void removeProperty_stage2();

    template<typename PropType>
    void duplicateProperty_stage1();
    template<typename PropHandle, typename Iterator>
    void duplicateProperty_stage2(Iterator first, Iterator last);



    virtual void clearFaceProp();
    virtual void clearEdgeProp();
    virtual void clearHalfedgeProp();
    virtual void clearVertexProp();

    virtual void setFacePropertyFromText(unsigned int index, QString text);
    virtual void setEdgePropertyFromText(unsigned int index, QString text);
    virtual void setHalfedgePropertyFromText(unsigned int index, QString text);
    virtual void setVertexPropertyFromText(unsigned int index, QString text);

    OpenMesh::Vec4f convertColor(QColor color);
    virtual void setPropertyFromText(unsigned int index, QString text);

    virtual int getEntityCount();

    virtual QString getHeader();

    unsigned int getClosestFaceId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestEdgeId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestHalfedgeId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestVertexId(unsigned int _face, ACG::Vec3d& _hitPoint);


    template <typename InnerType >
    QString getPropertyText_(unsigned int index);

private:
    template<typename PropHandleT>
    class CopyProperty
    {
        public:
            CopyProperty(const PropHandleT& p1, const PropHandleT& p2, MeshT*& mesh) :
                p1(p1), p2(p2), mesh(mesh) {}

            template<typename PrimitiveHandleT>
            inline void operator() (const PrimitiveHandleT &pr) {
                mesh->property(p1, pr) = mesh->property(p2, pr);
            }

        private:
            const PropHandleT &p1, &p2;
            MeshT*& mesh;
    };

};



#if defined(INCLUDE_TEMPLATES) && !defined(OM_PROPERTY_VISUALIZER_CC)
#include "OMPropertyVisualizerT.cc"
#endif


#endif /* OM_PROPERTY_VISUALIZER_HH */
