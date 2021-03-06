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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

//== INCLUDES =================================================================
#include "elementFunction.hh"

#include "graphicsScene.hh"
#include "sceneElement.hh"
#include "../parser/function.hh"
#include "../parser/element.hh"

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



