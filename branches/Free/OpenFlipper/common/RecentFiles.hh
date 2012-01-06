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
  

//===========================================================================
/** @name Recent Files
* @{ */
//===========================================================================  
  
  /// Add a file to the recent files list ( removes one, if list grows to larger then maxRecent )
  DLLEXPORT
  void addRecentFile(QString _file, DataType _type);

  /**
   * Bookkeeping helper for a list of recent items.
   *
   * Usage example:
   * OpenFlipper::Options::rememberRecentItem(QString::fromUtf8("Plugin-MIQParameterize/File/RecentGurobiEnvironmentFiles"), file);
   */
  DLLEXPORT
  void rememberRecentItem(const QString &propName, const QString &itemName, const int RECENT_ITEMS_MAX_SIZE = 10);

  /**
   * Bookkeeping helper for a list of recent items.
   */
  DLLEXPORT
  QStringList getRecentItems(const QString &propName);
    
/** @} */      
  
  
}
}

//=============================================================================
#endif // OPTIONS_HH defined
//=============================================================================
