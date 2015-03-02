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
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QFileInfo>
#include <QSettings>
#include <QPushButton>

#include "FileOBJ.hh"

#include <iostream>


// Defines for the type handling drop down box
#define TYPEAUTODETECT 0
#define TYPEASK        1
#define TYPEPOLY       2
#define TYPETRIANGLE   3

//-----------------------------------------------------------------------------
// help functions
namespace{
  template<typename T>
  class HasSeen : public std::unary_function <T, bool>
  {
  public:
    HasSeen () : seen_ () { }

    bool operator ()(const T& i) const
    {
      return (!seen_.insert(i).second);
    }

  private:
    mutable std::set<T> seen_;
  };
}

void remove_duplicated_vertices(VHandles& _indices)
{
  _indices.erase(std::remove_if(_indices.begin(),_indices.end(),HasSeen<int>()),_indices.end());
}

//-----------------------------------------------------------------------------

/// Constructor
FileOBJPlugin::FileOBJPlugin()
: loadOptions_(0),
  saveOptions_(0),
  saveBinary_(0),
  saveVertexColor_(0),
  saveFaceColor_(0),
  saveAlpha_(0),
  saveNormals_(0),
  saveTexCoords_(0),
  saveTextures_(0),
  saveCopyTextures_(0),
  saveCreateTexFolder_(0),
  savePrecisionLabel_(0),
  savePrecision_(0),
  saveDefaultButton_(0),
  triMeshHandling_(0),
  loadVertexColor_(0),
  loadFaceColor_(0),
  loadAlpha_(0),
  loadNormals_(0),
  loadTexCoords_(0),
  loadTextures_(0),
  loadDefaultButton_(0),
  forceTriangleMesh_(false),
  forcePolyMesh_(false),
  textureIndexPropFetched_(false)
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
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH | DATA_GROUP;

    #ifdef ENABLE_BSPLINECURVE_SUPPORT
      type |= DATA_BSPLINE_CURVE;
    #endif

    #ifdef ENABLE_BSPLINESURFACE_SUPPORT
      type |= DATA_BSPLINE_SURFACE;
    #endif

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
  std::fstream matStream( _filename.toUtf8(), std::ios_base::in );

  if ( !matStream ){
    emit log(LOGERR, tr("readMaterial : cannot open file %1").arg(_filename) );
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

    if( ( isspace(line[0]) && line[0] != '\t' ) || line[0] == '#' )
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

void FileOBJPlugin::createAllGroupObjects(OBJImporter& _importer) {

    for(unsigned int i = 0; i < _importer.numGroups(); ++i) {

        // Get group name
        QString name = _importer.groupName(i);
        convertToOBJName(name);

        if ( _importer.isTriangleMesh( i )  ){

            // add a triangle mesh
            int id = -1;
            emit addEmptyObject(DATA_TRIANGLE_MESH, id);

            BaseObjectData* object(0);

            if (PluginFunctions::getObject(id, object)) {

                _importer.setObject(object, i);

                object->setPath(_importer.path());
                object->setName(name);
            }

        } else if (_importer.isPolyMesh( i )) {

            int id = -1;
            emit addEmptyObject(DATA_POLY_MESH, id);

            BaseObjectData* object(0);

            if (PluginFunctions::getObject(id, object)) {

                _importer.setObject(object, i);

                object->setPath(_importer.path());
                object->setName(name);
            }
        }

#ifdef ENABLE_BSPLINECURVE_SUPPORT

        else if (_importer.isCurve( i )) {

            int id = -1;
            emit addEmptyObject(DATA_BSPLINE_CURVE, id);

            BaseObjectData* object(0);

            if (PluginFunctions::getObject(id, object)) {

                _importer.setObject(object, i);

                object->setPath(_importer.path());
                object->setName(name);
            }
        }

#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT

        else if (_importer.isSurface( i )) {

            int id = -1;
            emit addEmptyObject(DATA_BSPLINE_SURFACE, id);

            BaseObjectData* object(0);

            if (PluginFunctions::getObject(id, object)) {

                _importer.setObject(object, i);

                object->setPath(_importer.path());
                object->setName(name);
            }
        }

#endif

        //force gui settings
        if (OpenFlipper::Options::gui() && loadOptions_ != 0) {

            if (!loadFaceColor_->isChecked())
                _importer.objectOptions()[ i ] |= OBJImporter::FORCE_NOCOLOR;

            if (!loadNormals_->isChecked())
                _importer.objectOptions()[ i ] |= OBJImporter::FORCE_NONORMALS;

            if (!loadTexCoords_->isChecked() || !loadTextures_->isChecked())
                _importer.objectOptions()[ i ] |= OBJImporter::FORCE_NOTEXTURES;
        }
    }
}

void FileOBJPlugin::convertToOBJName(QString& _name) {

    QFileInfo fi(_name);

    QString n = fi.baseName();

    _name = n.trimmed() + ".obj";
}

/// creates a backup of the original per vertex/face texture coordinates
template <class MeshT>
void FileOBJPlugin::backupTextureCoordinates(MeshT& _mesh) {

    // Create a backup of the original per Vertex texture Coordinates
    if (_mesh.has_vertex_texcoords2D()) {

      OpenMesh::VPropHandleT< typename MeshT::TexCoord2D > oldVertexCoords;
      if (!_mesh.get_property_handle(oldVertexCoords, "Original Per Vertex Texture Coords"))
        _mesh.add_property(oldVertexCoords, "Original Per Vertex Texture Coords");

      for (typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it)
        _mesh.property(oldVertexCoords, *v_it) =  _mesh.texcoord2D(*v_it);

    }

    // Create a backup of the original per Face texture Coordinates
    if (_mesh.has_halfedge_texcoords2D()) {

      OpenMesh::HPropHandleT< typename MeshT::TexCoord2D > oldHalfedgeCoords;
      if (!_mesh.get_property_handle(oldHalfedgeCoords,"Original Per Face Texture Coords"))
        _mesh.add_property(oldHalfedgeCoords,"Original Per Face Texture Coords");

      for (typename MeshT::HalfedgeIter he_it = _mesh.halfedges_begin(); he_it != _mesh.halfedges_end(); ++he_it)
        _mesh.property(oldHalfedgeCoords, *he_it) =  _mesh.texcoord2D(*he_it);

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

    for (unsigned int i=0; i < matNames.size(); i++){

      Material& material = _importer.materials()[ matNames[i] ];

      int textureId = -1;

      QString textureBlock = QString( material.map_Kd().c_str());


      QStringList options = textureBlock.split(" ",QString::SkipEmptyParts);

      while ( options.size() > 1 ) {
        if ( options[0] == "-blendu" ) {
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
        emit addMultiTexture("OBJ Data", info.baseName().trimmed(), fullName, object->id(), textureId );
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

      if (! mesh.get_property_handle(indexProperty,TEXTUREINDEX) )
        return;

      for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
        mesh.property(indexProperty, *f_it) = newMapping[ mesh.property(indexProperty, *f_it) ];

      backupTextureCoordinates(mesh);

      return;
    }

    //handle new TriMeshes
    TriMeshObject* triMeshObj = dynamic_cast< TriMeshObject* > (object);

    if ( triMeshObj ){

      TriMesh& mesh = *(triMeshObj->mesh());

      TriMesh::FaceIter f_it;
      TriMesh::FaceIter f_end = mesh.faces_end();

      if (! mesh.get_property_handle(indexProperty,TEXTUREINDEX) )
        return;

      for (f_it = mesh.faces_begin(); f_it != f_end; ++f_it)
        mesh.property(indexProperty, *f_it) = newMapping[ mesh.property(indexProperty, *f_it) ];

      backupTextureCoordinates(mesh);

      return;
    }
  }
}

void FileOBJPlugin::readOBJFile(QString _filename, OBJImporter& _importer)
{

  QString path = QFileInfo(_filename).absolutePath();

  //setup filestream

  std::fstream input( _filename.toUtf8(), std::ios_base::in );

  if ( !input.is_open() || !input.good() ){
    emit log(LOGERR, tr("readOBJFile : cannot open file %1").arg(_filename) );
    return;
  }

  QString currentFileName = QFileInfo(_filename).fileName() ;

  ReaderMode mode = NONE;

  std::string line;
  std::string keyWrd;
  std::string nextKeyWrd = "";

#ifdef ENABLE_BSPLINECURVE_SUPPORT
  unsigned int curveCount = 0;
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  unsigned int surfaceCount = 0;
#endif

  float x, y, z, u, v;
  int   deg;

  std::vector<VertexHandle> vhandles;
  std::vector<int>          face_texcoords;
  std::string               matname;

#if defined (ENABLE_BSPLINECURVE_SUPPORT) || defined (ENABLE_BSPLINESURFACE_SUPPORT)
  std::vector< int > cpIndices;
  std::vector< double > knotsU,knotsV;
#endif

  int faceCount = 0;

  // We have to keep track of the already read number of vertices to resolve relative (negative indices)
  int currentVertexCount = 0;

  // We have to keep track of the already read number of Texture coordinates to resolve relative (negative indices)
  int currentTextureCoordCount = 0;

  // We have to keep track of the already read number of normals to resolve relative (negative indices)
  int currentNormalCount = 0;

  // keeps track if faces belong to a group or the default group
  bool inGroup = false;
  // keeps track if the first face of a mesh has been read yet or not
  bool firstFace = true;

  _importer.setPath( path );

  // Set filename for default mesh
  _importer.setGroupName(0, currentFileName);

  // Now add all meshes for every group (if exists)
  createAllGroupObjects(_importer);

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
    if ( line.empty() || line[0] == '#' || isspace(line[0]) ) {
      continue;
    }

    std::stringstream stream(line);

    //unless the keyWrd for the new line is not determined by the previous line
    //read it from stream
    if (nextKeyWrd == "")
      stream >> keyWrd;
    else {
      keyWrd = nextKeyWrd;
      nextKeyWrd = "";
    }

    // material file
    if (mode == NONE && keyWrd == "mtllib")
    {
      std::string matString;

      // This will define the filename of the texture
      std::getline(stream,matString);

      QString matFile = path + QDir::separator() + QString( matString.c_str() ).trimmed();

      emit log( tr("Loading material file: %1").arg( matFile ) );

      readMaterial( matFile, _importer );
    }

    // usemtl
    else if (mode == NONE && keyWrd == "usemtl")
    {
      stream >> matname;
      if ( _importer.materials().find(matname)==_importer.materials().end() )
      {
        emit log( LOGERR, tr("Warning! Material '%1' not defined in material file").arg( QString(matname.c_str()) ) );
        matname="";

      }else{

        Material& mat = _importer.materials()[matname];

        if ( mat.has_Texture() ){
          //add object if not already there
//          if (_importer.currentObject() == -1) {
//            addNewObject(_importer, currentFileName );
//          }

          _importer.useMaterial( matname );
        }
      }
    }
    else if (mode == NONE && keyWrd == "v")
    {
     if (!firstFace)
       firstFace = true;

     currentVertexCount++;
    }
    // texture coord
    else if (mode == NONE && keyWrd == "vt")
    {
      if (!firstFace)
        firstFace = true;

      // New texture coordinate read so increase counter
      currentTextureCoordCount++;

      stream >> u; stream >> v;

      if ( !stream.fail() ){

        _importer.addTexCoord( OpenMesh::Vec2f(u, v) );

      }else{

        emit log( LOGERR, tr("Could not add TexCoord. Possible NaN or Inf?\nOnly single 2D texture coordinate per vertex allowed"));
      }
    }


    // normal
    else if (mode == NONE && keyWrd == "vn")
    {
      if (!firstFace)
        firstFace = true;

      // New normal read so increase counter
      currentNormalCount++;

      stream >> x; stream >> y; stream >> z;

      if ( !stream.fail() ){
        _importer.addNormal( OpenMesh::Vec3f(x,y,z) );
      }else{
        emit log( LOGERR, tr("Could not read normal. Possible NaN or Inf?"));
      }
    }

    // degree (for curves)
    else if (mode == NONE && keyWrd == "deg")
    {
      stream >> deg;

      if ( !stream.fail() )
        _importer.setDegreeU( deg );

      stream >> deg;

      if ( !stream.fail() )
        _importer.setDegreeV( deg );
    }

    // group
    else if (mode == NONE && keyWrd == "g"){
      if (!firstFace)
        firstFace = true;

      std::string groupName;
      std::getline(stream, groupName);

      if(faceCount == 0) {
        currentFileName = QString(groupName.c_str());
      }

      int id = _importer.groupId(groupName.c_str());
      if(id == -1) {
          std::cerr << "Error: Group has not been added before!" << std::endl;
          return;
      }
      _importer.setCurrentGroup(id);
      inGroup = true;

      faceCount = 0;
    }

    // face
    else if (mode == NONE && keyWrd == "f")
    {
      if (firstFace) {
        // store faces in the default Group if we aren't in a group already
        if (!inGroup)
          _importer.setCurrentGroup(0);

        firstFace = false;
      }

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
                value = currentVertexCount + value + 1;
              }

              // Obj counts from 1 and not zero .. array counts from zero therefore -1
              vhandles.push_back( value-1 );
              break;

            case 1: // texture coord
              if ( value < 0 ) {
                // Calculation of index :
                // -1 is the last vertex in the list
                // As obj counts from 1 and not zero add +1
                value = currentTextureCoordCount + value + 1;
              }

              if (vhandles.empty())
              {
                emit log (LOGWARN, tr("Texture coordinates defined, but no vertex coordinates found!"));
                break;
              }
              if ((unsigned int)(value-1) >= _importer.n_texCoords())
              {
                emit log (LOGWARN, tr("Too many texcoords defined, skipping the rest"));
                break;
              }

              if ( _importer.n_texCoords() > 0 ) {
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
                value = currentNormalCount + value + 1;
              }

              if (vhandles.empty())
              {
                emit log (LOGWARN, tr("Texture coordinates defined, but no vertex coordinates found!"));
                break;
              }

              if ((unsigned int)(value-1) >= _importer.n_normals())
              {
                emit log (LOGWARN, tr("Too many normals defined, skipping the rest"));
                break;
              }

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

      // remove vertices which can lead to degenerated faces
      remove_duplicated_vertices(vhandles);

      // from spec: A minimum of three vertices are required.
      if( vhandles.size() > 2 ){

        if ( !face_texcoords.empty() )
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

#ifdef ENABLE_BSPLINECURVE_SUPPORT
    // param
    else if ( (mode == CURVE && keyWrd == "parm") || (mode == CURVE && keyWrd == "parm_add") ){

      //get curve knots
      std::string paramLine;
      std::string tmp;

      std::getline(stream, paramLine);

      // value may contain a / as line separator
      if ( QString( paramLine.c_str() ).endsWith("\\")){
        paramLine = paramLine.substr(0, paramLine.length()-1);
        nextKeyWrd = "parm_add";
      }

      std::stringstream lineData( paramLine );

      if ( keyWrd != "parm_add")
        lineData >> tmp; //push the first u out

      // work on the line until nothing left to read
      while ( !lineData.eof() && !lineData.fail() )
      {

        double knot;

        lineData >> knot;

        if ( !lineData.fail() )
          knotsU.push_back( knot );
      }
    }

    // curve
    else if ( (mode == NONE && keyWrd == "curv") || (mode == CURVE && keyWrd == "curv_add") ){
      if (!firstFace)
        firstFace = true;

      inGroup = false;

      mode = CURVE;

      if ( keyWrd == "curv" ) {

          //int id = _importer.groupId(QString("spline_curve_%1").arg(curveCount));
          int id = _importer.getCurveGroupId(curveCount);
          if(id == -1) {
              std::cerr << "Error: Group has not been added before!" << std::endl;
              return;
          }
          _importer.setCurrentGroup(id);
          curveCount++;
      }

      //get curve control points
      std::string curveLine;

      std::getline(stream, curveLine);

      // value may contain a / as line separator
      if ( QString( curveLine.c_str() ).endsWith("\\")){
        curveLine = curveLine.substr(0, curveLine.length()-1);
        nextKeyWrd = "curv_add";
      }

      std::stringstream lineData( curveLine );

      // Read knots at the beginning before the indices
      if ( keyWrd == "curv" ) {
        double trash;
        lineData >> trash;
        lineData >> trash;
      }



      // work on the line until nothing left to read
      while ( !lineData.eof() && !lineData.fail() )
      {
        int index = 0;

        lineData >> index;

        if ( index < 0 ) {
          // Calculation of index :
          // -1 is the last vertex in the list
          // As obj counts from 1 and not zero add +1
          index = currentVertexCount + index + 1;
        }

        if ( !lineData.fail() )
          cpIndices.push_back( index -1 );
      }
    }

    // end
    else if (mode == CURVE && keyWrd == "end"){

      if ( _importer.isCurve( _importer.currentObject() ) ){
        // set up the spline curve
        _importer.currentCurve()->set_degree( _importer.degreeU() );
        _importer.currentCurve()->autocompute_knotvector(false);

        // add the control points
        std::vector< ACG::Vec3d > controlPolygon;

        for (unsigned int i = 0; i < cpIndices.size(); ++i)
          controlPolygon.push_back( (ACG::Vec3d) _importer.vertex( cpIndices[i] ) );

        _importer.currentCurve()->set_control_polygon( controlPolygon );

        _importer.currentCurve()->set_knots(knotsU);
      }

      cpIndices.clear();
      knotsU.clear();

      mode = NONE;
    }
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
    // param
    else if ( (mode == SURFACE && keyWrd == "parm") || (mode == SURFACE && keyWrd == "parm_add") ){

      //get surface knots
      std::string paramLine;
      std::string tmp;

      std::getline(stream, paramLine);

      // value may contain a / as line separator
      if ( QString( paramLine.c_str() ).endsWith("\\")){
        paramLine = paramLine.substr(0, paramLine.length()-1);
        nextKeyWrd = "parm_add";
      }

      std::stringstream lineData( paramLine );

      if ( keyWrd == "parm_add_u")
        tmp = "u";
      else if ( keyWrd == "parm_add_v")
        tmp = "v";
      else
        lineData >> tmp; //get the direction (u or v)

      std::vector< double >* knots;

      //Decide if these are knots in U or V direction
      if (tmp == "u")
        knots = &knotsU;
      else
        knots = &knotsV;

      if (nextKeyWrd != "")
        nextKeyWrd += "_" + tmp;

      // work on the line until nothing left to read
      while ( !lineData.eof() && !lineData.fail() )
      {

        double knot;

        lineData >> knot;

        if ( !lineData.fail() ) {
          knots->push_back( knot );
        }
      }
    }

    // surface
    else if ( (mode == NONE && keyWrd == "surf") || (mode == SURFACE && keyWrd == "surf_add") ){
      if (!firstFace)
        firstFace = true;

      inGroup = false;

      mode = SURFACE;

      if ( keyWrd == "surf" ) {

          //int id = _importer.groupId(QString("spline_surface_%1").arg(surfaceCount));
          int id = _importer.getSurfaceGroupId(surfaceCount);
          if(id == -1) {
              std::cerr << "Error: Group has not been added before!" << std::endl;
              return;
          }
          _importer.setCurrentGroup(id);
          surfaceCount++;
      }

      //get surface control points
      std::string surfLine;

      std::getline(stream, surfLine);

      // value may contain a / as line separator
      if ( QString( surfLine.c_str() ).endsWith("\\")){
        surfLine = surfLine.substr(0, surfLine.length()-1);
        nextKeyWrd = "surf_add";
      }

      std::stringstream lineData( surfLine );

      // work on the line until nothing left to read
      while ( !lineData.eof() && !lineData.fail() )
      {
        int index = 0;

        lineData >> index;

        if ( index < 0 ) {
          // Calculation of index :
          // -1 is the last vertex in the list
          // As obj counts from 1 and not zero add +1
          index = currentVertexCount + index + 1;
        }

        if ( !lineData.fail() )
          cpIndices.push_back( index -1 );
      }
    }

    // end
    else if (mode == SURFACE && keyWrd == "end"){

      if ( _importer.isSurface( _importer.currentObject() ) ){

        // remove first 4 entries since they are the first and last knot (for both direction)
        cpIndices.erase(cpIndices.begin());
        cpIndices.erase(cpIndices.begin());
        cpIndices.erase(cpIndices.begin());
        cpIndices.erase(cpIndices.begin());

        // set up the spline surface
        _importer.currentSurface()->set_degree( _importer.degreeU(), _importer.degreeV() );

        // compute number of control points in m and in n direction
        int dimU = knotsU.size() - _importer.degreeU() - 1;
        int dimV = knotsV.size() - _importer.degreeV() - 1;

        // add the control points
        std::vector< ACG::Vec3d > controlPolygon;

        for (int i = 0; i < dimU; ++i)
        {
          controlPolygon.clear();

          for (int j = 0; j < dimV; ++j){
            controlPolygon.push_back( (ACG::Vec3d) _importer.vertex( cpIndices[dimU * j + i] ) );
          }

          _importer.currentSurface()->add_vector_m(controlPolygon);
        }

        _importer.currentSurface()->set_knots_m(knotsU);
        _importer.currentSurface()->set_knots_n(knotsV);


      }

      cpIndices.clear();
      knotsU.clear();
      knotsV.clear();

      mode = NONE;
    }
#endif

  }


  //checks, if an object with a specified type was added. if not, point cloud was read
  bool isType = faceCount != 0;
#ifdef ENABLE_BSPLINECURVE_SUPPORT
  isType = isType || curveCount != 0;
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  isType = isType || surfaceCount != 0;
#endif

  // we have only read points so far and no faces or modes
  // treat them as a polymesh
  if (!isType && currentVertexCount != 0 ) {
    _importer.forceMeshType( OBJImporter::POLYMESH ); //actually it is a pointcloud
    if (!inGroup)
      _importer.setCurrentGroup(0);
  }
}

///check file types and read general info like vertices
void FileOBJPlugin::checkTypes(QString _filename, OBJImporter& _importer, QStringList& _includes)
{
  //setup filestream

  std::fstream input( _filename.toUtf8(), std::ios_base::in );

  if ( !input.is_open() || !input.good() ){
    emit log(LOGERR, tr("readOBJFile : cannot open file %1").arg(_filename) );
    return;
  }

  ReaderMode mode = NONE;

  std::string line;
  std::string keyWrd;
  std::string nextKeyWrd = "";

#ifdef ENABLE_BSPLINECURVE_SUPPORT
  unsigned int curveCount = 0;
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  unsigned int surfaceCount = 0;
#endif

  float x, y, z;
  int faceCount = 0;

  int PolyMeshCount = 0;
  int TriMeshCount = 0;

  OBJImporter::ObjectOptions options = OBJImporter::NONE;

  // keeps track if faces belong to a group or the default group
  bool inGroup = false;
  // keeps track if the first face of a mesh has been read yet or not
  bool firstFace = true;

#if defined ENABLE_BSPLINECURVE_SUPPORT || defined ENABLE_BSPLINESURFACE_SUPPORT
  QString currentGroupName;
  int parentId = -1;
#endif

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
    if ( line.empty() || line[0] == '#' || isspace(line[0]) ) {
      continue;
    }

    std::stringstream stream(line);

    //unless the keyWrd for the new line is not determined by the previous line
    //read it from stream
    if (nextKeyWrd == "")
      stream >> keyWrd;
    else {
      keyWrd = nextKeyWrd;
      nextKeyWrd = "";
    }

    //call - included obj files
    if (mode == NONE && keyWrd == "call"){
      firstFace = true;

      std::string include;
      std::getline(stream,include);

      //replace relative path
      QString includeStr = QString( include.c_str() ).trimmed();

      if ( !includeStr.isEmpty() ){

        if (includeStr[0] == '.'){
          includeStr = includeStr.right( includeStr.length()-1 );

          QFileInfo fi(_filename);

          includeStr = fi.path() + QDir::separator() + includeStr;
        }

        _includes.append( includeStr );
      }

      _importer.setObjectOptions(OBJImporter::NONE);
    }

    // vertex
    else if (mode == NONE && keyWrd == "v")
    {
      if (!firstFace)
        firstFace = true;

      stream >> x; stream >> y; stream >> z;

      if ( !stream.fail() )
        _importer.addVertex( OpenMesh::Vec3f(x,y,z) );
      else
        emit log(LOGERR, tr("Could not add Vertex %1. Possible NaN or Inf?").arg(_importer.n_vertices()));
    }

    // group
    else if (mode == NONE && keyWrd == "g"){
        if (!firstFace)
          firstFace = true;

        //give options to importer and reinitialize
        //for next object

        std::string grpName;
        std::getline(stream, grpName);

        if ( options & OBJImporter::TRIMESH  ) TriMeshCount++;
        if ( options & OBJImporter::POLYMESH ) PolyMeshCount++;

        int id = _importer.addGroup(grpName.c_str());
#if defined ENABLE_BSPLINECURVE_SUPPORT || defined ENABLE_BSPLINESURFACE_SUPPORT
        parentId = id;
        currentGroupName = grpName.c_str();
        currentGroupName.remove(".obj");
#endif
        _importer.setCurrentGroup(id);

        // all following elements are in this group until
        // a new group is created
        inGroup = true;

        _importer.setObjectOptions( options );
        options = OBJImporter::NONE;
        faceCount = 0;
    }

    // face
    else if (mode == NONE && keyWrd == "f"){

      if (firstFace) {
        // store faces in the default group if we aren't in a group already
        if (!inGroup)
          _importer.setCurrentGroup(0);
        firstFace = false;
      }

      faceCount++;

      int verticesPerFace = 0;
      int value;

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


        //get the vertex component (vertex/texCoord/normal)
        size_t found=vertex.find("/");

        // parts are seperated by '/' So if no '/' found its the last component
        if( found != std::string::npos ){

          // read the index value
          std::stringstream tmp( vertex.substr(0,found) );

          // Read current value
          tmp >> value;

          if ( tmp.fail() )
            emit log(LOGERR, tr("readOBJFile : Error reading vertex index!"));

        } else {

          // last component of the vertex, read it.
          std::stringstream tmp( vertex );
          tmp >> value;

          if ( tmp.fail() )
            emit log(LOGERR, tr("readOBJFile : Error reading vertex index!"));
        }


        if ( value < 0 ) {
          // Calculation of index :
          // -1 is the last vertex in the list
          // As obj counts from 1 and not zero add +1
          value = _importer.n_vertices() + value + 1;
        }

        // Obj counts from 1 and not zero .. array counts from zero therefore -1
        // the importer has to know which vertices are used by the object for correct vertex order
        _importer.useVertex( value -1 );
      }


      if( verticesPerFace > 3 ) {
        options = OBJImporter::POLYMESH;
        _importer.setObjectOptions(options);
      } else if ( verticesPerFace == 3 ) {
        options = OBJImporter::TRIMESH;
        _importer.setObjectOptions(options);
      }
    }

#ifdef ENABLE_BSPLINECURVE_SUPPORT

    // curve
    if ( (mode == NONE && keyWrd == "curv") || (mode == CURVE && keyWrd == "curv_add") ){
      if (!firstFace)
        firstFace = true;

      inGroup = false;

      mode = CURVE;

      if ( keyWrd == "curv" ) {

        //give options to importer and reinitialize
        //for next object
        if ( options & OBJImporter::TRIMESH  ) TriMeshCount++;
        if ( options & OBJImporter::POLYMESH ) PolyMeshCount++;

        QString name = currentGroupName;
        if (name.size() == 0)
          name = "DefaultGroup";

        name.append(QString("_curve_%1").arg(curveCount));
        int id = _importer.addGroup(name);

        if (_importer.numCurves() == 0) {
          if (currentGroupName.size() == 0)
            _importer.setGroupName(id, QString("DefaultGroup"));
          else
            _importer.setGroupName(id, currentGroupName);
        }	else {
          if (curveCount == 1) {
            int first = _importer.getCurveGroupId(0);
            QString tmp = _importer.groupName(first);
            tmp.append(QString("_curve_0"));
            _importer.setGroupName(first, tmp);
          }
          _importer.setGroupName(id, name);
        }
        _importer.setCurveParentId(id, parentId);
        _importer.setCurrentGroup(id);
        _importer.setCurveGroupId(curveCount, id);
        curveCount++;

        _importer.setObjectOptions( options );

        options = OBJImporter::CURVE;
      }

      //get curve control points
      std::string curveLine;

      std::getline(stream, curveLine);

      // value may contain a / as line separator
      if ( QString( curveLine.c_str() ).endsWith("\\")){
        curveLine = curveLine.substr(0, curveLine.length()-1);
        nextKeyWrd = "curv_add";
      }

      std::stringstream lineData( curveLine );

      // Read knots at the beginning before the indices
      if ( keyWrd == "curv" ) {
        double trash;
        lineData >> trash;
        lineData >> trash;
      }

      // work on the line until nothing left to read
      while ( !lineData.eof() && !lineData.fail() )
      {
        int index = 0;

        lineData >> index;

        if ( !lineData.fail() ){
          // the importer has to know which vertices are used by the object for correct vertex order
          _importer.useVertex( index -1 );
        }
      }

    }

    // end
    else if (mode == CURVE && keyWrd == "end"){

      mode = NONE;

      _importer.setObjectOptions( options );
      options = OBJImporter::TRIMESH;
      faceCount = 0;
    }
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT

    // surface
    if ( (mode == NONE && keyWrd == "surf") || (mode == SURFACE && keyWrd == "surf_add") ){
      if (!firstFace)
        firstFace = true;

      inGroup = false;

      mode = SURFACE;

      if ( keyWrd == "surf" ){

        //give options to importer and reinitialize
        //for next object
        if ( options & OBJImporter::TRIMESH  ) TriMeshCount++;
        if ( options & OBJImporter::POLYMESH ) PolyMeshCount++;

        QString name = currentGroupName;
        if (name.size() == 0)
          name = "DefaultGroup";

        name.append(QString("_surface_%1").arg(surfaceCount));
        int id = _importer.addGroup(name);

        if (_importer.numSurfaces() == 0) {
          if (currentGroupName.size() == 0)
            _importer.setGroupName(id, "DefaultGroup");
          else
            _importer.setGroupName(id, currentGroupName);
        }	else {
          if (surfaceCount == 1) {
            int first = _importer.getSurfaceGroupId(0);
            QString tmp = _importer.groupName(first);
            tmp.append(QString("_surface_0"));
            _importer.setGroupName(first, tmp);
          }
          _importer.setGroupName(id, name);
        }
        _importer.setSurfaceParentId(id, parentId);
        _importer.setCurrentGroup(id);
        _importer.setSurfaceGroupId(surfaceCount, id);
        surfaceCount++;

        _importer.setObjectOptions( options );

        options = OBJImporter::SURFACE;
      }

      //get surface control points
      std::string surfLine;

      std::getline(stream, surfLine);

      // value may contain a / as line separator
      if ( QString( surfLine.c_str() ).endsWith("\\")){
        surfLine = surfLine.substr(0, surfLine.length()-1);
        nextKeyWrd = "surf_add";
      }

      std::stringstream lineData( surfLine );

      // work on the line until nothing left to read
      while ( !lineData.eof() && !lineData.fail() )
      {
        int index = 0;

        lineData >> index;

        if ( !lineData.fail() ){
          // the importer has to know which vertices are used by the object for correct vertex order
          _importer.useVertex( index -1 );
        }
      }
    }

    // end
    else if (mode == SURFACE && keyWrd == "end"){

      mode = NONE;

      _importer.setObjectOptions( options );
      options = OBJImporter::TRIMESH;
      faceCount = 0;
    }
#endif

  }

  if(faceCount > 0) {
    if ( options & OBJImporter::TRIMESH  ) TriMeshCount++;
    if ( options & OBJImporter::POLYMESH ) PolyMeshCount++;
    _importer.setObjectOptions( options );
  } else {
    // Mesh does not contain any faces
    PolyMeshCount++;
    if (keyWrd != "call") {
      // we only have vertices and no faces
      if (keyWrd == "v" && !inGroup) {
        _importer.setCurrentGroup(0);
        // treat the file as a polymesh
        forceTriangleMesh_ = false;
        forcePolyMesh_ = true;
        _importer.setObjectOptions(OBJImporter::POLYMESH);
        _importer.forceMeshType( OBJImporter::POLYMESH );
        for (unsigned int i = 0; i < _importer.n_vertices(); ++i)
          _importer.useVertex(i);
      } else {
        unsigned int currentOptions = _importer.objectOptions()[_importer.currentObject()];
        // this is only a triangle mesh if the object is not a curve and not a surface
        // also ignore if it is set to NONE
        if (!(currentOptions & OBJImporter::CURVE) &&
            !(currentOptions & OBJImporter::SURFACE) &&
            (currentOptions != OBJImporter::NONE))
          _importer.setObjectOptions(OBJImporter::TRIMESH);
      }
    }
  }

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

  // If we do not have a gui, we will always use the last default
  // If we need a gui and the triMeshHandling box is not generated (==0) we also use the last default
  if ( OpenFlipper::Options::gui() && triMeshHandling_ != 0 ){

    QMessageBox msgBox;
    QPushButton *detectButton = msgBox.addButton(tr("Auto-Detect"), QMessageBox::ActionRole);
    QPushButton *triButton    = msgBox.addButton(tr("Open as triangle mesh"), QMessageBox::ActionRole);
    QPushButton *polyButton   = msgBox.addButton(tr("Open as poly mesh"), QMessageBox::ActionRole);

    switch( triMeshHandling_->currentIndex() ){
      case TYPEAUTODETECT : //Detect
        break;

      case TYPEASK: //ask
        msgBox.setWindowTitle( tr("Mesh types in file") );
        msgBox.setText( tr("You are about to open a file containing one or more mesh types. \n\n Which mesh type should be used?") );
        msgBox.setDefaultButton( detectButton );
        msgBox.exec();

        if (msgBox.clickedButton() == triButton)
          _importer.forceMeshType( OBJImporter::TRIMESH );
        else if (msgBox.clickedButton() == polyButton)
          _importer.forceMeshType( OBJImporter::POLYMESH );

        break;

      case TYPEPOLY       : //polyMesh
        _importer.forceMeshType( OBJImporter::POLYMESH ); break;

      case TYPETRIANGLE   : //trimesh
        _importer.forceMeshType( OBJImporter::TRIMESH ); break;

      default: break;

    }

  }

}

//-----------------------------------------------------------------------------------------------------

int FileOBJPlugin::loadObject(QString _filename) {

  OBJImporter importer;

  //included filenames
  QStringList includes;

  //preprocess file and store types in ObjectOptions
  checkTypes( _filename, importer, includes );

  IdList objIDs;

  //load included obj files
  for (int i=0; i < includes.size(); i++){

    //int id = loadObject( includes[i], importer );
    int id = loadObject( includes[i] );

    if (id != -1)
      objIDs.push_back( id );
  }

  //add a group if we have includes
  if ( ! includes.empty() )
    importer.addGroup( QFileInfo(_filename).fileName() );

  //check if something was found
  if ( importer.objectOptions().empty() && objIDs.empty() ){

    forceTriangleMesh_ = false;
    forcePolyMesh_     = false;

    return -1;
  }

  //then parse the obj
  readOBJFile( _filename, importer );

  // finish up
  importer.finish();

  int returnID = -1;

  //perhaps add group
  if ( importer.numGroups() > 1){

    bool dataControlExists = false;
    pluginExists( "datacontrol", dataControlExists );

    if ( dataControlExists ){

      std::vector<OBJImporter::ObjectOptions> options = importer.objectOptions();
#if defined ENABLE_BSPLINECURVE_SUPPORT || defined ENABLE_BSPLINESURFACE_SUPPORT
      std::map<int, QString> groupNames;
#endif

#ifdef ENABLE_BSPLINECURVE_SUPPORT
      std::vector< std::vector<int> > curveGroups;
      std::vector<int> curveIds;
      int lastCurveParent = -2;
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
      std::vector< std::vector<int> > surfaceGroups;
      std::vector<int> surfaceIds;
      int lastSurfaceParent = -2;
#endif
      for(unsigned int i = 0; i < importer.objectCount(); i++) {
        // skip the object if it has no option
        // this can happen if the object only included other objects
        if (options[i] != NONE) {
          BaseObject* obj = importer.object(i);
          if(obj) {
#ifdef ENABLE_BSPLINECURVE_SUPPORT
            if (options[i] & OBJImporter::CURVE) {
              // store the parent group name for later grouping
              groupNames[obj->id()] = importer.groupName(importer.getCurveParentId(i));

              // first curve group
              if (lastCurveParent == -2) {
                lastCurveParent = importer.getCurveParentId(i);
                curveIds.push_back(obj->id());
                BaseObject* parent = importer.object(lastCurveParent);
                if (parent) {
                  curveIds.push_back(parent->id());
                  // don't group the parent in the objIDs group
                  std::vector<int>::iterator pos = std::find(objIDs.begin(), objIDs.end(), parent->id());
                  if (pos != objIDs.end())
                    objIDs.erase(pos);
                }
              // new curve group
              } else if (lastCurveParent != importer.getCurveParentId(i)) {
                lastCurveParent = importer.getCurveParentId(i);
                curveGroups.push_back(curveIds);
                curveIds.clear();
                curveIds.push_back(obj->id());
                BaseObject* parent = importer.object(lastCurveParent);
                if (parent) {
                  curveIds.push_back(parent->id());
                  // don't group the parent in the objIDs group
                  std::vector<int>::iterator pos = std::find(objIDs.begin(), objIDs.end(), parent->id());
                  if (pos != objIDs.end())
                    objIDs.erase(pos);
                }
              // add curves to group
              } else
                curveIds.push_back(obj->id());
            }

#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT
            if (options[i] & OBJImporter::SURFACE) {
              // store the parent group name for later grouping
              groupNames[obj->id()] = importer.groupName(importer.getSurfaceParentId(i));

              // first surface group
              if (lastSurfaceParent == -2) {
                lastSurfaceParent = importer.getSurfaceParentId(i);
                surfaceIds.push_back(obj->id());
                BaseObject* parent = importer.object(lastSurfaceParent);
                if (parent) {
                  surfaceIds.push_back(parent->id());
                  std::vector<int>::iterator pos = std::find(objIDs.begin(), objIDs.end(), parent->id());
                  if (pos != objIDs.end())
                    objIDs.erase(pos);
                }
              // new surface group
              } else if (lastSurfaceParent != importer.getSurfaceParentId(i)) {
                lastSurfaceParent = importer.getSurfaceParentId(i);
                surfaceGroups.push_back(surfaceIds);
                surfaceIds.clear();
                surfaceIds.push_back(obj->id());
                BaseObject* parent = importer.object(lastSurfaceParent);
                if (parent) {
                  surfaceIds.push_back(parent->id());
                  std::vector<int>::iterator pos = std::find(objIDs.begin(), objIDs.end(), parent->id());
                  if (pos != objIDs.end())
                    objIDs.erase(pos);
                }
              // add surfaces to group
              } else
                surfaceIds.push_back(obj->id());

            }
#endif
            if ( (options[i] & OBJImporter::TRIMESH) || (options[i] & OBJImporter::POLYMESH) )
              objIDs.push_back( obj->id() );
          } else {
              std::cerr << "Object is NULL!" << std::endl;
          }
        }
      }

#ifdef ENABLE_BSPLINECURVE_SUPPORT
      // add last group
      curveGroups.push_back(curveIds);
      std::vector< std::vector<int> >::iterator it = curveGroups.begin();
      for (; it != curveGroups.end(); ++it) {
        // only group if we have more than one curve
        if (it->size() > 2) {
          if (groupNames[it->back()].size() == 0)
            RPC::callFunctionValue<int>("datacontrol","groupObjects", *it, QFileInfo(_filename).fileName());
          else
            RPC::callFunctionValue<int>("datacontrol","groupObjects", *it, groupNames[it->back()]);
        }
      }
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
      // add last group
      surfaceGroups.push_back(surfaceIds);
      std::vector< std::vector<int> >::iterator it2 = surfaceGroups.begin();
      for (; it2 != surfaceGroups.end(); ++it2) {
        // only group if we have more than one surface
        if (it2->size() > 2) {
          if (groupNames[it2->back()].size() == 0)
            RPC::callFunctionValue<int>("datacontrol","groupObjects", *it2, QFileInfo(_filename).fileName());
          else
            RPC::callFunctionValue<int>("datacontrol","groupObjects", *it2, groupNames[it2->back()]);
        }
      }
#endif

      // only group if we have more than one object
      if (objIDs.size() > 1)
        returnID = RPC::callFunctionValue<int>("datacontrol","groupObjects", objIDs, importer.groupName(0));
    }
  }

  //check all new objects
  for(unsigned int i=0; i < importer.objectCount(); i++){

    BaseObject* object = importer.object(i);
    if(object == NULL) continue;

    object->setFromFileName(_filename);

    //remember the id of the first opened object
    if ( returnID == -1)
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

#ifdef ENABLE_BSPLINECURVE_SUPPORT
    //handle new BSplineCurves
    BSplineCurveObject* bscObj = dynamic_cast< BSplineCurveObject* > (object);

    if ( bscObj ){
      bscObj->splineCurveNode()->updateGeometry();
    }
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
    //handle new BSplineCurves
    BSplineSurfaceObject* bssObj = dynamic_cast< BSplineSurfaceObject* > (object);

#endif

    //textures
    if ( importer.hasTexture(i) && !importer.hasOption( i, OBJImporter::FORCE_NOTEXTURES ) ){

      //add the textures to the object
      addTextures( importer, i );

      //set the texture index property to be used
      emit setTextureMode("OBJ Data","indexProperty=OriginalTexIndexMapping", object->id() );

      emit switchTexture("OBJ Data", object->id() );

      PluginFunctions::setDrawMode( ACG::SceneGraph::DrawModes::SOLID_2DTEXTURED_FACE_SHADED, PluginFunctions::ALL_VIEWERS );

    }

    //general stuff
    emit openedFile( object->id() );

    PluginFunctions::viewAll();
  }

  forceTriangleMesh_ = false;
  forcePolyMesh_     = false;

//   if ( topLevelObj )
//     OpenFlipper::Options::loadingSettings(false);

  return returnID;
}

//-----------------------------------------------------------------------------------------------------

/// load a obj and force mesh datatype
int FileOBJPlugin::loadObject(QString _filename, DataType _type){

  if ( _type == DATA_TRIANGLE_MESH )
    forceTriangleMesh_ = true;
  else if ( _type == DATA_POLY_MESH )
    forcePolyMesh_ = true;

  return loadObject(_filename);
}

//-----------------------------------------------------------------------------------------------------

bool FileOBJPlugin::saveObject(int _id, QString _filename)
{
  BaseObjectData* object;
  if ( !PluginFunctions::getObject(_id,object) ) {
    emit log(LOGERR, tr("saveObject : cannot get object id %1 for save name %2").arg(_id).arg(_filename) );
    return false;
  }

  //open output stream
  std::string filename = std::string( _filename.toUtf8() );

  std::fstream objStream( filename.c_str(), std::ios_base::out );

  if ( !objStream ){

    emit log(LOGERR, tr("saveObject : cannot not open file %1").arg(_filename) );
    return false;
  }

  //write object
  if ( object->dataType( DATA_POLY_MESH ) ) {

    object->setFromFileName(_filename);
    object->setName(object->filename());

    PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );


    if ( writeMesh( objStream, _filename, *polyObj->mesh(), polyObj->id() ) ){

      emit log(LOGINFO, tr("Saved object to ") + _filename );
      objStream.close();
      return true;

    } else {

      emit log(LOGERR, tr("Unable to save ") + _filename);
      objStream.close();
      return false;
    }

  } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    object->setFromFileName(_filename);
    object->setName(object->filename());

    TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );


    if ( writeMesh( objStream, _filename, *triObj->mesh(), triObj->id() )) {

      emit log(LOGINFO, tr("Saved object to ") + _filename );
      objStream.close();
      return true;

    } else {

      emit log(LOGERR, tr("Unable to save ") + _filename );
      objStream.close();
      return false;
    }

