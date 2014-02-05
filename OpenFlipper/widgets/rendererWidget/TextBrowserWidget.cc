
/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 17080 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2013-07-19 12:58:31 +0200 (Fri, 19 Jul 2013) $                     *
*                                                                            *
\*===========================================================================*/


#if QT_VERSION >= 0x050000
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include "TextBrowserWidget.hh"

#include <iostream>

QString const TextBrowserWidget::startRenderObjectTag_ = "name:";
QString const TextBrowserWidget::startVertexShaderTag_ = "--vertex-shader--";
QString const TextBrowserWidget::endVertexShaderTag_ = "--end-vertex-shader--";
QString const TextBrowserWidget::startGeometryShaderTag_ = "--geometry-shader--";
QString const TextBrowserWidget::endGeometryShaderTag_ = "--end-geometry-shader--";
QString const TextBrowserWidget::startFragmentShaderTag_ = "--fragment-shader--";
QString const TextBrowserWidget::endFragmentShaderTag_ = "--end-fragment-shader--";


TextBrowserWidget::TextBrowserWidget(QWidget *parent) : QPlainTextEdit(parent) {
  sideArea_ = new TextBrowserSideArea(this);
  updateTextBrowserSideAreaWidth();

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateTextBrowserSideAreaWidth()));
  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateFolds()));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateTextBrowserSideArea(QRect,int)));

  setReadOnly(true);
}



int TextBrowserWidget::sideAreaWidth() {
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return space;
}



void TextBrowserWidget::updateTextBrowserSideAreaWidth() {
  setViewportMargins(sideAreaWidth(), 0, 0, 0);
}

