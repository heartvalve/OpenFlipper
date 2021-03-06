/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS PolyLineNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_POLYLINENODET_C

//== INCLUDES =================================================================

#include "PolyLineNodeT.hh"
#include <ACG/GL/gl.hh>
#include <ACG/Utils/VSToolsT.hh>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

/// Constructor
template <class PolyLine>
PolyLineNodeT<PolyLine>::PolyLineNodeT(PolyLine& _pl, BaseNode* _parent, std::string _name) :
        BaseNode(_parent, _name),
        polyline_(_pl),
        vbo_(0),
        updateVBO_(true),
        sphere_(0)

{

  // Initialize our local draw mode references
  POINTS_SPHERES = DrawModes::DrawMode(ACG::SceneGraph::DrawModes::addDrawMode("Points (as Spheres)",
                                                                               ACG::SceneGraph::DrawModes::DrawModeProperties(ACG::SceneGraph::DrawModes::PRIMITIVE_POLYGON,
                                                                               ACG::SceneGraph::DrawModes::LIGHTSTAGE_SMOOTH,
                                                                               ACG::SceneGraph::DrawModes::NORMAL_PER_VERTEX)));

  POINTS_SPHERES_SCREEN = DrawModes::DrawMode(ACG::SceneGraph::DrawModes::addDrawMode("Points (as Spheres, constant screen size)",
                                                                               ACG::SceneGraph::DrawModes::DrawModeProperties(ACG::SceneGraph::DrawModes::PRIMITIVE_POLYGON,
                                                                               ACG::SceneGraph::DrawModes::LIGHTSTAGE_SMOOTH,
                                                                               ACG::SceneGraph::DrawModes::NORMAL_PER_VERTEX)));;

  // Initial default draw mode
  drawMode(DrawModes::WIREFRAME | DrawModes::POINTS );
}

//----------------------------------------------------------------------------

template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  for (unsigned int i=0; i< polyline_.n_vertices(); ++i)
  {
    _bbMin.minimize(polyline_.point(i));
    _bbMax.maximize(polyline_.point(i));
  }
}


//----------------------------------------------------------------------------


