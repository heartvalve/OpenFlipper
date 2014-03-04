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
//  CLASS MeshNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_MESHNODE_C

#include <ACG/Geometry/GPUCacheOptimizer.hh>
#include <ACG/GL/DrawMesh.hh>
#include <ACG/GL/GLError.hh>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {
    

//== INCLUDES =================================================================


#include "MeshNode2T.hh"

template<class Mesh>
MeshNodeT<Mesh>::
MeshNodeT(Mesh&        _mesh,
              BaseNode*    _parent,
              std::string  _name ): 
  BaseNode(_parent, _name),
  mesh_(_mesh),
  drawMesh_(0),
  enableNormals_(true),
  enableColors_(true),
  enabled_arrays_(0),
  updateVertexPicking_(true),
  vertexPickingBaseIndex_(0),
  updateEdgePicking_(true),
  edgePickingBaseIndex_(0),
  updateFacePicking_(true),
  facePickingBaseIndex_(0),
  updateAnyPicking_(true),
  anyPickingBaseIndex_(0),
  perFaceTextureIndexAvailable_(false),
  perFaceTextureCoordsAvailable_(false),
  textureMap_(0),
  polyEdgeBuf_(0),
  polyEdgeBufSize_(0),
  polyEdgeBufTex_(0)
{
 
  /// \todo : Handle vbo not supported
  if ( ! checkExtensionSupported("GL_ARB_vertex_buffer_object") ) {
    std::cerr << "Error! Vertex buffer objects are not supported! The meshNode will not work without them!" << std::endl;
  }
  
  drawMesh_ = new DrawMeshT<Mesh>(mesh_);
  
}  

template<class Mesh>
MeshNodeT<Mesh>::
~MeshNodeT()
{
  // Delete all allocated buffers
  delete drawMesh_;
}

template<class Mesh>
DrawModes::DrawMode
MeshNodeT<Mesh>::
availableDrawModes() const {
  DrawModes::DrawMode drawModes(DrawModes::NONE);
  
  // We can always render points and a wireframe.
  drawModes |= DrawModes::POINTS;
  drawModes |= DrawModes::HIDDENLINE;
  drawModes |= DrawModes::WIREFRAME;
  drawModes |= DrawModes::HALFEDGES;
  
  if (mesh_.has_vertex_normals())
  {
    drawModes |= DrawModes::POINTS_SHADED;  
    drawModes |= DrawModes::SOLID_SMOOTH_SHADED;
    drawModes |= DrawModes::SOLID_PHONG_SHADED;
  }
  
  if (mesh_.has_face_normals())
    drawModes |= DrawModes::SOLID_FLAT_SHADED;

  if (mesh_.has_halfedge_normals())
    drawModes |= DrawModes::SOLID_SMOOTH_SHADED_FEATURES;
  
  if (mesh_.has_vertex_colors())
  {
    drawModes |= DrawModes::POINTS_COLORED;
    drawModes |= DrawModes::SOLID_POINTS_COLORED;

    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_POINTS_COLORED_SHADED;
  }

  if(mesh_.has_edge_colors())
  {
    drawModes |= DrawModes::EDGES_COLORED;
  }

  if(mesh_.has_halfedge_colors())
  {
    drawModes |= DrawModes::HALFEDGES_COLORED;
  }
  
  if (mesh_.has_face_colors()) {
    drawModes |= DrawModes::SOLID_FACES_COLORED;
    
    if( mesh_.has_face_normals() )
      drawModes |= DrawModes::SOLID_FACES_COLORED_FLAT_SHADED;

    if( mesh().has_vertex_normals() )
      drawModes |= DrawModes::SOLID_FACES_COLORED_SMOOTH_SHADED;
  }
  
  if ( mesh_.has_vertex_texcoords2D() ) {
    drawModes |= DrawModes::SOLID_TEXTURED;
    
    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_TEXTURED_SHADED; 
  }
  
  if ( perFaceTextureCoordsAvailable_ ) {
    drawModes |= DrawModes::SOLID_2DTEXTURED_FACE;
    
    if (mesh_.has_face_normals())
      drawModes |= DrawModes::SOLID_2DTEXTURED_FACE_SHADED;
  }
  
  return drawModes;
}