#ifdef ENABLE_BSPLINECURVE_SUPPORT
  } else if ( object->dataType( DATA_BSPLINE_CURVE ) ) {

    object->setFromFileName(_filename);
    object->setName(object->filename());

    BSplineCurveObject* bscObj = dynamic_cast<BSplineCurveObject* >( object );


    if ( writeCurve( objStream, _filename, bscObj->splineCurve()) ) {

      emit log(LOGINFO, tr("Saved object to ") + _filename );
      objStream.close();
      return true;

    } else {

      emit log(LOGERR, tr("Unable to save ") + _filename );
      objStream.close();
      return false;
    }
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  } else if ( object->dataType( DATA_BSPLINE_SURFACE ) ) {

    object->setFromFileName(_filename);
    object->setName(object->filename());

    BSplineSurfaceObject* bssObj = dynamic_cast<BSplineSurfaceObject* >( object );


    if ( writeSurface( objStream, _filename, bssObj->splineSurface()) ) {

      emit log(LOGINFO, tr("Saved object to ") + _filename );
      objStream.close();
      return true;

    } else {

      emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
      objStream.close();
      return false;
    }
#endif

  } else {

    emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
    objStream.close();
    return false;
  }
}

//-----------------------------------------------------------------------------------------------------

void FileOBJPlugin::slotHandleCheckBoxes(bool _checked) {

    if(saveCopyTextures_) {
        saveCreateTexFolder_->setEnabled(_checked);
        saveCreateTexFolder_->setChecked(_checked);
    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOBJPlugin::saveOptionsWidget(QString /*_currentFilter*/) {

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

        saveTextures_ = new QCheckBox("Save Textures");
        layout->addWidget(saveTextures_);

        saveCopyTextures_ = new QCheckBox("Copy Texture Files");
        layout->addWidget(saveCopyTextures_);

        saveCreateTexFolder_ = new QCheckBox("Create Textures Folder");
        layout->addWidget(saveCreateTexFolder_);

        savePrecisionLabel_ = new QLabel("Writer Precision");
        layout->addWidget(savePrecisionLabel_);

        savePrecision_ = new QSpinBox();
        savePrecision_->setMinimum(1);
        savePrecision_->setMaximum(12);
        savePrecision_->setValue(6);
        layout->addWidget(savePrecision_);

        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);

        saveOptions_->setLayout(layout);

        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));
        connect(saveCopyTextures_, SIGNAL(toggled(bool)), this, SLOT(slotHandleCheckBoxes(bool)));

        saveFaceColor_->setChecked( OpenFlipperSettings().value("FileObj/Save/FaceColor",true).toBool() );
        saveAlpha_->setChecked( OpenFlipperSettings().value("FileObj/Save/Alpha",true).toBool() );
        saveNormals_->setChecked( OpenFlipperSettings().value("FileObj/Save/Normals",true).toBool() );
        saveTexCoords_->setChecked( OpenFlipperSettings().value("FileObj/Save/TexCoords",true).toBool() );
        saveTextures_->setChecked( OpenFlipperSettings().value("FileObj/Save/Textures",true).toBool() );
        saveCopyTextures_->setChecked( OpenFlipperSettings().value("FileObj/Save/CopyTextures",true).toBool() );
        saveCreateTexFolder_->setChecked( OpenFlipperSettings().value("FileObj/Save/CreateTexFolder",true).toBool() );

        slotHandleCheckBoxes(saveCopyTextures_->isChecked());
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
        triMeshHandling_->addItem( tr("Detect correct type") );
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Open as PolyMesh") );
        triMeshHandling_->addItem( tr("Open as TriangleMesh") );

        layout->addWidget(triMeshHandling_);

        loadFaceColor_ = new QCheckBox("Load Face Colors");
        layout->addWidget(loadFaceColor_);

        loadNormals_ = new QCheckBox("Load Normals");
        layout->addWidget(loadNormals_);

        loadTexCoords_ = new QCheckBox("Load Texture Coordinates");
        layout->addWidget(loadTexCoords_);

        loadTextures_ = new QCheckBox("Load Textures");
        layout->addWidget(loadTextures_);

        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);

        loadOptions_->setLayout(layout);

        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));


        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FileObj/Load/TriMeshHandling",TYPEAUTODETECT).toInt() );

        loadFaceColor_->setChecked( OpenFlipperSettings().value("FileObj/Load/FaceColor",true).toBool()  );
        loadNormals_->setChecked( OpenFlipperSettings().value("FileObj/Load/Normals",true).toBool()  );
        loadTexCoords_->setChecked( OpenFlipperSettings().value("FileObj/Load/TexCoords",true).toBool()  );
        loadTextures_->setChecked( OpenFlipperSettings().value("FileObj/Load/Textures",true).toBool()  );
    }

    return loadOptions_;
}

void FileOBJPlugin::slotLoadDefault() {
  OpenFlipperSettings().setValue( "FileObj/Load/FaceColor",   loadFaceColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Load/Normals",     loadNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Load/TexCoords",   loadTexCoords_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Load/Textures",    loadTextures_->isChecked()  );
  OpenFlipperSettings().setValue("FileObj/Load/TriMeshHandling", triMeshHandling_->currentIndex() );

  OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}


void FileOBJPlugin::slotSaveDefault() {
  OpenFlipperSettings().setValue( "FileObj/Save/FaceColor",         saveFaceColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Save/Normals",           saveNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Save/TexCoords",         saveTexCoords_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Save/Textures",          saveTextures_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Save/CopyTextures",      saveCopyTextures_->isChecked()  );
  OpenFlipperSettings().setValue( "FileObj/Save/CreateTexFolder",   saveCreateTexFolder_->isChecked()  );

}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( fileobjplugin , FileOBJPlugin );
#endif


