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
#include <cmath>

#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QDoubleSpinBox>

#include "numWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::NumWidget - IMPLEMENTATION
//
//=============================================================================

/// Constructor
NumWidget::NumWidget(QMap< QString, QString > &_hints, QString _typeName, QWidget *_parent) :
  TypeWidget (_hints, _typeName, _parent),
  precision_ (1.0),
  isFloat_ (false),
  numDecimals_ (0),
  sliderMul_ (1.0),
  slider_ (0),
  spin_ (0)
{
  bool ok;

  QHBoxLayout *hL = new QHBoxLayout;

  if (_hints.contains ("precision"))
  {
    double v = _hints["precision"].toDouble (&ok);
    if (ok)
      precision_ = v;
  }

  

  while (precision_ * sliderMul_ != floor(precision_ * sliderMul_) && numDecimals_ <= 10)
  {
    sliderMul_ *= 10.0;
    numDecimals_++;
  }

  if (_typeName == "Float")
    isFloat_ = true;
  else if (_typeName == "Integer")
    isFloat_ = false;
  else if (precision_ != int(precision_))
    isFloat_ = true;
    

  if (_hints.contains ("min") && _hints.contains ("max"))
  {
    double min = _hints["min"].toDouble (&ok);
    if (!ok)
      min = -65535;
    if (!isFloat_)
      min = int (min);

    double max = _hints["max"].toDouble (&ok);
    if (!ok)
      max = 65535;
    if (!isFloat_)
      max = int (max);

    if (min > max)
      max = min + precision_;

    max *= sliderMul_;
    min *= sliderMul_;

    slider_ = new QSlider;
    slider_->setRange (min, max);
    slider_->setSingleStep (precision_ * sliderMul_);
    slider_->setOrientation (Qt::Horizontal);

    connect (slider_, SIGNAL (valueChanged(int)), this, SLOT (sliderValueChanged(int)));

    hL->addWidget(slider_);
  }
  else
  {
    hL->addStretch ();
  }

  spin_ = new QDoubleSpinBox;

  if (_hints.contains ("min") && _hints.contains ("max"))
  {
    double min = _hints["min"].toDouble (&ok);
    if (!ok)
      min = -65535;
    if (!isFloat_)
      min = int (min);

    double max = _hints["max"].toDouble (&ok);
    if (!ok)
      max = 65535;
    if (!isFloat_)
      max = int (max);

    if (min > max)
      max = min + precision_;

    spin_->setRange (min, max);
  }
  else
    spin_->setRange (-1000000, 1000000);

  spin_->setSingleStep (precision_);
  spin_->setDecimals (numDecimals_);
  connect (spin_, SIGNAL (valueChanged(double)), this, SLOT (spinValueChanged(double)));

  ok = false;
  if (_hints.contains ("default"))
    default_ = _hints["default"].toFloat (&ok);

  if (!ok)
    default_ = (spin_->minimum () + spin_->maximum ()) / 2;

  spin_->setValue (default_);

  hL->addWidget (spin_);

  setLayout (hL);
}

/// Destructor
NumWidget::~ NumWidget()
{
}

//------------------------------------------------------------------------------

/// Convert current value to string
QString NumWidget::toValue()
{
  return QString::number (spin_->value ());
}

//------------------------------------------------------------------------------

/// Read value from string
void NumWidget::fromValue(QString _from)
{
  spin_->setValue (_from.toDouble());
}

//------------------------------------------------------------------------------

// handle slider changes
void VSI::NumWidget::sliderValueChanged(int _value)
{
  double v = _value / sliderMul_;

  if (v != spin_->value ())
    spin_->setValue (v);
}

//------------------------------------------------------------------------------

// handle spinbox changes
void VSI::NumWidget::spinValueChanged(double _value)
{
  double v = _value * sliderMul_;

  if (slider_ && v != slider_->value ())
    slider_->setValue (v);
}

//------------------------------------------------------------------------------

/// Reset to default
void VSI::NumWidget::toDefault()
{
  spin_->setValue (default_);
}

//------------------------------------------------------------------------------
}

