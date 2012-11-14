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

#ifndef PROPERTY_MODEL_FACTORY_HH
#define PROPERTY_MODEL_FACTORY_HH

#include <OpenMesh/Core/Utils/SingletonT.hh>

#include "PropertyModel.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT
    #include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
    #include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */

class PropertyVisPlugin;

/*! \class __PropertyModelFactory
 *  \brief This class manages the creation of PropertyModels.
 *
 * This class is used to create the correct PropertyModel for a given object.
 * Using this factory the PropertyVisPlugin does not need to know about different
 * object types such as TriMesh, PolyMesh, PolyhedralMesh etc. If in future a new
 * type of object should be visualized only the factory needs to be updated in
 * order to recognize the new type and create the correct PropertyModel.
 *
 * A map of PropertyModels is kept so that for each object only one
 * PropertyModel is created.
 *
 * Note that this class is transformed to a Singleton and accessed via
 * PropertyModelFactory::Instance().
 */

class __PropertyModelFactory
{
    friend class OpenMesh::SingletonT<__PropertyModelFactory>;

public:
    /// Returns the PropertyModel.
    PropertyModel* getModel(int objectID);

private:
    __PropertyModelFactory(){}
    __PropertyModelFactory(__PropertyModelFactory&){}
    ~__PropertyModelFactory()
    {
        for (PropertyModelMap::iterator it = propertyModelMap.begin(); it != propertyModelMap.end(); ++it)
            delete it->second;
        propertyModelMap.erase(propertyModelMap.begin(), propertyModelMap.end()); //is this necessary?
    }

    typedef std::map<int, PropertyModel*> PropertyModelMap;
    PropertyModelMap propertyModelMap;
};

typedef OpenMesh::SingletonT<__PropertyModelFactory> PropertyModelFactory;

#endif /* PROPERTY_VISUALISER_FACTORY_HH */
