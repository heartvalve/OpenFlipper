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
#include <QVBoxLayout>

#include "../scene/elementInput.hh"
#include "../parser/type.hh"
#include "../parser/inout.hh"
#include "../parser/element.hh"
#include "../parser/context.hh"

#include "dynamicValue.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::DynamicValue - IMPLEMENTATION
//
//=============================================================================

/// Constructor
DynamicValue::DynamicValue (Input *_input) :
  input_ (_input),
  main_ (0)
{
  group_ = new QGroupBox (_input->shortDescription ());
  group_->setCheckable (false);

  Type *t = _input->element ()->context ()->getType (_input->typeString ());

  if (!t)
    return;

  QVBoxLayout *vL = new QVBoxLayout;
  QHBoxLayout *hL = new QHBoxLayout;

  main_ = t->widget (_input->hints (), _input->typeString ());

  hL->addWidget (main_);
  hL->setStretchFactor (main_, 2);

  QFrame *f = new QFrame ();

  f->setFrameStyle (QFrame::VLine | QFrame::Plain);
  hL->addWidget (f);

  default_ = new QPushButton ("Default");
  hL->addWidget (default_);

  vL->addLayout (hL);
  vL->addStretch(1);
  group_->setLayout (vL);

  connect (default_, SIGNAL (pressed()), this, SLOT (toDefault()));
}

//------------------------------------------------------------------------------

/// Destructor
DynamicValue::~ DynamicValue ()
{
  if (main_)
    delete main_;
  delete group_;
}

//------------------------------------------------------------------------------

// reset to default
void DynamicValue::toDefault()
{
  if (main_)
    main_->toDefault ();
}

//------------------------------------------------------------------------------
}
