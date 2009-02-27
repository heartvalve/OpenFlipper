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

#ifndef CoordsysPlugin_HH
#define CoordsysPlugin_HH

#include <QObject>
#include <QtGui>
#include <QCheckBox>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ACG/Scenegraph/CoordsysNode.hh>

class CoordsysPlugin : public QObject, BaseInterface, KeyInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(KeyInterface)

signals:

  void updateView();

private slots:
  void slotObjectUpdated(int /*_identifier*/){};

//   void slotKeyEvent( QKeyEvent* _event ) {};

  void pluginsInitialized();

public :

  ~CoordsysPlugin() {};

  QString name() { return (QString("Coordsys")); };
  QString description( ) { return (QString("Paints a coordinate system frame")); };

private :

  ACG::SceneGraph::CoordsysNode* node_;

public slots:
  QString version() { return QString("0.1"); };
};

#endif //CoordsysPlugin_HH
