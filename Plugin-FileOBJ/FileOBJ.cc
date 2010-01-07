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
*   $Revision: 7926 $                                                       *
*   $Author: wilden $                                                      *
*   $Date: 2009-12-15 11:04:11 +0100 (Tue, 15 Dec 2009) $                   *
*                                                                           *
\*===========================================================================*/

#include <QtGui>
#include <QFileInfo>
#include <QSettings>
#include <QPushButton>

#include "FileOBJ.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

/// Constructor
FileOBJPlugin::FileOBJPlugin()
: loadOptions_(0),
  saveOptions_(0),
  triMeshHandling_(0),
  forceTriangleMesh_(false),
  forcePolyMesh_(false)
{
}

//-----------------------------------------------------------------------------------------------------

void FileOBJPlugin::initializePlugin() {
}

//-----------------------------------------------------------------------------------------------------

QString FileOBJPlugin::getLoadFilters() {
    return QString( tr("Alias/Wavefront ( *.obj )") );
};

//-----------------------------------------------------------------------------------------------------

QString FileOBJPlugin::getSaveFilters() {
    return QString( tr("Alias/Wavefront ( *.obj )") );
};

//-----------------------------------------------------------------------------------------------------

DataType  FileOBJPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH;
    return type;
}

//-----------------------------------------------------------------------------------------------------

void trimString( std::string& _string) {
  // Trim Both leading and trailing spaces

  size_t start = _string.find_first_not_of(" \t\r\n");
  size_t end   = _string.find_last_not_of(" \t\r\n");

  if(( std::string::npos == start ) || ( std::string::npos == end))
    _string = "";
  else
    _string = _string.substr( start, end-start+1 );
}

//-----------------------------------------------------------------------------

bool FileOBJPlugin::readMaterial(QString _filename, OBJImporter& _importer)
{
  std::string line;
  std::string keyWrd;
  std::string textureName;

  std::string matName;
  Material    mat;
  float       f1,f2,f3;
  bool        insideDefintion = false;
  int         textureId = 1;
  
 
  //open stream
  std::fstream matStream( _filename.toStdString().c_str(), std::ios_base::in );

  if ( !matStream ){
    emit log(LOGERR, tr("readMaterial : cannot not open file %1").arg(_filename) );
    return false;
  }

  //clear temp material
  mat.cleanup();

  //parse material file
  while( matStream && !matStream.eof() )
  {
    std::getline(matStream,line);
    if ( matStream.bad() ){
      emit log(LOGERR, tr("readMaterial : Warning! Could not read file properly!"));
      return false;
    }

    if ( line.empty() )
      continue;

    std::stringstream stream(line);

    stream >> keyWrd;

    if( isspace(line[0]) || line[0] == '#' )
    {
      if (insideDefintion && !matName.empty() && mat.is_valid())
      {
        _importer.materials()[matName] = mat;
        mat.cleanup();
      }
    }

    else if (keyWrd == "newmtl") // begin new material definition
    {
      stream >> matName;
      insideDefintion = true;
    }

    else if (keyWrd == "Kd") // diffuse color
    {
      stream >> f1; stream >> f2; stream >> f3;

      if( !stream.fail() )
        mat.set_Kd(f1,f2,f3);
    }

    else if (keyWrd == "Ka") // ambient color
    {
      stream >> f1; stream >> f2; stream >> f3;

      if( !stream.fail() )
        mat.set_Ka(f1,f2,f3);
    }

    else if (keyWrd == "Ks") // specular color
    {
      stream >> f1; stream >> f2; stream >> f3;

      if( !stream.fail() )
        mat.set_Ks(f1,f2,f3);
    }
#if 0
    else if (keyWrd == "illum") // diffuse/specular shading model
    {
      ; // just skip this
    }

    else if (keyWrd == "Ns") // Shininess [0..200]
    {
      ; // just skip this
    }

    else if (keyWrd == "map_") // map images
    {
      // map_Ks, specular map
      // map_Ka, ambient map
      // map_Bump, bump map
      // map_d,  opacity map
      ; // just skip this
    }
#endif
    else if (keyWrd == "map_Kd" ) {
      // Get the rest of the line, removing leading or trailing spaces
      // This will define the filename of the texture
      std::getline(stream,textureName);
      trimString(textureName);
      if ( ! textureName.empty() )
        mat.set_map_Kd( textureName, textureId++ );
    }
    else if (keyWrd == "Tr") // transparency value
    {
      stream >> f1;

      if( !stream.fail() )
        mat.set_Tr(f1);
    }
    else if (keyWrd == "d") // transparency value
    {
      stream >> f1;

      if( !stream.fail() )
        mat.set_Tr(f1);
    }

    if ( matStream && insideDefintion && mat.is_valid() && !matName.empty())
      _importer.materials()[matName] = mat;
  }


//   for ( MaterialList::iterator material = _importer.materials().begin(); material != _importer.materials().end(); material++ )
//   {
//     emit log("Material:" + QString(((*material).first).c_str()) );
//   }

  emit log( tr("%1 materials loaded.").arg( _importer.materials().size() ) );

  return true;
}

