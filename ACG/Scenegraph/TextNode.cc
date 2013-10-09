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
//  CLASS TextNode - IMPLEMENTATION
//
//=============================================================================



//== INCLUDES =================================================================

#include "TextNode.hh"
#include "../GL/gl.hh"
#include <cstdlib>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================

// static members
#ifdef WIN32
// fonts in windows are drawn wider
QFont TextNode::qfont_ = QFont("Helvetica", 20);
#else
QFont TextNode::qfont_ = QFont("Helvetica", 30);
#endif
GLuint TextNode::texture_ = 0;
int TextNode::imageWidth_ = 0;
int TextNode::imageHeight_ = 0;
qreal TextNode::maxFontWidth_ = 0.0;
bool TextNode::initialised_ = false;
std::map< char, std::pair<unsigned int, unsigned int> > TextNode::charToIndex_ = TextNode::createMap();
QColor TextNode::color_ = QColor(255, 0, 0);


//----------------------------------------------------------------------------


TextNode::
TextNode( BaseNode*    _parent,
          std::string  _name,
          TextMode     _textMode,
          bool         _alwaysOnTop)
  : BaseNode(_parent, _name),
    size_(1.0),
    pixelSize_(12),
    textMode_(_textMode),
    vbo_(0),
    vertexBuffer_(0),
    blendEnabled_(false),
    texture2dEnabled_(false),
    cullFaceEnabled_(false),
    depthEnabled_(false),
    alwaysOnTop_(_alwaysOnTop),
    alphaTest_(false),
    alphaTestValue_(0.5f),
    alphaTestFunc_(GL_GREATER),
    blendSrc_(0),
    blendDest_(0),
    lastScale_(0.f)

{
  updateFont();
  vertexDecl_.addElement(GL_FLOAT, 3, ACG::VERTEX_USAGE_POSITION);
  vertexDecl_.addElement(GL_FLOAT, 2, ACG::VERTEX_USAGE_TEXCOORD);
  updateVBO();
}



//----------------------------------------------------------------------------


TextNode::
~TextNode()
{
  glDeleteBuffers(1, &vbo_);
}


//----------------------------------------------------------------------------



void
TextNode::
boundingBox(Vec3d& /*_bbMin*/, Vec3d& /*_bbMax*/)
{
}


//----------------------------------------------------------------------------


DrawModes::DrawMode
TextNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS |
           DrawModes::POINTS_SHADED |
           DrawModes::POINTS_COLORED );
}


//----------------------------------------------------------------------------


void
TextNode::
setRenderingMode(TextMode _textMode) {
  textMode_ = _textMode;
}



//----------------------------------------------------------------------------

void
TextNode::
setAlwaysOnTop(bool _alwaysOnTop)
{
  alwaysOnTop_ = _alwaysOnTop;
}


//----------------------------------------------------------------------------

bool
TextNode::
alwaysOnTop()
{
  return alwaysOnTop_;
}


//----------------------------------------------------------------------------


TextNode::TextMode
TextNode::
renderingMode() {
  return textMode_;
}



//----------------------------------------------------------------------------


void
TextNode::
setText(std::string _text) {
  text_ = _text; updateVBO();
}



//----------------------------------------------------------------------------


void
TextNode::
setSize(const double _size) {
  size_ = _size; updateVBO();
}


//----------------------------------------------------------------------------


std::map< char, std::pair<unsigned int, unsigned int> >
TextNode::
createMap() {
  std::map< char, std::pair<unsigned int, unsigned int> > m;
  unsigned char c = ' ';
  for (unsigned int i = 0; i < rows_; ++i) {
    for (unsigned int j = 0; j < columns_; ++j, ++c) {
      m[c] = std::make_pair(j, i);
    }
  }

  return m;
}


//----------------------------------------------------------------------------


