/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Mi, 26. Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/

#include "philipsStereoSettingsWidget.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

#include <iostream>

PhilipsStereoSettingsWidget::PhilipsStereoSettingsWidget(QWidget* _parent) : QWidget(_parent) {
  setupUi(this);

  connect(closeButton,SIGNAL(clicked()),this,SLOT(closeWidget()));

  connect(headerContentType,SIGNAL(currentIndexChanged (int)),this,SLOT(updateValues()));
  connect(headerFactor,SIGNAL(valueChanged(int)),this,SLOT(updateValues()));
  connect(headerOffsetCC,SIGNAL(valueChanged(int)),this,SLOT(updateValues()));
  connect(headerSelect,SIGNAL(currentIndexChanged(int)),this,SLOT(updateValues()));
}

void PhilipsStereoSettingsWidget::closeWidget()  {
  updateValues();
  hide();
}

void PhilipsStereoSettingsWidget::updateValues()  {

  // Update labels that display the current values
  factorCounter->setNum(headerFactor->value());
  offsetCounter->setNum(headerOffsetCC->value());

  // Set option entries
  OpenFlipperSettings().setValue("Core/Stereo/Philips/Content",headerContentType->currentIndex());
  OpenFlipperSettings().setValue("Core/Stereo/Philips/Factor",headerFactor->value());
  OpenFlipperSettings().setValue("Core/Stereo/Philips/Offset",headerOffsetCC->value());
  OpenFlipperSettings().setValue("Core/Stereo/Philips/Select",headerSelect->currentIndex());

  // Tell others that we should update the view
  emit updateView();
}