//-----------------------------------------------------------------------------


void FileOBJPlugin::addNewObject( OBJImporter& _importer, QString _name )
{
  
  //if no additional object is needed return
  if ( _importer.currentObject()+1 >= (int)_importer.objectOptions().size() )
    return;
  
  if ( _importer.isTriangleMesh( _importer.currentObject()+1 )  ){
  
    // add a triangle mesh
    int id = -1;
    emit addEmptyObject(DATA_TRIANGLE_MESH, id);
    
    
    BaseObjectData* object(0);
    
    if(PluginFunctions::getObject( id, object)){
      
      _importer.addObject( object );

      object->path( _importer.path() );
      object->setName( _name );
    }
  
  } else if ( _importer.isPolyMesh( _importer.currentObject()+1 )  ){
  
    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    
    BaseObjectData* object(0);
    
    if(PluginFunctions::getObject( id, object)){
      
      _importer.addObject( object );

      object->path( _importer.path() );
      object->setName( _name );
    }
  }
  
  //force gui settings
  if ( OpenFlipper::Options::gui() && loadOptions_ != 0 ){
    
    if ( !loadFaceColor_->isChecked() )
      _importer.objectOptions()[ _importer.currentObject() ] |= OBJImporter::FORCE_NOCOLOR;
    
    if ( !loadNormals_->isChecked() )
      _importer.objectOptions()[ _importer.currentObject() ] |= OBJImporter::FORCE_NONORMALS;
    
    if ( !loadTexCoords_->isChecked() )
      _importer.objectOptions()[ _importer.currentObject() ] |= OBJImporter::FORCE_NOTEXTURES;
  }
  
}


