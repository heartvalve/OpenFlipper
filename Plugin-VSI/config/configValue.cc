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
#include <QHBoxLayout>
#include <QFrame>

#include "../scene/elementInput.hh"
#include "../parser/type.hh"
#include "../parser/inout.hh"
#include "../parser/element.hh"
#include "../parser/context.hh"

#include "configValue.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ConfigValue - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ConfigValue::ConfigValue (ElementInput *_input) :
  input_ (_input),
  main_ (0),
  constant_ (0),
  optional_ (0),
  forceAskUser_ (0),
  buttonGroup_ (0)
{
  group_ = new QGroupBox (_input->inOut ()->shortDescription ());
  group_->setCheckable (false);

  Type *t = _input->inOut ()->element ()->context ()->getType (_input->inOut ()->typeString ());
  if (!t)
    return;

  QVBoxLayout *vL = new QVBoxLayout;
  QHBoxLayout *hL = new QHBoxLayout;

  if (_input->state () & Input::Optional || !(_input->state () & Input::NoExternalInput) ||
      !(_input->state () & Input::NoRuntimeUserInput))
  {
    QHBoxLayout *bL = new QHBoxLayout;
    QString dI = (_input->state () & Input::NoExternalInput) ? "" : "Direct Input / ";

    buttonGroup_ = new QButtonGroup (this);

    constant_ = new QRadioButton ("Constant value");
    if (_input->state () & Input::Optional)
    {
      optional_ = new QRadioButton (dI + "Optional");
      if (!(_input->state () & Input::NoRuntimeUserInput))
      {
        forceAskUser_ = new QRadioButton ("Ask during execution");
        buttonGroup_->addButton (forceAskUser_, 2);
      }
    }
    else
      optional_ = new QRadioButton (dI + "Ask during execution");
    buttonGroup_->addButton (optional_, 0);
    buttonGroup_->addButton (constant_, 1);

    bL->addWidget (optional_);
    bL->addWidget (constant_);
    if (forceAskUser_)
    {
      bL->addWidget (forceAskUser_);
      buttonGroup_->addButton (forceAskUser_, 2);
    }

    vL->addLayout (bL);

    QFrame *f = new QFrame ();

    f->setFrameStyle (QFrame::HLine | QFrame::Plain);
    vL->addWidget (f);

    connect (buttonGroup_, SIGNAL (buttonClicked (int)),
             this, SLOT (selectionChange()));
  }


  main_ = t->widget (_input->inOut ()->hints (), _input->inOut ()->typeString ());

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

  if (_input->isSet ())
  {
    main_->fromValue (_input->value ());

    if (constant_)
      constant_->setChecked (true);
  }
  else
  {
    if (!_input->value ().isEmpty ())
      main_->fromValue (_input->value ());

    if (_input->isForceAskSet () && forceAskUser_)
    {
      forceAskUser_->setChecked (true);
      main_->setEnabled (false);
      default_->setEnabled (false);
    }
    else if (optional_)
    {
      optional_->setChecked (true);
      main_->setEnabled (false);
      default_->setEnabled (false);
    }
  }

  connect (default_, SIGNAL (pressed()), this, SLOT (toDefault()));
}

//------------------------------------------------------------------------------

/// Destructor
ConfigValue::~ ConfigValue ()
{
  if (main_)
    delete main_;
  delete group_;
}

//------------------------------------------------------------------------------

// reset to default
void ConfigValue::toDefault()
{
  if (main_)
    main_->toDefault ();
}

//------------------------------------------------------------------------------

void ConfigValue::selectionChange()
{

  if (constant_->isChecked())
  {
    main_->setEnabled (true);
    default_->setEnabled (true);
  }
  else
  {
    main_->setEnabled (false);
    default_->setEnabled (false);
  }
}


//------------------------------------------------------------------------------
}

