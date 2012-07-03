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

#define PROPERTYVISPLUGIN_CC

#include "PropertyVisPlugin.hh"
#include <iostream>
#include <typeinfo>
#include <limits>
#include <math.h>
#include <time.h>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ACG/Utils/ColorCoder.hh>

//------------------------------------------------------------------------------


template<class MeshT>
void PropertyVisPlugin::visualizeVector(
        MeshT* _mesh, const PropertyNameListModel::PROP_INFO &currentProp) {

    if (tool_->vectors_strokes_rb->isChecked()) {
        visualizeVector_asStroke(_mesh, currentProp);
    } else if (tool_->vectors_colors_rb->isChecked()) {
        visualizeVector_asColor(_mesh, currentProp);
    } else {
        throw VizException("Unknown vector viz mode selected.");
    }
}

namespace {

template<typename PROPTYPE, typename MeshT, typename ENTITY_IT, typename PROPINFO_TYPE>
void visualizeVector_asColorForEntity(MeshT *mesh, const ENTITY_IT e_begin, const ENTITY_IT e_end,
                                      const PROPINFO_TYPE &propinfo) {
    PROPTYPE prop;
    if (!mesh->get_property_handle(prop, propinfo.propName()))
        throw VizException("Getting PropHandle from mesh for selected property failed.");
    for (ENTITY_IT e_it = e_begin; e_it != e_end; ++e_it) {

        typename MeshT::Point v = mesh->property(prop, e_it).normalized() * .5 + typename MeshT::Point(.5, .5, .5);
        mesh->set_color(*e_it, typename MeshT::Color(v[0], v[1], v[2], 1.0));
    }
}

} /* anonymous namespace */

template<class MeshT>
void PropertyVisPlugin::visualizeVector_asColor(
        MeshT* _mesh, const PropertyNameListModel::PROP_INFO &currentProp) {

    if (currentProp.isFaceProp()) {
        visualizeVector_asColorForEntity<OpenMesh::FPropHandleT<typename MeshT::Point> >(
                _mesh, _mesh->faces_begin(), _mesh->faces_end(), currentProp);
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);
    } else if (currentProp.isVertexProp()) {
        visualizeVector_asColorForEntity<OpenMesh::VPropHandleT<typename MeshT::Point> >(
                _mesh, _mesh->vertices_begin(), _mesh->vertices_end(), currentProp);
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);
    } else if (currentProp.isEdgeProp()) {
        visualizeVector_asColorForEntity<OpenMesh::EPropHandleT<typename MeshT::Point> >(
                _mesh, _mesh->edges_begin(), _mesh->edges_end(), currentProp);
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);
    } else {
        throw VizException("PropertyVisPlugin::visualizeVector_asColor: Unknown property type.");
    }
}

template<class MeshT>
void PropertyVisPlugin::visualizeVector_asStroke(
        MeshT* _mesh, const PropertyNameListModel::PROP_INFO &currentProp) {

  //perhaps add the node
  
  ACG::SceneGraph::LineNode*  lineNode = new ACG::SceneGraph::LineNode( ACG::SceneGraph::LineNode::LineSegmentsMode, dynamic_cast < ACG::SceneGraph::SeparatorNode* >( PluginFunctions::getRootNode() ) );

  lineNodes_.push_back(lineNode);

  lineNode->clear();
  
  typename MeshT::Color color = convertColor(tool_->lineColor->color());

  //VERTEX
  if ( currentProp.isVertexProp() ){
    //TODO check if this also works if the property is Vec3f
    OpenMesh::VPropHandleT< typename MeshT::Point > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){

      typename MeshT::Point v1 = _mesh->point( v_it );
      typename MeshT::Point v  = _mesh->property(prop, v_it);

      if (tool_->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(tool_->scale->isChecked())
        v *= tool_->scaleBox->value();

      lineNode->add_line( v1, (v1+v) );
      lineNode->add_color(color);
    }

  //FACE
  } else if ( currentProp.isFaceProp() ){
    //TODO check if this also works if the property is Vec3f
    OpenMesh::FPropHandleT< typename MeshT::Point > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it){

      typename MeshT::Point center(0.0, 0.0, 0.0);
      int vCount = 0;

      for (typename MeshT::FaceVertexIter fv_it(*_mesh,f_it); fv_it; ++fv_it){
        vCount++;
        center += _mesh->point(fv_it.handle());
      }

      center /= vCount;

      typename MeshT::Point v  = (_mesh->property(prop, f_it));

      if (tool_->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(tool_->scale->isChecked())
        v *= tool_->scaleBox->value();

      lineNode->add_line( center, (center+v) );
      lineNode->add_color(color);
    }

  //EDGE
  } else if ( currentProp.isEdgeProp()){
    //TODO check if this also works if the property is Vec3f
    OpenMesh::EPropHandleT< typename MeshT::Point > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    for (typename MeshT::EdgeIter e_it = _mesh->edges_begin() ; e_it != _mesh->edges_end() ; ++e_it){

      typename MeshT::HalfedgeHandle hh = _mesh->halfedge_handle( e_it, 0 );

      typename MeshT::VertexHandle vh0 = _mesh->from_vertex_handle( hh );
      typename MeshT::VertexHandle vh1 = _mesh->to_vertex_handle( hh );

      typename MeshT::Point v1 = _mesh->point(vh0) + 0.5 * (_mesh->point(vh1) - _mesh->point(vh0));
      typename MeshT::Point v  = _mesh->property(prop, e_it);
      if (tool_->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(tool_->scale->isChecked())
        v *= tool_->scaleBox->value();

      lineNode->add_line( v1, (v1+v) );
      lineNode->add_color(color);
    }
  } else if ( currentProp.isHalfedgeProp() )
  {
    OpenMesh::HPropHandleT< typename MeshT::Point > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName()) )
      return;
    for (typename MeshT::HalfedgeIter he_it = _mesh->halfedges_begin() ; he_it != _mesh->halfedges_end() ; ++he_it){

      typename MeshT::VertexHandle vh0 = _mesh->from_vertex_handle( he_it.handle() );
      typename MeshT::VertexHandle vh1 = _mesh->to_vertex_handle( he_it.handle() );

      typename MeshT::Point hePBase = halfedge_point(he_it.handle(),_mesh);
      typename MeshT::Point hePPrev = halfedge_point(_mesh->prev_halfedge_handle(he_it.handle()),_mesh);

      typename MeshT::Point v1 = hePBase - (hePBase - hePPrev)*0.5;
      typename MeshT::Point v  = _mesh->property(prop, he_it);
      if (tool_->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(tool_->scale->isChecked())
        v *= tool_->scaleBox->value();

      lineNode->add_line( v1, (v1+v) );
      lineNode->add_color(color);
    }
  }
}

