/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                      *
 *   $Author$                                                          *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS PointNode
//
//=============================================================================

#ifndef ACG_TEXTNODE_HH
#define ACG_TEXTNODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "TransformNode.hh"
#include "DrawModes.hh"
#include <ACG/GL/IRenderer.hh>
#include <ACG/GL/VertexDeclaration.hh>
#include <vector>
#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPainter>
#include <QGLWidget>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================



/** \class TextNode TextNode.hh <ACG/Scenegraph/TextNode.hh>
 *  TextNode can be used to display a string on quads in OpenGL. This string can be set with
 * 	setText(std::string _text). A TextNode can be attached to a parent node by using the function
 * 	BaseObjectData::addAdditionalNode. The quads can then be set to align to the parent (OBJECT_ALIGNED)
 *	via the setRenderingMode(TextMode _textMode) function. Alternatively the quads can be aligned
 *	to the screen (SCREEN_ALIGNED). The font that is used to display text on the screen
 *  can be set with the setFont(const QFont& _font) function. Finally the quads can be scaled
 * 	via the setSize(unsigned int _size) function.
**/

class ACGDLLEXPORT TextNode : public BaseNode
{
public:

  enum TextMode {
    SCREEN_ALIGNED, /// Text will always stay parallel to screen
    OBJECT_ALIGNED  /// Text will be transformed and projected by Modelview and projection matrix
  };

  /** default constructor
   * @param _parent Define the parent Node this node gets attached to
   * @param _name Name of this Node
   * @param _textMode Define the text rendering style ( see TextNode::TextMode )
   * @param _alwaysOnTop Draw text on top of everything else?
   */
  TextNode( BaseNode*     _parent=0,
            std::string  _name="<TextNode>",
            TextMode     _textMode = SCREEN_ALIGNED,
            bool         _alwaysOnTop = false);

  /// destructor
  ~TextNode();

  /// static name of this class
  ACG_CLASSNAME(TextNode);

  /// return available draw modes
  DrawModes::DrawMode  availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw Text
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// set texture and drawing states
  void enter(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// restore texture and drawing states
  void leave(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// set RenderObject for Shader pipeline renderer
  void getRenderObjects(ACG::IRenderer* _renderer, ACG::GLState&  _state , const ACG::SceneGraph::DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat);

  /** Set the rendering mode ( see TextNode::TextMode )
   */
  void setRenderingMode(TextMode _textMode);

  /// draw the text always on top
  void setAlwaysOnTop(bool _alwaysOnTop);

  /// returns wheter always on top is setted or not
  bool alwaysOnTop();

  /// returns the rendering mode (SCREEN_ALIGNED or OBJECT_ALIGNED)
  TextMode renderingMode();

  /// sets the string that will be rendered
  void setText(std::string _text);

  /// sets the size by which the quads displaying the text will be scaled
  void setSize(unsigned int _size);

  /// sets the font to be used for generating a texture with most characters of the chosen font
  void setFont(const QFont& _font);

protected:
  /**
   * Generates a texture by drawing most characters into one texture.
   * This texture is then used to draw single characters onto quads
   */
  static void updateFont();

private:
  /// returns the nearest greater power of 2 to num
  static quint32 nearestPowerOfTwo(quint32 num);

  /**
   * generates a quad for each character in #text_ and also
   * calculates the texture coordinates for each quad's character
   */
  void updateVBO();

  /// binds #vbo_ and sets the necessary OpenGL states
  void bindVBO();

  /// unbinds #vbo_
  void unbindVBO();

  /**
   * Creates a map #charToIndex_ from most characters to an incrementing set of indices.
   * These indices are used to create the texture coordinates in updateVBO().
   */
  static std::map< char, std::pair<unsigned int, unsigned int> > createMap();

private:

  /// scaling factor by which the quads in #vbo_ are scaled
  unsigned int size_;

  /// text to be displayed on quads in #vbo_
  std::string text_;

  /// current display mode of #text_ (SCREEN_ALIGNED or OBJECT_ALIGNED)
  TextMode textMode_;

  /**
   * handle to the vertex buffer object, containing the quads on which the characters in
   * #text_ are rendered
   */
  GLuint vbo_;

  /// buffer of vertex coordinates and texture coordinates of the quads
  std::vector<GLfloat> vertexBuffer_;

  /// stores if GL_BLEND was enabled on entering TextNode
  bool blendEnabled_;

  /// stores if GL_TEXTURE_2D was enabled on entering TextNode
  bool texture2dEnabled_;

  /// stores if GL_CULL_FACE was enabled on entering TextNode
  bool cullFaceEnabled_;

  /// stores if GL_DEPTH_TEST was enabled on entering TextNode
  bool depthEnabled_;

  /// stores if text should be drawn always on top
  bool alwaysOnTop_;

  /// stores the sfactor parameter of glBlendFunc on entering TextNode
  GLint blendSrc_;

  /// stores the dfactor parameter of glBlendFunc on entering TextNode
  GLint blendDest_;

  /// stores the vertex declaration
  ACG::VertexDeclaration vertexDecl_;

  /// maps most readable characters to indices for texture coordinate calculation in updateVBO()
  static std::map< char, std::pair<unsigned int, unsigned int> > charToIndex_;

  /// font that is used to generate the texture in updateFont()
  static QFont qfont_;

  /// handle for the texture into which characters from #qfont_ are painted in updateFont()
  static GLuint texture_;

  /// width of the generated texture
  static int imageWidth_;

  /// height of the generated texture
  static int imageHeight_;

  /// max width of qfont
  static qreal maxFontWidth_;

  /// number of characters that are drawn into the texture
  static const int numberOfChars_ = 94;

  /// number of rows of characters in the texture
  static const unsigned int rows_ = 10;

  /// number of columns of characters in the texture
  static const unsigned int columns_ = 10;

  /// this is used to ensure that the texture is only generated once when necessary
  static bool initialised_;

  /// color that is used to draw the characters into the texture in updateFont()
  static QColor color_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TEXTNODE_HH defined
//=============================================================================
