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

#ifndef VSI_INPUT_HH
#define VSI_INPUT_HH

//== INCLUDES =================================================================
#include <QString>
#include <QVariant>

#include "inout.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Class representing an input of an element
  */
class Input : public InOut {

  public:

    /// Input states
    enum State {
      NoExternalInput = 0x1,
      NoUserInput = 0x2,
      NoRuntimeUserInput = 0x4,
      Optional = 0x8
    };

    /// Constructor
    Input (Element *_f);

    /// Current state
    unsigned int state ();

    /// Sets the state
    void setState (unsigned int _state) { state_ = _state; };

    friend class Context;

  private:

    unsigned int state_;

};

//=============================================================================
}
//=============================================================================

#endif