template <class PolyLine>
DrawModes::DrawMode
PolyLineNodeT<PolyLine>::
availableDrawModes() const
{
  return (DrawModes::WIREFRAME | DrawModes::POINTS | POINTS_SPHERES | POINTS_SPHERES_SCREEN );
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
  // Block if we do not have any vertices
  if ( polyline_.n_vertices() == 0 ) 
    return;

  // Update the vbo only if required.
  if ( updateVBO_ )
    updateVBO();

  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_TEXTURE_2D);
  
  // Bind the vertex array
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, vbo_);

  // We have to define the correct stride here, as the array ay be interleaved with normals
  // or binormals
  int stride = 0;

  if ( polyline_.vertex_normals_available() )
    stride = 24;

  ACG::GLState::vertexPointer(3, GL_FLOAT , stride, 0);


  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);

  ACG::Vec4f color = _state.ambient_color()  + _state.diffuse_color();

  // draw points
  if (_drawMode & DrawModes::POINTS)
  {
   
    // draw selection
    if( polyline_.vertex_selections_available() && !selectedVertexIndexBuffer_.empty())
    {
      // save old values
      float point_size_old = _state.point_size();
      _state.set_color( Vec4f(1,0,0,1) );
      _state.set_point_size(point_size_old+4);

      glDrawElements(GL_POINTS, selectedVertexIndexBuffer_.size(), GL_UNSIGNED_INT, &(selectedVertexIndexBuffer_[0]));

      _state.set_point_size(point_size_old);
    }

    _state.set_color( color );

    // Draw all vertices (don't care about selection)
    glDrawArrays(GL_POINTS,0,polyline_.n_vertices());
    
  }

  // draw line segments
  if (_drawMode & DrawModes::WIREFRAME) {

    // draw selection
    if (polyline_.edge_selections_available() && !selectedEdgeIndexBuffer_.empty()) {
      // save old values
      float line_width_old = _state.line_width();
      _state.set_color(Vec4f(1, 0, 0, 1));
      _state.set_line_width(2 * line_width_old);

      glDrawElements(GL_LINES, selectedEdgeIndexBuffer_.size(), GL_UNSIGNED_INT, &(selectedEdgeIndexBuffer_[0]));

      _state.set_line_width(line_width_old);
    }

    _state.set_color( color );

    if ( polyline_.is_closed() )
      glDrawArrays(GL_LINE_STRIP, 0, polyline_.n_vertices() + 1);
    else
      glDrawArrays(GL_LINE_STRIP, 0, polyline_.n_vertices());

  }


  // draw normals
  if (polyline_.vertex_normals_available()) {
    double avg_len = polyline_.n_edges() > 0 ? (polyline_.length() / polyline_.n_edges() * 0.75) : 0;
    std::vector<Point> ps;
    for (unsigned int i = 0; i < polyline_.n_vertices(); ++i) {
      ps.push_back(polyline_.point(i));
      ps.push_back(polyline_.point(i) + polyline_.vertex_normal(i) * avg_len);
      if (polyline_.vertex_binormals_available())
        ps.push_back(polyline_.point(i) + polyline_.vertex_binormal(i) * avg_len);
    }

    // Disable the big buffer and switch to in memory buffer
    ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
    ACG::GLState::vertexPointer(&ps[0]);

    float line_width_old = _state.line_width();
    _state.set_color( Vec4f(0.8f, 0.f, 0.f, 1.f) );
    _state.set_line_width(1);

    int stride = polyline_.vertex_binormals_available() ? 3 : 2;
    glBegin(GL_LINES);
    for (unsigned int i = 0; i < polyline_.n_vertices(); ++i) {
      glArrayElement(stride * i);
      glArrayElement(stride * i + 1);
    }
    glEnd();

    if (polyline_.vertex_binormals_available()) {
      _state.set_color( Vec4f(0.f, 0.f, 0.8f, 1.f) );
      glBegin(GL_LINES);
      for (unsigned int i = 0; i < polyline_.n_vertices(); ++i) {
        glArrayElement(stride * i);
        glArrayElement(stride * i + 2);
      }
      glEnd();
    }

    _state.set_line_width(line_width_old);
  }
  
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);

  //Disable the vertex array
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);

  // draw vertices as spheres, using the radius given in the polyline
  if (_drawMode & POINTS_SPHERES)
  {
    // create sphere if not yet done
    if(!sphere_)
      sphere_ = new GLSphere(10,10);

    if( polyline_.vertex_selections_available())
    {
      for(unsigned int i=0; i<polyline_.n_vertices(); ++i)
      {
        if(polyline_.vertex_selected(i))
          _state.set_color( Vec4f(1,0,0,1) );
        else
          _state.set_color( color );

        sphere_->draw(_state, polyline_.vertex_radius(), (Vec3f)polyline_.point(i));
      }
    }
    else
    {
      _state.set_color( color );
      for(unsigned int i=0; i<polyline_.n_vertices(); ++i)
        sphere_->draw(_state, polyline_.vertex_radius(), (Vec3f)polyline_.point(i));
    }
  }
  // draw vertices as spheres with constant size on screen
  if (_drawMode & POINTS_SPHERES_SCREEN)
  {
    // create sphere if not yet done
    if(!sphere_)
      sphere_ = new GLSphere(10,10);

    // precompute desired radius of projected sphere
    double r = 0.5*_state.point_size()/double(_state.viewport_height())*2.0*_state.near_plane()*tan(0.5*_state.fovy());
    r /= _state.near_plane();

    if( polyline_.vertex_selections_available())
    {
      for(unsigned int i=0; i<polyline_.n_vertices(); ++i)
      {
        if(polyline_.vertex_selected(i))
          _state.set_color( Vec4f(1,0,0,1) );
        else
          _state.set_color( color );

        // compute radius in 3D
        const Vec3d p = (Vec3d)polyline_.point(i) - _state.eye();
        const double l = (p|_state.viewing_direction());
        sphere_->draw(_state, r*l, (Vec3f)polyline_.point(i));
      }
    }
    else
    {
      _state.set_color( color );
      for(unsigned int i=0; i<polyline_.n_vertices(); ++i)
      {
        // compute radius in 3D
        const Vec3d p = (Vec3d)polyline_.point(i) - _state.eye();
        const double l = (p|_state.viewing_direction());
        sphere_->draw(_state, r*l, (Vec3f)polyline_.point(i));
      }

    }
  }



}

