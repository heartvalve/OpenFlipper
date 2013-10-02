#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include "FilePolyLine.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

#include <ObjectTypes/PolyLine/PolyLineCircleData.hh>
#include <ObjectTypes/PolyLine/PolyLineBezierSplineData.hh>

#include <OpenFlipper/BasePlugin/RPCInterface.hh>


//-----------------------------------------------------------------------------

void FilePolyLinePlugin::loadPolyLine(PolyLineObject* lineObject,  const char* _filename)
{
  PolyLine& _polyLine = *lineObject->line();

  // clear old polyline
  _polyLine.clear();

  std::ifstream fin(_filename, std::ios::in);

  // closed ?
  bool closed = false;
  fin >> closed;

  _polyLine.set_closed(closed);

  // number of points
  int num_points;
  fin >> num_points;

  // read points
  for(int i=0; i<num_points; ++i)
  {
    typename PolyLine::Point::value_type x,y,z;
    fin >> x;
    fin >> y;
    fin >> z;
    PolyLine::Point p(x,y,z);
    _polyLine.add_point(p);
  }

  // ###########################
  // READ properties

  std::string token;
  while(true)
  {
    token = "-";
    fin >> token;
    if(token == "VVHANDLES")
    {
      if(!_polyLine.vertex_vhandles_available()) _polyLine.request_vertex_vhandles();
      for(unsigned int i=0; i<_polyLine.n_vertices(); ++i)
        fin >> _polyLine.vertex_vhandle(i);
    }
    else if(token == "VEHANDLES")
    {
      if(!_polyLine.vertex_ehandles_available()) _polyLine.request_vertex_ehandles();
      for(unsigned int i=0; i<_polyLine.n_vertices(); ++i)
        fin >> _polyLine.vertex_ehandle(i);
    }
    else if(token == "VFHANDLES")
    {
      if(!_polyLine.vertex_fhandles_available()) _polyLine.request_vertex_fhandles();
      for(unsigned int i=0; i<_polyLine.n_vertices(); ++i)
        fin >> _polyLine.vertex_fhandle(i);
    }
    else if(token == "VNORMALS")
    {
      if(!_polyLine.vertex_normals_available()) _polyLine.request_vertex_normals();
      for(unsigned int i=0; i<_polyLine.n_vertices(); ++i)
      {
        fin >> _polyLine.vertex_normal(i)[0];
        fin >> _polyLine.vertex_normal(i)[1];
        fin >> _polyLine.vertex_normal(i)[2];
      }
    }
    else if(token == "VBINORMALS")
    {
      if(!_polyLine.vertex_binormals_available()) _polyLine.request_vertex_binormals();
      for(unsigned int i=0; i<_polyLine.n_vertices(); ++i)
      {
        fin >> _polyLine.vertex_binormal(i)[0];
        fin >> _polyLine.vertex_binormal(i)[1];
        fin >> _polyLine.vertex_binormal(i)[2];
      }
    }
    else if(token == "CIRCLEDATA") {
        ACG::Vec3d center, normal, main, side;
        double rmain, rside;
        fin >> center;
        fin >> main;
        fin >> rmain;
        fin >> normal;
        fin >> side;
        fin >> rside;
        PolyLineCircleData* circleData = new PolyLineCircleData(center,normal,main,side,rmain,rside,std::numeric_limits<unsigned int>::max());
        lineObject->setObjectData(CIRCLE_DATA, circleData);
    }
    else if(token == "SPLINEDATA") {
        PolyLineBezierSplineData* splineData = new PolyLineBezierSplineData(std::numeric_limits<unsigned int>::max());
        unsigned int pointCount, handleCount;
        fin >> pointCount;
        PolyLineBezierSplineData::InterpolatePoint point;
        for(unsigned int i = 0; i < pointCount; i++) {
            fin >> point.normal;
            fin >> point.position;
            splineData->points_.push_back(point);
        }
        fin >> handleCount;
        for(unsigned int i = 0; i < handleCount; i++) {
            fin >> point.position;
            splineData->handles_.push_back(point.position);
        }
        lineObject->setObjectData(BEZSPLINE_DATA, splineData);
    }
    else if(token != "") break; //eat up empty lines
  }
  fin.close();
}