//------------------------------------------------------------------------------
//computed in DrawMesh.cc
template <class MeshT>
typename MeshT::Point PropertyVisPlugin::halfedge_point(const typename MeshT::HalfedgeHandle _heh, const MeshT *_mesh) {

  typename MeshT::Point p  = _mesh->point(_mesh->to_vertex_handle  (_heh));
  typename MeshT::Point pp = _mesh->point(_mesh->from_vertex_handle(_heh));
  typename MeshT::Point pn = _mesh->point(_mesh->to_vertex_handle(_mesh->next_halfedge_handle(_heh)));

  //  typename Mesh::Point n  = (p-pp)%(pn-p);
  typename MeshT::Point fn;
  if( !_mesh->is_boundary(_heh))
    fn = _mesh->normal(_mesh->face_handle(_heh));
  else
    fn = _mesh->normal(_mesh->face_handle(_mesh->opposite_halfedge_handle(_heh)));

  typename MeshT::Point upd = ((fn%(pn-p)).normalize() + (fn%(p-pp)).normalize()).normalize();

  upd *= ((pn-p).norm()+(p-pp).norm())*0.08;

  return (p+upd);

  // double alpha = 0.1;
  // // correct weighting for concave triangles (or at concave boundaries)
  // if( (fn | n)  < 0.0) alpha *=-1.0;

  // return (p*(1.0-2.0*alpha) + pp*alpha + pn*alpha);
}

//------------------------------------------------------------------------------

template< class MeshT >
void PropertyVisPlugin::visualizeDouble( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp)
{
  typename MeshT::Color colorMin, colorMax;
  
  colorMin = convertColor(tool_->doubleMin->color());
  colorMax = convertColor(tool_->doubleMax->color());

  // color codern in [0,1]
  ACG::ColorCoder cc;

  //VERTEX
  if ( currentProp.isVertexProp() ){
    //TODO check if this also works if the property is Vec3d
    OpenMesh::VPropHandleT< double > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    double min, max;

    if ( tool_->intAbsolute->isChecked() ){
      min = FLT_MAX;
      max = 0.0;
    } else {
      min = FLT_MAX;
      max = FLT_MIN;
    }

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){
      if ( tool_->doubleAbsolute->isChecked() ){
        min = std::min( min, fabs(_mesh->property(prop, v_it)));
        max = std::max( max, fabs(_mesh->property(prop, v_it)));
      } else {
        min = std::min( min, _mesh->property(prop, v_it));
        max = std::max( max, _mesh->property(prop, v_it));
      }
    }

    // fixed range?
    if( tool_->doubleFixedRange->isChecked())
    {
      min = tool_->doubleFixedRangeMin->value();
      max = tool_->doubleFixedRangeMax->value();
    }
    else
    {
      tool_->doubleFixedRangeMin->setValue(min);
      tool_->doubleFixedRangeMax->setValue(max);
    }

    const double range = max - min;

    if ( ! _mesh->has_vertex_colors() )
      _mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){

      if (range == 0.0)
        _mesh->set_color(v_it, colorMin);
      else {

        double v = _mesh->property(prop, v_it);

        // absolut value?
        if ( tool_->doubleAbsolute->isChecked())
          v = fabs(v);

        // clamping
        v = std::max(min,v);
        v = std::min(max,v);

        double t = (v-min)/range;

        typename MeshT::Color color;

        if( tool_->doubleColorCoder->isChecked())
          color = cc.color_float4(t);
        else {
          color = (colorMin)*(1.0-t) + (colorMax)*t;
        }

        // set color
        _mesh->set_color(v_it, color);
      }
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);

  //FACE
  } else if ( currentProp.isFaceProp() ){

    //TODO check if this also works if the property is Vec3d
    OpenMesh::FPropHandleT< double > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    double min, max;

    if ( tool_->doubleAbsolute->isChecked() ){
      min = FLT_MAX;
      max = 0.0;
    } else {
      min = FLT_MAX;
      max = FLT_MIN;
    }

    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it){
      if ( tool_->doubleAbsolute->isChecked() ){
        min = std::min( min, fabs(_mesh->property(prop, f_it)));
        max = std::max( max, fabs(_mesh->property(prop, f_it)));
      } else {
        min = std::min( min, _mesh->property(prop, f_it));
        max = std::max( max, _mesh->property(prop, f_it));
      }
    }

    // fixed range?
    if( tool_->doubleFixedRange->isChecked())
    {
      min = tool_->doubleFixedRangeMin->value();
      max = tool_->doubleFixedRangeMax->value();
    }
    else
    {
      tool_->doubleFixedRangeMin->setValue(min);
      tool_->doubleFixedRangeMax->setValue(max);
    }


    double range = max - min;

    if ( ! _mesh->has_face_colors() )
      _mesh->request_face_colors();

    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it){

      if (range == 0.0)
        _mesh->set_color(f_it, colorMin);
      else {

        double v = _mesh->property(prop, f_it);

        // absolut value?
        if ( tool_->doubleAbsolute->isChecked())
          v = fabs(v);

        // clamping
        v = std::max(min,v);
        v = std::min(max,v);

        double t = (v-min)/range;

        typename MeshT::Color color;

        if( tool_->doubleColorCoder->isChecked())
          color = cc.color_float4(t);
        else
          color = (colorMin)*(1.0-t) + (colorMax)*t;

        // set color
        _mesh->set_color(f_it, color);
      }
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);

    //EDGE
  }
  else if ( currentProp.isEdgeProp() )
  {
    //TODO check if this also works if the property is Vec3d
    OpenMesh::EPropHandleT< double > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    double min, max;

    if ( tool_->doubleAbsolute->isChecked() ){
      min = FLT_MAX;
      max = 0.0;
    } else {
      min = FLT_MAX;
      max = FLT_MIN;
    }

    for (typename MeshT::EdgeIter e_it = _mesh->edges_begin() ; e_it != _mesh->edges_end() ; ++e_it){
      if ( tool_->doubleAbsolute->isChecked() ){
        min = std::min( min, fabs(_mesh->property(prop, e_it)));
        max = std::max( max, fabs(_mesh->property(prop, e_it)));
      } else {
        min = std::min( min, _mesh->property(prop, e_it));
        max = std::max( max, _mesh->property(prop, e_it));
      }
    }

    // fixed range?
    if( tool_->doubleFixedRange->isChecked())
    {
      min = tool_->doubleFixedRangeMin->value();
      max = tool_->doubleFixedRangeMax->value();
    }
    else
    {
      tool_->doubleFixedRangeMin->setValue(min);
      tool_->doubleFixedRangeMax->setValue(max);
    }


    double range = max - min;

    if ( ! _mesh->has_edge_colors() )
      _mesh->request_edge_colors();

    for (typename MeshT::EdgeIter e_it = _mesh->edges_begin() ; e_it != _mesh->edges_end() ; ++e_it){

      if (range == 0.0)
        _mesh->set_color(e_it, colorMin);
      else {

        double v = _mesh->property(prop, e_it);

        // absolut value?
        if ( tool_->doubleAbsolute->isChecked())
          v = fabs(v);

        // clamping
        v = std::max(min,v);
        v = std::min(max,v);

        double t = (v-min)/range;

        typename MeshT::Color color;

        if( tool_->doubleColorCoder->isChecked())
          color = cc.color_float4(t);
        else
          color = (colorMin)*(1.0-t) + (colorMax)*t;

        // set color
        _mesh->set_color(e_it, color);
      }
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);
  }
  else if ( currentProp.isHalfedgeProp() )
  {
    //TODO check if this also works if the property is Vec3d
    OpenMesh::HPropHandleT< double > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    double min, max;

    if ( tool_->doubleAbsolute->isChecked() ){
      min = FLT_MAX;
      max = 0.0;
    } else {
      min = FLT_MAX;
      max = FLT_MIN;
    }

    for (typename MeshT::HalfedgeIter he_it = _mesh->halfedges_begin() ; he_it != _mesh->halfedges_end() ; ++he_it){
      if ( tool_->doubleAbsolute->isChecked() ){
        min = std::min( min, fabs(_mesh->property(prop, he_it)));
        max = std::max( max, fabs(_mesh->property(prop, he_it)));
      } else {
        min = std::min( min, _mesh->property(prop, he_it));
        max = std::max( max, _mesh->property(prop, he_it));
      }
    }

    // fixed range?
    if( tool_->doubleFixedRange->isChecked())
    {
      min = tool_->doubleFixedRangeMin->value();
      max = tool_->doubleFixedRangeMax->value();
    }
    else
    {
      tool_->doubleFixedRangeMin->setValue(min);
      tool_->doubleFixedRangeMax->setValue(max);
    }

    double range = max - min;

    if ( ! _mesh->has_halfedge_colors() )
      _mesh->request_halfedge_colors();

    for (typename MeshT::HalfedgeIter he_it = _mesh->halfedges_begin() ; he_it != _mesh->halfedges_end() ; ++he_it){

      if (range == 0.0)
        _mesh->set_color(he_it, colorMin);
      else {

        double v = _mesh->property(prop, he_it);

        // absolut value?
        if ( tool_->doubleAbsolute->isChecked())
          v = fabs(v);

        // clamping
        v = std::max(min,v);
        v = std::min(max,v);

        double t = (v-min)/range;

        typename MeshT::Color color;

        if( tool_->doubleColorCoder->isChecked())
          color = cc.color_float4(t);
        else {
          color = (colorMin)*(1.0-t) + (colorMax)*t;
        }

        // set color
        _mesh->set_color(he_it, color);
      }
    }
    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::HALFEDGES_COLORED);
  }
  else
    emit log(LOGERR, "Selected Type Coloring not supported atm");
}