template<class Mesh>
void
MeshNodeT<Mesh>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax) {
  _bbMin.minimize(bbMin_);
  _bbMax.maximize(bbMax_);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode) {
/*  
  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ) ||
    ( _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
    ( _drawMode & DrawModes::SOLID_TEXTURED) ||
    ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE))
  {
    drawMesh_->setFlatShading();
  }
  else
    drawMesh_->setSmoothShading();


  if ( (_drawMode & DrawModes::SOLID_FACES_COLORED ||
    _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED))
  {
    drawMesh_->usePerFaceColors();
  }
  else
    drawMesh_->usePerVertexColors();
*/
    
  GLenum prev_depth = _state.depthFunc();
  
  glPushAttrib(GL_ENABLE_BIT);

  /// get bound texture buffer and target
  GLuint lastBuffer = ACG::GLState::getBoundTextureBuffer();
  GLenum lastTarget = ACG::GLState::getBoundTextureTarget();

  // Unbind to avoid painting textures on non textured primitives
  ACG::GLState::bindTexture(lastTarget,0);


  if ( (_drawMode & DrawModes::POINTS) || (_drawMode & DrawModes::POINTS_COLORED) || (_drawMode & DrawModes::POINTS_SHADED )  ) {
    
    _state.set_color( _state.specular_color() );

    ACG::GLState::shadeModel(GL_FLAT);
    
    if ( _drawMode & DrawModes::POINTS_SHADED  ) {
      ACG::GLState::enable(GL_LIGHTING);
    } else
      ACG::GLState::disable(GL_LIGHTING);
  
    // Use Colors in this mode if allowed
    if ( enableColors_ && (_drawMode & DrawModes::POINTS_COLORED) )
    {
      drawMesh_->usePerVertexColors();

      // If we have colors and lighting with normals, we have to use colormaterial
      if ( enableNormals_ && (_drawMode & DrawModes::POINTS_SHADED ) )
        ACG::GLState::enable(GL_COLOR_MATERIAL);
      else
        ACG::GLState::disable(GL_COLOR_MATERIAL);
    }
    else
      drawMesh_->disableColors();
    
    // Bring the arrays online
//    enable_arrays(arrays);
    
    // Draw vertices
    draw_vertices();
  }
  
  
  /// \todo We can render also wireframe shaded and with vertex colors
  if (_drawMode & DrawModes::WIREFRAME)
  {

    const Vec4f oldColor = _state.color();

    // If the mode is atomic, we use the specular, otherwise we take the overlay color
    if (_drawMode.isAtomic() )
      _state.set_color( _state.specular_color() );
    else
      _state.set_color( _state.overlay_color() );

    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);

    drawMesh_->disableColors();

    draw_lines();

    _state.set_color(oldColor);
  }  
  
  if (_drawMode & DrawModes::HIDDENLINE)
  {
//    enable_arrays(VERTEX_ARRAY);
    
    // First:
    // Render all faces in background color to initialize z-buffer
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//    drawMesh_->SetFlatShading();
    drawMesh_->disableColors();

    Vec4f  clear_color  = _state.clear_color();
    clear_color[3] = 1.0;
    _state.set_color( clear_color );

    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    // Second
    // Render the lines. All lines not on the front will be skipped in z-test
//    enable_arrays(VERTEX_ARRAY|LINE_INDEX_ARRAY);
    ACG::GLState::depthFunc(GL_LEQUAL);

    _state.set_color( _state.specular_color() );

    draw_lines();
    
    //restore depth buffer comparison function for the next draw calls inside this function
    ACG::GLState::depthFunc(prev_depth);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (_drawMode & DrawModes::EDGES_COLORED)
  {
    enable_arrays( PER_EDGE_VERTEX_ARRAY | PER_EDGE_COLOR_ARRAY );
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_lines();
  }  

  if (_drawMode & DrawModes::HALFEDGES)
  {
    _state.set_color( _state.specular_color() );

    enable_arrays( PER_HALFEDGE_VERTEX_ARRAY);
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_halfedges();
  }  

  if (_drawMode & DrawModes::HALFEDGES_COLORED)
  {
    enable_arrays( PER_HALFEDGE_VERTEX_ARRAY | PER_HALFEDGE_COLOR_ARRAY );
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_halfedges();
  }  

  if ( ( _drawMode & DrawModes::SOLID_POINTS_COLORED ) && mesh_.has_vertex_colors() )
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->usePerVertexColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }

  if ( ( _drawMode & DrawModes::SOLID_POINTS_COLORED_SHADED ) && mesh_.has_vertex_colors() && mesh_.has_vertex_normals() )
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);
    if ( enableNormals_ ) {
       ACG::GLState::enable(GL_COLOR_MATERIAL);
    } else {
      ACG::GLState::disable(GL_COLOR_MATERIAL);
    }

    drawMesh_->usePerVertexColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }

  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ) && mesh_.has_face_normals() && mesh_.n_faces() > 0)
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setFlatShading();
    drawMesh_->disableColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED ) && mesh_.has_vertex_normals() )
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->usePerVertexNormals();
    drawMesh_->setSmoothShading();
    drawMesh_->disableColors();
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_PHONG_SHADED ) && mesh_.has_vertex_normals() )
  {
    ///\todo Integrate shader here! 
    //     if ( parent() != 0 ) {
    //       if ( parent()->className() == "ShaderNode" ) {
    //
    //         ShaderNode* node = dynamic_cast< ShaderNode* > ( parent() );
    //
    //         GLSL::PtrProgram program = node->getShader( DrawModes::SOLID_PHONG_SHADED );
    //
    //         // Enable own Phong shader
    //         program->use();
//    enable_arrays(VERTEX_ARRAY | NORMAL_VERTEX_ARRAY );
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->usePerVertexNormals();
    drawMesh_->setSmoothShading();
    drawMesh_->disableColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    //disable own Phong shader
    //         program->disable();
    //       }
    //     }
  }
  
  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED ) && mesh_.has_face_colors()  && mesh_.n_faces() > 0)
  {
    Vec4f base_color_backup = _state.base_color();
    
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);
//    enable_arrays(PER_FACE_VERTEX_ARRAY | PER_FACE_COLOR_ARRAY);    

    drawMesh_->usePerFaceColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    _state.set_base_color(base_color_backup);
  }


  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED_FEATURES ) && mesh_.has_halfedge_normals()  && mesh_.n_faces() > 0)
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->disableColors();
    drawMesh_->setSmoothShading();
    drawMesh_->usePerHalfedgeNormals();

    draw_faces();

    ACG::GLState::depthRange(0.0, 1.0);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED ) && mesh_.has_face_colors() && mesh_.has_face_normals()  && mesh_.n_faces() > 0 )
  {
    Vec4f base_color_backup = _state.base_color();
    ACG::GLState::enable(GL_LIGHTING);
    
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);
//    enable_arrays(PER_FACE_VERTEX_ARRAY | PER_FACE_COLOR_ARRAY | PER_FACE_NORMAL_ARRAY );

    drawMesh_->setFlatShading();
    drawMesh_->usePerFaceColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    _state.set_base_color(base_color_backup);
  }

  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED_SMOOTH_SHADED ) && mesh_.has_face_colors() && mesh_.has_vertex_normals() && mesh_.n_faces() > 0)
  {
    Vec4f base_color_backup = _state.base_color();

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setSmoothShading();
    drawMesh_->usePerVertexNormals();
    drawMesh_->usePerFaceColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);

    _state.set_base_color(base_color_backup);
  }

  // Rebind the previous texture
  ACG::GLState::bindTexture(lastTarget,lastBuffer);

  if ( ( _drawMode & DrawModes::SOLID_TEXTURED )  && mesh_.has_vertex_texcoords2D())
  {
    ///\todo enableTexCoords_
//    enable_arrays(VERTEX_ARRAY | TEXCOORD_VERTEX_ARRAY );
    ACG::GLState::enable(GL_TEXTURE_2D);
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->disableColors();
    drawMesh_->usePerVertexTexcoords();


    // texture environment: fragment color = texture sample
    GLint prevTexEnvMode = 0;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &prevTexEnvMode);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_TEXTURE_2D);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, prevTexEnvMode);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_TEXTURED_SHADED ) && mesh_.has_vertex_texcoords2D() && mesh_.has_vertex_normals())
  {
//    enable_arrays(VERTEX_ARRAY | NORMAL_VERTEX_ARRAY | TEXCOORD_VERTEX_ARRAY);
    ACG::GLState::enable(GL_TEXTURE_2D);
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setSmoothShading();
    drawMesh_->disableColors();
    drawMesh_->usePerVertexTexcoords();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_TEXTURE_2D);
  }
  
    
  // Textured by using coordinates stored in halfedges ... arrays generated by stripprocessor
  if ( (_drawMode & DrawModes::SOLID_2DTEXTURED_FACE)  && mesh_.n_faces() > 0 )
  {
    ACG::GLState::enable(GL_TEXTURE_2D);
    
//    enable_arrays( PER_FACE_VERTEX_ARRAY | PER_FACE_TEXCOORD_ARRAY );
    
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->disableColors();
    drawMesh_->usePerHalfedgeTexcoords();

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    ACG::GLState::disable(GL_TEXTURE_2D);
  }
  

  // Textured by using coordinates stored in halfedges
  if ( ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED ) && mesh_.has_face_normals()  && mesh_.n_faces() > 0)
  {
    ACG::GLState::enable(GL_TEXTURE_2D);

    //    enable_arrays( PER_FACE_VERTEX_ARRAY | PER_FACE_TEXCOORD_ARRAY | PER_FACE_PER_VERTEX_NORMAL_ARRAY );

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setFlatShading();
    drawMesh_->disableColors();
    drawMesh_->usePerHalfedgeTexcoords();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_TEXTURE_2D);

  }

  enable_arrays(0);
  
  // Unbind all remaining buffers
  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB , 0 );
  
  glPopAttrib();
}