//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick(GLState& _state, PickTarget _target)
{
  if (  polyline_.n_vertices() == 0 ) 
    return;
  
  // Bind the vertex array
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::vertexPointer( &(polyline_.points())[0] );   
  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
  
  unsigned int n_end = polyline_.n_edges()+1;
  if( !polyline_.is_closed()) --n_end;
  
  // (draw lines slightly in front of everything else)
  //disabled right now because of rendering artifacts.
  //This probably doesn't make sense anyways since the lines are drawn as cylinders and therefore have a width
  // glDepthRange(0.0,0.99)

  switch (_target)
  {
    case PICK_VERTEX:
    {
      _state.pick_set_maximum (polyline_.n_vertices());
      if (drawMode() & DrawModes::POINTS)
          pick_vertices( _state);

      if (drawMode() & POINTS_SPHERES)
          pick_spheres( _state);

      if (drawMode() & POINTS_SPHERES_SCREEN)
          pick_spheres_screen( _state);

      break;
    }

    case PICK_EDGE:
    {
      _state.pick_set_maximum (n_end);
      pick_edges(_state, 0);
      break;
    }

    case PICK_ANYTHING:
    {
      _state.pick_set_maximum (polyline_.n_vertices() + n_end);

      if (drawMode() & DrawModes::POINTS)
          pick_vertices( _state);

      if (drawMode() & POINTS_SPHERES)
          pick_spheres( _state);

      if (drawMode() & POINTS_SPHERES_SCREEN)
          pick_spheres_screen( _state);

      pick_edges( _state, polyline_.n_vertices());
      break;
    }

    default:
      break;
  }
   
  //see above
  // glDepthRange(0.0,1.0)
  
  //Disable the vertex array
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_vertices( GLState& _state )
{
  float point_size_old = _state.point_size();
  glPointSize(18);

  _state.pick_set_name(0);


  glDepthRange(0.0, 0.999999);
  for (unsigned int i=0; i< polyline_.n_vertices(); ++i) {
    _state.pick_set_name (i);
    glBegin(GL_POINTS);
      glArrayElement( i );
    glEnd();
  }
  glDepthRange(0.0, 1.0);
  
  
  glPointSize(point_size_old);
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_spheres( GLState& _state )
{
  if(!sphere_)
    sphere_ = new GLSphere(10,10);

  _state.pick_set_name(0);

  for(unsigned int i=0; i<polyline_.n_vertices(); ++i)
  {
    _state.pick_set_name (i);
    sphere_->draw(_state, polyline_.vertex_radius(), (Vec3f)polyline_.point(i));
  }
}

//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_spheres_screen( GLState& _state )
{
  if(!sphere_)
    sphere_ = new GLSphere(10,10);

  _state.pick_set_name(0);

  // precompute desired radius of projected sphere
  double r = 0.5*_state.point_size()/double(_state.viewport_height())*2.0*_state.near_plane()*tan(0.5*_state.fovy());
  r /= _state.near_plane();

  for(unsigned int i=0; i<polyline_.n_vertices(); ++i)
  {
    _state.pick_set_name (i);
    // compute radius in 3D
    const Vec3d p = (Vec3d)polyline_.point(i) - _state.eye();
    double l = (p|_state.viewing_direction());
    sphere_->draw(_state, r*l, (Vec3f)polyline_.point(i));

//    ToFix: _state does still not provide the near_plane in picking mode!!!
//    std::cerr << "radius in picking: " << r*l << std::endl;
  }
}


//----------------------------------------------------------------------------


template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
pick_edges( GLState& _state, unsigned int _offset)
{
  // Check if we have any edges to draw ( % 0 causes division by zero on windows)
  if ( polyline_.n_edges() == 0 )
    return;
  
  // save old values
  float line_width_old = _state.line_width();
  //  _state.set_line_width(2*line_width_old);
  _state.set_line_width(14);
  
  unsigned int n_end = polyline_.n_edges()+1;
  if( !polyline_.is_closed()) --n_end;

  _state.pick_set_name (0);


  glDepthRange(0.0, 0.999999);
  for (unsigned int i=0; i<n_end; ++i) {
    _state.pick_set_name (i+_offset);
    glBegin(GL_LINES);
      glArrayElement( i     % polyline_.n_vertices() );
      glArrayElement( (i+1) % polyline_.n_vertices() );
    glEnd();
  }
  glDepthRange(0.0, 1.0);
  
  _state.set_line_width(line_width_old);
}

//----------------------------------------------------------------------------

template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
updateVBO() {

  // register custom properties defined in polyline

  for (unsigned int i = 0; i < polyline_.get_num_custom_properties(); ++i) {

    typename PolyLine::CustomPropertyHandle proph = polyline_.enumerate_custom_property_handles(i);

    const void* propDataBuf = polyline_.get_custom_property_buffer(proph);

    typename std::map< typename PolyLine::CustomPropertyHandle, int >::iterator mapEntry = polylinePropMap_.find(proph);

    // insert newly defined properties
    if (mapEntry == polylinePropMap_.end()) {

      // setup element description
      ACG::VertexElement desc;

      unsigned int propSize = 0;
      polyline_.get_custom_property_shader_binding(proph, &propSize, &desc.shaderInputName_, &desc.type_);

      // assume aligned memory without byte padding
      desc.numElements_ = propSize / VertexDeclaration::getGLTypeSize(desc.type_);
      desc.pointer_ = 0;

      polylinePropMap_[proph] = addCustomBuffer(desc, propDataBuf);
    }
    else // otherwise update pointer of property data buffer
      setCustomBuffer(mapEntry->second, propDataBuf);
  }


  // Update the vertex declaration based on the input data:
  vertexDecl_.clear();

  // We always output vertex positions
  vertexDecl_.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_POSITION);

  // Use the normals if available
  if ( polyline_.vertex_normals_available() )
    vertexDecl_.addElement(GL_FLOAT, 3 , ACG::VERTEX_USAGE_NORMAL);

  // Add custom vertex elements to declaration
  for (size_t i = 0; i < customBuffers_.size(); ++i) {
    ACG::VertexElement tmp = customBuffers_[i].first;
    tmp.pointer_ = 0;
    tmp.usage_ = ACG::VERTEX_USAGE_SHADER_INPUT;
    vertexDecl_.addElement(&tmp);
  }

  // create vbo if it does not exist
  if (!vbo_)
    GLState::genBuffersARB(1, &vbo_);

  // size in bytes of vbo,  create additional vertex for closed loop indexing
  unsigned int bufferSize = vertexDecl_.getVertexStride() * (polyline_.n_vertices() + 1);

  // Create the required array
  char* vboData_ = new char[bufferSize];

  // Index buffer for selected vertices
  selectedVertexIndexBuffer_.clear();

  // Index buffer for selected vertices
  selectedEdgeIndexBuffer_.clear();

  for (unsigned int  i = 0 ; i < polyline_.n_vertices(); ++i) {

    writeVertex(i, vboData_ + i * vertexDecl_.getVertexStride());

    // Create an ibo in system memory for vertex selection
    if ( polyline_.vertex_selections_available() && polyline_.vertex_selected(i) )
      selectedVertexIndexBuffer_.push_back(i);

    // Create an ibo in system memory for edge selection
    if ( polyline_.edge_selections_available() && polyline_.edge_selected(i) ) {
      selectedEdgeIndexBuffer_.push_back(i);
      selectedEdgeIndexBuffer_.push_back( (i + 1) % polyline_.n_vertices() );
    }

  }

  // First point is added to the end for a closed loop
  writeVertex(0, vboData_ + polyline_.n_vertices() * vertexDecl_.getVertexStride());

  // Move data to the buffer in gpu memory
  GLState::bindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);
  GLState::bufferDataARB(GL_ARRAY_BUFFER_ARB, bufferSize , vboData_ , GL_STATIC_DRAW_ARB);

  // Remove the local storage
  delete[] vboData_;

  // Update done.
  updateVBO_ = false;
}