//------------------------------------------------------------------------------

template< class MeshT >
void PropertyVisPlugin::visualizeInteger( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp)
{
  typename MeshT::Color colorMin, colorMax;

  colorMin = convertColor(tool_->intMin->color());
  colorMax = convertColor(tool_->intMax->color());

  std::map< int, typename MeshT::Color> randomColor;

  if ( tool_->intRandom->isChecked() && tool_->intMapBlack->isChecked() )
    randomColor[ tool_->intMapBlackValue->value() ] = typename MeshT::Color(0.0, 0.0, 0.0, 1.0);


  if ( tool_->intRandom->isChecked() )
    srand ( time(NULL) );

  //VERTEX
  if ( currentProp.isVertexProp() ){
    //TODO check if this also works if the property is Vec3d
    OpenMesh::VPropHandleT< int > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    int min, max;

    if ( tool_->intAbsolute->isChecked() ){
      min = std::numeric_limits<int>::max();
      max = 0;
    } else {
      min = std::numeric_limits<int>::max();
      max = std::numeric_limits<int>::min();
    }

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){
      if ( tool_->intAbsolute->isChecked() ){
        min = std::min( min, std::abs(_mesh->property(prop, v_it)));
        max = std::max( max, std::abs(_mesh->property(prop, v_it)));
      } else {
        min = std::min( min, _mesh->property(prop, v_it));
        max = std::max( max, _mesh->property(prop, v_it));
      }
    }

    int range = max - min;

    if ( ! _mesh->has_vertex_colors() )
      _mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){

      if (range == 0)
        _mesh->set_color(v_it, colorMin);
      else {

        double pos;

        if ( tool_->intAbsolute->isChecked() )
          pos = ( std::abs(_mesh->property(prop, v_it)) - min) / (double) range;
        else
          pos = (_mesh->property(prop, v_it) - min) / (double) range;

        typename MeshT::Color color;

        if ( !tool_->intRandom->isChecked() ){

          color[0] = colorMin[0] * (1-pos) + pos * colorMax[0];
          color[1] = colorMin[1] * (1-pos) + pos * colorMax[1];
          color[2] = colorMin[2] * (1-pos) + pos * colorMax[2];

        } else {

          if ( randomColor.find( _mesh->property(prop, v_it) ) == randomColor.end() ){

            color = colorGenerator_.generateNextColor();
            color[3] = 1.0;

            randomColor[ _mesh->property(prop, v_it) ] = color;
          }

          color = randomColor[ _mesh->property(prop, v_it) ];
        }

        _mesh->set_color(v_it, color);
      }
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);

    //FACE
  } else if ( currentProp.isFaceProp() ){
    //TODO check if this also works if the property is Vec3d
    OpenMesh::FPropHandleT< int > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    int min, max;
    
    if ( tool_->intAbsolute->isChecked() ){
      min = std::numeric_limits<int>::max();
      max = 0;
    } else {
      min = std::numeric_limits<int>::max();
      max = std::numeric_limits<int>::min();
    }
    
    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it){
      if ( tool_->intAbsolute->isChecked() ){
        min = std::min( min, std::abs(_mesh->property(prop, f_it)));
        max = std::max( max, std::abs(_mesh->property(prop, f_it)));
      } else {
        min = std::min( min, _mesh->property(prop, f_it));
        max = std::max( max, _mesh->property(prop, f_it));
      }
    }

    int range = max - min;

    if ( ! _mesh->has_face_colors() )
      _mesh->request_face_colors();

    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it){

      if (range == 0)
        _mesh->set_color(f_it, colorMin);
      else {

        double pos;

        if ( tool_->intAbsolute->isChecked() )
          pos = ( std::abs(_mesh->property(prop, f_it)) - min) / (double) range;
        else
          pos = (_mesh->property(prop, f_it) - min) / (double) range;

        typename MeshT::Color color;
        if ( !tool_->intRandom->isChecked() ){

          color[0] = colorMin[0] * (1-pos) + pos * colorMax[0];
          color[1] = colorMin[1] * (1-pos) + pos * colorMax[1];
          color[2] = colorMin[2] * (1-pos) + pos * colorMax[2];
          color[3] = 1.0;

        } else {

          if ( randomColor.find( _mesh->property(prop, f_it) ) == randomColor.end() ){

            color = colorGenerator_.generateNextColor();
            color[3] = 1.0;

            randomColor[ _mesh->property(prop, f_it) ] = color;
          }

          color = randomColor[ _mesh->property(prop, f_it) ];
        }

        _mesh->set_color(f_it, color);
      }
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);

  //EDGE
  } else
    emit log(LOGERR, "Edge Coloring not supported atm");
}


