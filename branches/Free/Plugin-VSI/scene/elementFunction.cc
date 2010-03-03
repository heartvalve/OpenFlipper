//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2009 by Computer Graphics Group, RWTH Aachen
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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

//== INCLUDES =================================================================
#include "elementFunction.hh"

#include "graphicsScene.hh"
#include "sceneElement.hh"
#include "parser/function.hh"
#include "parser/element.hh"

#include "baseWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ElementFunction - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ElementFunction::ElementFunction (Function *_f, SceneElement *_element) :
  element_ (_element),
  function_ (_f)
{
  scene_ = new GraphicsScene (_f->element()->context(), this);
  connect (scene_, SIGNAL (contentChanged ()),
           _element->graphicsScene (), SLOT (contentChange ()));
}

//------------------------------------------------------------------------------

/// Destructor
ElementFunction::~ElementFunction ()
{
  delete scene_;
}

//------------------------------------------------------------------------------

/// Save to xml
void ElementFunction::saveToXml(QDomDocument & _doc, QDomElement & _root)
{
  QDomElement main = _doc.createElement("function");
  _root.appendChild(main);
  main.setAttribute ("name",function_->name ());
  scene_->saveToXml (_doc, main);
}

//------------------------------------------------------------------------------

/// Load from xml
void ElementFunction::loadFromXml(QXmlQuery & _xml)
{
  scene_->loadFromXml (_xml);
}

//------------------------------------------------------------------------------

// initiate editing
void VSI::ElementFunction::editFunction()
{
  BaseWidget::getBaseWidget ()->pushScene (scene_);
}

//------------------------------------------------------------------------------
}