//----------------------------------------------------------------------------

template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
writeVertex(unsigned int _vertex, void* _dst) {

  // position and normal in float
  float* fdata = (float*)_dst;

  // Copy from internal storage to VBO in CPU memory
  for ( unsigned int j = 0 ; j < 3 ; ++j)
    *(fdata++) = polyline_.point(_vertex)[j];

  // Also write normal into buffer if available
  if ( polyline_.vertex_normals_available()  )
    for ( unsigned int j = 0 ; j < 3 ; ++j)
      *(fdata++) = polyline_.vertex_normal(_vertex)[j];

  // id offset of custom elements in the vertex declaration
  unsigned int customElementOffset = 1;
  if ( polyline_.vertex_normals_available()  )
    customElementOffset++;

  // copy custom data byte-wise
  for (unsigned int i = 0; i < customBuffers_.size(); ++i) {

    // element in custom input buffer
    const ACG::VertexElement* veInput = &customBuffers_[i].first;
    unsigned int elementInputStride = veInput->getByteOffset();
    unsigned int elementSize = ACG::VertexDeclaration::getElementSize(veInput);

    if (!elementInputStride)
      elementInputStride = elementSize;

    // element in vertex buffer
    const ACG::VertexElement* ve = vertexDecl_.getElement(i + customElementOffset);

    const char* src = (const char*)customBuffers_[i].second;

    memcpy((char*)_dst + ve->getByteOffset(), src + elementInputStride * _vertex, elementSize);
  }

}

