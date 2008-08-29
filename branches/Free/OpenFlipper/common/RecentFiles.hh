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
//   $Revision: 2122 $
//   $Author: moebius $
//   $Date: 2008-06-30 15:12:28 +0200 (Mon, 30 Jun 2008) $
//
//=============================================================================




//=============================================================================
//
//  Options used throughout the System
//
//=============================================================================

/**
\file GlobalOptions.hh 
 * This Header provides access to globally defined variables such as 
 * Aplication Directories. You may use these Functions inside your plugins.
*/

#ifndef RECENTFILES_HH
#define RECENTFILES_HH

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#include <QDir>
#include <QStringList>

#include <OpenFlipper/common/Types.hh>

namespace OpenFlipper {
namespace Options {
  
/// struct storing infos about recent files
struct RecentFile{
  QString filename;
  DataType type;
};
   

//===========================================================================
/** @name Recent Files
* @{ */
//===========================================================================  
  
  /// Add a file to the recent files list ( removes one, if list grows to larger then maxRecent )
  DLLEXPORT
  void addRecentFile(QString _file, DataType _type);
  
  /// Return the list of recent files
  DLLEXPORT
  QVector< RecentFile > recentFiles();
  
  /// Set the list of recent files
  DLLEXPORT
  void recentFiles(QVector< RecentFile > _list );
  
  /// Get the maximum number of recent files kept in the list
  DLLEXPORT
  int maxRecent();
  
  /// Set the maximum number of recent files kept in the list
  DLLEXPORT
  void maxRecent(int _max);
    
/** @} */      
  
  
}
}

//=============================================================================
#endif // OPTIONS_HH defined
//=============================================================================