//add textures to the mesh
void FileOBJPlugin::addTextures(OBJImporter& _importer, int _objectID ){

  // TODO : If only one Texture, use single Texturing mode
  if ( true ) {

    BaseObject* object = _importer.object(_objectID);
    
    if (!object)
      return;
    
    std::map< int,int > newMapping;
    // zero ( no texture ) always maps to to zero
    newMapping[0]=0;
    
    const std::vector<std::string> matNames = _importer.usedMaterials( _objectID );
    
    for (uint i=0; i < matNames.size(); i++){
    
      Material& material = _importer.materials()[ matNames[i] ];
      
      int textureId = -1;
      
      QString textureBlock = QString( material.map_Kd().c_str());


      QStringList options = textureBlock.split(" ",QString::SkipEmptyParts);

      while ( options.size() > 1 ) {
        if ( options[0] == "-blendu" ) {
        } else if ( options[0] == "-blendu" ) {
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-blendv" ) {
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-cc" ) {
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-clamp" ) {
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-mm" ) {
          options.pop_front();
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-o" ) {
          options.pop_front();
          options.pop_front();
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-s" ) {
          options.pop_front();
          options.pop_front();
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-t" ) {
          options.pop_front();
          options.pop_front();
          options.pop_front();
          options.pop_front();
        } else if ( options[0] == "-texres" ) {
          options.pop_front();
          options.pop_front();
        } else  {
          break;
        }
      }

      QString fullName = _importer.path() + QDir::separator() + options.join(" ");

      QFileInfo info(fullName);
      if ( info.exists() )
        emit addMultiTexture("OBJ Data", fullName, fullName, object->id(), textureId );
      else {
        emit log(LOGWARN, tr("Unable to load texture image %1").arg( QString(material.map_Kd().c_str()) ) );
        addMultiTexture("OBJ Data","Unknown Texture image " + QString::number(textureId), "unknown.png", object->id(), textureId );
      }
  
      newMapping[ material.map_Kd_index() ] = textureId;
    }
    
    //now map all texture indices to the real texture indices used in OpenFlipper
    
    OpenMesh::FPropHandleT< int > indexProperty;
    
    //handle PolyMeshes
    PolyMeshObject* polyMeshObj = dynamic_cast< PolyMeshObject* > (object);
    
    if ( polyMeshObj ){
      
      PolyMesh& mesh = *(polyMeshObj->mesh());

      PolyMesh::FaceIter f_it;
      PolyMesh::FaceIter f_end = mesh.faces_end();

      if (! mesh.get_property_handle(indexProperty,"OriginalTexIndexMapping") )
        return;
      
      for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
        mesh.property(indexProperty, f_it) = newMapping[ mesh.property(indexProperty, f_it) ];
        
      return;
    }
    
    //handle new TriMeshes
    TriMeshObject* triMeshObj = dynamic_cast< TriMeshObject* > (object);
    
    if ( triMeshObj ){
      
      TriMesh& mesh = *(triMeshObj->mesh());

      TriMesh::FaceIter f_it;
      TriMesh::FaceIter f_end = mesh.faces_end();

      if (! mesh.get_property_handle(indexProperty,"OriginalTexIndexMapping") )
        return;
      
      for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
        mesh.property(indexProperty, f_it) = newMapping[ mesh.property(indexProperty, f_it) ];
        
      return;
    }
  }
}


