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
//  CLASS MViewWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Logging.hh"
// -------------------- ACG
#include "OpenFlipper/common/GlobalOptions.hh"

//== IMPLEMENTATION ========================================================== 

/** 
 * @param _pluginName Name of the Plugin which uses this logger (Prepended to all Output)
 * @param _type Default log type ( used for streaming operations )
 */
PluginLogger::PluginLogger(QString _pluginName , Logtype _type) :
    pluginName_(_pluginName),
    defaultLogType_(_type)
{
}

PluginLogger::~PluginLogger() {
}

/** Receive log events from plugins and pass them to core
 * @param _type Logtype (defines the color of the output)
 * @param _message The message for output
 */
void PluginLogger::slotLog(Logtype _type, QString _message) {
  QStringList strings;
  strings = _message.split("\n",QString::SkipEmptyParts);
  for ( int i = 0 ; i < strings.size(); ++i )
    emit log(_type,pluginName_ + " : " + strings[i]);
}

/** Receive log events from plugins (defaults to LOGOUT Message type)
 * 
 * @param _message The message for output
 */
void PluginLogger::slotLog(QString _message) {
  QStringList strings;
  strings = _message.split("\n",QString::SkipEmptyParts);
  for ( int i = 0 ; i < strings.size(); ++i )
    emit log(LOGOUT,pluginName_ + " : " + strings[i]);
}

/** This operator is used for streaming ( e.g. catch omerr and omerr streams) 
 *  and redirect them to the logging widget. The default log level passed to
 *  the constructor will be used for the message
 * 
 * @param _s String to display
 */
void PluginLogger::operator<< ( const std::string& _s ) {
  slotLog(defaultLogType_,QString( _s.c_str() ));
}


//=============================================================================