template <class Mesh>
void ACG::SceneGraph::MeshNodeT<Mesh>::getRenderObjects( IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat )
{
  RenderObject ro;

  ro.debugName = "MeshNode";
   
  // shader gen setup (lighting, shademode, vertex-colors..)
  
  for (unsigned int i = 0; i < _drawMode.getNumLayers(); ++i)
  {
    const DrawModes::DrawModeProperties* props = _drawMode.getLayer(i);

    // reset renderobject
    ro.initFromState(&_state);
    ro.priority = 0;
    ro.depthRange = Vec2f(0.0f, 1.0f);
    ro.depthTest = true; // some previous node disabled depth testing
    ro.depthWrite = true;
    ro.depthFunc = GL_LESS;
    ro.setMaterial(_mat);

    ro.shaderDesc.vertexTemplateFile = ""; //QString(props->vertexShader().c_str());
    ro.shaderDesc.geometryTemplateFile = ""; //QString(props->geometryShader().c_str());
    ro.shaderDesc.fragmentTemplateFile = ""; //QString(props->fragmentShader().c_str());

    // ------------------------
    // 1. setup drawMesh based on property source


    if (props->flatShaded())
      drawMesh_->setFlatShading();
    else
      drawMesh_->setSmoothShading();


    ro.shaderDesc.vertexColors = true;

    switch (props->colorSource())
    {
    case DrawModes::COLOR_PER_VERTEX: drawMesh_->usePerVertexColors(); break;
    case DrawModes::COLOR_PER_FACE: drawMesh_->usePerFaceColors(); break;
    default:
      {
        drawMesh_->disableColors(); 
        ro.shaderDesc.vertexColors = false;
      } break;
    }

    switch (props->normalSource())
    {
    case DrawModes::NORMAL_PER_VERTEX: drawMesh_->usePerVertexNormals(); break;
    case DrawModes::NORMAL_PER_HALFEDGE: drawMesh_->usePerHalfedgeNormals(); break;
    default: break;
    }

    ro.shaderDesc.addTextureType(GL_TEXTURE_2D,false,0);

    switch (props->texcoordSource())
    {
    case DrawModes::TEXCOORD_PER_VERTEX: drawMesh_->usePerVertexTexcoords(); break;
    case DrawModes::TEXCOORD_PER_HALFEDGE: drawMesh_->usePerHalfedgeTexcoords(); break;
    default:
      {
        ro.shaderDesc.clearTextures();
      }break;
    }

    // ------------------------
    // 2. prepare renderobject


    // enable / disable lighting
    ro.shaderDesc.numLights = props->lighting() ? 0 : -1;

    // TODO: better handling of attribute sources in shader gen
    switch (props->lightStage())
    {
      case DrawModes::LIGHTSTAGE_SMOOTH:
        ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;
        break;
      case DrawModes::LIGHTSTAGE_PHONG:
        ro.shaderDesc.shadeMode = SG_SHADE_PHONG;
        break;
      case DrawModes::LIGHTSTAGE_UNLIT:
        ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
        break;
    }

    if (props->flatShaded())
      ro.shaderDesc.shadeMode = SG_SHADE_FLAT;

    // handle 'special' primitives (wireframe, hiddenline, primitives in sysmem buffers)..

    if (props->primitive() == DrawModes::PRIMITIVE_WIREFRAME)
    {
      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
      drawMesh_->disableColors();

      // use specular color for lines
      if (_drawMode.isAtomic() )
        ro.emissive = ro.specular;
      else
        ro.emissive = OpenMesh::color_cast<ACG::Vec3f>(_state.overlay_color());

      // allow wireframe + solid mode
      ro.depthFunc = GL_LEQUAL;

      // use alpha blending for anti-aliasing in combined wireframe + solid mode
      ro.blending = true;
      ro.blendSrc = GL_SRC_ALPHA;
      ro.blendDest = GL_ONE_MINUS_SRC_ALPHA;

      // use geometry shaders to simulate line width
   
      QString geomTemplate = ShaderProgGenerator::getShaderDir();
      geomTemplate += "Wireframe/geometry_wire.tpl";

      QString fragTemplate = ShaderProgGenerator::getShaderDir();
      fragTemplate += "Wireframe/fragment_wire.tpl";

#ifdef GL_ARB_texture_buffer_object
      if (!drawMesh_->getMeshCompiler()->isTriangleMesh())
      {
        // use modified shader for npoly meshes
        // the shader can identify non-face edges with the poly edge buffer
        updatePolyEdgeBuf();

        if (polyEdgeBufTex_)
        {
          geomTemplate = ShaderProgGenerator::getShaderDir();
          geomTemplate += "Wireframe/geometry_npoly_wire.tpl";

          fragTemplate = ShaderProgGenerator::getShaderDir();
          fragTemplate += "Wireframe/fragment_npoly_wire.tpl";

          RenderObject::Texture roTex;
          roTex.id = polyEdgeBufTex_;
          roTex.type = GL_TEXTURE_BUFFER;

          // bind poly edge buffer to texture stage 2
          ro.setUniform("polyEdgeBuffer", 2);
          ro.addTexture(roTex, 2, false);
        }
      }
#endif

      ro.shaderDesc.geometryTemplateFile = geomTemplate;
      ro.shaderDesc.fragmentTemplateFile = fragTemplate;

      ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
      ro.setUniform("lineWidth", _state.line_width());

      add_face_RenderObjects(_renderer, &ro);
    }

    if (props->primitive()  == DrawModes::PRIMITIVE_HIDDENLINE)
    {
      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
      drawMesh_->disableColors();

      // use specular color for lines
      if (_drawMode.isAtomic() )
        ro.emissive = ro.specular;
      else
        ro.emissive = OpenMesh::color_cast<ACG::Vec3f>(_state.overlay_color());

      // use shaders to simulate line width
      QString geomTemplate = ShaderProgGenerator::getShaderDir();
      geomTemplate += "Wireframe/geometry_hidden.tpl";

      QString fragTemplate = ShaderProgGenerator::getShaderDir();
      fragTemplate += "Wireframe/fragment_hidden.tpl";

#ifdef GL_ARB_texture_buffer_object
      if (!drawMesh_->getMeshCompiler()->isTriangleMesh())
      {
        // use modified shader for npoly meshes
        // the shader can identify non-face edges with the poly edge buffer
        updatePolyEdgeBuf();

        if (polyEdgeBufTex_)
        {
          geomTemplate = ShaderProgGenerator::getShaderDir();
          geomTemplate += "Wireframe/geometry_npoly_hidden.tpl";

          fragTemplate = ShaderProgGenerator::getShaderDir();
          fragTemplate += "Wireframe/fragment_npoly_hidden.tpl";

          RenderObject::Texture roTex;
          roTex.id = polyEdgeBufTex_;
          roTex.type = GL_TEXTURE_BUFFER;

          // bind poly edge buffer to texture stage 2
          ro.setUniform("polyEdgeBuffer", 2);
          ro.addTexture(roTex, 2, false);
        }
      }
#endif


      ro.shaderDesc.geometryTemplateFile = geomTemplate;
      ro.shaderDesc.fragmentTemplateFile = fragTemplate;

      ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
      ro.setUniform("lineWidth", _state.line_width());
      ro.setUniform("bkColor", _state.clear_color());

      add_face_RenderObjects(_renderer, &ro);

    }

    if (props->colored() && props->primitive()  == DrawModes::PRIMITIVE_EDGE)
    {
      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
      ro.shaderDesc.vertexColors = true;

      // note: colored edges are in sysmem, so they are directly bound to the VertexDeclaration
      ro.vertexDecl = drawMesh_->getEdgeColoredVertexDeclaration();
      ro.glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));

      // use specular color for lines
      ro.emissive = ro.specular;



      // use shaders to simulate line width
      QString geomTemplate = ShaderProgGenerator::getShaderDir();
      geomTemplate += "Wireframe/geom_line2quad.tpl";

      ro.shaderDesc.geometryTemplateFile = geomTemplate;

      ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
      ro.setUniform("lineWidth", _state.line_width());

      _renderer->addRenderObject(&ro);

       // skip other edge primitives for this drawmode layer
      continue;
    }  

    if (props->primitive()  == DrawModes::PRIMITIVE_HALFEDGE)
    {
      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

      // buffers in system memory
      if (props->colored())
        ro.vertexDecl = drawMesh_->getHalfedgeVertexDeclaration();
      else
        ro.vertexDecl = drawMesh_->getHalfedgeColoredVertexDeclaration();

      // use specular color for lines
      ro.emissive = ro.specular;

      ro.glDrawArrays(GL_LINES, 0, int(mesh_.n_halfedges() * 2));



      // use shaders to simulate line width
      QString geomTemplate = ShaderProgGenerator::getShaderDir();
      geomTemplate += "Wireframe/geom_line2quad.tpl";

      ro.shaderDesc.geometryTemplateFile = geomTemplate;

      ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
      ro.setUniform("lineWidth", _state.line_width());

      _renderer->addRenderObject(&ro);
    }  


    // -----------------------------------------------------
    // take care of all the other primitives

    ro.depthRange = Vec2f(0.01f, 1.0f);

    switch (props->primitive())
    {
    case DrawModes::PRIMITIVE_POINT:
      {
        if (ro.shaderDesc.shadeMode == SG_SHADE_UNLIT)
        {
          // use specular color for points
          if (_drawMode.isAtomic() )
            ro.emissive = ro.specular;
          else
            ro.emissive = OpenMesh::color_cast<ACG::Vec3f>(_state.overlay_color());
        }

        // use shaders to simulate line width
        QString geomTemplate = ShaderProgGenerator::getShaderDir();
        geomTemplate += "PointSize/geometry.tpl";

        QString fragTemplate = ShaderProgGenerator::getShaderDir();
        fragTemplate += "PointSize/fragment.tpl";

        ro.shaderDesc.geometryTemplateFile = geomTemplate;
        ro.shaderDesc.fragmentTemplateFile = fragTemplate;

        ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
        ro.setUniform("pointSize", _mat->pointSize());

        add_point_RenderObjects(_renderer, &ro);
      } break;
    case DrawModes::PRIMITIVE_EDGE:
      {
        // use specular color for lines
        ro.emissive = ro.specular;

        // use shaders to simulate line width
        QString geomTemplate = ShaderProgGenerator::getShaderDir();
        geomTemplate += "Wireframe/geom_line2quad.tpl";

        ro.shaderDesc.geometryTemplateFile = geomTemplate;

        ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
        ro.setUniform("lineWidth", _state.line_width());

        add_line_RenderObjects(_renderer, &ro);
      } break;
    case DrawModes::PRIMITIVE_POLYGON: add_face_RenderObjects(_renderer, &ro); break;
    default: break;
    }
  }

}