//------------------------------------------------------------------------------

template< class MeshT >
void PropertyVisPlugin::visualizeSkinWeights( MeshT*   _mesh, int _boneId, const PropertyNameListModel::PROP_INFO &currentProp) {

  typename MeshT::Color colorMin, colorMax;


  //VERTEX
  if ( currentProp.isVertexProp() ){
    OpenMesh::VPropHandleT< BaseSkin::SkinWeights  > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    // Request vertex colors if required
    if ( ! _mesh->has_vertex_colors() )
      _mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){

      BaseSkin::SkinWeights weight = _mesh->property(prop, v_it);

      double value = 0.0;
      if ( weight.find( _boneId ) != weight.end() ) {
        value = weight[_boneId];
      }


      typename MeshT::Color color(value,1.0-value,0.0,1.0);

      // set color
      _mesh->set_color(v_it, color);
    }
  }

  PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);
}

//------------------------------------------------------------------------------


template< class MeshT >
void PropertyVisPlugin::visualizeUnsignedInteger( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp)
{
  typename MeshT::Color colorMin, colorMax;

  colorMin = convertColor(tool_->uintMin->color());
  colorMax = convertColor(tool_->uintMax->color());

  std::map< int, typename MeshT::Color> randomColor;

  if ( tool_->uintRandom->isChecked() && tool_->uintMapBlack->isChecked() )
    randomColor[ tool_->uintMapBlackValue->value() ] = typename MeshT::Color(0.0, 0.0, 0.0,  1.0);


  if ( tool_->uintRandom->isChecked() )
    srand ( time(NULL) );

  //VERTEX
  if ( currentProp.isVertexProp() ){
    //TODO check if this also works if the property is Vec3d
    OpenMesh::VPropHandleT< unsigned int > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    unsigned int min = std::numeric_limits<unsigned int>::max();
    unsigned int max = std::numeric_limits<unsigned int>::min();

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){
      min = std::min( min, _mesh->property(prop, v_it));
      max = std::max( max, _mesh->property(prop, v_it));
    }

    int range = max - min;

    if ( ! _mesh->has_vertex_colors() )
      _mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it){

      if (range == 0)
        _mesh->set_color(v_it, colorMin);
      else {

        double pos = (_mesh->property(prop, v_it) - min) / (double) range;

        typename MeshT::Color color;

        if ( !tool_->uintRandom->isChecked() ){

          color[0] = colorMin[0] * (1-pos) + pos * colorMax[0];
          color[1] = colorMin[1] * (1-pos) + pos * colorMax[1];
          color[2] = colorMin[2] * (1-pos) + pos * colorMax[2];
          color[3] = 1.0;

        } else {

          if ( randomColor.find( _mesh->property(prop, v_it) ) == randomColor.end() ){

            color = colorGenerator_.generateNextColor();
            color[3] = 1.0;

            randomColor[ _mesh->property(prop, v_it) ] = color;
          }

          color = randomColor[ _mesh->property(prop, v_it) ];
        }

        _mesh->set_color(v_it, color);
      }
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);

  //FACE
  } else if ( currentProp.isFaceProp() ){
    //TODO check if this also works if the property is Vec3d
    OpenMesh::FPropHandleT< unsigned int > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    unsigned int min = std::numeric_limits<unsigned int>::max();
    unsigned int max = std::numeric_limits<unsigned int>::min();

    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it){
      min = std::min( min, _mesh->property(prop, f_it));
      max = std::max( max, _mesh->property(prop, f_it));
    }

    int range = max - min;

    if ( ! _mesh->has_face_colors() )
      _mesh->request_face_colors();

    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it){

      if (range == 0)
        _mesh->set_color(f_it, colorMin);
      else {

        double pos = (_mesh->property(prop, f_it) - min) / (double) range;

        typename MeshT::Color color;
        if ( !tool_->uintRandom->isChecked() ){

          color[0] = colorMin[0] * (1-pos) + pos * colorMax[0];
          color[1] = colorMin[1] * (1-pos) + pos * colorMax[1];
          color[2] = colorMin[2] * (1-pos) + pos * colorMax[2];
          color[3] = 1.0;

        } else {

          if ( randomColor.find( _mesh->property(prop, f_it) ) == randomColor.end() ){

            color = colorGenerator_.generateNextColor();
            color[3] = 1.0;

            randomColor[ _mesh->property(prop, f_it) ] = color;
          }

          color = randomColor[ _mesh->property(prop, f_it) ];
        }

        _mesh->set_color(f_it, color);
      }
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);

  //EDGE
  } else
    emit log(LOGERR, "Edge Coloring not supported atm");
}

//------------------------------------------------------------------------------

