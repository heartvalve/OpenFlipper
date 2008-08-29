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




#ifndef UNLOADPLUGINSWIDGET_HH
#define UNLOADPLUGINSWIDGET_HH

#include "ui_unloadPlugins.hh"
#include <QtGui>
#include <QStringList>

class unloadPluginsWidget : public QWidget, public Ui::unloadPlugins
{
  Q_OBJECT
  public:
    unloadPluginsWidget(QStringList plugins, QWidget *parent = 0 );
    
  signals:
    void unload(QString name);
    void dontLoadPlugins(QStringList plugins);
  private slots:
    void slotUnload();
  public:
    void setPlugins(QStringList plugins);
};

#endif //UNLOADPLUGINSWIDGET_HH
