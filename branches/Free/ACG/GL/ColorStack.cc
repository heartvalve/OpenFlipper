//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fr, 09. Mai 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS ColorStack - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "ColorStack.hh"
#include <iostream>


//== NAMESPACES ===============================================================


namespace ACG {


//== IMPLEMENTATION ========================================================== 

ColorStack::ColorStack () :
  initialized_(false),
  root_ (0),
  currentNode_ (0),
  error_(false)
{
}

//----------------------------------------------------------------------------

ColorStack::~ColorStack ()
{
  if (root_)
    delete root_;
}

//----------------------------------------------------------------------------

void ColorStack::initialize ()
{
  if (initialized_)
  {
    delete root_;
  }
  error_ = false;
  translator_.initialize ();
  root_ = currentNode_ = new ColorStack::Node (0, 0, &translator_);
  initialized_ = true;
}

//----------------------------------------------------------------------------

bool ColorStack::setMaximumIndex (unsigned int _idx)
{
  if (initialized_)
  {
    bool rv = currentNode_->setMaximumIndex (_idx);
    if (!rv)
      error_ = true;
    return rv;
  }
  return false;
}

//----------------------------------------------------------------------------

void ColorStack::setIndex (unsigned int _idx)
{
  if (initialized_)
  {
    if (!currentNode_->setIndex (_idx))
      error_ = true;
  }
}

//----------------------------------------------------------------------------

Vec4uc ColorStack::getIndexColor (unsigned int _idx)
{
  if (initialized_)
  {
    Vec4uc rv;
    if (!currentNode_->getIndexColor (_idx, rv))
      error_ = true;
    else
      return rv;
  }
  
  return Vec4uc (0, 0, 0, 0);
}

//----------------------------------------------------------------------------

void ColorStack::pushIndex (unsigned int _idx)
{
  if (initialized_)
    currentNode_ = currentNode_->pushIndex (_idx);
}

//----------------------------------------------------------------------------

void ColorStack::popIndex ()
{
  if (initialized_)
    currentNode_ = currentNode_->popIndex ();
}

//----------------------------------------------------------------------------

std::vector<unsigned int> ColorStack::colorToStack (Vec4uc _rgba) const
{
  std::vector<unsigned int> rv(0);
  if (initialized_ && !error_)
  {
    unsigned int idx = translator_.color2index (_rgba);
    if (idx >= root_->startIndex () && idx < root_->endIndex ())
      root_->colorToStack (rv, idx);
  }
  return rv;
}

//----------------------------------------------------------------------------

unsigned int ColorStack::freeIndicies() const
{
  if (initialized_)
  {
    return translator_.max_index () - currentNode_->endIndex ();
  }
  else
    return 0; 
}

//----------------------------------------------------------------------------

unsigned int ColorStack::currentIndex () const
{
  if (initialized_)
  {
    return currentNode_->colorIndex ();
  }
  return 0;
}


//----------------------------------------------------------------------------

ColorStack::Node::Node (unsigned int _idx, Node *_parent, ColorTranslator *_ct) :
  parent_(_parent),
  index_(_idx),
  translator_(_ct),
  colorStartIdx_(0),
  colorEndIdx_(0)
{
  if (parent_)
    startIdx_ = endIdx_ = parent_->endIndex ();
  else
    startIdx_ = endIdx_ = 1;
}

//----------------------------------------------------------------------------

ColorStack::Node::~Node ()
{
  for (std::vector<Node *>::iterator it = nodes_.begin (); it != nodes_.end(); ++it)
    delete (*it);
}

//----------------------------------------------------------------------------

bool ColorStack::Node::setMaximumIndex (unsigned int _idx)
{
  if (_idx == 0)
    _idx = 1;

  if (colorStartIdx_ == 0 && translator_->max_index () > endIdx_ + _idx)
  {
    colorStartIdx_ = endIdx_;
    endIdx_ = colorEndIdx_ = colorStartIdx_ + _idx;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------

bool ColorStack::Node::setIndex (unsigned int _idx) const
{
  if (colorStartIdx_ && colorStartIdx_ + _idx < colorEndIdx_)
  {
    glColor(translator_->index2color(colorStartIdx_ + _idx));
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------

bool ColorStack::Node::getIndexColor (unsigned int _idx, Vec4uc &_rgba) const
{
  if (colorStartIdx_ && colorStartIdx_ + _idx < colorEndIdx_)
  {
    _rgba = translator_->index2color(colorStartIdx_ + _idx);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------

ColorStack::Node * ColorStack::Node::pushIndex (unsigned int _idx)
{
  ColorStack::Node *n = new ColorStack::Node (_idx, this, translator_);
  nodes_.push_back (n);
  return n;
}

//----------------------------------------------------------------------------

ColorStack::Node * ColorStack::Node::popIndex ()
{
  parent_->endIdx_ = endIdx_;
  return parent_;
}

//----------------------------------------------------------------------------

void ColorStack::Node::colorToStack (std::vector<unsigned int> &_stack, unsigned int _index)
{
  if (_index >= colorStartIdx_ && _index < colorEndIdx_)
  {
    _stack.push_back (_index - colorStartIdx_);
  }
  else
  {
    for (std::vector<Node *>::iterator it = nodes_.begin (); it != nodes_.end(); ++it)
    {
      ColorStack::Node *n = (*it);
      if (_index >= n->startIndex () && _index < n->endIndex ())
        n->colorToStack (_stack, _index);
    }
  }
  _stack.push_back (index_);
}

//=============================================================================
} // namespace ACG
//=============================================================================