template<class Mesh>
void
MeshNodeT<Mesh>::
add_point_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj) {
  drawMesh_->addPointRenderObjects(_renderer, _baseObj);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_vertices() {
  drawMesh_->drawVertices();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_lines() {

  if ((enabled_arrays_ & PER_EDGE_COLOR_ARRAY) && (enabled_arrays_ & PER_EDGE_VERTEX_ARRAY))
  {
    // colored edges still slow
    glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));
  }
  else
    drawMesh_->drawLines();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
add_line_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj) {

  if ((enabled_arrays_ & PER_EDGE_COLOR_ARRAY) && (enabled_arrays_ & PER_EDGE_VERTEX_ARRAY))
  {
    // colored edges still slow
    glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));
  }
  else
    drawMesh_->addLineRenderObjects(_renderer, _baseObj);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_halfedges() {
  // If we are rendering per edge per vertex attributes, we need to use a seperated vertex buffer!
  if ( enabled_arrays_ & PER_HALFEDGE_VERTEX_ARRAY )
    glDrawArrays(GL_LINES, 0, int(mesh_.n_halfedges() * 2));
  // Something went wrong here!
  else
    std::cerr << "Unable to Draw! halfedge array configuration is invalid!!" << std::endl;
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_faces() {
  drawMesh_->draw(textureMap_);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
add_face_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj) {
  drawMesh_->addTriRenderObjects(_renderer, _baseObj, textureMap_);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
enable_arrays(unsigned int _arrays) {

  // Unbind everything to ensure sane settings
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  //===================================================================
  // per Edge Vertex Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if (_arrays & PER_EDGE_VERTEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_EDGE_VERTEX_ARRAY)) {
      enabled_arrays_ |= PER_EDGE_VERTEX_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::vertexPointer( drawMesh_->perEdgeVertexBuffer() );   
      
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_EDGE_VERTEX_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_EDGE_VERTEX_ARRAY;
    ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  } 
  
  //===================================================================
  // per Edge Color Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if ( mesh_.has_edge_colors()  && ( _arrays & PER_EDGE_COLOR_ARRAY) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_EDGE_COLOR_ARRAY)) {
      enabled_arrays_ |= PER_EDGE_COLOR_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::colorPointer( drawMesh_->perEdgeColorBuffer() );
      
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_EDGE_COLOR_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_EDGE_COLOR_ARRAY;
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);
  }   


  //===================================================================
  // per Halfedge Vertex Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if (_arrays & PER_HALFEDGE_VERTEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_HALFEDGE_VERTEX_ARRAY)) {
      enabled_arrays_ |= PER_HALFEDGE_VERTEX_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::vertexPointer( drawMesh_->perHalfedgeVertexBuffer() );   
      
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_HALFEDGE_VERTEX_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_HALFEDGE_VERTEX_ARRAY;
    ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  } 
  
  //===================================================================
  // per Halfedge Color Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if ( mesh_.has_halfedge_colors()  && ( _arrays & PER_HALFEDGE_COLOR_ARRAY) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_HALFEDGE_COLOR_ARRAY)) {
      enabled_arrays_ |= PER_HALFEDGE_COLOR_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::colorPointer( drawMesh_->perHalfedgeColorBuffer() );
      
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_HALFEDGE_COLOR_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_HALFEDGE_COLOR_ARRAY;
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);
  }   

  //===================================================================
  // Check for OpenGL Errors
  //===================================================================    
  glCheckErrors();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick(GLState& _state, PickTarget _target) {
  switch (_target)
  {
    case PICK_VERTEX:
    {
      pick_vertices(_state);
      break;
    }
    case PICK_FRONT_VERTEX:
    {
      pick_vertices(_state, true);
      break;
    }
    
    case PICK_ANYTHING:
    {
      pick_any(_state);
      break;
    }
    case PICK_FACE:
    {
      pick_faces(_state);
      break;
    }
    
    case PICK_EDGE:
    {
      pick_edges(_state);
      break;
    }
    
    case PICK_FRONT_EDGE:
    {
      pick_edges(_state, true);
      break;
    }
    
    default:
      break;
  }
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_vertices(GLState& _state, bool _front)
{  
  GLenum prev_depth = _state.depthFunc();
  
  typename Mesh::ConstVertexIter v_it(mesh_.vertices_begin()),
  v_end(mesh_.vertices_end());
  
  if (!_state.pick_set_maximum (static_cast<unsigned int>(mesh_.n_vertices()))) {
    omerr() << "MeshNode::pick_vertices: color range too small, " << "picking failed\n";
    return;
  }
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_vertices: No vertices in Mesh!" << std::endl;
    return;
  }
  
  if (_front && ( mesh_.n_faces() != 0 ) ) {
    
    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor(clear_color);
    
    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ACG::GLState::depthFunc(GL_LEQUAL);
    glColor(base_color);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  
  if (_state.color_picking () ) {
    
    if ( updateVertexPicking_ || _state.pick_current_index () != vertexPickingBaseIndex_) {
      drawMesh_->updatePickingVertices(_state);
      vertexPickingBaseIndex_ = _state.pick_current_index ();
      updateVertexPicking_    = false;
    }
    
    if (drawMesh_) {
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::colorPointer( drawMesh_->pickVertexColorBuffer() );   
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);    
      
      // vertex positions
      ACG::GLState::vertexPointer( drawMesh_->pickVertexBuffer() );
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      
      // Draw color picking
      glDrawArrays(GL_POINTS, 0, int(mesh_.n_vertices()));
      
      // Disable color array
      ACG::GLState::disableClientState(GL_COLOR_ARRAY);
      
      // disable vertex array
      ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
      
    } else {
     std::cerr << "pick_vertices: No vertices in Mesh!" << std::endl; 
    }
    
  } else {
    std::cerr << "No fallback pick_vertices!" << std::endl;
  }
  
  ACG::GLState::depthFunc(prev_depth);
  
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_edges(GLState& _state, bool _front)
{  
  GLenum prev_depth = _state.depthFunc();
  
  if (!_state.pick_set_maximum (static_cast<unsigned int>(mesh_.n_edges()))) {
    omerr() << "MeshNode::pick_edges: color range too small, " << "picking failed\n";
    return;
  }
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_edges: No vertices in Mesh!" << std::endl;
    return;
  }
  
  if ( _front && ( mesh_.n_faces() != 0 ) ) {
    
    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor(clear_color);
    
    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ACG::GLState::depthFunc(GL_LEQUAL);
    glColor(base_color);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // disable all other arrays
    enable_arrays(0);
  }
  
  if (_state.color_picking () ) {
    
    if ( updateEdgePicking_ || _state.pick_current_index () != edgePickingBaseIndex_) {
      drawMesh_->updatePickingEdges(_state);
      edgePickingBaseIndex_ = _state.pick_current_index ();
      updateEdgePicking_    = false;
    }
    
    if ( mesh_.n_edges() != 0 && drawMesh_) {
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      
      ACG::GLState::vertexPointer(drawMesh_->perEdgeVertexBuffer());
      ACG::GLState::colorPointer(drawMesh_->pickEdgeColorBuffer());
      
      glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));
      
      ACG::GLState::disableClientState(GL_COLOR_ARRAY);
      ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
      
      // disable all other arrays
      enable_arrays(0); 
      
    }
    
  } else {
    std::cerr << "No fallback pick_edges!" << std::endl;
  }
  
  ACG::GLState::depthFunc(prev_depth);
  
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_faces(GLState& _state)
{
  typename Mesh::ConstFaceIter        f_it(mesh_.faces_sbegin()),
  f_end(mesh_.faces_end());
  typename Mesh::ConstFaceVertexIter  fv_it;
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_faces: No vertices in Mesh!" << std::endl;
    return;
  }
  
  if ( mesh_.n_faces() > 0 ) {
    if (!_state.pick_set_maximum (static_cast<unsigned int>(mesh_.n_faces()))) {
      omerr() << "MeshNode::pick_faces: color range too small, " << "picking failed\n";
      return;
    }
  } else {
    if (!_state.pick_set_maximum (1)) {
      omerr() << "Strange pickSetMAximum failed for index 1 in MeshNode\n";
      return;
    }
  }
  
  if (_state.color_picking ()) {
    
    if ( updateFacePicking_ || _state.pick_current_index () != facePickingBaseIndex_) {
      drawMesh_->updatePickingFaces(_state);
      facePickingBaseIndex_ = _state.pick_current_index ();
      updateFacePicking_    = false;
    }
    
    if ( mesh_.n_faces() != 0 ) {
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      
      ACG::GLState::vertexPointer(drawMesh_->pickFaceVertexBuffer());
      ACG::GLState::colorPointer(drawMesh_->pickFaceColorBuffer());
      
      glDrawArrays(GL_TRIANGLES, 0, int(3 * drawMesh_->getNumTris()));
      
      ACG::GLState::disableClientState(GL_COLOR_ARRAY);
      ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
      
      // disable all other arrays
      enable_arrays(0);
      
    }
    
  } else
    std::cerr << "No fallback pick_faces!" << std::endl;
  
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_any(GLState& _state)
{    
  GLenum prev_depth = _state.depthFunc();
  size_t numElements = mesh_.n_faces() + mesh_.n_edges() + mesh_.n_vertices();
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_any: No vertices in Mesh!" << std::endl;
    return;
  }
  
  // nothing to pick ?
  if (numElements == 0) {
    std::cerr << "pick_any: Number of elements : 0 " << std::endl;
    return;
  }
  
  if (!_state.pick_set_maximum (static_cast<unsigned int>(numElements)))
  {
    omerr() << "MeshNode::pick_any: color range too small, " << "picking failed\n";
    return;
  }
  
  if (_state.color_picking()) {

    if ( updateAnyPicking_ || _state.pick_current_index () != anyPickingBaseIndex_) {
      drawMesh_->updatePickingAny(_state);
      anyPickingBaseIndex_ = _state.pick_current_index ();
      updateAnyPicking_    = false;
    }
    
    // For this version we load the colors directly, not from vbo
    ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);

    ACG::GLState::disableClientState(GL_NORMAL_ARRAY);
    ACG::GLState::disableClientState(GL_TEXTURE_COORD_ARRAY);

    ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
    ACG::GLState::enableClientState(GL_COLOR_ARRAY);
    
    // If we do not have any faces, we generate an empty list here.  
    if ( mesh_.n_faces() != 0 && drawMesh_) {
      
      ACG::GLState::vertexPointer(drawMesh_->pickFaceVertexBuffer());
      ACG::GLState::colorPointer(drawMesh_->pickAnyFaceColorBuffer());
      
      glDrawArrays(GL_TRIANGLES, 0, int(3 * drawMesh_->getNumTris()));
    }
    
    ACG::GLState::depthFunc(GL_LEQUAL);
    
    // If we do not have any edges, we generate an empty list here.  
    if ( mesh_.n_edges() != 0 && drawMesh_) {
      
      ACG::GLState::vertexPointer(drawMesh_->perEdgeVertexBuffer());
      ACG::GLState::colorPointer(drawMesh_->pickAnyEdgeColorBuffer());
    
      glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));
    }
    
    // For this version we load the colors directly not from vbo
    ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
    ACG::GLState::vertexPointer( drawMesh_->pickVertexBuffer() );
    ACG::GLState::colorPointer(drawMesh_->pickAnyVertexColorBuffer());
    
    // Draw color picking
    glDrawArrays(GL_POINTS, 0, int(mesh_.n_vertices()));
    
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);
    ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
    
    // disable all other arrays
    enable_arrays(0);
  } else
    std::cerr << "No fallback pick_any!" << std::endl;

  //restore depth buffer comparison function for the active display list
  ACG::GLState::depthFunc(prev_depth);
  
  glCheckErrors();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
