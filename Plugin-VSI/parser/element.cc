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
#include "element.hh"
#include "input.hh"
#include "output.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Element - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Element::Element (Context *_context, QString _name) :
  context_ (_context),
  name_ (_name),
  precode_ (""),
  code_ (""),
  dataIn_ (0),
  dataOut_ (0),
  flags_ (0),
  id_ (0)
{
};

//------------------------------------------------------------------------------

/// Destructor
Element::~Element ()
{
  foreach (Input *i, inputs_)
    delete i;

  foreach (Output *o, outputs_)
    delete o;

  if (dataIn_)
    delete dataIn_;

  if (dataOut_)
    delete dataOut_;
}

//------------------------------------------------------------------------------

/// Returns an unused id number
unsigned int Element::getNewId()
{
  return id_++;
}

//------------------------------------------------------------------------------

/// sets the minimum for an unused id
void Element::setMinId(unsigned int _id)
{
  if (id_ < _id)
    id_ = _id;
}

//------------------------------------------------------------------------------
}

