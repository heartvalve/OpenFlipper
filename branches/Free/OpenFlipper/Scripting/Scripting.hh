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
//  Logging System
//
//=============================================================================

#ifndef SCRIPTING_HH
#define SCRIPTING_HH

//=============================================================== INCLUDES ====

#include <QObject>

//======================================================= CLASS DEFINITION ====
/**
 * \file Scripting.hh 
 * This File contains the header for the scripting wrapper used for Plugins
*/


/** \class ScriptingWrapper
 * A wrapper class, getting events from an plugin, prepending plugin name to the scripting
 * and passing it to the core. This class is generated for every plugin of the core.
 */
class ScriptingWrapper : public QObject {
  
Q_OBJECT

signals:
  /// complete signal for scripting plugins

  void scriptInfo( QString _pluginName , QString _functionName  );
    
public:
  
  /** \brief Standard Constructor
   *
   * @param _pluginName Name of the Plugin which uses this logger (Prepended to all Output)
   */
  ScriptingWrapper(QString _pluginName);
  
  /// Destructor
  ~ScriptingWrapper();
  
private slots:
  /** \brief This function prepends the plugin name and emits the extended signal to the core.
   *
   * @param _functionName Name of the function
   */
  void slotScriptInfo( QString _functionName  );

private:
  /// The Scriptng name of the plugin
  QString pluginName_;
    
};


//=============================================================================
#endif // SCRIPTING_HH
//=============================================================================
