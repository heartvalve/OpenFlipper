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
// C++ Interface: BasePlugin
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef FILEINTERFACE_HH
#define FILEINTERFACE_HH

 #include <QtGui>
 #include <OpenFlipper/common/Types.hh>

 /** \brief Interface class for file and object handling
  *
  * This interface class has to be fully implemented. When you want to support save and load
  * for an object type you have to implement all functions in this class. The core manages the
  * load and save interaction and gives the load and save actions to your plugin. The decision
  * which plugin is called for saving an object depends on your supportedTypes() and the filters
  * defined by your plugin.
 */

class FileInterface {

//===========================================================================
/** @name Widgets
* @{ */
//===========================================================================
  public:

    /// Destructor
    virtual ~FileInterface() {};  

    /** You can provide a special widget showing options for saving your file types
      *  depending on the current filter
      *
      *  _currentFilter: one filter from saveFilters
      *
      *  Return 0 when you dont need one
      */
    virtual QWidget* saveOptionsWidget(QString /*_currentFilter*/) = 0;

    /** You can provide a special widget showing options for loading your file types
      *  depending on the current filter
      *
      *  _currentFilter: one filter from loadFilters
      *
      *  Return 0 when you dont need one
      */
    virtual QWidget* loadOptionsWidget(QString /*_currentFilter*/) = 0;

/** @} */

//===========================================================================
/** @name Supported types
* @{ */
//===========================================================================

public:

  /** \brief Return your supported object type( e.g. DATA_TRIANGLE_MESH )
   *
   * If you support multiple datatypes you can use the bitwise or to combine them here.
   * The function is used from addEmpty to check if your plugin can create an object of
   * a given dataType.
   */
  virtual DataType supportedType() = 0;


  /** Return the Qt Filters for Filedialogs when loading files (e.g. "Off files ( *.off )" )
   *
   */
  virtual QString getLoadFilters() = 0;

  /** Get the Qt Filters for Filedialogs when saving files (e.g. "Off files ( *.off )" )
   *
   */
  virtual QString getSaveFilters() = 0;

/** @} */

//===========================================================================
/** @name Load Save Implementation
* @{ */
//===========================================================================

public slots:

      /** \brief Create an empty object
       *
       * When this slot is called you have to create an object of your supported type.
       * @return Id of the new Object
       * \todo Add an addEmpty function with a given type as an attribute. The plugin should then create only an
       * empty object of the given type. It can be assumed that there is only one type given at a time.
       */
      virtual int addEmpty() { return -1 };
      
      /** \brief Load an object from the given file
       *
       * The Core will call this slot if you should load a file. The core will
       * check if you support the given file type depending on the provided
       * filters and dataTypes ( see supportedType and getLoadFilters )\n
       *
       * if you just opened a file and did not create any object, return 0 here,
       * telling the core that it was successfully loaded but no new object 
       * has been created!
       */
      virtual int loadObject(QString /*_filename*/) = 0;

      /** \brief Save an object from the given file
       *
       * The Core will call this slot if you should save an object to a file.
       * The core will check if you support the given object depending on the
       * provided dataTypes ( see supportedType ).
       * Additionally to the filename you get the id of the object to save
       */
      virtual bool saveObject(int /*_id*/, QString /*_filename*/) = 0;

/** @} */

};

Q_DECLARE_INTERFACE(FileInterface,"OpenFlipper.FileInterface/0.3")

#endif // FILEINTERFACE_HH
