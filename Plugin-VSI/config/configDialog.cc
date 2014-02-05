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
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QLabel>


#include "scene/elementInput.hh"

#include "configDialog.hh"
#include "configValue.hh"
//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ConfigDialog - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ConfigDialog::ConfigDialog (QVector<ElementInput *> _inputs, QString _name, QWidget * _parent) :
  QDialog (_parent),
  inputs_ (_inputs),
  name_(0)
{
  QVBoxLayout *vL = new QVBoxLayout;
  QVBoxLayout *configL = new QVBoxLayout;

  if (!_name.isNull ())
  {
    QHBoxLayout *nameL = new QHBoxLayout;
    QLabel *l = new QLabel(tr("Element name:"));
    name_ = new QLineEdit;
    name_->setText (_name);
    nameL->addWidget (l);
    nameL->addWidget (name_);
    nameL->setStretchFactor(name_, 1);

    vL->addLayout (nameL);
  }

  foreach (ElementInput *i, _inputs)
  {
    ConfigValue *v = new ConfigValue (i);

    if (!v->main_)
    {
      delete v;
      continue;
    }

    if (_inputs.size () > 1)
      configL->addWidget (v->group_);
    else
      vL->addWidget (v->group_);

    values_.append (v);
  }

  configL->addStretch(1);
  
  QPushButton *ok = new QPushButton ("OK");
  QPushButton *cancel = new QPushButton ("Cancel");

  connect (ok, SIGNAL (pressed ()), this, SLOT (ok ()));
  connect (cancel, SIGNAL (pressed ()), this, SLOT (reject ()));

  QHBoxLayout *buttonL = new QHBoxLayout;
  buttonL->addStretch();
  buttonL->addWidget (ok);
  buttonL->addWidget (cancel);

  if (_inputs.size () > 1)
  {
    QWidget *sW = new QWidget;
    sW->setLayout (configL);
    QScrollArea *sA = new QScrollArea;
    sA->setWidget (sW);
    sA->setWidgetResizable (true);
    vL->addWidget (sA);
    resize (500, 300);
  }
  else
    resize (500, 0);

  vL->addLayout (buttonL);

  setLayout (vL);

  
}

//------------------------------------------------------------------------------

/// Destructor
ConfigDialog::~ ConfigDialog()
{
  foreach (ConfigValue *v, values_)
    delete v;
}

//------------------------------------------------------------------------------

QString ConfigDialog::name ()
{

  if (name_)
    return name_->text ();

  return QString ();

}

//------------------------------------------------------------------------------

// OK pressed
void VSI::ConfigDialog::ok ()
{
  foreach (ConfigValue *v, values_)
  {
    if (!v->constant_ || v->constant_->isChecked ())
      v->input_->set (true);
    else
      v->input_->set (false);

    if (v->forceAskUser_ && v->forceAskUser_->isChecked ())
      v->input_->setForceAsk (true);
    else
      v->input_->setForceAsk (false);
    v->input_->setValue (v->main_->toValue ());
  }
  QDialog::accept ();
}

//------------------------------------------------------------------------------
}
