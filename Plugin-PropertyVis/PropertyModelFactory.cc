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

#include "PropertyModelFactory.hh"


#include "OpenMesh/OMPropertyModel.hh"

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT
    #include "OpenVolumeMesh/OVMPropertyModel.hh"
#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */


/**
 * @brief Returns the PropertyModel
 *
 * @param objectID The index of the object for which we want to get the
 * property model
 * @return The PropertyModel.
 *
 * For the given objectID this method finds out which type the object has
 * and creates and return the PropertyModel for that type of object.
 *
 * Currently supported are TriMesh, PolyMesh and - if found - also
 * PolyhedralMesh and HexahedralMesh.
 * For an unrecognized object a PropertyModel with no functionality will
 * be returned.
 */
PropertyModel* __PropertyModelFactory::getModel(int objectID)
{
    PropertyModelMap::iterator it = propertyModelMap.find(objectID);
    if (it != propertyModelMap.end())
        return it->second;

    BaseObjectData* object = 0;

    PluginFunctions::getObject( objectID, object );

    PropertyModel* propertyModel;

    if (object == 0) {
        return new PropertyModel();
    }

    if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
        TriMesh* mesh = PluginFunctions::triMesh(object);
        propertyModel = new OMPropertyModel<TriMesh>(mesh, objectID);
    }
    else if ( object->dataType(DATA_POLY_MESH) )
    {
        PolyMesh* mesh = PluginFunctions::polyMesh(object);
        propertyModel = new OMPropertyModel<PolyMesh>(mesh, objectID);
    }
#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT
    else if ( object->dataType(DATA_POLYHEDRAL_MESH) )
    {
        PolyhedralMesh* mesh = PluginFunctions::polyhedralMesh(object);
        propertyModel = new OVMPropertyModel<PolyhedralMesh>(mesh, objectID);
    }
    else if ( object->dataType(DATA_HEXAHEDRAL_MESH) )
    {
        HexahedralMesh* mesh = PluginFunctions::hexahedralMesh(object);
        propertyModel = new OVMPropertyModel<HexahedralMesh>(mesh, objectID);
    }
#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
    else
    {
        propertyModel = new PropertyModel();
    }

    propertyModelMap.insert(std::pair<int, PropertyModel*>(objectID, propertyModel));
    return propertyModel;
}
