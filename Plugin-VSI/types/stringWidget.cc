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
#include <QLineEdit>
#include <QPushButton>

#include "stringWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::StringWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
StringWidget::StringWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent),
  default_ (""),
  line_ (0)
{
  QHBoxLayout *hL = new QHBoxLayout;

  line_ = new QLineEdit;

  if (_hints.contains ("default"))
    default_ = _hints["default"];

  line_->setText (default_);

  hL->addWidget (line_);

  setLayout (hL);
}

//------------------------------------------------------------------------------

/// Destructor
StringWidget::~ StringWidget()
{
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString StringWidget::toValue()
{
  return "\"" + line_->text () + "\"";
}

//------------------------------------------------------------------------------

/// Read value from string
void StringWidget::fromValue(QString _from)
{
  if (_from.isEmpty ())
  {
    line_->clear ();
    return;
  }

  _from.remove (0, 1);
  _from.remove (_from.length () - 1, 1);

  line_->setText (_from);
}

//------------------------------------------------------------------------------

/// Reset to default
void StringWidget::toDefault()
{
  line_->setText (default_);
}

//------------------------------------------------------------------------------
}
