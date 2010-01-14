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
 *   $Revision: 7994 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-12-18 12:18:13 +0100 (Fr, 18. Dez 2009) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS MeshNodeT
//
//=============================================================================

#ifndef ACG_MESHNODE_HH
#define ACG_MESHNODE_HH

//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <vector>
#include <string>
#include "../ShaderUtils/GLSLShader.hh"


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class MeshNodeT MeshNodeT.hh <ACG/Scenegraph/MeshNodeT.hh>

    This node is used to draw an ACG mesh.

    It uses compile-time-tags for recognizing whether
    the mesh is a TriMeshT or a PolyMeshT.

    The available draw modes are collected based on these mesh
    properties like normal or color for faces or vertices.
*/

template <class Mesh>
class MeshNodeT : public BaseNode
{

public:

  /** Default constructor.
   * @param _textureMap if the mesh has a face_texture_index and a map is provided textures are switched based on the given ids
   */
  MeshNodeT(const Mesh&  _mesh,
	    BaseNode*    _parent=0,
	    std::string  _name="<MeshNode>");

  /// Destructor
  virtual ~MeshNodeT();


  ACG_CLASSNAME(MeshNode);


  /** \brief Setup a mapping between internal texture ids on the mesh and the ids for the loaded textures in opengl
   *
   * @param _map maps between an int index stored in the Mesh describing which texture to use for a face,
   *             and the GluInt name of the texture bound by the TextureNode. \n
   *             If such a map is not available ( =0 ), assume TextureNode has already bound a texture
   *             And render without switching textures
   */
  void set_texture_map( std::map< int, GLuint>* _map){ textureMap_ = _map; };

  /** \brief Setup a mapping between internal texture ids on the mesh and the properties containing texture coordinates
   *
   * @param _map maps between an int index stored in the Mesh describing which texture to use
   *             and a property name giving 2D Texture coordinates for halfedges ( texcoords for to vertex )
   */
  void set_property_map( std::map< int, std::string>* _map){ propertyMap_ = _map; };

  /** \brief Set default property name to get texture coordinates for per face texcoords
   *
   * Property has to be Mesh::TexCoord2D as a halfedge property describing vertex coordinates for
   * to Vertex per face. Defaults to h:texcoords2D
   */
  void set_default_halfedge_textcoord_property( std::string _default_halfedge_textcoord_property )
    { default_halfedge_textcoord_property_ = _default_halfedge_textcoord_property; };

  /** \brief Property to use when switching between multiple textures
   *
   * The given mesh property has to contain an int index of textures to use per face.
   * If it is not given, the default face_texture_index property of OpenMesh will be used.
   * If there is no texture map given via set_texture_map, texture switching will be disabled and
   * only the currently bound texture will be used.
   */
  void set_index_property_name( std::string _index_property_name) { indexPropertyName_ = _index_property_name; };

protected:


  // types
  enum FaceMode { FACE_NORMALS, FACE_COLORS, PER_VERTEX, FACE_NORMALS_COLORS, FACE_HALFEDGE_TEXTURED };


  // draw polygons. to be overridden by TriStripNodeT
  virtual void draw_faces(FaceMode _mode);

  // update pick buffer sizes
  void update_pick_buffers ();

  // opengl vertex array stuff
  enum ArrayType
  {
    NONE                    = 0,
    COLOR_ARRAY             = 4,
    TEXTURE_COORD_1D_ARRAY  = 8,
    TEXTURE_COORD_2D_ARRAY  = 16,
    TEXTURE_COORD_3D_ARRAY  = 32
  };

protected:


  // vertex buffer objects
  unsigned int  face_index_buffer_;

  // index list for fast rendering (will be accessed by TriStripNodeT
  std::vector<unsigned int>  indices_;

private:
  
  bool faceIndexBufferInitialized_;



  // Mapping of mesh face texture indices to gltexture id ( has to be provided externally )
  std::map< int, GLuint>* textureMap_;

  // Mapping of mesh face texture indices to coordinate property names ( has to be provided externally )
  std::map< int, std::string>* propertyMap_;

  // Property name of the per face texture coordinates
  std::string default_halfedge_textcoord_property_;

  // Property name of the per face texture index.
  std::string indexPropertyName_;

  // display Lists used for picking
  GLuint faceList_;
  GLuint vertexList_;
  GLuint edgeList_;
  GLuint anyList_;

  // do we need to update our display lists?
  bool updateFaceList_;
  bool updateVertexList_;
  bool updateEdgeList_;
  bool updateAnyList_;
  unsigned int faceBaseIndex_;
  unsigned int vertexBaseIndex_;
  unsigned int edgeBaseIndex_;
  unsigned int anyBaseIndex_;

  // buffers for draw arrays during picking
  std::vector< ACG::Vec3f > pickVertexBuf_;
  std::vector< ACG::Vec4uc > pickColorBuf_;

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_MESHNODE_C)
#define ACG_MESHNODE_TEMPLATES
#include "MeshNodeT.cc"
#endif
//=============================================================================
#endif // ACG_MESHNODE_HH defined
//=============================================================================
