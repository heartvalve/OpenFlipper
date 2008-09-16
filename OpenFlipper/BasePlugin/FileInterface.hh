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

   public :

      /** \brief The core gives the root of the object tree via this function */
      virtual void setObjectRoot( BaseObject* _root ) = 0;

      /// Return support for adding empty objects
      virtual bool supportAddEmpty() = 0;

      /// Add an empty object
      virtual int addEmpty() = 0;

      /// Return name for the types of data this plugin handles
      virtual QString typeName() = 0;

      /// Return supported Object type
      virtual DataType supportedType() = 0;

      /// Get the Qt Filters for Filedialogs when loading files
      virtual QString getLoadFilters() = 0;

      /// Get the Qt Filters for Filedialogs when saving files
      virtual QString getSaveFilters() = 0;

      /** You can provide a special widget showing options for saving your file types
       *  depending on the current filter
       *
       *  _currentFilter: one filter from saveFilters
       *
       *  Return 0 when you dont need one
       */
      virtual QWidget* saveOptionsWidget(QString _currentFilter) = 0;

      /** You can provide a special widget showing options for loading your file types
       *  depending on the current filter
       *
       *  _currentFilter: one filter from loadFilters
       *
       *  Return 0 when you dont need one
       */
      virtual QWidget* loadOptionsWidget(QString _currentFilter) = 0;

public slots:

      /// load an object from the given file
      virtual int loadObject(QString _filename) = 0;

      /// Save the object with given id,filename
      virtual bool saveObject(int _id, QString _filename) = 0;

      //Optionswidget

};

Q_DECLARE_INTERFACE(FileInterface,"OpenFlipper.FileInterface/0.3")

#endif // FILEINTERFACE_HH
