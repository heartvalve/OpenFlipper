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


#ifndef FILEINTERFACE_HH
#define FILEINTERFACE_HH

 #include <QtGui>
 #include <OpenFlipper/common/Types.hh>

/** \file FileInterface.hh
*
* Interface class for file handling.\ref fileInterfacePage
*/

/** \brief Interface class for file handling.
  *
  * \ref fileInterfacePage "Detailed description"
  * \n
  *
  * Loading and saving of files in OpenFlipper is handled via the file plugins.
  * These have to implement this class.
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
      *  Return 0 when you don't need one
      */
    virtual QWidget* saveOptionsWidget(QString _currentFilter) = 0;

    /** You can provide a special widget showing options for loading your file types
      *  depending on the current filter
      *
      *  _currentFilter: one filter from loadFilters
      *
      *  Return 0 when you don't need one
      */
    virtual QWidget* loadOptionsWidget(QString _currentFilter) = 0;

/** @} */

//===========================================================================
/** @name Supported types
* @{ */
//===========================================================================

public:

  /** \brief Return your supported object type( e.g. DATA_TRIANGLE_MESH )
   *
   * If you support multiple DataTypes you can combine them bitwise here.
   * The function is used from the load and save code to check if your plugin
   * can handle an object of a given dataType.
   */
  virtual DataType supportedType() = 0;


  /** Return the Qt Filters for File dialogs when loading files (e.g. "Off files ( *.off )" )
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

signals:

      /** \brief This signal has to be emitted if an object has been loaded from a file.
       *
       * Every time a new object is added to the scene from a file, this signal has to be emitted.
       * Emit this signal once each time a file has been completely processed.
       *
       * @param _id Id of the new object
       */
      virtual void openedFile( int _id ) {};

public slots:

      /** \brief Create an empty object
       *
       * When this slot is called you have to create an object of your supported type.
       * @return Id of the new Object
       * \todo Add an addEmpty function with a given type as an attribute. The plugin should then create only an
       * empty object of the given type. It can be assumed that there is only one type given at a time.
       */
      virtual int addEmpty() { return -1; };

      /** \brief Load an object from the given file
       *
       * The Core will call this slot if you should load a file. The core will
       * check if you support the given file type depending on the provided
       * filters and dataTypes ( see supportedType and getLoadFilters )\n
       *
       * If loading fails, you have to return -1. If you created an empty
       * object and then your load procedure fails, you should remove the newly
       * generated object.\n
       *
       * if you just opened a file and did not create any object, return 0 here,
       * telling the core that it was successfully loaded but no new object
       * has been created!
       */
      virtual int loadObject(QString _filename) = 0;

      /** \brief Load an object from the given file
       *
       * The Core will call this slot if you should load a file. The core will
       * check if you support the given file type depending on the provided
       * filters and dataTypes ( see supportedType and getLoadFilters )\n
       *
       * if you just opened a file and did not create any object, return 0 here,
       * telling the core that it was successfully loaded but no new object
       * has been created!
       *
       * If loading fails, you have to return -1. If you created an empty
       * object and then your load procedure fails, you should remove the newly
       * generated object.\n
       *
       * If the Plugin is able to open the file in different DataTypes
       * one of these DataTypes can be forced with this function.
       *
       * This forcing is used for example by the recent files function for
       * choosing between poly or triangle mesh loading.
       *
       */
      virtual int loadObject(QString _filename, DataType _type){ return -1;};

      /** \brief Save an object from the given file
       *
       * The Core will call this slot if you should save an object to a file.
       * The core will check if you support the given object depending on the
       * provided dataTypes ( see supportedType ).
       * Additionally to the filename you get the id of the object to save
       */
      virtual bool saveObject(int _id, QString _filename) = 0;

      /** \brief Save multiple objects to one file
       *
       * The Core will call this slot if the user wants to save multiple files in one file.
       * The core will check if you support the given object depending on the
       * provided dataTypes ( see supportedType ).
       * Additionally to the filename you get ids of the objects to save
       */
      virtual bool saveObjects(IdList _ids, QString _filename){return false;};
/** @} */

};

