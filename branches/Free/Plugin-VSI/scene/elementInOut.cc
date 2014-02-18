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
#include <QFont>

#include "sceneElement.hh"
#include "elementInOut.hh"
#include "text.hh"
#include "connection.hh"
#include "connectionPoint.hh"
#include "../parser/inout.hh"
#include "../parser/element.hh"
#include "../parser/context.hh"

#include <iostream>

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ElementInOut - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ElementInOut::ElementInOut (InOut *_io, SceneElement *_parent):
  io_ (_io),
  element_ (_parent)
{
  conn_ = new ConnectionPoint (this, _parent);
  typeText_ = new Text (_io->typeString (), _parent);
  QFont font = typeText_->font ();
  font.setItalic (true);
  font.setPointSize (8);
  typeText_->setFont (font);
  descText_ = new Text (_io->shortDescription (), _parent);
  font = descText_->font ();
  if (_io->name() == "data")
    font.setPointSize (10);
  else
    font.setPointSize (8);
  descText_->setFont (font);

  conn_->setToolTip (_io->typeString () + " : " + _io->longDescription ());
  typeText_->setToolTip (_io->typeString () + " : " + _io->longDescription ());
  descText_->setToolTip (_io->typeString () + " : " + _io->longDescription ());

  typeText_->setHorizontalStretch (true);
  descText_->setHorizontalStretch (true);
}


ElementInOut::ElementInOut(const ElementInOut& _orig) :
  io_(NULL),
  element_(NULL),
  conn_(NULL),
  typeText_(NULL),
  descText_(NULL)
{
  std::cerr << "Illegal use of copy constructor in class ElementInOut, which is not implemented yet!" << std::endl;
};

//------------------------------------------------------------------------------

/// Destructor
ElementInOut::~ElementInOut ()
{
  foreach (Connection *c, connections_)
    delete c;

  delete conn_;
  delete typeText_;
  delete descText_;
}

//------------------------------------------------------------------------------

/// Can this input/output be connected to _e
bool ElementInOut::validConnection (ElementInOut *_e)
{
  // Can't be connected to itself
  if (_e == this)
    return false;
  // Can't be connected to another input/output of the same element
  if (element_ == _e->element_)
    return false;

  // Can't be connected if types are different and can't be converted
  if (io_->typeString () != _e->io_->typeString () &&
      !io_->element ()->context ()->canConvert (io_->typeString (), _e->io_->typeString ()))
    return false;

  // An input can only have one connection
  if (type () == TypeInput && !connections ().isEmpty ())
    return false;
  if (_e->type () == TypeInput && !_e->connections().isEmpty ())
    return false;

  // Circular dependency check
  if ((type () == TypeInput && _e->element ()->isAfter (element ())) ||
      (type () == TypeOutput && _e->element ()->isBefore (element ())))
    return false;

  // inputs can only be connected to outputs and vice versa
  if (type () == _e->type ())
    return false;

  return true;
}

//------------------------------------------------------------------------------

/// Add the connection
void ElementInOut::addConnection (Connection *_conn)
{
  connections_.append (_conn);
}

//------------------------------------------------------------------------------

/// Remove the Connection
void ElementInOut::removeConnection (Connection *_conn)
{
  connections_.removeAll (_conn);
}

//------------------------------------------------------------------------------
}
