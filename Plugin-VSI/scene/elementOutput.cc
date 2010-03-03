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
#include "elementOutput.hh"
#include "connectionPoint.hh"
#include "text.hh"
#include "parser/output.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ElementOutput - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ElementOutput::ElementOutput (Output *_out, SceneElement *_parent) :
  ElementInOut (_out, _parent),
  out_ (_out)
{
  connectionPointItem ()->setState (ConnectionPoint::OutputNotConnected);

  typeTextItem ()->setBackground (true, false);
  typeTextItem ()->setBackgroundBrush (Qt::gray);
  typeTextItem ()->setBrush (Qt::black);
  descriptionTextItem()->setBackground (true, false);
  descriptionTextItem ()->setBackgroundBrush (Qt::white);
  descriptionTextItem ()->setBrush (Qt::black);

  connectionPointItem ()->setWidth (qMax (typeTextItem ()->preferredHeight (),
                                          descriptionTextItem ()->preferredHeight ()));
}

//------------------------------------------------------------------------------

/// Destructor
ElementOutput::~ElementOutput ()
{
}

//------------------------------------------------------------------------------

/// Add connection
void ElementOutput::addConnection(Connection * _conn)
{
  ElementInOut::addConnection (_conn);

  connectionPointItem ()->setState (ConnectionPoint::Connected);
}

//------------------------------------------------------------------------------

/// Remove connection
void ElementOutput::removeConnection (Connection * _conn)
{
  ElementInOut::removeConnection (_conn);

  if (connections().isEmpty())
    connectionPointItem ()->setState (ConnectionPoint::OutputNotConnected);
}

//------------------------------------------------------------------------------
}
