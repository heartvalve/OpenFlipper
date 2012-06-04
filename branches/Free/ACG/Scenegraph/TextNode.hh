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
 *   $Revision$                                                       *
 *   $Author$                                                      *
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

    TextNode renders Text.

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
   * @param _fontfile Font File ( should include full path )
   * @param _name Name of this Node
   * @param _textMode Define the text rendering style ( see TextNode::TextMode )
   */
  TextNode( std::string       _fontFile,
            BaseNode*         _parent=0,
	         std::string  _name="<TextNode>",
            TextMode     _textMode = SCREEN_ALIGNED)
            : BaseNode(_parent, _name),
              size_(1),
              fontFile_(_fontFile),
              textMode_(_textMode),
              vbo_(0),
              vertexBuffer_(0),
              blendEnabled_(false),
              texture2dEnabled_(false),
              cullFaceEnabled_(false),
              blendSrc_(0),
              blendDest_(0)
  {
    UpdateFont();
    updateVBO();
  }

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

  /** Set the rendering mode ( see TextNode::TextMode )
   */

  void setRenderingMode(TextMode _textMode) { textMode_ = _textMode; };

  TextMode renderingMode() { return textMode_; };

	/// sets the text that will be rendered
  void setText(std::string _text) {text_ = _text; updateVBO();};

	/// sets the scaling size of the TextNode
  void setSize(unsigned int _size) {size_ = _size; updateVBO();};

	/// sets the font to be used
  void setFont(const QFont& _font);

protected:
  static void UpdateFont();

private:
  static quint32 nearestPowerOfTwo(quint32 num);
  void updateVBO();
  void bindVBO();
  void unbindVBO();
  static std::map<char, unsigned int> createMap();

private:
  unsigned int size_;

  std::string text_;

  std::string fontFile_;

  TextMode textMode_;

  GLuint vbo_;

  std::vector<GLfloat> vertexBuffer_;

  bool blendEnabled_;

  bool texture2dEnabled_;

  bool cullFaceEnabled_;

  GLint blendSrc_;

  GLint blendDest_;

  static std::map<char, unsigned int> charToIndex_;

  static QFont qfont_;

  static GLuint texture_;

  static int imageWidth_;

  static const int numberOfChars_ = 94;

  static bool initialised_;

  static QColor color_;
};

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TEXTNODE_HH defined
//=============================================================================