void
TextNode::
enter(GLState& _state, const DrawModes::DrawMode& /*_drawmode*/) {
  if (text_.empty())
    return;

  // store current gl state
  cullFaceEnabled_ = glIsEnabled(GL_CULL_FACE);
  texture2dEnabled_ = glIsEnabled(GL_TEXTURE_2D);
  blendEnabled_ = glIsEnabled(GL_BLEND);
  depthEnabled_ = glIsEnabled(GL_DEPTH_TEST);
  alphaTest_ = glIsEnabled(GL_ALPHA_TEST);
  if (alphaTest_)
    ACG::GLState::getAlphaFunc(&alphaTestFunc_, &alphaTestValue_);

  glGetIntegerv(GL_BLEND_SRC, &blendSrc_);
  glGetIntegerv(GL_BLEND_DST, &blendDest_);

  // set texture and drawing states
  ACG::GLState::disable(GL_CULL_FACE);
  ACG::GLState::enable(GL_TEXTURE_2D);
  ACG::GLState::enable(GL_BLEND);
  ACG::GLState::enable(GL_ALPHA_TEST);
  ACG::GLState::alphaFunc(GL_GREATER, 0.2);
  ACG::GLState::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  if (alwaysOnTop_)
    ACG::GLState::disable(GL_DEPTH_TEST);
}



//----------------------------------------------------------------------------


void
TextNode::
leave(GLState& /*_state*/, const DrawModes::DrawMode& /*_drawmode*/) {
  if (text_.empty())
      return;

  // restore the GLState as it was when entering TextNode
  if (cullFaceEnabled_)
    ACG::GLState::enable(GL_CULL_FACE);
  if (!texture2dEnabled_)
    ACG::GLState::disable(GL_TEXTURE_2D);
  if (!blendEnabled_)
    ACG::GLState::disable(GL_BLEND);
  if (depthEnabled_)
    ACG::GLState::enable(GL_DEPTH_TEST);
  else
    ACG::GLState::disable(GL_DEPTH_TEST);
  if (!alphaTest_)
    ACG::GLState::disable(GL_ALPHA_TEST);
  else
    ACG::GLState::alphaFunc(alphaTestFunc_, alphaTestValue_);

  ACG::GLState::blendFunc(blendSrc_, blendDest_);
}



//----------------------------------------------------------------------------


void
TextNode::
draw(GLState& _state, const DrawModes::DrawMode& /*_drawMode*/)
{
  if (!text_.empty()) {
    bindVBO();

    // do not rotate the quads in this case
    if (textMode_ == SCREEN_ALIGNED || textMode_ == SCREEN_ALIGNED_STATIC_SIZE)
      applyScreenAligned(_state);


    _state.push_modelview_matrix();
    _state.scale(size_);
    glDrawArrays(GL_QUADS, 0, int(text_.size() * 4) );
    _state.pop_modelview_matrix();

    if (textMode_ == SCREEN_ALIGNED || textMode_ == SCREEN_ALIGNED_STATIC_SIZE) {
      _state.pop_modelview_matrix();
    }
    unbindVBO();
  }
}


//----------------------------------------------------------------------------


quint32
TextNode::nearestPowerOfTwo(quint32 num) {
  quint32 n = num > 0 ? num - 1 : 0;

  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;

  return n;
}


//----------------------------------------------------------------------------


void
TextNode::setFont(const QFont& _font) {
  qfont_ = QFont(_font);
  initialised_ = false;
  updateFont();
  updateVBO();
}


//----------------------------------------------------------------------------

void
TextNode::
updateFont() {

  // do not generate a new texture for every TextNode unless necessary
  if (initialised_)
    return;

  // since metric.maxWidth() returns 0 for Mac we calculate it here
  QFontMetricsF metric(qfont_);
  for (char c = ' '; c < '~'; ++c) {
    qreal width = metric.width(c) + abs(metric.leftBearing(c)) + abs(metric.rightBearing(c));
    if (width > maxFontWidth_)
      maxFontWidth_ = width;
  }

  qreal height = metric.height();
  // ensure that the height of the texture is a power of 2
  int heightPow2 = nearestPowerOfTwo(height);
  // ensure that the width of the texture is a power of 2
  int widthPow2 = nearestPowerOfTwo(maxFontWidth_);
  imageWidth_ = widthPow2 * columns_;
  imageHeight_ = heightPow2 * rows_;

  QImage finalImage(imageWidth_, imageHeight_, QImage::Format_ARGB32);
  finalImage.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&finalImage);
  painter.setRenderHints(QPainter::HighQualityAntialiasing
                         | QPainter::TextAntialiasing);
  painter.setFont(qfont_);
  painter.setPen(color_);

  // characters are drawn aligned to the left into the QImage finalImage
  for (char c = ' '; c < '~'; ++c) {
    std::pair<unsigned int, unsigned int> coords = charToIndex_[c];
    painter.drawText(coords.first*widthPow2, imageHeight_ - (coords.second+1)*heightPow2, widthPow2, heightPow2, Qt::AlignLeft | Qt::AlignBottom, QString(c));
  }
  painter.end();

  // convert finalImage to an OpenGL friendly format
  finalImage = QGLWidget::convertToGLFormat(finalImage);

  // generate a new texture from finalImage
  if (!texture_)
    glGenTextures(1, &texture_);

  ACG::GLState::bindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, finalImage.width(), finalImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, finalImage.bits());
  glGenerateMipmap(GL_TEXTURE_2D);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);

  initialised_ = true;
}


