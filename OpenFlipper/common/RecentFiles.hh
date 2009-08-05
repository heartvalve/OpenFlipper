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
