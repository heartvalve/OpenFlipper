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
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"
// -------------------- ACG
#include <ACG/Scenegraph/DrawModes.hh>

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/common/RecentFiles.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <QFile>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

//== IMPLEMENTATION ==========================================================

/// \todo After obj writing to one file is implemented in obj plugin, remove this Function and the whole File!
void Core::writeObjFile(QString _filename, bool _relativePaths, bool _targetOnly, std::map<int,QString>& _fileMapping)
{
  // open file
  std::string fname = _filename.toStdString();
  std::fstream out(fname.c_str(), std::ios_base::out);
  if (!out)
  {
    emit log(LOGERR,tr("Cannot open to obj file") + _filename);
    return;
  }


  if ( OpenFlipper::Options::gui() )
  {
    coreWidget_->statusMessage( tr("Saving obj File ") + _filename + " ...");
    coreWidget_->setStatus(ApplicationStatus::BLOCKED );
  }

  PluginFunctions::IteratorRestriction restriction;
  if ( _targetOnly )
    restriction = PluginFunctions::TARGET_OBJECTS;
  else
    restriction = PluginFunctions::ALL_OBJECTS;

  // write all objects to a separate obj file and save external references in the global obj file
  for ( PluginFunctions::ObjectIterator o_it (restriction) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)
  {
    QString file;
    std::map<int,QString>::iterator f = _fileMapping.find(o_it->id());
    if(f == _fileMapping.end()) {
        file = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();
    } else {
        file = f->second;
    }

    if (QFile(file).exists())
    {
      // Modify filename if relativePaths are wanted
      if (_relativePaths)
      {
        int prefixLen = _filename.section(OpenFlipper::Options::dirSeparator(),0,-2).length();
        file.remove(0, prefixLen);
        file = "." + file;
      }

      // save to global inifile
      out << "call " << file.toStdString() << "\n";
    }
  }

  out.close();

  if ( OpenFlipper::Options::gui() )
  {
    coreWidget_->statusMessage( tr("Saving obj File ") + _filename + tr(" ... done"), 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }

}


//=============================================================================
