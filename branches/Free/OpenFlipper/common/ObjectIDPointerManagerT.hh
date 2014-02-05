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




//=============================================================================
//
// CLASS ObjectIDPointerManagerT
//
//
// Author:  Henrik Zimmer <henrik@zimmer.to>
//
// Version: $Revision: 1$
// Date:    $Author$
//   $Date: XX-XX-200X$
//
//=============================================================================


#ifndef OBJECTIDPOINTERMANAGERT_HH
#define OBJECTIDPOINTERMANAGERT_HH


//== INCLUDES =================================================================

#include <map>
#include <vector>
#include <iostream>

#ifndef WIN32
#warning Included deprecated Header ObjectIDPointerManger
#warning You can attach data to objects by using the perObjectData class!
#warning Derive from perObjectData and use setObjectData on an Object
#warning to attach the data to it. You can read the data by using
#warning the objectData function of each object.
#endif

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================

/** Use this class to manage pointers associated with an object in the framework.
 */
template < class Pointer >
class ObjectIDPointerManagerT
{

public:

  /// typedef for the idmap
  typedef  std::map< int, Pointer > PointerObjectIDMap;

  /// typedef for the pairs
  typedef  std::pair< int, Pointer > PointerObjectIDPair;

  /// Constructor
  ObjectIDPointerManagerT() {}

  /// Destructor
  ~ObjectIDPointerManagerT() {}

  /** try to get a pointer with the given id
   * @param _identifier identifier of corresponding object
   * @param _pointer The returned pointer
   * @return true if object exists
   * */
  bool get_pointer(int _identifier, Pointer & _pointer);

  /** try to get a pointer with the given id
   * @param _identifier identifier of corresponding object
   * @return returns either a pointer or if the id does not exist NULL
   * */
  Pointer get_pointer(int _identifier);

  /** Add a pointer to the map
   * @param _identifier identifier of corresponding object
   * @param _pointer pointer to be added
   * */
  bool add_pointer(int _identifier, const Pointer & _pointer);

  /// Doesn't actually delete object (pointed to by pointer) only clears entry from the map
  bool delete_object(int _identifier);

  void get_object_ids(std::vector<int> & _ids);

private:

  /// Copy constructor (not used)
  ObjectIDPointerManagerT(const ObjectIDPointerManagerT& _rhs);

  /// Assignment operator (not used)
  ObjectIDPointerManagerT& operator=(const ObjectIDPointerManagerT& _rhs);

  /// mapping
  PointerObjectIDMap pointer_objectID_map_;
};


//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(OBJECTIDPOINTERMANAGERT_C)
#define OBJECTIDPOINTERMANAGERT_TEMPLATES
#include "ObjectIDPointerManagerT.cc"
#endif
//=============================================================================
#endif // OBJECTIDPOINTERMANAGERT_HH defined
//=============================================================================