template< class MeshT >
void PropertyVisPlugin::visualizeBool( MeshT*   _mesh, const PropertyNameListModel::PROP_INFO &currentProp)
{
  typename MeshT::Color colorTrue, colorFalse;
  
  colorTrue  = convertColor(tool_->colorTrue->color());
  colorFalse = convertColor(tool_->colorFalse->color());

  //VERTEX
  if ( currentProp.isVertexProp() ){
    //TODO check if this also works if the property is Vec3d
    OpenMesh::VPropHandleT< bool > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    if ( ! _mesh->has_vertex_colors() )
      _mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it)
      if ( _mesh->property(prop, v_it) )
        _mesh->set_color(v_it, colorTrue);
      else
        _mesh->set_color(v_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);

  //FACE
  } else if ( currentProp.isFaceProp() ){
    //TODO check if this also works if the property is Vec3d
    OpenMesh::FPropHandleT< bool > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    if ( ! _mesh->has_face_colors() )
      _mesh->request_face_colors();

    for (typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_end() ; ++f_it)
      if ( _mesh->property(prop, f_it) )
        _mesh->set_color(f_it, colorTrue);
      else
        _mesh->set_color(f_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);

  //HalfEdge
  } else if ( currentProp.isHalfedgeProp() ){
    OpenMesh::HPropHandleT< bool > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    if ( ! _mesh->has_halfedge_colors() )
      _mesh->request_halfedge_colors();

    for (typename MeshT::HalfedgeIter he_it = _mesh->halfedges_begin() ; he_it != _mesh->halfedges_end() ; ++he_it)
      if ( _mesh->property(prop, he_it) )
        _mesh->set_color(he_it, colorTrue);
      else
        _mesh->set_color(he_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::HALFEDGES_COLORED);

    //EDGE
  } else if ( currentProp.isEdgeProp() ){

    OpenMesh::EPropHandleT< bool > prop;

    if ( !_mesh->get_property_handle(prop, currentProp.propName() ) )
      return;

    if ( ! _mesh->has_edge_colors() )
      _mesh->request_edge_colors();

    for (typename MeshT::EdgeIter e_it = _mesh->edges_begin() ; e_it != _mesh->edges_end() ; ++e_it)
      if ( _mesh->property(prop, e_it) )
        _mesh->set_color(e_it, colorTrue);
      else
        _mesh->set_color(e_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);
  } else {
    emit log( LOGERR, "Visualize Bool not supported for the given datatype. Please report this error message!");
  }
}

//------------------------------------------------------------------------------

///check available properties and display them in the toolbox
template< class MeshT >
void PropertyVisPlugin::updatePropertyList(MeshT* _mesh){

  propertyNameListModel_.refresh(_mesh, PropertyNameListModel::EF_ANY, 0);
//
//  if (_mesh == 0)
//    return;
//
//  int index = tool_->propertyName->currentIndex();
//
//  tool_->propertyName->clear();
//
//  typename MeshT::prop_iterator it;
//  typename MeshT::prop_iterator itEnd;
//
//  //VERTEX PROPERTIES
//  if ( tool_->propertyType->currentText() == tr("Vertex") ){
//    it = _mesh->vprops_begin();
//    itEnd = _mesh->vprops_end();
//
//  // EDGE PROPERTIES
//  } else if ( tool_->propertyType->currentText() == tr("Edge") ){
//    it = _mesh->eprops_begin();
//    itEnd = _mesh->eprops_end();
//
//  // FACE PROPERTIES
//  } else if ( tool_->propertyType->currentText() == tr("Face") ){
//    it = _mesh->fprops_begin();
//    itEnd = _mesh->fprops_end();
//
//  // HALFEDGE PROPERTIES
//  } else if ( tool_->propertyType->currentText() == tr("HalfEdge") ){
//    it = _mesh->hprops_begin();
//    itEnd = _mesh->hprops_end();
//
//  }
//
//  //init type names
//  std::string Vec3dStr;
//  {
//    OpenMesh::PropertyT< ACG::Vec3d >* type = new OpenMesh::PropertyT< ACG::Vec3d >();
//    Vec3dStr = typeid(*type).name();
//  }
//  std::string Vec3fStr;
//  {
//    OpenMesh::PropertyT< ACG::Vec3f >* type = new OpenMesh::PropertyT< ACG::Vec3f >();
//    Vec3fStr = typeid(*type).name();
//  }
//  std::string doubleStr;
//  {
//    OpenMesh::PropertyT< double >* type = new OpenMesh::PropertyT< double >();
//    doubleStr = typeid(*type).name();
//  }
//  std::string uintStr;
//  {
//    OpenMesh::PropertyT< unsigned int >* type = new OpenMesh::PropertyT< unsigned int >();
//    uintStr = typeid(*type).name();
//  }
//  std::string intStr;
//  {
//    OpenMesh::PropertyT< int >* type = new OpenMesh::PropertyT< int >();
//    intStr = typeid(*type).name();
//  }
//  std::string boolStr;
//  {
//    OpenMesh::PropertyT< bool >* type = new OpenMesh::PropertyT< bool >();
//    boolStr = typeid(*type).name();
//  }
//  std::string skineWeightStr;
//  {
//    OpenMesh::PropertyT< BaseSkin::SkinWeights >* type = new OpenMesh::PropertyT< BaseSkin::SkinWeights >();
//    skineWeightStr = typeid(*type).name();
//  }
//
//  //check dataType
//  for (; it != itEnd; ++it){
//
//    OpenMesh::BaseProperty* baseProp = (*it);
//
//    if (baseProp == 0)
//      continue;
//
//    //VECTOR
//    if ( tool_->propertyDataType->currentText() == tr("3D Vector") ){
//
//      std::string type = typeid(*baseProp).name();
//
//      if ( type == Vec3dStr || type == Vec3fStr )
//        tool_->propertyName->addItem( (QString) baseProp->name().c_str() );
//
//    //DOUBLE
//    } else if ( tool_->propertyDataType->currentText() == tr("Double") ){
//
//      std::string type = typeid(*baseProp).name();
//
//      if ( type == doubleStr )
//        tool_->propertyName->addItem( (QString) baseProp->name().c_str() );
//    //UNSIGNED INT
//    } else if ( tool_->propertyDataType->currentText() == tr("Unsigned Integer") ){
//
//      std::string type = typeid(*baseProp).name();
//
//      if ( type == uintStr )
//        tool_->propertyName->addItem( (QString) baseProp->name().c_str() );
//
//    //INT
//    } else if ( tool_->propertyDataType->currentText() == tr("Integer") ){
//
//      std::string type = typeid(*baseProp).name();
//
//      if ( type == intStr )
//        tool_->propertyName->addItem( (QString) baseProp->name().c_str() );
//
//    //BOOL
//    } else if ( tool_->propertyDataType->currentText() == tr("Bool") ){
//
//      std::string type = typeid(*baseProp).name();
//
//      if ( type == boolStr )
//        tool_->propertyName->addItem( (QString) baseProp->name().c_str() );
//    // Skin Weights
//    } else if ( tool_->propertyDataType->currentText() == tr("Skin Weights") ){
//
//      std::string type = typeid(*baseProp).name();
//
//      if ( type == skineWeightStr )
//        tool_->propertyName->addItem( (QString) baseProp->name().c_str() );
//    }
//  }
//
//  if ( index < tool_->propertyName->count() )
//    tool_->propertyName->setCurrentIndex( index );
}

