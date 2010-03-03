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
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>

#include "boolWidget.hh"


//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::BoolWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
BoolWidget::BoolWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent),
  default_ (true),
  true_ (0),
  false_ (0)
{
  QHBoxLayout *hL = new QHBoxLayout;

  true_ = new QRadioButton ("True");
  hL->addWidget (true_);

  false_ = new QRadioButton ("False");
  hL->addWidget (false_);

  if (_hints.contains ("default"))
    default_ = strToBool (_hints["default"]);

  if (default_)
    true_->setChecked (true);
  else
    false_->setChecked (true);

  setLayout (hL);
}

//------------------------------------------------------------------------------

/// Destructor
BoolWidget::~BoolWidget()
{
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString BoolWidget::toValue()
{
  return (true_->isChecked()) ? "true" : "false";
}

//------------------------------------------------------------------------------

/// Read value from string
void BoolWidget::fromValue(QString _from)
{
  bool in = strToBool (_from);
  if (in)
    true_->setChecked (true);
  else
    false_->setChecked (true);
}

//------------------------------------------------------------------------------

/// Reset to default
void BoolWidget::toDefault()
{
  if (default_)
    true_->setChecked (true);
  else
    false_->setChecked (true);
}

//------------------------------------------------------------------------------

// Convert string to bool
bool BoolWidget::strToBool (QString _str)
{
  if (_str == "1" || _str == "true"  || _str == "True" || _str == "TRUE"  ||
      _str == "yes"  || _str == "Yes"  || _str == "YES")
    return true;
  return false;
}

//------------------------------------------------------------------------------
}
