#include "PolyLineObjectSerializer.hh"

#include <limits>

#include <ObjectTypes/PolyLine/PolyLineCircleData.hh>
#include <ObjectTypes/PolyLine/PolyLineBezierSplineData.hh>

PolyLineObjectSerializer::PolyLineObjectSerializer(PolyLineObject* _object)
    : instance(_object)
{
}

void PolyLineObjectSerializer::serialize(std::ostream& _stream) {
    std::ostream& fout = _stream;
    PolyLine& _polyLine = *instance->line();
    //  is polyline closed?
    fout << _polyLine.is_closed() << std::endl;

    // number of points
    fout << _polyLine.n_vertices() << std::endl;

    //std::cerr << "write " << _filename << std::endl;
    std::cerr << "#points: " << _polyLine.n_vertices() << std::endl;


    // write each point
    for(unsigned int i=0; i< _polyLine.n_vertices(); ++i)
    {

    const PolyLine::Point& current_point = _polyLine.point(i);
    fout << current_point[0] << " ";
    fout << current_point[1] << " ";
    fout << current_point[2] << std::endl;
    }

    fout << "VERTEXRADIUS" << std::endl << _polyLine.vertex_radius() << std::endl;
    fout << "EDGERADIUS" << std::endl << _polyLine.edge_radius() << std::endl;


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

    if(_polyLine.vertex_scalars_available())
    {
      fout << "VSCALARS" << std::endl;
      for( unsigned int i=0; i<_polyLine.n_vertices(); ++i)
        fout << _polyLine.vertex_scalar(i) << std::endl;
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

    PolyLineCircleData* circleData = dynamic_cast<PolyLineCircleData*>(instance->objectData(CIRCLE_DATA));
    if(circleData) {
      fout << "CIRCLEDATA" << std::endl;
      fout << circleData->circleCenter_ << std::endl;
      fout << circleData->circleMainAxis_ << std::endl;
      fout << circleData->circleMainRadius_ << std::endl;
      fout << circleData->circleNormal_ << std::endl;
      fout << circleData->circleSideAxis_ << std::endl;
      fout << circleData->circleSideRadius_ << std::endl;
    }

    PolyLineBezierSplineData* splineData = dynamic_cast<PolyLineBezierSplineData*>(instance->objectData(BEZSPLINE_DATA));
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
}

void PolyLineObjectSerializer::deserialize(std::istream& _stream) {
    std::istream& fin = _stream;
    PolyLine& _polyLine = *instance->line();

    // clear old polyline
    _polyLine.clear();
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
      PolyLine::Point::value_type x,y,z;
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
      if(token == "VERTEXRADIUS")
      {
        double r;
        fin >> r;
        _polyLine.set_vertex_radius(r);
      }
      else if(token == "EDGERADIUS")
      {
        double r;
        fin >> r;
        _polyLine.set_edge_radius(r);
      }
      else if(token == "VVHANDLES")
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
      else if(token == "VSCALARS")
      {
        if(!_polyLine.vertex_scalars_available()) _polyLine.request_vertex_scalars();
        for(unsigned int i=0; i<_polyLine.n_vertices(); ++i)
          fin >> _polyLine.vertex_scalar(i);
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
          instance->setObjectData(CIRCLE_DATA, circleData);
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
          instance->setObjectData(BEZSPLINE_DATA, splineData);
      }
      else if(token != "") break; //eat up empty lines
    }

}