void FileOBJPlugin::readOBJFile(QString _filename, OBJImporter& _importer)
{
 
  QString path = QFileInfo(_filename).absolutePath();
  
  //setup filestream
  
  std::fstream input( _filename.toStdString().c_str(), std::ios_base::in );

  if ( !input.is_open() || !input.good() ){
    emit log(LOGERR, tr("readOBJFile : cannot not open file %1").arg(_filename) );
    return;
  }

  std::string line;
  std::string keyWrd;

  float                  x, y, z, u, v;

  std::vector<VertexHandle> vhandles;
  std::vector<int>          face_texcoords;
  std::string               matname;

  int faceCount = 0;

  _importer.setPath( path );
  
  addNewObject(_importer, QFileInfo(_filename).fileName() );
  
  while( input && !input.eof() )
  {
    std::getline(input,line);
    if ( input.bad() ){
      emit log(LOGERR, tr("readOBJFile : Warning! Could not read file properly!"));
      return;
    }

    // Trim Both leading and trailing spaces
    trimString(line);
    
    // comment
    if ( line.size() == 0 || line[0] == '#' || isspace(line[0]) ) {
      continue;
    }

    std::stringstream stream(line);

    stream >> keyWrd;

    // material file
    if (keyWrd == "mtllib")
    {
      std::string matString;

      // This will define the filename of the texture
      std::getline(stream,matString);
      
      QString matFile = path + QDir::separator() + QString( matString.c_str() ).trimmed();

      emit log( tr("Loading material file: %1").arg( matFile ) );

      readMaterial( matFile, _importer );
    }

    // usemtl
    else if (keyWrd == "usemtl")
    {
      stream >> matname;
      if ( _importer.materials().find(matname)==_importer.materials().end() )
      {
        emit log( LOGERR, tr("Warning! Material '%1' not defined in material file").arg( QString(matname.c_str()) ) );
        matname="";
        
      }else{
        
        Material& mat = _importer.materials()[matname];
    
        if ( mat.has_map_Kd() )
          _importer.useMaterial( matname );
      }
    }

    // vertex
    else if (keyWrd == "v")
    {
      stream >> x; stream >> y; stream >> z;

      if ( !stream.fail() )
        _importer.addVertex( OpenMesh::Vec3f(x,y,z) );
    }

    // texture coord
    else if (keyWrd == "vt")
    {
      stream >> u; stream >> v;

      if ( !stream.fail() ){

        _importer.addTexCoord( OpenMesh::Vec2f(u, v) );

      }else{

        emit log( LOGERR, tr("Only single 2D texture coordinate per vertex allowed") );
        return;
      }
    }


    // normal
    else if (keyWrd == "vn")
    {
      stream >> x; stream >> y; stream >> z;

      if ( !stream.fail() ){
        _importer.addNormal( OpenMesh::Vec3f(x,y,z) );
      }
    }

    // group
    else if (keyWrd == "g"){
      
      std::string groupName;
      std::getline(stream,groupName);

      if ( faceCount > 0 )
        addNewObject( _importer, QString(groupName.c_str()) );
      else
        _importer.setObjectName( _importer.currentObject(), QString(groupName.c_str()) );

      //since obj-groups are used, all new objects will be grouped together in OpenFlipper
      _importer.setGroup( QFileInfo(_filename).fileName() );

      faceCount = 0;
    }

    // face
    else if (keyWrd == "f")
    {

      int component(0), nV(0);
      int value;

      vhandles.clear();
      face_texcoords.clear();

      // read full line after detecting a face
      std::string faceLine;
      std::getline(stream,faceLine);
      std::stringstream lineData( faceLine );

      // work on the line until nothing left to read
      while ( !lineData.eof() )
      {
        // read one block from the line ( vertex/texCoord/normal )
        std::string vertex;
        lineData >> vertex;

        do{

          //get the component (vertex/texCoord/normal)
          size_t found=vertex.find("/");

          // parts are seperated by '/' So if no '/' found its the last component
          if( found != std::string::npos ){

            // read the index value
            std::stringstream tmp( vertex.substr(0,found) );

            // If we get an empty string this property is undefined in the file
            if ( vertex.substr(0,found).empty() ) {
              // Switch to next field
              vertex = vertex.substr(found+1);

              // Now we are at the next component
              ++component;

              // Skip further processing of this component
              continue;
            }

            // Read current value
            tmp >> value;

            // remove the read part from the string
            vertex = vertex.substr(found+1);

          } else {

            // last component of the vertex, read it.
            std::stringstream tmp( vertex );
            tmp >> value;

            // Clear vertex after finished reading the line
            vertex="";

            // Nothing to read here ( garbage at end of line )
            if ( tmp.fail() ) {
              continue;
            }
          }

          // store the component ( each component is referenced by the index here! )
          switch (component)
          {
            case 0: // vertex
              if ( value < 0 ) {
                // Calculation of index :
                // -1 is the last vertex in the list
                // As obj counts from 1 and not zero add +1
                value = _importer.n_vertices() + value + 1;
              }

              // Obj counts from 1 and not zero .. array counts from zero therefore -1
              vhandles.push_back( value-1 );
              _importer.checkExistance( value -1 );
              break;

            case 1: // texture coord
              if ( value < 0 ) {
                // Calculation of index :
                // -1 is the last vertex in the list
                // As obj counts from 1 and not zero add +1
                value = _importer.n_texCoords() + value + 1;
              }
              assert(!vhandles.empty());
              if ( _importer.n_texCoords() > 0 && (unsigned int)(value-1) < _importer.n_texCoords() ) {
                // Obj counts from 1 and not zero .. array counts from zero therefore -1
                _importer.setVertexTexCoord( vhandles.back(), value-1 );
                face_texcoords.push_back( value-1 );
              } else {
                emit log( LOGERR, tr("Error setting Texture coordinates") );
              }

              break;

            case 2: // normal
              if ( value < 0 ) {
                // Calculation of index :
                // -1 is the last vertex in the list
                // As obj counts from 1 and not zero add +1
                value = _importer.n_normals() + value + 1;
              }
              assert(!vhandles.empty());
              assert((unsigned int)(value-1) < _importer.n_normals() );
              // Obj counts from 1 and not zero .. array counts from zero therefore -1
              _importer.setNormal(vhandles.back(), value-1);
              break;
          }

          // Prepare for reading next component
          ++component;

          // Read until line does not contain any other info
        } while ( !vertex.empty() );

        component = 0;
        nV++;
      }


      if( !vhandles.empty() ){

        if ( face_texcoords.size() > 0 )
          //if we have texCoords add face+texCoords
          _importer.addFace(vhandles, face_texcoords );
        else
          //otherwise just add the face
          _importer.addFace(vhandles); 

        faceCount++;
      }


      //add material to the last added face(s)
      //if polygons get triangulated this can be more than one face
      _importer.addMaterial( matname );
    }
  }
}