update_geometry() {
  /// \todo check the following statements. If only geometry changed, the normals,vertices have to be updated nothing else!
  /*
  updateFaceList_ = true;
  updateAnyList_ = true;
  */
  
  updateVertexPicking_ = true;
  updateEdgePicking_   = true;
  updateFacePicking_   = true;
  updateAnyPicking_    = true;
  
  // Set per edge arrays to invalid as they have to be regenerated
  drawMesh_->invalidatePerEdgeBuffers();

  // Set per halfedge arrays to invalid as they have to be regenerated
  drawMesh_->invalidatePerHalfedgeBuffers();
  
  drawMesh_->updateGeometry();


  // First of all, we update the bounding box:
  bbMin_ = Vec3d(FLT_MAX,  FLT_MAX,  FLT_MAX);
  bbMax_ = Vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  typename Mesh::ConstVertexIter  v_it(mesh_.vertices_begin()),
  v_end(mesh_.vertices_end());
  
  for (; v_it!=v_end; ++v_it)
  {
    bbMin_.minimize(mesh_.point(*v_it));
    bbMax_.maximize(mesh_.point(*v_it));
  }
}

template<class Mesh>
void
MeshNodeT<Mesh>::
update_topology() {

  drawMesh_->invalidatePerEdgeBuffers();
  drawMesh_->invalidatePerHalfedgeBuffers();


  drawMesh_->updateTopology();

  // Unbind the buffer after the work has been done
  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
update_textures() {
  drawMesh_->updateTextures();

}



template<class Mesh>
void
MeshNodeT<Mesh>::
update_color() {
  
  drawMesh_->invalidatePerEdgeBuffers();
  drawMesh_->invalidatePerHalfedgeBuffers();
  
  // TODO: optimize update strategy:
  // if only vertex colors have changed, then call UpdateGeometry() (faster)
  // for face colors we have to use UpdateFull()
  drawMesh_->updateFull();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
setIndexPropertyName( std::string _indexPropertyName ) { 

  drawMesh_->setTextureIndexPropertyName(_indexPropertyName);
  perFaceTextureIndexAvailable_ =  drawMesh_->perFaceTextureIndexAvailable() != 0;
}

template<class Mesh>
const std::string&
MeshNodeT<Mesh>::
indexPropertyName() const {

  return drawMesh_->getTextureIndexPropertyName();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
setHalfedgeTextcoordPropertyName( std::string _halfedgeTextcoordPropertyName ){ 

  drawMesh_->setPerFaceTextureCoordinatePropertyName(_halfedgeTextcoordPropertyName);
  perFaceTextureCoordsAvailable_ = drawMesh_->perFaceTextureCoordinateAvailable() != 0;
  
}




template<class Mesh>
unsigned int
MeshNodeT<Mesh>::getMemoryUsage()
{
  unsigned int res = 0;

  if (drawMesh_)
    res += drawMesh_->getMemoryUsage();

  return res;
}


template<class Mesh>
DrawMeshT<Mesh>*
MeshNodeT<Mesh>::getDrawMesh()
{
  return drawMesh_;
}


template<class Mesh>
void MeshNodeT<Mesh>::updatePolyEdgeBuf()
{

#ifdef GL_ARB_texture_buffer_object
  MeshCompiler* mc = drawMesh_->getMeshCompiler();
  if (mc && !mc->isTriangleMesh())
  {
    // create/update the poly-edge buffer
    if (!polyEdgeBuf_)
      glGenBuffers(1, &polyEdgeBuf_);

    const int nTris = mc->getNumTriangles();

    const int newBufSize = (nTris/2+1);

    if (polyEdgeBufSize_ != newBufSize)
    {
      glBindBuffer(GL_TEXTURE_BUFFER, polyEdgeBuf_);

      // The poly-edge buffer is a texture buffer that stores one byte for each triangle, which encodes triangle edge properties.
      // An inner edge is an edge that was added during the triangulation of a n-poly, 
      // whereas outer edges are edges that already exist in the input mesh object.
      // This information is used in the wireframe/hiddenline geometry shader to identify edges, which should not be rendered.
      // Buffer storage:
      // each triangle uses 3 bits to mark edges as visible or hidden
      //  outer edge -> bit = 1 (visible)
      //  inner edge -> bit = 0 (hidden)
      // each byte can store edges for two triangles and the remaining 2 bits are left unused

      polyEdgeBufSize_ = newBufSize;
      unsigned char* polyEdgeBufData = new unsigned char[newBufSize];
      
      // set zero
      memset(polyEdgeBufData, 0, newBufSize);

      // build buffer
      for (int i = 0; i < nTris; ++i)
      {
        int byteidx = i>>1;
        int bitidx = (i&1) * 3;

        for (int k = 0; k < 3; ++k)
          if (mc->isFaceEdge(i, k))
            polyEdgeBufData[byteidx] += 1 << (k + bitidx);
      }

      glBufferData(GL_TEXTURE_BUFFER, polyEdgeBufSize_, polyEdgeBufData, GL_STATIC_DRAW);


      delete [] polyEdgeBufData;
      polyEdgeBufData = 0;

      // create texture object for the texture buffer

      if (!polyEdgeBufTex_)
      {
        glGenTextures(1, &polyEdgeBufTex_);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, polyEdgeBufTex_);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R8UI, polyEdgeBuf_);

        glBindTexture(GL_TEXTURE_2D, 0);
      }

      ACG::glCheckErrors();
    }
  }
#endif
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
