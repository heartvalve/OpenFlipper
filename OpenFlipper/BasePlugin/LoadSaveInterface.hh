/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//
// C++ Interface: Load/Save Interface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef LOADSAVEINTERFACE_HH
#define LOADSAVEINTERFACE_HH

#include <OpenFlipper/common/Types.hh>

/** \brief Interface for all plugins which want to load or save files
  *
  * Using this interface you can instruct the core to open/save objects or
  * create new empty objects.
 */
class LoadSaveInterface {

   public:

      /// Destructor
      virtual ~LoadSaveInterface() {};

   signals :
      /** Tries to load the file as a given type
       *
       * @param _filename Filename of the File to load
       * @param _type Type to be loaded
       * @param _id Here the id of the loaded object is returned. id its -1 something went wrong
       */
      virtual void load(QString /*_filename*/, DataType /*_type*/, int& /*_id*/) {};

      /** Add an empty object of the given type
       *
       * @param _type Type to be created
       * @param _id Here the id of the loaded object is returned. id its -1 something went wrong
       */
      virtual void addEmptyObject( DataType /*_type*/, int& /*_id*/) {};

      /** Create a copy of an existing object
       *
       * @param _oldId id of the object to copy
       * @param _newId id of the new object created
       */
      virtual void copyObject( int /*_oldId*/, int& /*_newId*/) {};

      /**
       *
       * @param _id Object to Save
       * @param _filename Filename to save it to (Leave as "" to automatically determine filename)
       */
      virtual void save(int /*_id*/ , QString /*_filename*/ ) {};

      /** \brief file has been opened
       * @param _id Id of the opened object
       */
      virtual void openedFile( int /*_id*/ ) {};

      /** \brief Emit this signal if an empty object has been created
       * @param _id Id of the added object
       */
      virtual void emptyObjectAdded( int /*_id*/ ) {};

     /** \brief get a list of all Filters
       *
       *  request a list of all Filters
       *
       *  @param _list StringList where the filters should be put into
       */
      virtual void getAllFilters( QStringList& /*_list*/ ) {};


      /** \brief Delete an object
       *
       * @param _id Id of the object
       */
      virtual void deleteObject( int /*_id*/ ){};

      /** \brief Delete all Objects
       *
       */
      virtual void deleteAllObjects(){};

  private slots :

    /**  \brief A file has been opened
     *
     *  This slot is called if a file has been opened by the core.\n
     *  @param _id Id of the new object
     */
    virtual void fileOpened( int /*_id*/ ) {};

    /**  \brief An empty object has been added
     *
     *  This slot is called if an empty object has been added by the core.\n
     *  @param _id Id of the new object
     */
    virtual void addedEmptyObject( int /*_id*/ ) {};

    /** \brief An object was deleted
      *
      * @param _id Id of the object
      */
    virtual void objectDeleted( int /*_id*/ ){};

};

Q_DECLARE_INTERFACE(LoadSaveInterface,"OpenFlipper.LoadSaveInterface/1.0")

#endif // LOADSAVEINTERFACE_HH
