//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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

    /**  \brief get a list of all Filters
       * request a list of all Filters
       *  @param _list StringList where the filters should be put into
     */
      virtual void getAllFilters( QStringList& /*_list*/ ) {};
    
  private slots :
  
    /**  \brief A file has been opened
     * 
     *  This slot is called if a file has been opened by the core.\n
     *  @param _id Id of the new object
     */
    virtual void fileOpened( int /*_id*/ ) {};
      
};

Q_DECLARE_INTERFACE(LoadSaveInterface,"OpenFlipper.LoadSaveInterface/1.0")
      
#endif // LOADSAVEINTERFACE_HH
