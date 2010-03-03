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
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>


#include "parser/input.hh"

#include "dynamicDialog.hh"
#include "dynamicValue.hh"
//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::DynamicDialog - IMPLEMENTATION
//
//=============================================================================

/// Constructor
DynamicDialog::DynamicDialog (QVector<Input *> _inputs, QWidget * _parent) :
  QDialog (_parent),
  inputs_ (_inputs)
{
  QVBoxLayout *configL = new QVBoxLayout;

  foreach (Input *i, _inputs)
  {
    DynamicValue *v = new DynamicValue (i);

    if (!v->main_)
    {
      delete v;
      continue;
    }

    configL->addWidget (v->group_);

    values_.append (v);
  }

  configL->addStretch(1);

  QPushButton *ok = new QPushButton ("OK");

  connect (ok, SIGNAL (pressed ()), this, SLOT (accept ()));

  QHBoxLayout *buttonL = new QHBoxLayout;
  buttonL->addStretch();
  buttonL->addWidget (ok);

  QVBoxLayout *vL = new QVBoxLayout;

  QWidget *sW = new QWidget;
  sW->setLayout (configL);
  QScrollArea *sA = new QScrollArea;
  sA->setWidget (sW);
  sA->setWidgetResizable (true);
  
  vL->addWidget (sA);
  vL->addLayout (buttonL);

  setLayout (vL);

  resize (500, 300);
}

//------------------------------------------------------------------------------

/// Destructor
DynamicDialog::~ DynamicDialog()
{
  foreach (DynamicValue *v, values_)
    if (v)
      delete v;
}

//------------------------------------------------------------------------------

/// Results
QMap<QString, QString> DynamicDialog::getResults()
{
  QMap<QString, QString>  rv;

  foreach (DynamicValue *v, values_)
    rv[v->input_->name ()] = v->main_->toValue ();

  return rv;
}

//------------------------------------------------------------------------------
}