//-----------------------------------------------------------------------------

/** \brief Find closest vertex to selection
 *
 * @param _mesh Refernce to the mesh
 * @param _face_idx Index of the face that has been clicked on
 */

template <class MeshT>
int PropertyVisPlugin::getClosestVertex(MeshT* _mesh, int _face_idx) {

    typename MeshT::FaceVertexIter fv_it;

    int closest_v_idx = 0;
    double dist = DBL_MAX;
    double temp_dist = 0.0;

    ACG::Vec3d vTemp = ACG::Vec3d(0.0, 0.0, 0.0);
    typename MeshT::Point p;

    for (fv_it = _mesh->fv_iter(_mesh->face_handle(_face_idx)); fv_it; ++fv_it){

      p = _mesh->point( fv_it.handle() );

      // Find closest vertex to selection
      vTemp = ACG::Vec3d(p[0], p[1], p[2]);
      temp_dist = (vTemp - hit_point_).length();

      if (temp_dist < dist) {
          dist = temp_dist;
          closest_v_idx = fv_it.handle().idx();
      }

    }
    return closest_v_idx;
}

//-------------------------------------------------------------------------------------------

/** \brief Find closest edge to selection
 *
 * @param _mesh Reference to the mesh
 * @param _face_idx Index of the face that has been clicked on
 */

template <class MeshT>
int PropertyVisPlugin::getClosestEdge(MeshT* _mesh, int _face_idx) {

    typename MeshT::ConstFaceHalfedgeIter fh_it;
    typename MeshT::VertexHandle v1, v2;
    typename MeshT::Point p1, p2;

    ACG::Vec3d vp1, vp2, click, e, g, h;
    double x, temp_dist, dist = DBL_MAX;
    int closest_e_handle = 0;
    click = ACG::Vec3d(hit_point_[0], hit_point_[1], hit_point_[2]);

    for (fh_it = _mesh->fh_iter(_mesh->face_handle(_face_idx)); fh_it; ++fh_it){

      v1 = _mesh->from_vertex_handle(fh_it);
      v2 = _mesh->to_vertex_handle(fh_it);

      p1 = _mesh->point(v1);
      p2 = _mesh->point(v2);

      vp1 = ACG::Vec3d(p1[0], p1[1], p1[2]);
      vp2 = ACG::Vec3d(p2[0], p2[1], p2[2]);

      e = vp2 - vp1;
      e.normalize();
      g = click - vp1;
      x = g | e;

      temp_dist = (click - (vp1 + x * e)).length();

      if (temp_dist < dist) {
          dist = temp_dist;
          closest_e_handle = _mesh->edge_handle(fh_it.handle()).idx();
      }
    }

    return closest_e_handle;
}


/** \brief Find closest edge to selection
 *
 * @param _mesh Reference to the mesh
 * @param _face_idx Index of the face that has been clicked on
 */

template <class MeshT>
int PropertyVisPlugin::getClosestHalfEdge(MeshT* _mesh, int _face_idx) {

    typename MeshT::ConstFaceHalfedgeIter fh_it;
    typename MeshT::VertexHandle v1, v2;
    typename MeshT::Point p1, p2;

    ACG::Vec3d vp1, vp2, click, e, g, h;
    double x, temp_dist, dist = DBL_MAX;
    int closest_he_handle = 0;
    click = ACG::Vec3d(hit_point_[0], hit_point_[1], hit_point_[2]);

    for (fh_it = _mesh->fh_iter(_mesh->face_handle(_face_idx)); fh_it; ++fh_it){

      v1 = _mesh->from_vertex_handle(fh_it);
      v2 = _mesh->to_vertex_handle(fh_it);

      p1 = _mesh->point(v1);
      p2 = _mesh->point(v2);

      vp1 = ACG::Vec3d(p1[0], p1[1], p1[2]);
      vp2 = ACG::Vec3d(p2[0], p2[1], p2[2]);

      e = vp2 - vp1;
      e.normalize();
      g = click - vp1;
      x = g | e;

      temp_dist = (click - (vp1 + x * e)).length();

      if (temp_dist < dist) {
          dist = temp_dist;
          closest_he_handle = fh_it.handle().idx();
      }
    }

    return closest_he_handle;
}


//-----------------------------------------------------------------------------