void FileOBJPlugin::checkTypes(QString _filename, OBJImporter& _importer)
{
  //setup filestream
  
  std::fstream input( _filename.toStdString().c_str(), std::ios_base::in );

  if ( !input.is_open() || !input.good() ){
    emit log(LOGERR, tr("readOBJFile : cannot not open file %1").arg(_filename) );
    return;
  }

  std::string line;
  std::string keyWrd;
  
  int faceCount = 0;
  
  int PolyMeshCount = 0;
  int TriMeshCount = 0;
  
  OBJImporter::ObjectOptions options = OBJImporter::TRIMESH;
  
  while( input && !input.eof() )
  {
    std::getline(input,line);
    if ( input.bad() ){
      emit log(LOGERR, tr("readOBJFile : Warning! Could not read file properly!"));
      return;
    }

    // Trim Both leading and trailing spaces
    trimString(line);
    
    // comment
    if ( line.size() == 0 || line[0] == '#' || isspace(line[0]) ) {
      continue;
    }

    std::stringstream stream(line);

    stream >> keyWrd;

    // group
    if (keyWrd == "g"){
      if ( faceCount > 0 ){
        //give options to importer and reinitialize
        //for next object
        if ( options & OBJImporter::TRIMESH  ) TriMeshCount++;
        if ( options & OBJImporter::POLYMESH ) PolyMeshCount++;

        _importer.addObjectOptions( options );
        options = OBJImporter::TRIMESH;
        faceCount = 0;
      }
    }
    // face
    else if (keyWrd == "f"){

      faceCount++;
      
      int verticesPerFace = 0;

      // read full line after detecting a face
      std::string faceLine;
      std::getline(stream,faceLine);
      std::stringstream lineData( faceLine );

      // work on the line until nothing left to read
      while ( !lineData.eof() )
      {
        // read one block from the line ( vertex/texCoord/normal )
        std::string vertex;
        lineData >> vertex;

        verticesPerFace++;
      }


      if( verticesPerFace > 3 )
        options = OBJImporter::POLYMESH;
    }
  }
  
  if ( options & OBJImporter::TRIMESH  ) TriMeshCount++;
  if ( options & OBJImporter::POLYMESH ) PolyMeshCount++;
  _importer.addObjectOptions( options );
  
  if (TriMeshCount == 0 && PolyMeshCount == 0)
    return;
  
  if (forceTriangleMesh_){
    _importer.forceMeshType( OBJImporter::TRIMESH );
    return;
  }

  if (forcePolyMesh_){
    _importer.forceMeshType( OBJImporter::POLYMESH );
    return;
  }

  if ( OpenFlipper::Options::gui() && triMeshHandling_ != 0 ){
    
    QMessageBox msgBox;
    QPushButton *detectButton = msgBox.addButton(tr("Auto-Detect"), QMessageBox::ActionRole);
    QPushButton *triButton    = msgBox.addButton(tr("Open as triangle mesh"), QMessageBox::ActionRole);
    QPushButton *polyButton   = msgBox.addButton(tr("Open as poly mesh"), QMessageBox::ActionRole);
    
    switch( triMeshHandling_->currentIndex() ){
      case 0: //ask

        msgBox.setWindowTitle( tr("Mesh types in file") );
        msgBox.setText( tr("You are about to open a file containing one or more mesh types. \n\n Which mesh type should be used?") );
        msgBox.setDefaultButton( detectButton );
        msgBox.exec();

        if (msgBox.clickedButton() == triButton)
          _importer.forceMeshType( OBJImporter::TRIMESH );
        else if (msgBox.clickedButton() == polyButton)
          _importer.forceMeshType( OBJImporter::POLYMESH );

        break;

      case 1: //Detect
        break;
      case 2: //trimesh
        _importer.forceMeshType( OBJImporter::TRIMESH ); break;
      case 3: //polyMesh
        _importer.forceMeshType( OBJImporter::POLYMESH ); break;
      default: break;
    }

  }
  
}

