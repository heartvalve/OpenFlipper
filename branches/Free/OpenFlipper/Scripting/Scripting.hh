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




//=============================================================================
//
//  Logging System
//
//=============================================================================

#ifndef SCRIPTING_HH
#define SCRIPTING_HH

//=============================================================== INCLUDES ====

#include <iostream>

#include <QObject>

//======================================================= CLASS DEFINITION ====
/**
 * \file Scripting.hh 
 * This File contains the header for the scripting wrapper used for Plugins
*/


/** A wrapper class, getting events from an plugin, prepending plugin name to the scripting
 * and passing it to the core. This class is generated for every plugin of the core.
 */
class ScriptingWrapper : public QObject {
  
Q_OBJECT

signals:
  /// complete signalfor scripting plugins
  void scriptInfo( QString _pluginName , QString _functionName  );
    
public:
  
  /// Standard Constructor 
  ScriptingWrapper(QString _pluginName);
  
  /// Destructor
  ~ScriptingWrapper();
  
private slots:
  /// prepend pluginname and emit extended signal
  void slotScriptInfo( QString _functionName  );

private:
  /// The Scriptng name of the plugin
  QString pluginName_;
    
};


//=============================================================================
#endif // SCRIPTING_HH
//=============================================================================