//----------------------------------------------------------------------------


void
TextNode::
updateVBO() {
  if (text_.size() == 0)
    return;

  vertexBuffer_.clear();

  // generate a quad for each character next to each other
  // *--*--*----*-*
  // |  |  |    | |
  // |  |  |    | |
  // *--*--*----*-*
  QFontMetricsF metric(qfont_);
  qreal avgWidth = metric.averageCharWidth();
  int height = nearestPowerOfTwo(metric.height());
  float lastCharRight = 0.0f;
  for (unsigned int i = 0; i < text_.size(); ++i) {

    // left and right vertex coordinates
    float width = metric.width(text_[i]) / maxFontWidth_;
    float left, right;

    if (i == 0)
      left = 0.0f;
    else
      left = lastCharRight;

    right = (left + width);
    lastCharRight = right;

    // left and right texture coordinates
    qreal leftBearing = abs(metric.leftBearing(text_[i]));
    qreal rightBearing = abs(metric.rightBearing(text_[i]));
    qreal metricWidth = metric.width(text_[i]);

#ifdef WIN32
	metricWidth += leftBearing + rightBearing;
#elif QT_VERSION < 0x050000
    // QFontMetrics does not seem to always give the correct width
    // therefore we add a margin so that characters are not cut off
    if (leftBearing + rightBearing < 0.1*maxFontWidth_) {
      if (metricWidth + 0.25*maxFontWidth_ < maxFontWidth_)
        metricWidth += 0.25*maxFontWidth_;
      else
        metricWidth = maxFontWidth_;
    } else {
      metricWidth += leftBearing + rightBearing;
    }
#endif

    float widthTx = (float) metricWidth / (float) imageWidth_;
    float heightTx = (float) height/ (float) imageHeight_;
    // get the starting position of the character in the texture
    // note that the characters are drawn aligned to the bottom left in in the texture
    float leftTx = ((float) charToIndex_[text_[i]].first ) / (float) columns_;
    float rightTx = leftTx + widthTx;
    float bottomTx = charToIndex_[text_[i]].second / (float) rows_;
    float topTx = bottomTx + heightTx;

    // bottom left
    vertexBuffer_.push_back(left);
    vertexBuffer_.push_back(0.0f);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(leftTx);
    vertexBuffer_.push_back(bottomTx);

    // top left
    vertexBuffer_.push_back(left);
    vertexBuffer_.push_back(avgWidth*0.15);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(leftTx);
    vertexBuffer_.push_back(topTx);

    // top right
    vertexBuffer_.push_back(right);
    vertexBuffer_.push_back(avgWidth*0.15);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(rightTx);
    vertexBuffer_.push_back(topTx);

    // bottom right
    vertexBuffer_.push_back(right);
    vertexBuffer_.push_back(0.0f);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(rightTx);
    vertexBuffer_.push_back(bottomTx);
  }

  if (!vbo_)
    glGenBuffers(1, &vbo_);

  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertexBuffer_.size() * sizeof(GLfloat), &vertexBuffer_[0], GL_DYNAMIC_DRAW);
}


//----------------------------------------------------------------------------


void
TextNode::
bindVBO() {
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER, vbo_);
  ACG::GLState::vertexPointer(3, GL_FLOAT, 5*sizeof(GLfloat), 0);
  ACG::GLState::enableClientState(GL_VERTEX_ARRAY);

  ACG::GLState::activeTexture(GL_TEXTURE0);
  ACG::GLState::texcoordPointer(2, GL_FLOAT, 5*sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
  ACG::GLState::enableClientState(GL_TEXTURE_COORD_ARRAY);

  ACG::GLState::bindTexture(GL_TEXTURE_2D, texture_);
}


