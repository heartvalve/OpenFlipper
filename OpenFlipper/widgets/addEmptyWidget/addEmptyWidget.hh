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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#ifndef ADDEMPTYWIDGET_HH
#define ADDEMPTYWIDGET_HH

#include "ui_addEmpty.hh"
#include <QtGui>

#include "OpenFlipper/common/Types.hh"

class addEmptyWidget : public QDialog, public Ui::addEmpty
{
  Q_OBJECT
  public:
    addEmptyWidget(std::vector< DataType > _types , QStringList _typeNames, QWidget *parent = 0 );
    
  private:
    std::vector< DataType > types_;
    QStringList typeNames_;
  signals:
    void chosen(DataType type, int& id);
    
  private slots:
    void slotAddEmpty();
};

#endif