//-----------------------------------------------------------------------------------------------------

int FileOBJPlugin::loadObject(QString _filename) {
  
  OBJImporter importer;
  
  //preprocess file and store types in ObjectOptions
  checkTypes( _filename, importer );
  
  //then parse the obj
  readOBJFile( _filename, importer );
  
  int returnID = -1;
  
  //perhaps add group
  if ( importer.group() != "" ){
   
    bool dataControlExists = false;
    pluginExists( "datacontrol", dataControlExists );
    
    if ( dataControlExists ){
    
      IdList objIDs;
    
      for(uint i=0; i < importer.objectCount(); i++)
        objIDs.push_back( importer.object(i)->id() );
    
    
      returnID = RPC::callFunctionValue<int>("datacontrol","groupObjects", objIDs, importer.group());
    }
  }
  
  //check all new objects
  for(uint i=0; i < importer.objectCount(); i++){
    
    BaseObject* object = importer.object(i);
    
    //remember the id of the first opened object
    if ( i == 0 && importer.group() == "" )
      returnID = object->id();
    
    //handle new PolyMeshes
    PolyMeshObject* polyMeshObj = dynamic_cast< PolyMeshObject* > (object);
    
    if ( polyMeshObj ){
      
      if ( !importer.hasNormals(i) )
        polyMeshObj->mesh()->update_normals();
      else
        polyMeshObj->mesh()->update_face_normals();
      
      polyMeshObj->update();
      polyMeshObj->show();
    }
    
    //handle new TriMeshes
    TriMeshObject* triMeshObj = dynamic_cast< TriMeshObject* > (object);
    
    if ( triMeshObj ){
      
      if ( !importer.hasNormals(i) || importer.hasOption( i, OBJImporter::FORCE_NONORMALS ) )
        triMeshObj->mesh()->update_normals();
      else
        triMeshObj->mesh()->update_face_normals();
      
      triMeshObj->update();
      triMeshObj->show();
    }
    
    //general stuff
    emit log(LOGINFO,object->getObjectinfo());
    emit openedFile( object->id() );
    
    //textures
    if ( importer.hasTexture(i) && !importer.hasOption( i, OBJImporter::FORCE_NOTEXTURES ) ){
      
      //add the textures to the object
      addTextures( importer, i );
      
      //set the texture index property to be used
      emit setTextureMode("OBJ Data","indexProperty=OriginalTexIndexMapping", object->id() );

      emit switchTexture("OBJ Data", object->id() );
      
      PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE_SHADED, PluginFunctions::ALL_VIEWERS );

    }
    
    PluginFunctions::viewAll();
  }
  
  forceTriangleMesh_ = false;
  forcePolyMesh_     = false;
  
  return returnID;
}

//-----------------------------------------------------------------------------------------------------

/// load a triangle-mesh with given filename
int FileOBJPlugin::loadTriMeshObject(QString _filename){
  
  forceTriangleMesh_ = true;
  
  return loadObject(_filename);
}

//-----------------------------------------------------------------------------------------------------

/// load a poly-mesh with given filename
int FileOBJPlugin::loadPolyMeshObject(QString _filename){
  
  forcePolyMesh_     = true;
  
  return loadObject(_filename);
}

//-----------------------------------------------------------------------------------------------------

int FileOBJPlugin::getMaterial(OpenMesh::Vec4f _color)
{
  for (uint i=0; i < materials_.size(); i++)
    if(materials_[i] == _color)
      return i;

  //not found add new material
  materials_.push_back( _color );
  return materials_.size()-1;
}

//-----------------------------------------------------------------------------------------------------

