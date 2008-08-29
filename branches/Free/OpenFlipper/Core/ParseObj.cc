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
    emit log(LOGERR,"Cannot open to obj file" + _filename);
    return;
  }

  if ( OpenFlipper::Options::gui() )
  {
    coreWidget_->statusMessage( "Loading obj File " + _filename + " ...");
    coreWidget_->setStatus(ApplicationStatus::BLOCKED );
  }

  OpenFlipper::Options::openingIni(true);


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


  OpenFlipper::Options::openingIni(false);

  resetScenegraph();

  if ( OpenFlipper::Options::gui() )
  {
    coreWidget_->statusMessage( "Loading obj File " + _filename + " ... done", 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }

  in.close();

}


//-----------------------------------------------------------------------------


void Core::preprocessObjFile(QString _filename)
{
  bool newActiveObject = false;

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
  DataType dataType;

  std::string fname = _filename.toStdString();
  std::fstream in(fname.c_str(), std::ios_base::in);
  if (!in)
  {
    emit log(LOGERR,"Cannot open to obj file" + _filename);
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
      in >> tmp;
      if (tmp == "bspline")
      {
        dataType = DATA_BSPLINE_CURVE;
        typeFound = true;
      }
    }

  }

  if (!typeFound)
  {
    dataType = DATA_TRIANGLE_MESH;
    emit log(LOGWARN, "Unable to get DataType for object " +  _filename + " - assuming Triangle Mesh" );
  }


  if (dataType == DATA_POLY_MESH)
    std::cout << _filename.toStdString() << " is of type DATA_POLY_MESH" << std::endl;
  else if (dataType == DATA_TRIANGLE_MESH)
    std::cout << _filename.toStdString() << " is of type DATA_TRIANGLE_MESH" << std::endl;
  else if  (dataType == DATA_BSPLINE_CURVE)
    std::cout << _filename.toStdString() << " is of type DATA_BSPLINE_CURVE" << std::endl;
  else
    std::cout << "no data type found " << std::endl;


  int newObjectId = loadObject(dataType, path);

  BaseObject* object = objectRoot_->childExists( newObjectId );
  if ( object == 0 )
    emit log(LOGERR,"Unable to open Object " + path);

  if ( newActiveObject )
    emit activeObjectChanged();
}

//-----------------------------------------------------------------------------


void Core::writeObjFile(QString _filename, bool _relativePaths)
{
  // open file
  std::string fname = _filename.toStdString();
  std::fstream out(fname.c_str(), std::ios_base::out);
  if (!out)
  {
    emit log(LOGERR,"Cannot open to obj file" + _filename);
    return;
  }


  if ( OpenFlipper::Options::gui() )
  {
    coreWidget_->statusMessage( "Saving obj File " + _filename + " ...");
    coreWidget_->setStatus(ApplicationStatus::BLOCKED );
  }


  // write all objects to a separate obj file and save external references in the global obj file
  for ( PluginFunctions::ObjectIterator o_it (PluginFunctions::ALL_OBJECTS) ;
                                        o_it != PluginFunctions::objects_end(); ++o_it)
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
    coreWidget_->statusMessage( "Saving obj File " + _filename + " ... done", 4000);
    coreWidget_->setStatus(ApplicationStatus::READY );
  }

}


//=============================================================================
