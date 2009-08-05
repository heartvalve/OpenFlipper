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

#include <QDir>

#include "RecentFiles.hh"


namespace OpenFlipper {
namespace Options {
  

/// List of recently opened files
static QVector<RecentFile> recentFiles_;

/// Maximum number of recent files
static int maxRecent_ = 6;

int  maxRecent()      { return maxRecent_;  }

QVector<RecentFile> recentFiles()   { return recentFiles_;            }

void maxRecent(int _max)             { maxRecent_      = _max; }
void recentFiles(QVector<RecentFile> _list ) { recentFiles_    = _list;}



void addRecentFile(QString _file, DataType _type) {
  // Check if file already in recent list
  for ( int i = 0 ; i < recentFiles_.size() ; ++i)
    if ( _file == recentFiles_[i].filename && _type == recentFiles_[i].type )
      recentFiles_.remove(i);
  
  // Erase if too many files in list
  if ( recentFiles_.size() >= maxRecent_ ) {
    recentFiles_.pop_back();
  }
  
  RecentFile recent;
  recent.filename = _file;
  recent.type = _type;
  recentFiles_.push_front(recent);
}

}
}

//=============================================================================

