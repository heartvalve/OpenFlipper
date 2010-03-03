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
#include "input.hh"
#include "type.hh"
#include "element.hh"
#include "context.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Input - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Input::Input (Element *_e) :
  InOut (_e),
  state_ (0)
{
}

//------------------------------------------------------------------------------

/// Current state
unsigned int Input::state()
{
  unsigned int state = state_;

  // If type is not supported or has no configuration widged that we can't support user input
  Type *tp = element ()->context ()->getType (typeString ());
  if (!tp || !tp->hasWidget ())
  {
    state |= NoUserInput | NoRuntimeUserInput;
  }

  return state;
}

//------------------------------------------------------------------------------
}