/** \page fileInterfacePage File Interface
 * \n
\image html FileInterface.png
\n


\section fileInterface_Functionality Functionality
This interface class has to be fully implemented. When you want to support save and load
for an object type you have to implement all functions in this class in a file plugin.
The plugin has to be named Plugin-File\<FileExtension\>. The plugins are loaded directly after the
Type Plugins.

OpenFlipper manages loading and saving of files from the core.

\section fileInterface_filters Load/Save filters and Type Support
In these functions you have to return filters for your supported type. Use the standard format of Qt and
return your filters within the functions FileInterface::getLoadFilters() and FileInterface::getSaveFilters().
OpenFlipper will decide based on the filters, if your plugin is used to handle a load or save request.
Additionally you have to define, which DataTypes your plugin supports via FileInterface::supportedType().

\code
// File types you support when loading files
QString ExamplePlugin::getLoadFilters() {
    return QString( tr("Your FileType ( *.ext )") );
}

// File types you support when saving files
QString ExamplePlugin::getSaveFilters() {
    return QString( tr("Your FileType ( *.ext )") );
}

// Data types you support in your plugin
DataType  ExamplePlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH | DATA_GROUP;

    return type;
}


\endcode

\section fileInterface_widgets Widgets
When an object is loaded or saved via your file plugin, you can present an additional dialog, used to
control your reader/writer.
You have to implement the following functions. If you don't need such a widget, just return a 0.

\code
// In your plugin initialization
void ExamplePlugin::pluginsInitialized() {
  // Global variable:
  // QWidget* saveOptionsWidget_;

  // Create widget
  saveOptionsWidget_ = new QWidget(0);

  // Setup widget afterwards

  // Global variable:
  // QWidget* loadOptionsWidget_;

  // Create widget
  loadOptionsWidget_ = new QWidget(0);

  // Setup widget afterwards
}


// Return your widget for save options
QWidget* ExamplePlugin::saveOptionsWidget(QString _currentFilter) {

  // Modify widget based on the filter
  // ...

  // Return widget
  return saveOptionsWidget_;
}

// Return your widget for load options
QWidget* ExamplePlugin::loadOptionsWidget(QString _currentFilter) {

  // Modify widget based on the filter
  // ...

  // Return widget
  return loadOptionsWidget_;
}
\endcode

\section fileInterface_loading Loading Files
The FileInterface defines two functions for loading files. The first one ( FileInterface::loadObject(QString _filename) ) has
to be implemented. It has to load the data from the given file. The return value of this function has to be the id of the
new object. Additionally you have to
\code
emit FileInterface::openedFile( int _id );
\endcode
with the id of every object you loaded from the file.

If the file contains multiple objects, create a GroupObject and add all loaded objects to that group. Than return
the id of the group here. Don't forget the FileInterface::openedFile() signal for every newly loaded object from the file.

One function that can be implemented is FileInterface::loadObject(QString _filename, DataType _type). This function gets a
DataType along with the filename. This can be used to force a specific DataType when loading objects. E.g. A plugin loading
obj files which handles triangle and polygonal meshes which can than be forced to triangulate every input data.

\section fileInterface_saving Saving Files
The FileInterface defines two functions for loading files. The first one ( FileInterface::saveObject(int _id, QString _filename) )
has to be implemented. It has to save the object with the given id to the file specified by the filename. The return value of
this function has to be true if it succeeded, otherwise false. OpenFlipper will choose your plugin to save the data if two
prerequisites are fulfilled. Your  FileInterface::supportedTypes() has to support the DataType of the object and
your save filters have to contain the extension of the file to use. These checks are done within OpenFlipper such that
you only get information that can be handled by the plugin.

Additionally there is a function FileInterface::saveObjects(IdList _ids, QString _filename) which does the same as the above
but saves a set of objects to a file.


\section fileInterface_usage Usage
To use the FileInterface:
<ul>
<li> include FileInterface.hh in your plugins header file
<li> derive your plugin from the class FileInterface
<li> add Q_INTERFACES(FileInterface) to your plugin class
<li> And implement the required slots and functions
</ul>



TODO:
// Deprecated here? In TypeInterface?
virtual int addEmpty() { return -1; };

*/






Q_DECLARE_INTERFACE(FileInterface,"OpenFlipper.FileInterface/1.0")

#endif // FILEINTERFACE_HH