void TextBrowserWidget::updateTextBrowserSideArea(const QRect &rect, int dy) {
  if (dy)
    sideArea_->scroll(0, dy);
  else
    sideArea_->update(0, rect.y(), sideArea_->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateTextBrowserSideAreaWidth();
}



void TextBrowserWidget::resizeEvent(QResizeEvent *e) {
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  sideArea_->setGeometry(QRect(cr.left(), cr.top(), sideAreaWidth(), cr.height()));
}

void TextBrowserWidget::sideAreaPaintEvent(QPaintEvent *event) {

  QPainter painter(sideArea_);
  painter.fillRect(event->rect(), Qt::lightGray);
  painter.setPen(Qt::black);

  QTextBlock block = firstVisibleBlock();

  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  Fold found_fold;
  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      if (getFold(block.position(), found_fold)) {
        if (found_fold.type == SHADER) {
          int fold_first_block = document()->findBlock(found_fold.start).blockNumber();
          QString text = block.text();
          // only draw line numbers on actual shader code
          if (text.contains(TextBrowserWidget::startVertexShaderTag_) ||
              text.contains(TextBrowserWidget::endVertexShaderTag_) ||
              text.contains(TextBrowserWidget::startGeometryShaderTag_) ||
              text.contains(TextBrowserWidget::endGeometryShaderTag_) ||
              text.contains(TextBrowserWidget::startFragmentShaderTag_) ||
              text.contains(TextBrowserWidget::endFragmentShaderTag_)) {
            if (found_fold.folded)
              painter.drawText(0, top, sideArea_->width(), fontMetrics().height(),Qt::AlignRight, "+");
            else
              painter.drawText(0, top, sideArea_->width(), fontMetrics().height(),Qt::AlignRight, "-");
          } else {
            QString number = QString::number(blockNumber - fold_first_block);
            painter.drawText(0, top, sideArea_->width(), fontMetrics().height(),Qt::AlignRight, number);
          }
        } else {
          if (found_fold.folded)
            painter.drawText(0, top, sideArea_->width(), fontMetrics().height(),Qt::AlignRight, "+");
          else
            painter.drawText(0, top, sideArea_->width(), fontMetrics().height(),Qt::AlignRight, "-");
        }
      } else
        painter.drawText(0, top, sideArea_->width(), fontMetrics().height(),Qt::AlignRight, " ");
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}

bool TextBrowserWidget::getFold(int _position, Fold& _fold) {
  std::map<int,size_t>::iterator it = blockPosToFold_.find(_position);
  if (it != blockPosToFold_.end()) {
    _fold = folds_[it->second];
    return true;
  } else
    return false;
}

void TextBrowserWidget::mouseDoubleClickEvent(QMouseEvent* e) {
  QTextBlock block = firstVisibleBlock();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();
  const int y = e->y();
  // find the block that was clicked and toggle the folding
  while (block.isValid()) {
    if (top <= y && y <= bottom) {
      toggleFold(block.position());
      break;
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
  }
}

void TextBrowserWidget::foldAll() {
  for (std::vector<Fold>::iterator it = folds_.begin(); it != folds_.end(); ++it) {
    fold(*it);
  }
}

void TextBrowserWidget::unfoldAll() {
  for (std::vector<Fold>::iterator it = folds_.begin(); it != folds_.end(); ++it) {
    unfold(*it);
  }
}

void TextBrowserWidget::fold(Fold& _fold) {
  if (_fold.folded)
    return;

  QTextBlock startBlock = document()->findBlock(_fold.start);
  QTextBlock endBlock = document()->findBlock(_fold.end);

  startBlock = startBlock.next();
  while (startBlock.isValid() && startBlock != endBlock) {
    startBlock.setVisible(false);
    startBlock = startBlock.next();
  }
  if (_fold.type == RENDEROBJECT)
    endBlock.setVisible(false);

  _fold.folded = true;
  document()->markContentsDirty(_fold.start, _fold.end - _fold.start);
}

void TextBrowserWidget::unfold(Fold& _fold) {
  if (!_fold.folded)
    return;

  QTextBlock startBlock = document()->findBlock(_fold.start);
  QTextBlock endBlock = document()->findBlock(_fold.end);

  startBlock = startBlock.next();
  while (startBlock.isValid() && startBlock != endBlock) {
    startBlock.setVisible(true);
    startBlock = startBlock.next();
  }
  if (_fold.type == RENDEROBJECT)
    endBlock.setVisible(true);

  _fold.folded = false;
  document()->markContentsDirty(_fold.start, _fold.end-_fold.start);
}

void TextBrowserWidget::toggleFold(int _position) {
  for (std::vector<Fold>::iterator it = folds_.begin(); it != folds_.end(); ++it) {
    if (it->contains(_position)) {
      if (it->folded)
        unfold(*it);
      else
        fold(*it);

      break;
    }
  }
}

void TextBrowserWidget::updateFolds() {
  folds_.clear();

  // search for all vertex shader
  QTextCursor startCursor = document()->find(TextBrowserWidget::startVertexShaderTag_, 0, QTextDocument::FindWholeWords);
  QTextCursor endCursor = document()->find(TextBrowserWidget::endVertexShaderTag_, 0, QTextDocument::FindWholeWords);

  while (!startCursor.isNull() && !endCursor.isNull()) {
    startCursor.movePosition(QTextCursor::StartOfLine);
    endCursor.movePosition(QTextCursor::EndOfLine);
    folds_.push_back(Fold(startCursor.position(),endCursor.position(),SHADER));

    // map block position to fold
    int startPos = startCursor.position();
    const int endPos = endCursor.position();
    for (; startPos < endPos; ++startPos) {
      QTextBlock block = document()->findBlock(startPos);
      blockPosToFold_[block.position()] = folds_.size() - 1;
    }

    bool moved = startCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;
    moved = endCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;

    startCursor = document()->find(TextBrowserWidget::startVertexShaderTag_, startCursor, QTextDocument::FindWholeWords);
    endCursor = document()->find(TextBrowserWidget::endVertexShaderTag_, endCursor, QTextDocument::FindWholeWords);
  }

  // search for all geometry shader
  startCursor = document()->find(TextBrowserWidget::startGeometryShaderTag_, 0, QTextDocument::FindWholeWords);
  endCursor = document()->find(TextBrowserWidget::endGeometryShaderTag_, 0, QTextDocument::FindWholeWords);

  while (!startCursor.isNull() && !endCursor.isNull()) {
    startCursor.movePosition(QTextCursor::StartOfLine);
    endCursor.movePosition(QTextCursor::EndOfLine);
    folds_.push_back(Fold(startCursor.position(),endCursor.position(),SHADER));

    // map block position to fold
    int startPos = startCursor.position();
    const int endPos = endCursor.position();
    for (; startPos < endPos; ++startPos) {
      QTextBlock block = document()->findBlock(startPos);
      blockPosToFold_[block.position()] = folds_.size() - 1;
    }

    bool moved = startCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;
    moved = endCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;

    startCursor = document()->find(TextBrowserWidget::startGeometryShaderTag_, startCursor, QTextDocument::FindWholeWords);
    endCursor = document()->find(TextBrowserWidget::endGeometryShaderTag_, endCursor, QTextDocument::FindWholeWords);
  }

  // search for all fragment shader
  startCursor = document()->find(TextBrowserWidget::startFragmentShaderTag_, 0, QTextDocument::FindWholeWords);
  endCursor = document()->find(TextBrowserWidget::endFragmentShaderTag_, 0, QTextDocument::FindWholeWords);

  while (!startCursor.isNull() && !endCursor.isNull()) {
    startCursor.movePosition(QTextCursor::StartOfLine);
    endCursor.movePosition(QTextCursor::EndOfLine);
    folds_.push_back(Fold(startCursor.position(),endCursor.position(),SHADER));

    // map block position to fold
    int startPos = startCursor.position();
    const int endPos = endCursor.position();
    for (; startPos < endPos; ++startPos) {
      QTextBlock block = document()->findBlock(startPos);
      blockPosToFold_[block.position()] = folds_.size() - 1;
    }

    bool moved = startCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;
    moved = endCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;

    startCursor = document()->find(TextBrowserWidget::startFragmentShaderTag_, startCursor, QTextDocument::FindWholeWords);
    endCursor = document()->find(TextBrowserWidget::endFragmentShaderTag_, endCursor, QTextDocument::FindWholeWords);
  }

  // search for all render objects
  startCursor = document()->find(TextBrowserWidget::startRenderObjectTag_, 0, QTextDocument::FindWholeWords);
  endCursor = document()->find(TextBrowserWidget::startVertexShaderTag_, 0, QTextDocument::FindWholeWords);

  while (!startCursor.isNull() && !endCursor.isNull()) {
    startCursor.movePosition(QTextCursor::StartOfLine);
    // vertex shader does not belong to this fold
    endCursor.movePosition(QTextCursor::Up);
    endCursor.movePosition(QTextCursor::EndOfLine);
    folds_.push_back(Fold(startCursor.position(),endCursor.position(),RENDEROBJECT));

    // map block position to fold
    int startPos = startCursor.position();
    const int endPos = endCursor.position();
    for (; startPos < endPos; ++startPos) {
      QTextBlock block = document()->findBlock(startPos);
      blockPosToFold_[block.position()] = folds_.size() - 1;
    }

    bool moved = startCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;
    // skip to after the vertex shader starts
    moved = endCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;
    moved = endCursor.movePosition(QTextCursor::Down);
    if (!moved)
      break;

    startCursor = document()->find(TextBrowserWidget::startRenderObjectTag_, startCursor, QTextDocument::FindWholeWords);
    endCursor = document()->find(TextBrowserWidget::startVertexShaderTag_, endCursor, QTextDocument::FindWholeWords);
  }

  // fold shader blocks
  foldAll();
}