//----------------------------------------------------------------------------

template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
getRenderObjects(ACG::IRenderer* _renderer, ACG::GLState&  _state , const ACG::SceneGraph::DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat) {

  // Block if we do not have any vertices
  if ( polyline_.n_vertices() == 0 )
    return;

  // init base render object
  ACG::RenderObject ro;

  _state.enable(GL_COLOR_MATERIAL);
  _state.enable(GL_LIGHTING);
  ro.initFromState(&_state);

  ro.setMaterial(_mat);

  // draw after scene-meshes
  ro.priority = 1;

  // Update the vbo only if required.
  if ( updateVBO_ )
    updateVBO();

  // Set to the right vbo
  ro.vertexBuffer = vbo_;

  // decl must be static or member,  renderer does not make a copy
  ro.vertexDecl = &vertexDecl_;

  // Set style
  ro.debugName = "PolyLine";
  ro.blending = false;
  ro.depthTest = true;

  // Default color
  ACG::Vec4f defaultColor   = _state.ambient_color()  + _state.diffuse_color();
  ACG::Vec4f selectionColor = ACG::Vec4f(1.0,0.0,0.0,1.0);

  // Viewport size
  ACG::Vec2f screenSize(float(_state.viewport_width()), float(_state.viewport_height()));

  for (unsigned int i = 0; i < _drawMode.getNumLayers(); ++i) {
    ACG::SceneGraph::Material localMaterial = *_mat;

    const ACG::SceneGraph::DrawModes::DrawModeProperties* props = _drawMode.getLayer(i);

    ro.setupShaderGenFromDrawmode(props);
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

    //---------------------------------------------------
    // No lighting!
    // Therefore we need some emissive color
    //---------------------------------------------------
    localMaterial.baseColor(defaultColor);
    ro.setMaterial(&localMaterial);


    switch (props->primitive()) {

      case ACG::SceneGraph::DrawModes::PRIMITIVE_POINT:

        // Render all vertices which are selected via an index buffer
        ro.debugName = "polyline.Points.selected";
        localMaterial.baseColor(selectionColor);
        ro.setMaterial(&localMaterial);

        // Point Size geometry shader
        ro.setupPointRendering(_mat->pointSize(), screenSize);

        if (!selectedVertexIndexBuffer_.empty())
        {
          ro.glDrawElements(GL_POINTS, selectedVertexIndexBuffer_.size(), GL_UNSIGNED_INT, &(selectedVertexIndexBuffer_[0]));
          // apply user settings
          applyRenderObjectSettings(props->primitive(), &ro);

          _renderer->addRenderObject(&ro);
        }

        // Render all vertices (ignore selection here!)
        ro.debugName = "polyline.Points";
        localMaterial.baseColor(defaultColor);
        ro.setMaterial(&localMaterial);
        ro.glDrawArrays(GL_POINTS, 0, polyline_.n_vertices());

        // Point Size geometry shader
        ro.setupPointRendering(_mat->pointSize(), screenSize);

        // apply user settings
        applyRenderObjectSettings(props->primitive(), &ro);

        _renderer->addRenderObject(&ro);

        break;
      case ACG::SceneGraph::DrawModes::PRIMITIVE_WIREFRAME:

        // Render all edges which are selected via an index buffer
        ro.debugName = "polyline.Wireframe.selected";
        localMaterial.baseColor(selectionColor);
        ro.setMaterial(&localMaterial);

        // Line Width geometry shader
        ro.setupLineRendering(_state.line_width(), screenSize);

        if (!selectedEdgeIndexBuffer_.empty())
        {
          ro.glDrawElements(GL_LINES, selectedEdgeIndexBuffer_.size(), GL_UNSIGNED_INT, &(selectedEdgeIndexBuffer_[0]));

          // apply user settings
          applyRenderObjectSettings(props->primitive(), &ro);

          _renderer->addRenderObject(&ro);
        }

        ro.debugName = "polyline.Wireframe";
        localMaterial.baseColor(defaultColor);
        ro.setMaterial(&localMaterial);
        // The first point is mapped to an additional last point in buffer, so we can
        // just Render one point more to get a closed line
        if ( polyline_.is_closed() )
          ro.glDrawArrays(GL_LINE_STRIP, 0, polyline_.n_vertices() + 1);
        else
          ro.glDrawArrays(GL_LINE_STRIP, 0, polyline_.n_vertices());

        // Line Width geometry shader
        ro.setupLineRendering(_state.line_width(), screenSize);

        // apply user settings
        applyRenderObjectSettings(props->primitive(), &ro);

        _renderer->addRenderObject(&ro);

        break;
      default:
        break;
    }

  }

}

//----------------------------------------------------------------------------

template <class PolyLine>
int
PolyLineNodeT<PolyLine>::
addCustomBuffer( const ACG::VertexElement& _desc, const void* _buffer) {

  if (_buffer) {
    customBuffers_.push_back( std::pair<ACG::VertexElement, const void*>(_desc, _buffer) );
    update();

    return int(customBuffers_.size()-1);
  }
  else
  {
    std::cerr << "PolyLineNodeT::addCustomBuffer - null pointer buffer" << std::endl;
    return -1;
  }
}

//----------------------------------------------------------------------------

template <class PolyLine>
void
PolyLineNodeT<PolyLine>::
setCustomBuffer( int _id, const void* _buffer) {

  customBuffers_[_id].second = _buffer;
  update();
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