void FilePolyLinePlugin::savePolyLine(PolyLineObject* lineObject,  const char* _filename)
{
  PolyLine& _polyLine = *lineObject->line();

  std::ofstream fout(_filename, std::ios::out);

  //  is polyline closed?
  fout << _polyLine.is_closed() << std::endl;

  // number of points
  fout << _polyLine.n_vertices() << std::endl;

  std::cerr << "write " << _filename << std::endl;
  std::cerr << "#points: " << _polyLine.n_vertices() << std::endl;


  // write each point
  for(unsigned int i=0; i< _polyLine.n_vertices(); ++i)
  {

    const PolyLine::Point& current_point = _polyLine.point(i);
    fout << current_point[0] << " ";
    fout << current_point[1] << " ";
    fout << current_point[2] << std::endl;
  }

  // ###########################
  // Write properties(unsigned int)CircleType

  if(_polyLine.vertex_vhandles_available())
  {
    fout << "VVHANDLES" << std::endl;
    for( unsigned int i=0; i<_polyLine.n_vertices(); ++i)
      fout << _polyLine.vertex_vhandle(i) << std::endl;
  }

  if(_polyLine.vertex_ehandles_available())
  {
    fout << "VEHANDLES" << std::endl;
    for( unsigned int i=0; i<_polyLine.n_vertices(); ++i)
      fout << _polyLine.vertex_ehandle(i) << std::endl;
  }

  if(_polyLine.vertex_fhandles_available())
  {
    fout << "VFHANDLES" << std::endl;
    for( unsigned int i=0; i<_polyLine.n_vertices(); ++i)
      fout << _polyLine.vertex_fhandle(i) << std::endl;
  }

  if(_polyLine.vertex_normals_available())
  {
    fout << "VNORMALS" << std::endl;
    for( unsigned int i=0; i<_polyLine.n_vertices(); ++i)
    {
      const PolyLine::Point& current_vnormal = _polyLine.vertex_normal(i);
      fout << current_vnormal[0] << " ";
      fout << current_vnormal[1] << " ";
      fout << current_vnormal[2] << std::endl;
    }
  }

  if(_polyLine.vertex_binormals_available())
  {
    fout << "VBINORMALS" << std::endl;
    for( unsigned int i=0; i<_polyLine.n_vertices(); ++i)
    {
      const PolyLine::Point& current_bnormal = _polyLine.vertex_binormal(i);
      fout << current_bnormal[0] << " ";
      fout << current_bnormal[1] << " ";
      fout << current_bnormal[2] << std::endl;
    }
  }

  PolyLineCircleData* circleData = dynamic_cast<PolyLineCircleData*>(lineObject->objectData(CIRCLE_DATA));
  if(circleData) {
      fout << "CIRCLEDATA" << std::endl;
      fout << circleData->circleCenter_ << std::endl;
      fout << circleData->circleMainAxis_ << std::endl;
      fout << circleData->circleMainRadius_ << std::endl;
      fout << circleData->circleNormal_ << std::endl;
      fout << circleData->circleSideAxis_ << std::endl;
      fout << circleData->circleSideRadius_ << std::endl;
  }

  PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(lineObject->objectData(BEZSPLINE_DATA));
  if(splineData) {
        fout << "SPLINEDATA" << std::endl;
        fout << (unsigned int)splineData->points_.size() << std::endl;
        for(unsigned int i = 0; i < splineData->points_.size(); i++) {
            fout << splineData->points_[i].normal << std::endl;
            fout << splineData->points_[i].position << std::endl;
        }
        fout << (unsigned int)splineData->handles_.size() << std::endl;
        for(unsigned int i = 0; i < splineData->handles_.size(); i++)
            fout << splineData->handles_[i] << std::endl;
        fout << std::endl;
    }

  fout.close();
}

void FilePolyLinePlugin::initializePlugin() {
  // Data type has already been added by polyline type plugin
}

QString FilePolyLinePlugin::getLoadFilters() {
  return QString( tr("Poly-Line files ( *.pol )" ));
};

QString FilePolyLinePlugin::getSaveFilters() {
  return QString( tr("Poly-Line files ( *.pol )" ));
};

DataType  FilePolyLinePlugin::supportedType() {
  DataType type = DATA_POLY_LINE;
  return type;
}

int FilePolyLinePlugin::loadObject(QString _filename)
{
  int id = -1;
  emit addEmptyObject( DATA_POLY_LINE, id );

  PolyLineObject* pol(0);
  if(PluginFunctions::getObject( id, pol))
  {
    loadPolyLine(pol, _filename.toLatin1());

    pol->setFromFileName(_filename);

    pol->setName(QFileInfo(_filename).fileName());
  }

  emit openedFile( pol->id() );

  return id;
};

bool FilePolyLinePlugin::saveObject(int _id, QString _filename)
{
  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( _id, obj))
  {
    PolyLineObject* pol = PluginFunctions::polyLineObject(obj);
    if( pol)
    {
      std::cerr << "call Line Function\n";
      std::cerr << (const char*) _filename.toLatin1() << std::endl;

      obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
      obj->setPath(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

      savePolyLine(pol, _filename.toLatin1());
    }
  }

  return true;
}

void FilePolyLinePlugin::loadIniFileLast( INIFile& _ini ,int _id ) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,tr("Cannot find object for id %1 in saveFile!").arg(_id));
    return;
  }

  PolyLineObject* object = PluginFunctions::polyLineObject(baseObject);

  if ( object ) {
     ACG::Vec4f col(0.0,0.0,0.0,0.0);

    if ( _ini.get_entryVecf( col, object->name() , "BaseColor" ) )
      object->materialNode()->set_base_color(col);
  }

}

void FilePolyLinePlugin::saveIniFile( INIFile& _ini ,int _id) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,tr("Cannot find object for id %1 in saveFile!").arg(_id) );
    return;
  }

  PolyLineObject* object = PluginFunctions::polyLineObject(baseObject);

  if ( object ) {
      _ini.add_entryVec( object->name() ,
                         "BaseColor" ,
                         object->materialNode()->base_color() ) ;
  }
}
#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( filepolylineplugin , FilePolyLinePlugin );
#endif


