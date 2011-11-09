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
*   $Revision: 11270 $                                                       *
*   $LastChangedBy: wilden $                                                *
*   $Date: 2011-04-11 15:13:21 +0200 (Mo, 11 Apr 2011) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef DECIMATER_PLUGIN_HH_INCLUDED
#define DECIMATER_PLUGIN_HH_INCLUDED

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "DecimaterToolbarWidget.hh"

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModHausdorffT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalDeviationT.hh>
#include <OpenMesh/Tools/Decimater/ModEdgeLengthT.hh>

#include "Decimater/DecimaterMods/ModRoundnessT.hh"


//== CLASS DEFINITION =========================================================

class DecimaterPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, BackupInterface {
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(ToolboxInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(BackupInterface)

signals:

  // BaseInterface
  void updateView();
  void updatedObject(int _id, const UpdateType _type);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );

  // BackupInterface
  void createBackup(int _id, QString _name, UpdateType _type = UPDATE_ALL);

public:

  DecimaterPlugin() : tool_(0) {};
  ~DecimaterPlugin() {};

  QString name() { return QString("DecimaterPlugin"); };
  QString description() { return QString("Mesh Decimater Plugin"); };

private:

  /// Widget for Toolbox
  DecimaterToolbarWidget* tool_;
  QIcon* toolIcon_;

  typedef OpenMesh::Decimater::DecimaterT< TriMesh > DecimaterType;

  typedef OpenMesh::Decimater::ModQuadricT< DecimaterType >::Handle ModQuadricH;
  typedef OpenMesh::Decimater::ModNormalFlippingT< DecimaterType >::Handle ModNormalFlippingH;
  typedef OpenMesh::Decimater::ModHausdorffT< DecimaterType >::Handle ModHausdorffH;
  typedef OpenMesh::Decimater::ModNormalDeviationT< DecimaterType >::Handle ModNormalDeviationH;
  typedef OpenMesh::Decimater::ModRoundnessT< DecimaterType >::Handle ModRoundnessH;
  typedef OpenMesh::Decimater::ModEdgeLengthT< DecimaterType >::Handle ModEdgeLengthH;

private slots:

  // BaseInterface
  void initializePlugin();
  void slotObjectUpdated(int _identifier , const UpdateType _type);
  void slotObjectSelectionChanged(int _identifier);

  /// decimating called from button in toolbox
  void slot_decimate();

  // slider / spinbox updates
  void slotUpdateNumTriangles();

  void updateNormalDev(int _value);
  void updateAspectRatio(int _value);
  void updateAspectRatio(double _value);
  void updateNumTriangles(int _value);

  // checkbox updates
  void cbUpdateDistance(int _state);
  void cbUpdateNormalDev(int _state);
  void cbUpdateAspectRatio(int _state);
  void cbUpdateEdgeLength(int _state);
  void cbUpdateNumTriangles(int _state);


public slots:

  QString version() { return QString("0.1"); };
};

#endif
