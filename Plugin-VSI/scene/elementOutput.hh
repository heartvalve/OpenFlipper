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

#ifndef VSI_ELEMENTOUTPUT_HH
#define VSI_ELEMENTOUTPUT_HH

//== INCLUDES =================================================================
#include "elementInOut.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Output;


//== CLASS DEFINITION =========================================================
/** Class for outputs of a scene element.
  */
class ElementOutput : public ElementInOut {

  public:

    /// Constructor
    ElementOutput (Output *_out, SceneElement *_parent);

    /// Destructor
    ~ElementOutput ();

    /// Type (=Output)
    Type type () const { return TypeOutput; };

    /// Add connection
    void addConnection (Connection *_conn);

    /// Remove connection
    void removeConnection (Connection *_conn);

  private:
    Output *out_;
};

//=============================================================================
}
//=============================================================================

#endif
