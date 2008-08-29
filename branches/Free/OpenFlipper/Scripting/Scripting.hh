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
