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

#ifndef INIINTERFACE_HH 
#define INIINTERFACE_HH 

 #include <QtGui>
 #include <QMenuBar>
 #include <ACG/QtWidgets/QtExaminerViewer.hh>
 #include <OpenFlipper/common/Types.hh>
 
 #include <OpenFlipper/INIFile/INIFile.hh>
 
 /** \brief Interface class for Plugins which have to store information in ini files 
  * 
  * Using this interface you can store additional information to ini file on an
  * per object basis. After the basic information is written to the ini file by the core 
  * ( or loaded by the core ) the corresponding functions in this Interface will be executed.
  * You get the object id for the object which has to be used. For every object the functions
  * will get called and you have to save your data only for the given object!\n
  * the ini file in the functions is already open. You may not close it!
 */
class INIInterface {
   public : 
       
      /// Destructor
      virtual ~INIInterface() {};
      
   private slots:
      /** Load object information from an ini file as well as plugin options.\n
       *  @param _ini ini file
       *  @param _id Id of the object to load data for
       */
      virtual void loadIniFile( INIFile& /*_ini*/ ,int /*_id*/ ) {};
      
      /** Save object information to an ini file
       * @param _ini ini file
       * @param _id Identifier of the object to save (-1 for all)
       */
      virtual void saveIniFile( INIFile& /*_ini*/ ,int /*_id*/) {};
      
      /** Load Plugin options (state information) to an ini file
       * @param _ini  ini file
       */
      virtual void loadIniFileOptions( INIFile& /*_ini*/ ) {};
      
      /** Save Plugin options (state information) to an ini file
       * @param _ini ini file
       */
      virtual void saveIniFileOptions( INIFile& /*_ini*/ ) {};
      
};

Q_DECLARE_INTERFACE(INIInterface,"OpenFlipper.INIInterface/1.2")
      
#endif // INIINTERFACE_HH
