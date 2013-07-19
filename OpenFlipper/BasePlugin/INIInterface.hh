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


#ifndef INIINTERFACE_HH
#define INIINTERFACE_HH

 
#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

 #include <QMenuBar>
 #include <OpenFlipper/common/Types.hh>

 #include <OpenFlipper/INIFile/INIFile.hh>

 /** \brief Interface class for Plugins which have to store information in ini files
  *
  * Using this interface you can store or load additional information to an ini file on a
  * per object basis or settings for your plugin. After basic information is written to the
  * ini file by the core ( or loaded by the core ) the corresponding functions in this
  * Interface will be executed.
  * You get the object id for the object which has to be used. For every object the functions
  * will get called and you have to save your data only for the given object!\n
  * The ini file in the functions is already open. You may not close it!
  * Additionally there are two slots which are called once per plugin when writing a settings
  * file. These slots should be used to store information which is used by your plugin.
  *
  * See \ref dataFlow for a detailed overview of OpenFlipper's data flow and interface function calls.
 */
class INIInterface {
   public :

      /// Destructor
      virtual ~INIInterface() {};

   private slots:

    //=======================================
    /** @name Per object settings
    * @{ */
    //=======================================
      /** \brief Load per object settings
       *
       *  Every time the core opens a settings file containing objects
       *  the core will call this slot for each object it finds.
       *  The object itself is loaded before the slot is called. Therefore
       *  the object is available from within your plugin.
       *
       *  @param _ini ini file
       *  @param _id Id of the object to load data for
       */
      virtual void loadIniFile( INIFile& _ini ,int _id ) {};

      /** \brief Save per object settings
       *
       * Every time a settings file is created this slot is called
       * once per object. Here you can write additional information
       * handled by your plugin which is attached to the object.
       *
       * @param _ini ini file
       * @param _id Identifier of the object to save
       */
      virtual void saveIniFile( INIFile& _ini ,int _id) {};

    /** @} */


    //=======================================
    /** @name Plugin Options and Settings
    * @{ */
    //=======================================

    private slots:

      /** \brief Load per Plugin settings
       *
       *  When the core loads an ini file and it contains settings for
       *  Plugin or the core itself this slot will be
       *  called once per Plugin.
       *  This Slot will be called after loading the core settings and
       *  before objects are loaded
       *
       * @param _ini  ini file
       */
      virtual void loadIniFileOptions( INIFile& _ini ) {};

      /** \brief Load per Plugin settings after objects are loaded
       *
       *  When the core loads an ini file and it contains settings for
       *  Plugin or the core itself this slot will be
       *  called once per Plugin. In contrast to loadIniFileOptions
       *  this slot will be called after all objects are loaded
       *
       * @param _ini  ini file
       */
      virtual void loadIniFileOptionsLast( INIFile& _ini ) {};

      /** \brief Save Plugin Options
       *  When the core is about to save an ini file this slot will be
       *  called once per Plugin.
       *  This Slot will be called after saving the core settings and
       *  before objects are saved to the file
       *
       * @param _ini ini file
       */
      virtual void saveIniFileOptions( INIFile& _ini ) {};

      /** \brief Save per Plugin settings when application is about to quit
       *
       * Before the Core is closed, this slot will be called per plugin to
       * save per plugin settings.
       *
       * @param _ini ini file
       */
      virtual void saveOnExit( INIFile& _ini ) {};


   /** @} */


};

Q_DECLARE_INTERFACE(INIInterface,"OpenFlipper.INIInterface/1.2")

#endif // INIINTERFACE_HH