template< class MeshT >
void PropertyVisPlugin::getPropertyValue( MeshT* _mesh , int _id, unsigned int _face, ACG::Vec3d& _hitPoint ) {


  int closestVertex   = getClosestVertex(_mesh, _face);
  int closestEdge     = getClosestEdge(_mesh, _face);
  int closestHalfEdge = getClosestHalfEdge(_mesh, _face);
  
  if (!tool_->propertyName_lv->currentIndex().isValid()) return;
  const PropertyNameListModel::PROP_INFO &focusedProp = propertyNameListModel_[tool_->propertyName_lv->currentIndex().row()];

  const std::string &propName = focusedProp.propName();

  //==================================================================
  //VECTOR
  //==================================================================
  if ( tool_->propertyDataType->currentText() == tr("3D Vector") ){

    //VERTEX PROPERTIES
    if ( focusedProp.isVertexProp() ){
      
      OpenMesh::VPropHandleT<ACG::Vec3d> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestVertex ) );
        
        typename MeshT::Point p = _mesh->property(prop, typename MeshT::VertexHandle(closestVertex));
        
        QString value = "( " + QString::number(p[0]) + ", " + QString::number(p[1]) + ", " + QString::number(p[2]) + " )";
        
        tool_->pickedValue->setText( value );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // EDGE PROPERTIES
    } else if ( focusedProp.isEdgeProp() ){
      
      OpenMesh::EPropHandleT<ACG::Vec3d> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestEdge ) );
        
        typename MeshT::Point p = _mesh->property(prop, typename MeshT::EdgeHandle(closestEdge));
        
        QString value = "( " + QString::number(p[0]) + ", " + QString::number(p[1]) + ", " + QString::number(p[2]) + " )";
        
        tool_->pickedValue->setText( value );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // HALFEDGE PROPERTIES
    } else if ( focusedProp.isHalfedgeProp() ){
      
      OpenMesh::HPropHandleT<ACG::Vec3d> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestHalfEdge ) );
        
        typename MeshT::Point p = _mesh->property(prop, typename MeshT::HalfedgeHandle(closestHalfEdge));
        
        QString value = "( " + QString::number(p[0]) + ", " + QString::number(p[1]) + ", " + QString::number(p[2]) + " )";
        
        tool_->pickedValue->setText( value );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // FACE PROPERTIES
    } else if ( focusedProp.isFaceProp() ){

            
      OpenMesh::FPropHandleT<ACG::Vec3d> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( _face ) );
        
        typename MeshT::Point p = _mesh->property(prop, typename MeshT::FaceHandle(_face));
        
        QString value = "( " + QString::number(p[0]) + ", " + QString::number(p[1]) + ", " + QString::number(p[2]) + " )";
        
        tool_->pickedValue->setText( value );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

    }
    
  //==================================================================
  //DOUBLE
  //==================================================================
  } else if ( tool_->propertyDataType->currentText() == tr("Double") ){
  
    //VERTEX PROPERTIES
    if ( focusedProp.isVertexProp() ){
      
      OpenMesh::VPropHandleT<double> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestVertex ) );
        
        double value = _mesh->property(prop, typename MeshT::VertexHandle(closestVertex));
        
        tool_->pickedValue->setText( QString::number( value,'g',10) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // EDGE PROPERTIES
    } else if ( focusedProp.isEdgeProp() ){
      
      OpenMesh::EPropHandleT<double> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestEdge ) );
        
        
        double value = _mesh->property(prop, typename MeshT::EdgeHandle(closestEdge));
        
        tool_->pickedValue->setText( QString::number( value,'g',10) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // HALFEDGE PROPERTIES
    } else if ( focusedProp.isHalfedgeProp() ){
      
      OpenMesh::HPropHandleT<double> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestHalfEdge ) );
        
        
        double value = _mesh->property(prop, typename MeshT::HalfedgeHandle(closestHalfEdge));
        
        tool_->pickedValue->setText( QString::number( value,'g',10) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // FACE PROPERTIES
    } else if ( focusedProp.isFaceProp() ){

            
      OpenMesh::FPropHandleT<double> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( _face ) );
        
        double value = _mesh->property(prop, typename MeshT::FaceHandle(_face));
        
        tool_->pickedValue->setText( QString::number( value,'g',10) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

    }
    
  //==================================================================
  //INT
  //==================================================================
  } else if ( tool_->propertyDataType->currentText() == tr("Integer") ){

    //VERTEX PROPERTIES
    if ( focusedProp.isVertexProp() ){
      
      OpenMesh::VPropHandleT<int> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestVertex ) );
        
        int value = _mesh->property(prop, typename MeshT::VertexHandle(closestVertex));
        
        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // EDGE PROPERTIES
    } else if ( focusedProp.isEdgeProp() ){
      
      OpenMesh::EPropHandleT<int> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestEdge ) );
        
        
        int value = _mesh->property(prop, typename MeshT::EdgeHandle(closestEdge));
        
        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
      // HALFEDGE PROPERTIES
    } else if ( focusedProp.isHalfedgeProp() ){
      
      OpenMesh::HPropHandleT<int> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestHalfEdge ) );
        
        
        int value = _mesh->property(prop, typename MeshT::HalfedgeHandle(closestHalfEdge));
        
        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // FACE PROPERTIES
    } else if ( focusedProp.isFaceProp() ){

            
      OpenMesh::FPropHandleT<int> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( _face ) );
        
        int value = _mesh->property(prop, typename MeshT::FaceHandle(_face));
        
        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

    }

  //==================================================================
  //UNSIGNED INT
  //==================================================================
  } else if ( tool_->propertyDataType->currentText() == tr("Unsigned Integer") ){

    //VERTEX PROPERTIES
    if ( focusedProp.isVertexProp() ){

      OpenMesh::VPropHandleT<unsigned int> prop;

      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestVertex ) );

        unsigned int value = _mesh->property(prop, typename MeshT::VertexHandle(closestVertex));

        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

      // EDGE PROPERTIES
    } else if ( focusedProp.isEdgeProp() ){

      OpenMesh::EPropHandleT<unsigned int> prop;

      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestEdge ) );


        unsigned int value = _mesh->property(prop, typename MeshT::EdgeHandle(closestEdge));

        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

      // HALFEDGE PROPERTIES
    } else if ( focusedProp.isHalfedgeProp() ){

      OpenMesh::HPropHandleT<unsigned int> prop;

      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestHalfEdge ) );


        unsigned int value = _mesh->property(prop, typename MeshT::HalfedgeHandle(closestHalfEdge));

        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

      // FACE PROPERTIES
    } else if ( focusedProp.isFaceProp() ){


      OpenMesh::FPropHandleT<unsigned int> prop;

      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( _face ) );

        unsigned int value = _mesh->property(prop, typename MeshT::FaceHandle(_face));

        tool_->pickedValue->setText( QString::number(value) );
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

    }
    
  //==================================================================
  //BOOL
  //==================================================================
  } else if ( tool_->propertyDataType->currentText() == tr("Bool") ){

    //VERTEX PROPERTIES
    if ( focusedProp.isVertexProp() ){
      
      OpenMesh::VPropHandleT<bool> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestVertex ) );
        
        bool value = _mesh->property(prop, typename MeshT::VertexHandle(closestVertex));
        
        if (value)
          tool_->pickedValue->setText( "true" );
        else
          tool_->pickedValue->setText( "false" );
        
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
      
    // EDGE PROPERTIES
    } else if ( focusedProp.isEdgeProp() ){
      
      OpenMesh::EPropHandleT<bool> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestEdge ) );
        
        
        bool value = _mesh->property(prop, typename MeshT::EdgeHandle(closestEdge));
        
        if (value)
          tool_->pickedValue->setText( "true" );
        else
          tool_->pickedValue->setText( "false" );
        
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
    // HALFEDGE PROPERTIES
    } else if ( focusedProp.isHalfedgeProp() ){
      
      OpenMesh::HPropHandleT<bool> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( closestHalfEdge ) );
        
        
        bool value = _mesh->property(prop, typename MeshT::HalfedgeHandle(closestHalfEdge));
        
        if (value)
          tool_->pickedValue->setText( "true" );
        else
          tool_->pickedValue->setText( "false" );
        
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }
    // FACE PROPERTIES
    } else if ( focusedProp.isFaceProp() ){

            
      OpenMesh::FPropHandleT<bool> prop;
      
      if ( _mesh->get_property_handle(prop, propName) ){
        tool_->pickedHandle->setText( QString::number( _face ) );
        
        bool value = _mesh->property(prop, typename MeshT::FaceHandle(_face));
        
        if (value)
          tool_->pickedValue->setText( "true" );
        else
          tool_->pickedValue->setText( "false" );
        
      }else{
        tool_->pickedHandle->setText("-");
        tool_->pickedValue->setText("-");
      }

    }
    
    //==================================================================
    //Skin Weights
    //==================================================================
    } else if (tool_->propertyDataType->currentText() == tr("Skin Weights")) {


      //VERTEX PROPERTIES
      if (focusedProp.isVertexProp()) {

        OpenMesh::VPropHandleT<BaseSkin::SkinWeights> prop;

        if (_mesh->get_property_handle(prop, propName)) {
          tool_->pickedHandle->setText(QString::number(closestVertex));

          BaseSkin::SkinWeights value = _mesh->property(prop, typename MeshT::VertexHandle(closestVertex));

          QString text = "";
          for (map<unsigned int, double>::iterator it = value.begin(); it != value.end(); ++it) {
            text += "(";
            text += QString::number((*it).first);
            text += ",";
            text += QString::number((*it).second);
            text += ")\n";
          }

          tool_->pickedValue->setText(text);

        } else {
          tool_->pickedHandle->setText("-");
          tool_->pickedValue->setText("-");
        }

    }
  }

}

