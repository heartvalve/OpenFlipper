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
#include <stdlib.h>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================

// static members
QFont TextNode::qfont_ = QFont("Helvetica", 20);
GLuint TextNode::texture_ = 0;
int TextNode::imageWidth_ = 0;
bool TextNode::initialised_ = false;
std::map<char, unsigned int> TextNode::charToIndex_ = TextNode::createMap();
QColor TextNode::color_ = QColor(255, 0, 0);

TextNode::
~TextNode()
{
  glDeleteBuffers(1, &vbo_);
}

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


std::map<char, unsigned int>
TextNode::
createMap() {
  std::map<char, unsigned int> m;
  unsigned int i = 0;
  for (char c = ' '; c < '~'; ++c, ++i) {
    m[c] = i;
  }

  return m;
}


//----------------------------------------------------------------------------


void
TextNode::
enter(GLState& /*_state*/, const DrawModes::DrawMode& /*_drawmode*/) {
  // store current gl state
  cullFaceEnabled_ = glIsEnabled(GL_CULL_FACE);
  texture2dEnabled_ = glIsEnabled(GL_TEXTURE_2D);
  blendEnabled_ = glIsEnabled(GL_BLEND);
  glGetIntegerv(GL_BLEND_SRC, &blendSrc_);
  glGetIntegerv(GL_BLEND_DST, &blendDest_);

  // set texture and drawing states
  ACG::GLState::disable(GL_CULL_FACE);
  ACG::GLState::enable(GL_TEXTURE_2D);
  ACG::GLState::enable(GL_BLEND);
  ACG::GLState::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



//----------------------------------------------------------------------------


void
TextNode::
leave(GLState& /*_state*/, const DrawModes::DrawMode& /*_drawmode*/) {
  if (cullFaceEnabled_)
    ACG::GLState::enable(GL_CULL_FACE);
  if (!texture2dEnabled_)
    ACG::GLState::disable(GL_TEXTURE_2D);
  if (!blendEnabled_)
    ACG::GLState::disable(GL_BLEND);

  ACG::GLState::blendFunc(blendSrc_, blendDest_);
}



//----------------------------------------------------------------------------


void
TextNode::
draw(GLState& _state, const DrawModes::DrawMode& /*_drawMode*/)
{
  if (!text_.empty()) {
    bindVBO();
    if (textMode_ == SCREEN_ALIGNED) {
      _state.push_modelview_matrix();
      Vec3d projected = _state.project(Vec3d(0.0, 0.0, 0.0));
      _state.reset_modelview();
      Vec3d unprojected = _state.unproject(projected);
      _state.translate(unprojected);
    }

    _state.push_modelview_matrix();
    _state.scale(size_);
    glDrawArrays(GL_QUADS, 0, text_.size() * 4);
    _state.pop_modelview_matrix();

    if (textMode_ == SCREEN_ALIGNED) {
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
  UpdateFont();
  updateVBO();
}


//----------------------------------------------------------------------------

void
TextNode::
UpdateFont() {

  if (initialised_)
    return;

  QFontMetrics metric(qfont_);

  int height = metric.height();
  int heightPow2 = nearestPowerOfTwo(height);
  int width = metric.maxWidth();
  int widthPow2 = nearestPowerOfTwo(width);
  imageWidth_ = widthPow2 * numberOfChars_;

  QImage finalImage(imageWidth_, heightPow2, QImage::Format_ARGB32);
  finalImage.fill(Qt::transparent);
  QPainter painter;
  painter.begin(&finalImage);
  painter.setRenderHints(QPainter::HighQualityAntialiasing
                         | QPainter::TextAntialiasing);
  painter.setFont(qfont_);
  painter.setPen(color_);
  unsigned int i = 0;
  for (char c = ' '; c < '~'; ++c, ++i) {
    painter.drawText(i*widthPow2, 0, widthPow2, heightPow2, Qt::AlignLeft | Qt::AlignBottom, QString(c));
  }
  painter.end();

  finalImage = QGLWidget::convertToGLFormat(finalImage);

  glDeleteTextures(1, &texture_);
  glGenTextures(1, &texture_);
  ACG::GLState::bindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, finalImage.width(), finalImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, finalImage.bits());
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

  // generate a quad for each character
  QFontMetrics metric(qfont_);
  int maxWidth = metric.maxWidth();
  int avgWidth = metric.averageCharWidth();
  float lastCharRight = 0.0f;
  for (unsigned int i = 0; i < text_.size(); ++i) {

    // left and right vertex coordinates
    float width = (float) metric.width(text_[i]) / (float) maxWidth;
    float left, right;

    if (i == 0)
      left = 0.0f;
    else
      left = lastCharRight;

    right = (left + width);
    lastCharRight = right;

    // left and right texture coordinates
    int leftBearing = abs(metric.leftBearing(text_[i]));
    int rightBearing = abs(metric.rightBearing(text_[i]));
    int metricWidth = metric.width(text_[i]);

    // QFontMetrics does not seem to always give the correct width
    // therefore we add a margin so that characters are not cut off
    //if ( (leftBearing == 0) && (rightBearing == 0) ) {
    if (leftBearing + rightBearing < 0.1*maxWidth) {
      if (metricWidth + 0.25*maxWidth < maxWidth)
        metricWidth += 0.25*maxWidth;
      else
        metricWidth = maxWidth;
    } else {
      metricWidth += leftBearing + rightBearing;
    }

    float widthTx = (float) metricWidth / (float) imageWidth_;
    float leftTx = ((float) charToIndex_[text_[i]] ) / (float) numberOfChars_;
    float rightTx = leftTx + widthTx;

    // bottom left
    vertexBuffer_.push_back(left);
    vertexBuffer_.push_back(0.0f);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(leftTx);
    vertexBuffer_.push_back(0.0f);

    // top left
    vertexBuffer_.push_back(left);
    vertexBuffer_.push_back(avgWidth*0.15);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(leftTx);
    vertexBuffer_.push_back(1.0f);

    // top right
    vertexBuffer_.push_back(right);
    vertexBuffer_.push_back(avgWidth*0.15);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(rightTx);
    vertexBuffer_.push_back(1.0f);

    // bottom right
    vertexBuffer_.push_back(right);
    vertexBuffer_.push_back(0.0f);
    vertexBuffer_.push_back(0.0f);

    // texture coordinates
    vertexBuffer_.push_back(rightTx);
    vertexBuffer_.push_back(0.0f);
  }

  glDeleteBuffers(1, &vbo_);
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


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
