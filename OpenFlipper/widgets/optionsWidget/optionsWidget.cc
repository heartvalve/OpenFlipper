//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
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
//   $Revision: 1909 $
//   $Author: wilden $
//   $Date: 2008-06-03 18:45:21 +0200 (Tue, 03 Jun 2008) $
//
//=============================================================================




#include "optionsWidget.hh"
#include <iostream>
#include <OpenFlipper/common/GlobalOptions.hh>

OptionsWidget::OptionsWidget( QWidget *parent)
  : QWidget(parent)
{
  setupUi(this);
  connect(applyButton,SIGNAL(clicked()),this,SLOT(slotApply()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(slotCancel()));
}

void OptionsWidget::showEvent ( QShowEvent * event ) {
  std::cerr << "Todo : Set Options in Widget" << std::endl;
}

void OptionsWidget::slotApply() {
  std::cerr << "Todo : Get Options from Widget and set them in the core" << std::endl;
  emit applyOptions();
  hide();
}

void OptionsWidget::slotCancel() {
  hide();
}

