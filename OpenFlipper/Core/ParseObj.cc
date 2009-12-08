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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
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


void Core::openObjFile(QString _filename)
{
  std::string fname = _filename.toStdString();
  std::fstream in(fname.c_str(), std::ios_base::in);

  if (!in)
  {
    emit log(LOGERR,tr("Cannot open to obj file") + _filename);
    return;
  }

  if ( OpenFlipper::Options::gui() )
  {
    coreWidget_->statusMessage( tr("Loading obj File ") + _filename + " ...");
    coreWidget_->setStatus(ApplicationStatus::BLOCKED );
  }

  OpenFlipper::Options::loadingSettings(true);


  while(!in.eof())
  {
    std::string keyword;
    in >> keyword;

    // Parse for external files to open
    if (keyword.compare("call") == 0)
    {
      // open external file
      std::string file;
      in >> file;

      QString path(file.c_str());

      //check if path is relative
      if (path.startsWith( "." + OpenFlipper::Options::dirSeparator() ))
      {
        //check if _filename contains a path
        if (_filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) != "")
        {
          path.remove(0,1); // remove .
          path = _filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) + path;
        }
      }

      // preprocess file
      preprocessObjFile(path);
    }
//     else
//       std::cout << "ObjParser: unknown keyword " << keyword << std::endl;

  }


  OpenFlipper::Options::loadingSettings(false);

  // Reset scenegraph and recompute scene center containing all new objects
  resetScenegraph( true );

  if ( OpenFlipper::Options::gui() )
  {
    coreWidget_->statusMessage( tr("Loading obj File ") + _filename + tr(" ... done"), 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }

  in.close();

}


//-----------------------------------------------------------------------------


void Core::preprocessObjFile(QString _filename)
{
  std::cout << "[preprocessObjFile] filename: " << _filename.toStdString() << std::endl;

  QString path = _filename;

  //check if path is relative
  if (path.startsWith( "." + OpenFlipper::Options::dirSeparator() ))
  {
    //check if _filename contains a path
    if (_filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) != "")
    {
      path.remove(0,1); // remove .
      path = _filename.section(OpenFlipper::Options::dirSeparator(), 0, -2) + path;
    }
  }


  // find out data type
  DataType dataType = DATA_UNKNOWN;

  std::string fname = _filename.toStdString();
  std::fstream in(fname.c_str(), std::ios_base::in);
  if (!in)
  {
    emit log(LOGERR,tr("Cannot open to obj file") + _filename);
    return;
  }


  std::string token;
  std::string tmp;

  bool typeFound = false;
  bool meshFound = false;

  while ( (!typeFound) && (!in.eof()) )
  {
    // read next token
    in >> token;

    if (token == "v") // ignore vertices, since they occur in many objects
      continue;

    else if (token == "f")
    {
      // we have found a mesh. now, find out if its a poly or a triangle mesh
      bool nextLine = false;
      int numVerticesPerFace = 0;

      meshFound = true;

      while((!nextLine) && (!in.eof()))
      {
        in >> tmp;
        if (tmp == "f")  nextLine = true;
        else ++numVerticesPerFace;
      }

      if (numVerticesPerFace > 3)
      {
        dataType = DATA_POLY_MESH;
        typeFound = true;
      }
    }

    // all faces processed and no poly mesh found
//     else if ((meshFound) && (typeFound == false))
//     {
//       dataType = DATA_TRIANGLE_MESH;
//       typeFound = true;
//     }

    // curve/surface type
    else if (token == "cstype")
    {
      bool nextLine = false;
      in >> tmp;
      if (tmp == "bspline")
      {
        // find out if it is a curve or a surface
        while((!nextLine) && (!in.eof()))
        {
          in >> tmp;
          if (tmp == "curv")
          {
            nextLine = true;
            dataType = typeId("BSplineCurve");
            typeFound = true;
          }
          else if (tmp == "surf")
          {
            nextLine = true;
            dataType = typeId("BSplineSurface");
            typeFound = true;
          }
        }

      }
    }

  }

  if (!typeFound)
  {
    dataType = DATA_TRIANGLE_MESH;
    emit log(LOGWARN, tr("Unable to get DataType for object ") +  _filename + tr(" - assuming Triangle Mesh") );
  }


  if (dataType == DATA_POLY_MESH)
    std::cout << _filename.toStdString() << " is of type DATA_POLY_MESH" << std::endl;
  else if (dataType == DATA_TRIANGLE_MESH)
    std::cout << _filename.toStdString() << " is of type DATA_TRIANGLE_MESH" << std::endl;
  else if  (dataType == typeId("BSplineCurve"))
    std::cout << _filename.toStdString() << " is of type DATA_BSPLINE_CURVE" << std::endl;
  else if  (dataType == typeId("BSplineSurface"))
    std::cout << _filename.toStdString() << " is of type DATA_BSPLINE_SURFACE" << std::endl;
  else
    std::cout << "no data type found " << std::endl;


  int newObjectId = loadObject(dataType, path);

  BaseObject* object = objectRoot_->childExists( newObjectId );
  if ( object == 0 )
    emit log(LOGERR,tr("Unable to open Object ") + path);

}

//-----------------------------------------------------------------------------


void Core::writeObjFile(QString _filename, bool _relativePaths, bool _targetOnly)
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
    QString file = o_it->path() + OpenFlipper::Options::dirSeparator() + o_it->name();

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
