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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Mi, 26. Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/

#include "Properties.hh"

using namespace std;

Properties::BaseProperty::BaseProperty()
{
}

Properties::BaseProperty::~BaseProperty()
{
}

Properties::Properties()
{
	size_ = 0;
}

Properties::~Properties()
{
	clear_properties();
}

/**
 * @brief While preserving the properties themself, all values in the property are deleted
 *
 * This method preserves the properties, the property handles remain valid. Call this method
 * if all objects in the derived class are deleted, so there is no object left that has properties
 * attached to it. Notice the difference to Properties::clear_properties.
 */
void Properties::clean_properties()
{
	for(vector<BaseProperty*>::iterator it = properties_.begin(); it != properties_.end(); ++it)
		if((*it) != 0)
			(*it)->clear();
	size_ = 0;
}

/**
 * @brief Deletes all properties, including their values
 *
 * Notice the difference to Properties::clean_properties, which preserves the properties and only
 * deletes the values inside them.
 */
void Properties::clear_properties()
{
	for(vector<BaseProperty*>::iterator it = properties_.begin(); it != properties_.end(); ++it)
		if((*it) != 0)
			delete *it;
	properties_.clear();
	property_names_.clear();
	size_ = 0;
}

/**
 * @brief Inserts a property for a new object at the given index
 *
 * If the derived class inserts a new object at the given index, the properties have to insert a new
 * property, to keep it in sync. This is done by calling this method. To append at the end simply
 * give the size of the vector holding the objects as parameter.
 *
 * @param _index Index of the new object, all the property indices for objects with index >= _index
 * 				 are increased by one
 */
void Properties::insert_property_at(int _index)
{
	size_ += 1;

	for(vector<BaseProperty*>::iterator it = properties_.begin(); it != properties_.end(); ++it)
		if(*it != 0)
			(*it)->insert_at(_index);
}

/**
 * @brief Removes a property for an object that is being deleted from the derived class
 *
 * The same as for Properties::insert_property_at, just the other way around.
 *
 * @param _index Index of the new object, all the property indices for objects with index >= _index
 * 				 are decremented by one
 */
void Properties::remove_property_at(int _index)
{
	size_ -= 1;

	for(vector<BaseProperty*>::iterator it = properties_.begin(); it != properties_.end(); ++it)
		if(*it != 0)
			(*it)->remove_at(_index);
}

/**
 * @brief Returns true if a property with the given name exists
 *
 * @param _name The properties name
 * @return true if the property exists, false otherwise
 */
bool Properties::has_property(std::string _name)
{
	if(property_names_.find(_name) == property_names_.end())
		return false;
	return true;
}