bool FileOBJPlugin::saveObject(int _id, QString _filename)
{
  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);


  //open output stream
  std::string filename = std::string( _filename.toUtf8() );

  std::fstream objStream( filename.c_str(), std::ios_base::out );

  if ( !objStream ){

    emit log(LOGERR, tr("saveObject : cannot not open file %1").arg(_filename) );
    return false;
  }

  //write object
  if ( object->dataType( DATA_POLY_MESH ) ) {

    object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
    object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

    PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );


    if ( writeMesh( objStream, _filename, *polyObj->mesh() ) ){
      
      emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
      objStream.close();
      return true;
        
    } else {
      
      emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
      objStream.close();
      return false;
    }
      
  } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
    object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

    TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );


    if ( writeMesh( objStream, _filename, *triObj->mesh()) ) {
      
      emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
      objStream.close();
      return true;
        
    } else {
    
      emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
      objStream.close();
      return false;
    }
    
  } else {
    
    emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
    objStream.close();
    return false;
  }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOBJPlugin::saveOptionsWidget(QString _currentFilter) {
    
    if (saveOptions_ == 0){
        //generate widget
        saveOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        saveFaceColor_ = new QCheckBox("Save Face Colors");
        layout->addWidget(saveFaceColor_);
        
        saveAlpha_ = new QCheckBox("Save Color Alpha");
        layout->addWidget(saveAlpha_);
        
        saveNormals_ = new QCheckBox("Save Normals");
        layout->addWidget(saveNormals_);
        
        saveTexCoords_ = new QCheckBox("Save Texture Coordinates");
        layout->addWidget(saveTexCoords_);
        
        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);       
        
        saveOptions_->setLayout(layout);
        
        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));
        
        saveFaceColor_->setChecked( OpenFlipperSettings().value("FileObj/Save/FaceColor",true).toBool() );
        saveAlpha_->setChecked( OpenFlipperSettings().value("FileObj/Save/Alpha",true).toBool() );
        saveNormals_->setChecked( OpenFlipperSettings().value("FileObj/Save/Normals",true).toBool() );
        saveTexCoords_->setChecked( OpenFlipperSettings().value("FileObj/Save/Textures",true).toBool() );
        
    } 
    
    return saveOptions_;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOBJPlugin::loadOptionsWidget(QString /*_currentFilter*/) {
    
    if (loadOptions_ == 0){
        //generate widget
        loadOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        QLabel* label = new QLabel(tr("If file contains meshes:"));
        
        layout->addWidget(label);
        
        triMeshHandling_ = new QComboBox();
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Detect correct type") );
        triMeshHandling_->addItem( tr("Open as PolyMesh") );
        triMeshHandling_->addItem( tr("Open as TriangleMesh") );
        
        layout->addWidget(triMeshHandling_);
        
        loadFaceColor_ = new QCheckBox("Load Face Colors");
        layout->addWidget(loadFaceColor_);
        
        loadNormals_ = new QCheckBox("Load Normals");
        layout->addWidget(loadNormals_);
        
        loadTexCoords_ = new QCheckBox("Load Textures");
        layout->addWidget(loadTexCoords_);
 
        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
        
        
        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FileObj/Load/TriMeshHandling",2).toInt() );
        
        loadFaceColor_->setChecked( OpenFlipperSettings().value("FileObj/Load/FaceColor",true).toBool()  );
        loadNormals_->setChecked( OpenFlipperSettings().value("FileObj/Load/Normals",true).toBool()  );
        loadTexCoords_->setChecked( OpenFlipperSettings().value("FileObj/Load/Textures",true).toBool()  );
    }
    
    return loadOptions_;
}

void FileOBJPlugin::slotLoadDefault() {
  OpenFlipperSettings().setValue( "FileObj/Load/FaceColor",   loadFaceColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Load/Normals",     loadNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Load/Textures",   loadTexCoords_->isChecked()  );

  OpenFlipperSettings().setValue("FileObj/Load/TriMeshHandling", triMeshHandling_->currentIndex() );
}


void FileOBJPlugin::slotSaveDefault() {
  OpenFlipperSettings().setValue( "FileObj/Save/FaceColor",   saveFaceColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Save/Normals",     saveNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Save/Textures",   saveTexCoords_->isChecked()  );
}

Q_EXPORT_PLUGIN2( fileobjplugin , FileOBJPlugin );