//-----------------------------------------------------------------------------

template <class MeshT>
void PropertyVisPlugin::addNewProperty(MeshT* mesh) 
{
  // property type
  QString ptype = tool_->propertyType->currentText();
  QString dtype = tool_->propertyDataType->currentText();
  QString pname = tool_->property_name_le->text();

  if ( ptype == tr("Vertex") )
  {
    if ( dtype == tr("3D Vector") )
    {
      OpenMesh::VPropHandleT< TriMesh::Point > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Double") )
    {
      OpenMesh::VPropHandleT< double > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Unsigned Integer") )
    {
      OpenMesh::VPropHandleT< unsigned int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Integer") )
    {
      OpenMesh::VPropHandleT< int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Bool") )
    {
      OpenMesh::VPropHandleT< bool > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    //please update the doc if you write "skin weights"-property support
  }
  else if ( ptype == tr("Edge") )
  {
    if ( dtype == tr("3D Vector") )
    {
      OpenMesh::EPropHandleT< TriMesh::Point > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Double") )
    {
      OpenMesh::EPropHandleT< double > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Unsgined Integer") )
    {
      OpenMesh::EPropHandleT< unsigned int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Integer") )
    {
      OpenMesh::EPropHandleT< int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Bool") )
    {
      OpenMesh::EPropHandleT< bool > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
  }
  else if ( ptype == tr("Face") )
  {
    if ( dtype == tr("3D Vector") )
    {
      OpenMesh::FPropHandleT< TriMesh::Point > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Double") )
    {
      OpenMesh::FPropHandleT< double > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Unsigned Integer") )
    {
      OpenMesh::FPropHandleT< unsigned int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Integer") )
    {
      OpenMesh::FPropHandleT< int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Bool") )
    {
      OpenMesh::FPropHandleT< bool > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
  }
  else if ( ptype == tr("HalfEdge") )
  {
    if ( dtype == tr("3D Vector") )
    {
      OpenMesh::HPropHandleT< TriMesh::Point > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Double") )
    {
      OpenMesh::HPropHandleT< double > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Unsigned Integer") )
    {
      OpenMesh::HPropHandleT< unsigned int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Integer") )
    {
      OpenMesh::HPropHandleT< int > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
    else if ( dtype == tr("Bool") )
    {
      OpenMesh::HPropHandleT< bool > prop;
      mesh->add_property(prop, pname.toAscii().data());
    }
  }
}

//-----------------------------------------------------------------------------

template <typename PropertyType>
void
PropertyVisPlugin::
setVProp(QString _property, int _vidx, PropertyType _val)
{
  BaseObjectData* object = getSelectedObject();
  OpenMesh::VPropHandleT<PropertyType> prop;

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_vidx >= 0 && _vidx < (int)mesh->n_vertices())
            mesh->property(prop, mesh->vertex_handle(_vidx)) = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_vidx >= 0 && _vidx < (int) mesh->n_vertices())
            mesh->property(prop, mesh->vertex_handle(_vidx)) = _val;
      }
    }
  }
}

//-----------------------------------------------------------------------------

template <typename PropertyType>
void
PropertyVisPlugin::
setEProp  (QString _property, int _eidx, PropertyType _val)
{
  BaseObjectData* object = getSelectedObject();
  OpenMesh::EPropHandleT<PropertyType> prop;

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_eidx >= 0 && _eidx < (int) mesh->n_edges())
            mesh->property(prop, mesh->edge_handle(_eidx)) = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_eidx >= 0 && _eidx < (int) mesh->n_edges())
            mesh->property(prop, mesh->edge_handle(_eidx)) = _val;
      }
    }
  }
}

//-----------------------------------------------------------------------------

template <typename PropertyType>
void
PropertyVisPlugin::
setFProp  (QString _property, int _fidx, PropertyType _val)
{
  BaseObjectData* object = getSelectedObject();
  OpenMesh::FPropHandleT<PropertyType> prop;

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_fidx >= 0 && _fidx < (int) mesh->n_faces())
            mesh->property(prop, mesh->face_handle(_fidx)) = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_fidx >= 0 && _fidx < (int) mesh->n_faces())
            mesh->property(prop, mesh->face_handle(_fidx)) = _val;
      }
    }
  }
}

//-----------------------------------------------------------------------------

template <typename PropertyType>
void
PropertyVisPlugin::
setHProp  (QString _property, int _hidx, PropertyType _val)
{
  BaseObjectData* object = getSelectedObject();
  OpenMesh::HPropHandleT<PropertyType> prop;

  if (object != 0)
  {
    if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      TriMesh* mesh = PluginFunctions::triMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_hidx >= 0 && _hidx < (int) mesh->n_halfedges())
            mesh->property(prop, mesh->halfedge_handle(_hidx)) = _val;
      }
    }
    else if ( object->dataType(DATA_TRIANGLE_MESH) )
    {
      PolyMesh* mesh = PluginFunctions::polyMesh(object);

      if ( mesh != 0 )
      {
        if(mesh->get_property_handle(prop, _property.toAscii().data()))
          if(_hidx >= 0 && _hidx < (int) mesh->n_halfedges())
            mesh->property(prop, mesh->halfedge_handle(_hidx)) = _val;
      }
    }
  }
}