//----------------------------------------------------------------------------


void
TextNode::
unbindVBO() {
  ACG::GLState::bindTexture(GL_TEXTURE_2D, 0);
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER, 0);
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  ACG::GLState::disableClientState(GL_TEXTURE_COORD_ARRAY);
}

//----------------------------------------------------------------------------

void
TextNode::
getRenderObjects(ACG::IRenderer* _renderer, ACG::GLState&  _state , const ACG::SceneGraph::DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat)
{
  // init base render object
  ACG::RenderObject ro;

  ro.initFromState(&_state);

  ro.debugName = (std::string("TextNode: ")+name()).c_str();

  // do not rotate the quads in this case
  if (textMode_ == SCREEN_ALIGNED || textMode_ == SCREEN_ALIGNED_STATIC_SIZE)
    applyScreenAligned(_state);

  _state.push_modelview_matrix();
  _state.scale(size_);
  ro.modelview = _state.modelview();
  _state.pop_modelview_matrix();

  if (textMode_ == SCREEN_ALIGNED || textMode_ == SCREEN_ALIGNED_STATIC_SIZE)
  {
    _state.pop_modelview_matrix();
  }

  ro.culling = false;
  ro.blending = true;
  ro.alpha = 0.f;

  ro.blendSrc = GL_SRC_ALPHA;
  ro.blendDest = GL_ONE_MINUS_SRC_ALPHA;

  if (alwaysOnTop_)
    ro.priority = 1;//draw after scene meshes

  // Set the buffers for rendering
  ro.vertexBuffer = vbo_;
  ro.vertexDecl   = &vertexDecl_;

  // Set Texture
  RenderObject::Texture texture;
  texture.id = texture_;
  texture.type = GL_TEXTURE_2D;
  texture.shadow = false;
  ro.addTexture(texture);

  // Set shading
  ro.shaderDesc.vertexColors = false;
  ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

  ACG::SceneGraph::Material localMaterial;

  localMaterial.baseColor(ACG::Vec4f(0.0, 0.0, 0.0, 0.0 ));
  localMaterial.ambientColor(ACG::Vec4f(0.0, 0.0, 0.0, 0.0 ));
  localMaterial.diffuseColor(ACG::Vec4f(0.0, 0.0, 0.0, 0.0 ));
  localMaterial.specularColor(ACG::Vec4f(0.0, 0.0, 0.0, 0.0 ));
  ro.setMaterial(&localMaterial);

  ro.glDrawArrays(GL_QUADS, 0, static_cast<GLsizei>(text_.size()) * 4);
  _renderer->addRenderObject(&ro);
}

//----------------------------------------------------------------------------
void TextNode::applyScreenAligned(GLState &_state)
{
  _state.push_modelview_matrix();

  // try to get the scale factor from the parent TransformNode if it exists
  BaseNode* pParent = parent();
  double scale = 1.0;
  while (pParent) {
    TransformNode* pTrans = dynamic_cast<TransformNode*>(pParent);
    if (pTrans) {
      scale = pTrans->scale()(0,0);
      break;
    }
    pParent = pParent->parent();
  }

  // get the translation
  Vec3d projected = _state.project(Vec3d(0.0, 0.0, 0.0));
  _state.reset_modelview();
  Vec3d unprojected = _state.unproject(projected);

  _state.translate(unprojected);

  if (textMode_ == SCREEN_ALIGNED_STATIC_SIZE)
  {
    ACG::Vec3d nullProj = _state.project(Vec3d(0.0,0.0,0.0));
    ACG::Vec3d nullUnproj = _state.unproject(nullProj);
    ACG::Vec3d heightUnproj = _state.unproject(nullProj+ACG::Vec3d(0.0,pixelSize_,0.0));
    scale *= heightUnproj.length();
    lastScale_ = scale;
  }

  _state.scale(scale);
}
//----------------------------------------------------------------------------
void TextNode::setPixelSize(const unsigned int _size)
{
  pixelSize_ = _size;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
